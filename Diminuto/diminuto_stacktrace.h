/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_STACKTRACE_
#define _H_COM_DIAG_DIMINUTO_STACKTRACE_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <sys/types.h>

#define DIMINUTO_STACKTRACE_SIZE    64

/**
 * Generate a stack back trace in an array of void* pointers provided
 * by the caller. Display the trace on the indicated file descriptor.
 * N.B. Read carefully backtrace(3). Due to compiler optimizations and
 * the ABIs used on some architectures, the backtrace isn't always as
 * useful as it might seem.
 * @param buffer is an array of void* pointers.
 * @param size is the number of elements in the array.
 * @param fd is the file descriptor.
 * @return the number of pointers in the backtrace; if it equals size,
 * the trace may have been truncated.
 */
extern int diminuto_stacktrace_generic(void ** buffer, size_t size, int fd);

/**
 * Generate a stack back trace and display it to standard error.
 * The back trace is generated in an array with DIMINUTO_STACKTRACE_SIZE
 * elements. If the trace is deeper than this size, it will be truncated.
 */
extern void diminuto_stacktrace(void);

#define diminuto_stacktrace3 diminuto_stacktrace_generic

#endif
