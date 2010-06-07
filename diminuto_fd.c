/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_fd.h"
#include "diminuto_log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int diminuto_acquire(int fd, const char * device, int flags, mode_t mode)
{
    if (fd >= 0) {
        /* Do nothing: already open. */
    } else if ((fd = open(device, flags, mode)) < 0) {
        diminuto_perror(device);
    } else {
        /* Do nothing: success. */
    }

    return fd;
}

int diminuto_relinquish(int fd, const char * device)
{
    if (fd < 0) {
        /* Do nothing: not open. */
    } else if (close(fd) < 0) {
        diminuto_perror(device ? device : "close");
    } else {
        fd = -1;
    }

    return fd;
}
