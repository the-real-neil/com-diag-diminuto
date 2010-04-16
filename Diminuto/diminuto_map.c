/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_map.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

void * diminuto_map(uintptr_t start, size_t length, void ** startp, size_t * lengthp)
{
    void * result = (void *)0;
    int fd = -1;
    void * base = (void *)0;
    size_t size = 0;
    int pagesize;
    size_t modulo;
    off_t offset;

    do {

        pagesize = getpagesize();
        modulo = start % pagesize;
        offset = start - modulo;
        size = length + modulo;

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
            diminuto_perror("diminuto_map: open");
            break;
        }

        if ((base = mmap(0, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, offset)) == (void *)-1) {
            diminuto_perror("diminuto_map: mmap");
            break;
        }

        result = (unsigned char *)base + modulo;
        *startp = base;
        *lengthp = size;

    } while (0);

    if (fd < 0) {
        /* Do nothing. */
    } else if ((fd = close(fd)) < 0) {
        diminuto_perror("diminuto_map: close");
    }

    return result;
}

int diminuto_unmap(void * start, size_t length)
{
    int rc;

    if ((rc = munmap(start, length)) < 0) {
        diminuto_perror("diminuto_unmap: munmap");
    }

    return rc;
}

int diminuto_map_minimum(uintptr_t minimum)
{
    int fd = -1;
    FILE * fp = (FILE *)0;
    int result = 0;
    unsigned long value;

    do {

        if ((getuid() != 0) && (geteuid() != 0)) {
            result = -1;
            errno = EPERM;
            diminuto_perror("diminuto_map_minimum: open");
            break;
        }

        if ((fd = open("/proc/sys/vm/mmap_min_addr", O_WRONLY)) < 0) {
            break;
        }

        if ((fp = fdopen(fd, "w")) == (FILE *)0) {
            result = -2;
            diminuto_perror("diminuto_map_minimum: fdopen");
            break;
        }

        value = minimum;

        if (fprintf(fp, "%lu", value) < 0) {
            result = -3;
            diminuto_perror("diminuto_map_minimum: fprintf");
            break;
        }

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) == EOF) {
        result = -4;
        diminuto_perror("diminuto_map_minimum: fclose");
    } else {
        fp = (FILE *)0;
        fd = -1;
    }

    if (fd < 0) {
        /* Do nothing. */
    } else if (close(fd) < 0) {
        result = -5;
        diminuto_perror("diminuto_map_minimum: close");
    } else {
        fd = -1;
    }

    return result;
}
