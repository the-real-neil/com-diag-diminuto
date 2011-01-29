/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2011 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * mmdrivertool [ -d ] [ -U DEVICE ] [ -[1|2|4|8] ADDRESS ] [ -[s|S|c|C|w] NUMBER | -r ] [ -u USECONDS ] [ ... ]
 *
 * EXAMPLES
 *
 * mmdrivertool -U /dev/mmdriver -4 0xfffff400UL -s 0x1 -u 1000 -c 0x1
 *
 * ABSTRACT
 *
 * Allows manipulation of bytes, shorts, longs, and long longs within a fixed
 * real memory address space through the generic memory map driver.
 */

#include "diminuto_number.h"
#include "diminuto_delay.h"
#include "diminuto_mmdriver.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static const char * program = "mmdrivertool";

static const char * number(const char * string, uint64_t * valuep)
{
    const char * result;

    if (*(result = diminuto_number(string, valuep)) != '\0') {
        perror(string);
    }

    return result;
}

static int control(int fd, int request, diminuto_mmdriver_op * opp)
{
    int rc = -1;

    if (fd < 0) {
        /* Do nothing: not open. */
    } else if ((rc = ioctl(fd, request, opp)) < 0) {
        perror("ioctl");
    } else {
        /* Do nothing: success. */
    }

    return rc;
}

static void usage(void)
{
    fprintf(stderr, "usage: %s [ -d ] [ -U DEVICE ] [ -[1|2|4|8] OFFSET ] [ -m NUMBER ] [ -r | -[s|S|c|C|w] NUMBER ] [ -u USECONDS ] [ -t | -f ] [ ... ]\n", program);
    fprintf(stderr, "       -1 OFFSET   Use byte at OFFSET\n");
    fprintf(stderr, "       -2 OFFSET   Use halfword at OFFSET\n");
    fprintf(stderr, "       -4 OFFSET   Use word at OFFSET\n");
    fprintf(stderr, "       -8 OFFSET   Use doubleword at OFFSET\n");
    fprintf(stderr, "       -C NUMBER   Clear 1<<NUMBER mask at OFFSET\n");
    fprintf(stderr, "       -S NUMBER   Set 1<<NUMBER mask at OFFSET\n");
    fprintf(stderr, "       -U DEVICE   Use DEVICE instead of %s\n", DIMINUTO_MMDRIVER_NODE);
    fprintf(stderr, "       -c NUMBER   Clear NUMBER mask at OFFSET\n");
    fprintf(stderr, "       -d          Enable debug mode\n");
    fprintf(stderr, "       -f          Proceed if the last result was 0\n");
    fprintf(stderr, "       -m NUMBER   Mask at OFFSET with ~NUMBER prior to any set\n");
    fprintf(stderr, "       -r          Read OFFSET\n");
    fprintf(stderr, "       -s NUMBER   Set NUMBER mask at OFFSET\n");
    fprintf(stderr, "       -t          Proceed if the last result was !0\n");
    fprintf(stderr, "       -u USECONDS Sleep for USECONDS microseconds\n");
    fprintf(stderr, "       -w NUMBER   Write NUMBER to OFFSET\n");
    fprintf(stderr, "       -?          Print menu\n");
}

int main(int argc, char * argv[])
{
    const char * device = DIMINUTO_MMDRIVER_NODE;
    diminuto_unsigned_t value = 0;
    diminuto_unsigned_t mask = 0;
    static const uint64_t bit = 1;
    int done = 0;
    int error = 0;
    int fd = -1;
    int debug = 0;
    int opt;
    extern char * optarg;
    diminuto_mmdriver_op op;

    program = strrchr(argv[0], '/');
    program = (program == (char *)0) ? argv[0] : program + 1;

    op.offset = 0;
    op.width = THIRTYTWO;
    op.datum.thirtytwo = 0;

    while ((opt = getopt(argc, argv, "1:2:4:8:C:S:U:c:dfm:rs:tu:w:?")) >= 0) {

        switch (opt) {

        case '1':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
                op.offset = value;
                op.width = EIGHT;
            }
            break;

        case '2':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
                op.offset = value;
                op.width = SIXTEEN;
            }
            break;

        case '4':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
                op.offset = value;
                op.width = THIRTYTWO;
            }
            break;

        case '8':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
                op.offset = value;
                op.width = SIXTYFOUR;
            }
            break;

        case 'U':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            fd = diminuto_fd_relinquish(fd, device);
            device = optarg;
            break;

        case 'C':
        case 'c':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
            	if (opt == 'C') { value = bit << value; }
                switch (op.width) {
                case EIGHT:		op.datum.eight		= value; break;
                case SIXTEEN:	op.datum.sixteen	= value; break;
                case THIRTYTWO:	op.datum.thirtytwo	= value; break;
                case SIXTYFOUR:	op.datum.sixtyfour	= value; break;
                }
                if (!(error = ((fd = diminuto_fd_acquire(fd, device, O_RDWR, 0)) < 0))) {
                    error = control(fd, DIMINUTO_MMDRIVER_CLEAR, &op) < 0;
                }
            }
            break;

        case 'd':
            debug = !0;
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            break;

        case 'f':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            switch (op.width) {
            case EIGHT:		done = op.datum.eight		!= 0;	break;
            case SIXTEEN:	done = op.datum.sixteen		!= 0;	break;
            case THIRTYTWO:	done = op.datum.thirtytwo	!= 0;	break;
            case SIXTYFOUR:	done = op.datum.sixtyfour	!= 0;	break;
            }
            break;

		case 'm':
			if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
			if (!(error = (*number(optarg, &value) != '\0'))) {
				mask = value;
			}
			break;

        case 'r':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (!(error = ((fd = diminuto_fd_acquire(fd, device, O_RDWR, 0)) < 0))) {
                if (!(error = control(fd, DIMINUTO_MMDRIVER_READ, &op) < 0)) {
                    switch (op.width) {
                    case EIGHT:		printf("%u\n",		op.datum.eight);		break;
                    case SIXTEEN:	printf("%u\n",		op.datum.sixteen);		break;
                    case THIRTYTWO:	printf("%u\n",		op.datum.thirtytwo);	break;
                    case SIXTYFOUR:	printf("%llu\n",	op.datum.sixtyfour);	break;
                    }
                }
            }
            break;

        case 'S':
        case 's':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
            	if (opt == 'S') { value = bit << value; }
                switch (op.width) {
                case EIGHT:		op.datum.eight		= value; op.mask.eight		= mask; break;
                case SIXTEEN:	op.datum.sixteen	= value; op.mask.sixteen	= mask; break;
                case THIRTYTWO:	op.datum.thirtytwo	= value; op.mask.thirtytwo	= mask; break;
                case SIXTYFOUR:	op.datum.sixtyfour	= value; op.mask.sixtyfour	= mask; break;
                }
                if (!(error = ((fd = diminuto_fd_acquire(fd, device, O_RDWR, 0)) < 0))) {
                    error = control(fd, DIMINUTO_MMDRIVER_SET, &op) < 0;
                }
            }
            break;

        case 't':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            switch (op.width) {
            case EIGHT:		done = op.datum.eight		== 0; break;
            case SIXTEEN:	done = op.datum.sixteen		== 0; break;
            case THIRTYTWO:	done = op.datum.thirtytwo	== 0; break;
            case SIXTYFOUR:	done = op.datum.sixtyfour	== 0; break;
            }
            break;

        case 'u':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
                diminuto_delay(value, 0);
            }
            break;

        case 'w':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (!(error = (*number(optarg, &value) != '\0'))) {
                switch (op.width) {
                case EIGHT:		op.datum.eight		= value; break;
                case SIXTEEN:	op.datum.sixteen	= value; break;
                case THIRTYTWO:	op.datum.thirtytwo	= value; break;
                case SIXTYFOUR:	op.datum.sixtyfour	= value; break;
                }
                if (!(error = ((fd = diminuto_fd_acquire(fd, device, O_RDWR, 0)) < 0))) {
                    error = control(fd, DIMINUTO_MMDRIVER_WRITE, &op) < 0;
                }
            }
            break;

        case '?':
            usage();
            break;

        default:
            error = !0;
            break;

        }

        if (error) {
            break;
        }

        if (done) {
            break;
        }

    }

    fd = diminuto_fd_relinquish(fd, device);

    if (error) {
        usage();
        return 1;
    }

    return 0;
}
