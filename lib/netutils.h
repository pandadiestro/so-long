#ifndef NETUTILS
#include <log.h>

#define IsDecDigit(x) (x <= '9' && x >= '0')
#define IsHexDigit(x) ((x <= '9' && x >= '0') || (x >= 'A' && x <= 'F') || (x >= 'a' && x <= 'f'))
#define IsTerminal(x) (x == '\t' || x == ' ' || x == '\r')

typedef union {
    uint32_t addressv4;
    uint32_t addressv6[4];
} NetAddr;

uint8_t
isDecimalNumStr(char* str, size_t len)
{
    while (len--) {
        if ( !IsDecDigit(str[len]) )
            return 0;
    }

    return 1;
}

Err
hexToDec(const char hexDigit, uint32_t* result)
{
    if ( !IsHexDigit(hexDigit) )
        return INVALID_PARAM;

    if ('a' <= hexDigit && hexDigit <= 'f') {
        *result = hexDigit - 'a' + 10;
        return NO_ERR;
    }

    if ('A' <= hexDigit && hexDigit <= 'F') {
        *result = hexDigit - 'A' + 10;
        return NO_ERR;
    }

    if ('0' <= hexDigit && hexDigit <= '9') {
        *result = hexDigit - '0';
        return NO_ERR;
    }

    return UNEXPECTED;
}

Err
hexStrToSocketAddr4(const char* str, NetAddr* addr)
{
    uint32_t carrier = 0;
    for (size_t i = 0; i < 8; i++) {
        if ( !IsHexDigit(str[i]) )
            return INVALID_PARAM;

        addr->addressv4 *= 16;
        hexToDec(str[i], &carrier);
        addr->addressv4 += carrier;

        carrier = 0;
    }

    return NO_ERR;
}

Err
hexStrToSocketAddr6(const char* str, NetAddr* addr)
{
    uint32_t carrier = 0;
    for (size_t i = 0; i < 4; i++) {
        for (size_t ii = 0; ii < 8; ii++) {
            if ( !IsHexDigit(str[i * 8 + ii]) )
                return INVALID_PARAM;

            addr->addressv6[i] *= 16;
            hexToDec(str[i * 8 + ii], &carrier);
            addr->addressv6[i] += carrier;

            carrier = 0;
        }
    }

    return NO_ERR;
}

Err
hexStrToSocketAddr(const char* str, NetAddr* addr, uint8_t version, uint8_t* len)
{
    switch (version) {
        case 6:
            *len = sizeof(addr->addressv6);
            return hexStrToSocketAddr6(str, addr);
        case 4:
            *len = sizeof(addr->addressv4);
            return hexStrToSocketAddr4(str, addr);
        default:
            return INVALID_PARAM;
    }
}

#define NETUTILS
#endif

