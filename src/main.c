#define _POSIX_C_SOURCE 200809L

#include <netutils/netparse.h>
#include <procfd/procfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <opts.h>

/*
 * Current dependencies:
 *
 * - Unix-based system
 * - Implementation of stdint.h
 * - Linux kernel (I'm genuinely sorry BSD folks)
 * - Version of the Linux kernel over 2.6.22 (released in july '07)
 *   > read: https://lkml.iu.edu/hypermail/linux/kernel/0707.1/0195.html) */
int
main(int argc, char** argv)
{
    opts newopts = newOpts();
    parseOpts(argc, argv, &newopts);

    vect(pid_t) pids = getPIDsOfPort(&newopts);
    if (pids == NULL)
        exit(EXIT_FAILURE);

    if (pids->size == 0) {
        destroyvec(pid_t, pids);
        exit(EXIT_FAILURE);
    }

    if (newopts.print == 1) {
        printall(pid_t, pids, "%u\n");
        exit(EXIT_SUCCESS);
    }

    int8_t e = killAll(pids, &newopts);
    destroyvec(pid_t, pids);
    return e;
}

