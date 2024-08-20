#ifndef _netparse_h
#define _netparse_h
#include <netutils/types.h>
#include <stddef.h>
#include <strutils.h>

typedef struct {
    union {
        uint32_t addressv4;
        uint32_t addressv6[4];
    } addr;
    uint32_t port;
} NetAddr;

/*
 * structure of /proc/net/tcp(6)
 * https://www.kernel.org/doc/html/latest/networking/proc_net_tcp.html
 *
 * the /proc/net/udp(6) entries follow the same structure
 */
typedef struct {
    size_t index;
    NetAddr localAddress;
    NetAddr remAddress;

    /* connection state */
    uint8_t st;

    /* transmit and receive queue */
    uint32_t tx_queue;
    uint32_t rx_queue;

    uint8_t tr;
    uint16_t tm_when;

    uint16_t retrnsmt;

    /* take a look at <bits/typesizes.h> to see the equivalent
     * size (if different) for your cpu architecture
     *
     * more about this:
     * https://serverfault.com/questions/105260/how-big-in-bits-is-a-unix-uid */
    uid_t uid;

    uint8_t timeout;

    /* there are way too many variations on the absolute amount of possible
     * inodes in a filesystem so, instead of risk it using a non-standard
     * size, I will just leave it as the best fit for an unsigned integer
     * that big */
    inode_t inode;
} TCPEntry;

TCPEntry* parseEntry(const Line* line, const uint8_t version);


#endif

