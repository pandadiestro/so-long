#ifndef LOGGER
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    NO_ERR,
    HEAP_MALFUNC,
    INVALID_INPUT,
    INVALID_PARAM,
    UNEXPECTED,
} Err;

#define LOGGER
#endif

