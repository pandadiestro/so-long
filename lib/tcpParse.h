#include <stdint.h>
#include <stdio.h>

#include <log.h>
#include <netutils.h>

/*
 * structure of /proc/net/tcp{6}
 * https://www.kernel.org/doc/html/latest/networking/proc_net_tcp.html
*/

typedef struct {
    size_t index;
    NetAddr localAddress;
    uint16_t localAddressPort;

    NetAddr remAddress;
    uint16_t remAddressPort;

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
     * more about this: https://serverfault.com/questions/105260/how-big-in-bits-is-a-unix-uid */
    uint32_t uid;

    uint8_t timeout;

    /* there are way too many variations on the absolute amount of possible
     * inodes in a filesystem so, instead of risk it using a non-standard
     * size, I will just leave it as the best fit for an unsigned integer
     * that big */
    uintmax_t inode;

    /* When freeing, this will store the address to free
     * WARNING -- Only to be used on non-stack-allocated
     * strings */
    char* __free_point;

    /* originally unlabeled (for some reason) fields */
    char* referenceCount;
    char* memAddress;
    char* retransmitTimeout;
    char* tick;
    char* ackProcessed;
    char* congestion;
    char* treshold;
} TCPEntry;

Err
parseTcpStr(char* str, const uint8_t version, TCPEntry* obj)
{
    size_t index = 0;
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    /* .index */
    while ( !IsTerminal(str[index]) ) {
        if ( !IsDecDigit(str[index]) ) {
            index++;
            continue;
        }

        obj->index *= 10;
        obj->index += str[index] - '0';
        index++;
    }

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }


    uint8_t lenLocal = 0;
    /* local address */
    hexStrToSocketAddr(&str[index], &obj->localAddress, version, &lenLocal);

    /* port separator */
    index += lenLocal * 2;
    uint32_t carrier = 0;
    while (str[index] != ' ') {
        if ( !IsHexDigit(str[index]) ) {
            index++;
            continue;
        }

        obj->localAddressPort *= 16;

        hexToDec(str[index], &carrier);
        obj->localAddressPort += carrier;

        index++;
    }

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    /* remote address */
    uint8_t lenRemote = 0;
    hexStrToSocketAddr(&str[index], &obj->remAddress, version, &lenRemote);

    /* port separator */
    index += lenRemote * 2;
    carrier = 0;
    while (str[index] != ' ') {
        if ( !IsHexDigit(str[index]) ) {
            index++;
            continue;
        }

        hexToDec(str[index], &carrier);
        obj->remAddressPort *= 16;
        obj->remAddressPort += carrier;

        index++;
    }

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    carrier = 0;
    /* .connState */
    while (str[index] != ' ') {
        if ( !IsHexDigit(str[index]) ) {
            index++;
            continue;
        }

        hexToDec(str[index], &carrier);
        obj->st *= 16;
        obj->st += carrier;

        index++;
    }

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    carrier = 0;
    /* .tx_queue : .rx_queue
     * I am not quite sure on the numeric
     * base of this, so I'll just leave it as base-16 */
    for (size_t i = 0; i < 8; i++) {
        hexToDec(str[index + i], &carrier);
        obj->tx_queue *= 16;
        obj->tx_queue += carrier;
    }

    /* ':' separator */
    index += 10; /* 8 hex digits + separator + next queue */

    carrier = 0;
    /* .rx_queue */
    for (size_t i = 0; i < 8; i++) {
        hexToDec(str[index + i], &carrier);
        obj->rx_queue *= 16;
        obj->rx_queue += carrier;
    }

    index += 8; /* 8 hex digits + next */

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    carrier = 0;
    /* .tr
     * I am just going to asume base-16 on this one too */
    for (size_t i = 0; i < 2; i++) {
        hexToDec(str[index + i], &carrier);
        obj->tr *= 16;
        obj->tr += carrier;
    }

    index += 4; /* 2 hex digits + separator + next */

    carrier = 0;
    /* .tm_when */
    for (size_t i = 0; i < 8; i++) {
        hexToDec(str[index + i], &carrier);
        obj->tm_when *= 16;
        obj->tm_when += carrier;
    }

    index += 8; /* 7 digits jump + next*/

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    carrier = 0;
    /* .retrnsmt
     * Another (I hope) base-16 number */
    for (size_t i = 0; i < 8; i++) {
        hexToDec(str[index + i], &carrier);
        obj->retrnsmt *= 16;
        obj->retrnsmt += carrier;
    }

    index += 8; /* 7 digits jump + whitespace + next */

    /* .uid
     * First, thou shalt ignore yeth padding whitespaces */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    while ( !IsTerminal(str[index]) ) {
        if ( !IsDecDigit(str[index])) {
            index++;
            continue;
        }

        obj->uid *= 10;
        obj->uid += str[index] - '0';

        index++;
    }

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    /* .timeout */
    while ( !IsTerminal(str[index]) ) {
        if ( !IsDecDigit(str[index]) ) {
            index++;
            continue;
        }

        obj->timeout *= 10;
        obj->timeout += str[index] - '0';

        index++;
    }

    /* whitespace */
    while ( IsTerminal(str[index]) ) {
        index++;
    }

    /* .inode */
    obj->inode = 0;

    while ( !IsTerminal(str[index]) ) {
        if ( !IsDecDigit(str[index])) {
            index++;
            continue;
        }

        obj->inode *= 10;
        obj->inode += str[index] - '0';

        index++;
    }

    return NO_ERR;
}



