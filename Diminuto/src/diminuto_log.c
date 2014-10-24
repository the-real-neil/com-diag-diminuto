/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>

diminuto_log_mask_t diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;
const char * diminuto_log_ident = DIMINUTO_LOG_IDENT_DEFAULT;
int diminuto_log_option = DIMINUTO_LOG_OPTION_DEFAULT;
int diminuto_log_facility = DIMINUTO_LOG_FACILITY_DEFAULT;
int diminuto_log_stream = DIMINUTO_LOG_STREAM_DEFAULT;

static const char levels[] = "01234567";
static uint8_t initialized = 0;

/*******************************************************************************
 * Base Functions
 *****************************************************************************/

void diminuto_log_open(const char * name)
{
    if (name != (const char *)0) {
        diminuto_log_ident = name;
    }
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
    if (!initialized) {
        openlog(diminuto_log_ident, diminuto_log_option, diminuto_log_facility);
        initialized = !0;
    }
#endif
}

void diminuto_log_close(void)
{
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
    if (initialized) {
        closelog();
        initialized = 0;
    }
#endif
}

void diminuto_log_vsyslog(int priority, const char * format, va_list ap)
{
    diminuto_log_open((const char *)0);
#if !defined(COM_DIAG_DIMINUTO_PLATFORM_BIONIC)
    vsyslog(priority, format, ap);
#else
    __android_log_vprint(priority, diminuto_log_ident, format, ap);
#endif
}

void diminuto_log_vwrite(int fd, int priority, const char * format, va_list ap)
{
    diminuto_ticks_t now;
    int year, month, day, hour, minute, second, nanosecond;
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
    int rc;
    char * pointer = buffer;
    size_t space = sizeof(buffer);
    size_t total = 0;

    now = diminuto_time_clock();
    diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &nanosecond);

    rc = snprintf(pointer, space, "%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6dZ %c [%d] ", year, month, day, hour, minute, second, nanosecond / 1000, levels[priority & 0x7], getpid());
    if (rc < 0) {
        rc = 0;
    } else if (rc >= space) {
        rc = space - 1;
    } else {
        /* Do nothing. */
    }
    pointer += rc;
    space -= rc;
    total += rc;

    rc = vsnprintf(pointer, space, format, ap);
    if (rc < 0) {
        rc = 0;
    } else if (rc >= space) {
        rc = space - 1;
    } else {
        /* Do nothing. */
    }
    pointer += rc;
    space -= rc;
    total += rc;

    if (space <= 1) {
    	buffer[total - 1] = '\n';
    } else if (buffer[total - 1] != '\n') {
    	buffer[total++] = '\n';
    	buffer[total] = '\0';
    } else {
    	/* Do nothing. */
    }

    rc = write(fd, buffer, total);
    if (rc < 0) {
        /* Do nothing: what are we doing to do, log an error message? */
    } else if (rc == 0) {
        /* Do nothing: far end closed, which is the caller's problem. */
    } else if (rc < total) {
        /* Do nothing: accept partial write and proceed. */
    } else {
        /* Do nothing: nominal. */
    }
}

/*******************************************************************************
 * Routing Functions
 *****************************************************************************/

void diminuto_log_vlog(int priority, const char * format, va_list ap)
{
    if (getppid() == 1) {
        diminuto_log_vsyslog(priority, format, ap);
    } else {
        diminuto_log_vwrite(diminuto_log_stream, priority, format, ap);
    }
}

/*******************************************************************************
 * Variadic Functions
 *****************************************************************************/

void diminuto_log_syslog(int priority, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log_vsyslog(priority, format, ap);
    va_end(ap);
}

void diminuto_log_write(int fd, int priority, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log_vwrite(fd, priority, format, ap);
    va_end(ap);
}

void diminuto_log_log(int priority, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log_vlog(priority, format, ap);
    va_end(ap);
}

void diminuto_log_emit(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    diminuto_log_vlog(DIMINUTO_LOG_PRIORITY_DEFAULT, format, ap);
    va_end(ap);
}

/*******************************************************************************
 * Error Number Functions
 *****************************************************************************/

void diminuto_serror(const char * s)
{
    int save;
    save = errno;
    diminuto_log_syslog(DIMINUTO_LOG_PRIORITY_ERROR, "%s: %s\n", s, strerror(errno));
    errno = save;
}

void diminuto_perror(const char * s)
{
    int save;
    save = errno;
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, "%s: %s\n", s, strerror(errno));
    errno = save;
}
