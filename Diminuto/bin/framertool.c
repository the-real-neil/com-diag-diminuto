/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2023 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (mailto:coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * This is a simple tool to test the Framer against an actual
 * serial port (in this case, the SparkFun LoRa serial kit).
 * framertool only deals with newline-terminated printable strings.
 * Important safety tip: the LoRa serial kit is REALLY slow and
 * disconnects periodically.
 */

#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_framer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

static const size_t MAXDATAGRAM = diminuto_maximumof(uint16_t) - 8;

int main(int argc, char * argv[])
{
    extern char * optarg;
    int opt = '\0';
    char * endptr = (char *)0;
    const char * program = (const char *)0;
    const char * device = "/dev/null";
    int baudrate = 57600; /* Defaults to SparkFun serial LoRa radio pair. */
    int stopbits = 1;
    int databits = 8;
    int paritybit = 0; /* None. */
    int modemcontrol = 0;
    int rtscts = 0;
    int xonxoff = 0;
    size_t bufsize = MAXDATAGRAM;
    diminuto_ticks_t timeout = 1000000000;
    unsigned long milliseconds = 0;
    int rc = -1;
    int dev = -1;
    int std = -1;
    int fds = 0;
    int fd = -1;
    FILE * stream = (FILE *)0;
    diminuto_mux_t multiplexor = { 0 };
    bool error = false;
    bool done = false;
    ssize_t sent = 0;
    ssize_t received = 0;
    char * frame = (char *)0;
    char * line = (char *)0;
    char * here = (char *)0;
    diminuto_framer_t framer = DIMINUTO_FRAMER_INIT;
    diminuto_framer_t * ff = (diminuto_framer_t *)0;

    /*
     * SETUP
     */

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    (void)diminuto_core_enable();

    (void)diminuto_log_setmask();

    /*
     * PARSING
     */

    while ((opt = getopt(argc, argv, "1278B:D:b:dehmnort:x?")) >= 0) {

        switch (opt) {

        case '1':
            stopbits = 1;
            break;

        case '2':
            stopbits = 2;
            break;

        case '7':
            databits = 7;
            break;

        case '8':
            databits = 8;
            break;

        case 'B':
            baudrate = strtoul(optarg, &endptr, 0);
            if ((baudrate <= 0) || ((endptr != (char *)0) && (*endptr != '\0'))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            }
            break;

        case 'D':
            if ((strcmp(optarg, "-") != 0) && (diminuto_fs_type(optarg) == DIMINUTO_FS_TYPE_NONE)) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            }
            device = optarg;
            break;

        case 'b':
            bufsize = strtoul(optarg, &endptr, 0);
            if ((bufsize <= 0) || ((endptr != (char *)0) && (*endptr != '\0'))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            } else if (bufsize > MAXDATAGRAM) {
                bufsize = MAXDATAGRAM;
            } else {
                /* Do nothing. */
            }
            break;

        case 'e':
            paritybit = 2;
            break;

        case 'm':
            modemcontrol = !0;
            break;

        case 'n':
            paritybit = 0;
            break;

        case 'o':
            paritybit = 1;
            break;

        case 'r':
            rtscts = !0;
            break;

        case 't':
            milliseconds = strtoul(optarg, &endptr, 0);
            if ((milliseconds < 0) || ((endptr != (char *)0) && (*endptr != '\0'))) {
                errno = EINVAL;
                diminuto_perror(optarg);
                error = true;
            } else {
                timeout = diminuto_frequency_units2ticks(milliseconds, 1000);
            }
            break;

        case 'x':
            xonxoff = !0;
            break;

        case '?':
            fprintf(stderr, "       -?                   prints this help menu and exits.\n");
            fprintf(stderr, "       -1                   sets DEVICE to one stop bits.\n");
            fprintf(stderr, "       -2                   sets DEVICE to two stop bits.\n");
            fprintf(stderr, "       -7                   sets DEVICE to seven data bits.\n");
            fprintf(stderr, "       -8                   sets DEVICE to eight data bits.\n");
            fprintf(stderr, "       -B BAUDRATE          sets the DEVICE to BAUDRATE bits per second.\n");
            fprintf(stderr, "       -D DEVICE            is the serial device name.\n");
            fprintf(stderr, "       -b BYTES             sets the buffer sizes to BYTES bytes.\n");
            fprintf(stderr, "       -e                   sets DEVICE to even parity.\n");
            fprintf(stderr, "       -m                   enables modem control.\n");
            fprintf(stderr, "       -o                   sets the DEVICE to odd parity.\n");
            fprintf(stderr, "       -r                   enables RTS/CTS.\n");
            fprintf(stderr, "       -t MILLISECONDS      sets the multiplexor timeout to MILLISECONDS.\n");
            fprintf(stderr, "       -x                   enables XON/XOFF.\n");
            exit(1);
            break;

        }
 
    }

    if (error) {
        exit(2);
    }

    /*
     * SIGNAL HANDLERS
     */

    rc = diminuto_terminator_install(!0);
    if (rc < 0) {
        error = true;
    }

    if (error) {
        exit(3);
    }

    /*
     * DEVICE
     */

    DIMINUTO_LOG_INFORMATION("%s: device %s %d %d%c%d %s %s %s\n", program, device, baudrate, databits, (paritybit == 0) ? 'n' : ((paritybit % 2) == 0) ? 'e' : 'o', stopbits, modemcontrol ? "modemcontrol" : "local", xonxoff ? "xonxoff" : "", rtscts ? "rtscts" : "");

    if ((fd = open(device, O_RDWR)) < 0) {
        error = true;
    } else if ((stream = fdopen(fd, "a+")) == (FILE *)0) {
        error = true;
    } else if (!diminuto_serial_valid(fd)) {
        /* Do nothing. */
    } else if (diminuto_serial_set(fd, baudrate, databits, paritybit, stopbits, modemcontrol, xonxoff, rtscts) < 0) {
        error = true;
    } else if (diminuto_serial_raw(fd) < 0) {
        error = true;
    } else {
        /* Do nothing. */
    }

    if (error) {
        diminuto_perror(device);
        exit(4);
    }

    dev = fileno(stream);
    std = fileno(stdin);

    /*
     * MULTIPLEXER
     */

    (void)diminuto_mux_init(&multiplexor);

    rc = diminuto_mux_register_read(&multiplexor, dev);
    if (rc < 0) {
        error = true;
    }

    rc = diminuto_mux_register_read(&multiplexor, std);
    if (rc < 0) {
        error = true;
    }

    if (error) {
        exit(7);
    }

    /*
     * BUFFERS
     */

    frame = (char *)malloc(bufsize);
    if (frame == (char *)0) {
        diminuto_perror("malloc");
        error = true;
    }

    line = (char *)malloc(bufsize);
    if (line == (char *)0) {
        diminuto_perror("malloc");
        error = true;
    }

    if (error) {
        exit(8);
    }

    /*
     * FRAMER
     */

    ff = diminuto_framer_init(&framer, frame, bufsize);
    if (ff == (diminuto_framer_t *)0) {
        error = true;
    }

    if (error) {
        exit(9);
    }

    /*
     * WORK LOOP
     */

    do {

        if (diminuto_terminator_check()) {
            DIMINUTO_LOG_NOTICE("%s: SIGTERM\n", program);
            break;
        }

        /*
         * WAITING
         */

        fds = diminuto_mux_wait(&multiplexor, timeout);
        if (fds == 0) {
            diminuto_yield();
            continue;
        } else if (fds > 0) {
            /* Do nothing. */
        } else if (errno == EINTR) {
            diminuto_yield();
            continue;
        } else {
            break;
        }

        while ((fds--) > 0) {

            fd = diminuto_mux_ready_read(&multiplexor);

            if (fd == std) {

                here = fgets(line, bufsize, stdin);
                if (here == (char *)0) {
                    done = true;
                    break;
                } else {
                    line[bufsize - 1] = '\0';
                    received = strlen(line);
                    sent = diminuto_framer_writer(stream, &framer, line, received);
                    if (sent <= 0) {
                        done = true;
                        break;
                    }
                }

            } else if (fd == dev) {

                received = diminuto_framer_reader(stream, &framer);
                if (received == 0) {
                    /* Do nothing. */
                } else if (received < 0) {
                    done = true;
                    break;
                } else {
                    diminuto_framer_reset(&framer);
                    frame[bufsize - 1] = '\0';
                    sent = fputs(frame, stdout);
                    if (sent == EOF) {
                        done = true;
                        break;
                    }
                }

            } else {

                diminuto_yield();

            }
        }

    } while (!done);

    /*
     * FINALIZATING
     */

    (void)diminuto_mux_unregister_read(&multiplexor, dev);

    (void)diminuto_mux_unregister_read(&multiplexor, std);

    diminuto_mux_fini(&multiplexor);

    (void)fclose(stream);

    free(frame);
    free(line);

    exit(0);
}