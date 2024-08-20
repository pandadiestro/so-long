#include <netutils/netparse.h>
#include <stdio.h>
#include <stdlib.h>

static inline uint8_t
asciiDigit(char c)
{
    if (c <= 'f' && c >= 'a')
        return (c - 'a' + 10);
    else if (c <= 'F' && c >= 'A')
        return (c - 'A' + 10);
    else if (c <= '9' && c >= '0')
        return (c - '0');

    return 0;
}

static inline uintmax_t
nextInt(const Line* line, uint8_t base, size_t* offset, int8_t limdigits)
{
    uintmax_t carrier = 0;
    uint8_t digitStarted = 0;

    char c = '\0';
    int8_t cursor = 0;
    while (*offset < line->size && cursor <= limdigits) {
        c = line->str[*offset];
        switch (c) {
            case ':':
            case ' ':
                if (digitStarted)
                    return carrier;
                break;
            default:
                digitStarted = 1;
                carrier *= base;
                carrier += asciiDigit(c);
                cursor++;
                break;
        }

        (*offset)++;
    }

    return 0;
}

static TCPEntry*
parseEntry4(const Line* line)
{
    TCPEntry* entry = calloc(1, sizeof(TCPEntry));
    if (entry == NULL)
        exit(-1);

    if (line == NULL)
        return (NULL);

    size_t offset = 0;
    *entry = (TCPEntry) {
        .index = (size_t) nextInt(line, 10, &offset, 8),
        .localAddress.addr.addressv4 = (uint32_t) nextInt(line, 16, &offset, 8),
        .localAddress.port = (uint32_t) nextInt(line, 16, &offset, 8),
        .remAddress.addr.addressv4 = (uint32_t) nextInt(line, 16, &offset, 8),
        .remAddress.port = (uint32_t) nextInt(line, 16, &offset, 8),
        .st = (uint8_t) nextInt(line, 16, &offset, 8),
        .tx_queue = (uint32_t) nextInt(line, 16, &offset, 8),
        .rx_queue = (uint32_t) nextInt(line, 16, &offset, 8),
        .tr = (uint8_t) nextInt(line, 16, &offset, 8),
        .tm_when = (uint16_t) nextInt(line, 16, &offset, 8),
        .retrnsmt = (uint16_t) nextInt(line, 16, &offset, 8),
        .uid = (uint32_t) nextInt(line, 10, &offset, 8),
        .timeout = (uint8_t) nextInt(line, 10, &offset, 8),
        .inode = (uintmax_t) nextInt(line, 10, &offset, 8),
    };

    return (entry);
}

static TCPEntry*
parseEntry6(const Line* line)
{
    TCPEntry* entry = calloc(1, sizeof(TCPEntry));
    if (entry == NULL)
        exit(-1);

    if (line == NULL)
        return (NULL);

    size_t offset = 0;
    *entry = (TCPEntry) {
        .index = (size_t) nextInt(line, 10, &offset, 8),
        .localAddress.addr.addressv6[3] = (uint32_t) nextInt(line, 16, &offset, 8),
        .localAddress.addr.addressv6[2] = (uint32_t) nextInt(line, 16, &offset, 8),
        .localAddress.addr.addressv6[1] = (uint32_t) nextInt(line, 16, &offset, 8),
        .localAddress.addr.addressv6[0] = (uint32_t) nextInt(line, 16, &offset, 8),
        .localAddress.port = (uint32_t) nextInt(line, 16, &offset, 8),
        .remAddress.addr.addressv6[3] = (uint32_t) nextInt(line, 16, &offset, 8),
        .remAddress.addr.addressv6[2] = (uint32_t) nextInt(line, 16, &offset, 8),
        .remAddress.addr.addressv6[1] = (uint32_t) nextInt(line, 16, &offset, 8),
        .remAddress.addr.addressv6[0] = (uint32_t) nextInt(line, 16, &offset, 8),
        .remAddress.port = (uint32_t) nextInt(line, 16, &offset, 8),
        .st = (uint8_t) nextInt(line, 16, &offset, 8),
        .tx_queue = (uint32_t) nextInt(line, 16, &offset, 8),
        .rx_queue = (uint32_t) nextInt(line, 16, &offset, 8),
        .tr = (uint8_t) nextInt(line, 16, &offset, 8),
        .tm_when = (uint16_t) nextInt(line, 16, &offset, 8),
        .retrnsmt = (uint16_t) nextInt(line, 16, &offset, 8),
        .uid = (uint32_t) nextInt(line, 10, &offset, 8),
        .timeout = (uint8_t) nextInt(line, 10, &offset, 8),
        .inode = (uintmax_t) nextInt(line, 10, &offset, 8),
    };

    return (entry);
}

TCPEntry*
parseEntry(const Line* line, const uint8_t version)
{
    switch (version) {
        case 4:
            return parseEntry4(line);
        case 6:
            return parseEntry6(line);
        default:
            return NULL;
    }
}


