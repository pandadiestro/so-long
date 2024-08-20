#ifndef _procfd_h
#define _procfd_h

#include <netutils/netparse.h>
#include <netutils/types.h>
#include <vector/vector.h>
#include <dirent.h>
#include <strutils.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <opts.h>

#define proc_path "/proc/"
#define proc_path_size 6

#define fd_path "/fd"
#define fd_path_size 3

#define genericbufsize 128

#define tcp_path "/proc/net/tcp"
#define tcp_6_path "/proc/net/tcp6"

#define udp_path "/proc/net/udp"
#define udp6_path "/proc/net/udp6"

define_vector_for_type(pid_t)
define_vector_for_type(inode_t)

static inode_t
getInodeOfLink(char* restrict str)
{
    char* socka = strtok(str, ":");
    if (socka == NULL)
        return 0;

    if (strlen(socka) != 6)
        return 0;

    if (strcmp(socka, "socket") != 0)
        return 0;

    char* bracketedInode = strtok(NULL, ":");
    if (bracketedInode == NULL)
        return 0;

    if (*bracketedInode != '[')
        return 0;

    char* endptr = NULL;
    inode_t ino = (inode_t) strtoul(bracketedInode + 1, &endptr, 10);

    if (endptr == NULL)
        return 0;

    if (*endptr != ']')
        return 0;

    return ino;
}

static int8_t
matchInodes(vect(inode_t) nodes, const Line* restrict procname)
{
    if (nodes == NULL || procname == NULL)
        return -1;

    Line* abspath = calloc(1, sizeof(Line));
    abspath->size = procname->size + proc_path_size + fd_path_size;
    abspath->str = calloc(1, abspath->size + 1);

    memcpy(abspath->str, "/proc/", 6);
    memcpy(abspath->str + proc_path_size, procname->str, procname->size);
    memcpy(abspath->str + proc_path_size + procname->size, "/fd", fd_path_size);

    DIR* pidDir = opendir(abspath->str);
    if (pidDir == NULL) {
        free(abspath->str);
        free(abspath);
        return -1;
    }

    struct dirent* pidfd = NULL;
    char fdpath[genericbufsize] = { '\0' };
    struct stat stbuf = { 0 };

    while ((pidfd = readdir(pidDir)) != NULL) {
        sprintf(fdpath, "%s/%s", abspath->str, pidfd->d_name);

        if (stat(fdpath, &stbuf) < 0) {
            closedir(pidDir);
            free(abspath->str);
            free(abspath);
            return -1;
        }

        if (S_ISDIR(stbuf.st_mode))
            continue;

        char newbuf[genericbufsize] = { '\0' };
        if (readlink(fdpath, newbuf, genericbufsize) <= 0)
            continue;

        inode_t ino = getInodeOfLink(newbuf);
        if (ino == 0)
            continue;

        for (size_t i = 0; i < nodes->size; i++) {
            if (nodes->arr[i] != ino)
                continue;

            closedir(pidDir);
            free(abspath->str);
            free(abspath);
            return 1;
        }
    }

    closedir(pidDir);
    free(abspath->str);
    free(abspath);
    return 0;
}

static vect(inode_t)
getInodesOfPortAtPath(uint32_t port, const char* path, const uint8_t version)
{
    FILE* ff = fopen(path, "rb");
    Line* line = calloc(1, sizeof(Line));

    vect(inode_t) newnodes = newvector(inode_t);

    /* ignoring the table header */
    if (getdelim(&line->str, &line->size, '\n', ff) < 0)
        return NULL;

    if (line->str == NULL)
        return NULL;

    free(line->str);
    line->str = NULL;

    while (1) {
        if (getdelim(&line->str, &line->size, '\n', ff) < 0)
            break;

        TCPEntry* entr = parseEntry(line, version);
        if (entr->localAddress.port != port) {
            free(line->str);
            line->str = NULL;

            free(entr);
            continue;
        }

        pushtovec(inode_t, newnodes, entr->inode);
        free(line->str);
        line->str = NULL;
        free(entr);
    }

    free(line->str);
    free(line);
    fclose(ff);

    return newnodes;
}

static vect(pid_t)
getInodedPIDs(vect(inode_t) nodes)
{
    if (nodes == NULL)
        return NULL;

    DIR* procDir = opendir(proc_path);
    if (procDir == NULL)
        return NULL;

    vect(pid_t) newnodes = newvector(pid_t);
    struct dirent* proc = NULL;
    Line newline = { 0 };
    while ((proc = readdir(procDir)) != NULL) {
        newline.str = proc->d_name;
        newline.size = strlen(proc->d_name);

        char* buf = NULL;
        pid_t pid = strtoul(newline.str, &buf, 10);
        if (*buf != '\0')
            continue;

        if (matchInodes(nodes, &newline) == 1)
            pushtovec(pid_t, newnodes, pid);
    }

    closedir(procDir);
    return newnodes;
}

vect(pid_t)
getPIDsOfPort(const opts* restrict opts)
{
    struct netpath {
        char* path;
        uint8_t version;
    } paths[] = {
        {
            .path = "/proc/net/udp6",
            .version = 6,
        },
        {
            .path = "/proc/net/udp",
            .version = 4,
        },
        {
            .path = "/proc/net/tcp6",
            .version = 6,
        },
        {
            .path = "/proc/net/tcp",
            .version = 4,
        },
    };

    vect(inode_t) globinos = newvector(inode_t);
    size_t len = sizeof(paths) / sizeof(struct netpath);
    for (size_t i = 0; i < len; i++) {
        vect(inode_t) newnodes = getInodesOfPortAtPath(opts->port, paths[i].path, paths[i].version);
        if (newnodes == NULL)
            continue;

        for (size_t i = 0; i < newnodes->size; i++) {
            pushtovec(inode_t, globinos, newnodes->arr[i]);
        }

        destroyvec(inode_t, newnodes);
    }

    vect(pid_t) pids = getInodedPIDs(globinos);
    destroyvec(inode_t, globinos);
    return pids;
}

int8_t
killAll(vect(pid_t) pids, const opts* restrict opts)
{
    if (pids == NULL)
        return -1;

    if (pids->size == 0)
        return 1;

    for (size_t i = 0; i < pids->size; i++) {
        kill(pids->arr[i], opts->signal);
    }

    return 0;
}





#endif



