#ifndef _opt_h
#define _opt_h

#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define ERR_MSG "WEBONAZO, asi se usa mira:\nso-long -p [port | mandatory!] -s [signal | default:9] -p (print-mode | default:0) -v (verbose-mode | default:0)\n"

typedef struct opts {
    uint8_t print;
    uint8_t verbose;
    uint32_t port;
    uint16_t signal;
} opts;


opts
newOpts(void)
{
    return (opts) {
        .port = 0,
        .print = 0,
        .verbose = 0,
        .signal = SIGKILL,
    };
}

void
parseOpts(int argc, char** restrict argv, opts* newopts)
{
    uint8_t hasport = 0;
    int opt = 0;
    while ((opt = getopt(argc, argv, "s:Pvp:")) != -1) {
        switch (opt) {
            case 's':
                newopts->signal = (uint16_t) strtoul(optarg, NULL, 10);
                break;
            case 'P':
                newopts->print = 1;
                break;
            case 'v':
                newopts->verbose = 1;
                break;
            case 'p':
                hasport = 1;
                newopts->port = (uint32_t) strtoul(optarg, NULL, 10);
                break;
            default:
                fprintf(stdout, ERR_MSG);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (hasport == 0) {
        fprintf(stdout, ERR_MSG);
        exit(EXIT_FAILURE);
    }
}


#endif


