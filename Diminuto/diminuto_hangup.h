/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_HANGUP_
#define _H_COM_DIAG_DIMINUTO_HANGUP_

/**
 * @file
 *
 * Copyright 2008-2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <sys/types.h>

/**
 * Send a process a SIGHUP signal.
 * @param pid is the process identifier.
 * @return pid or <0 if an error occurred.
 */
extern pid_t diminuto_hangup(pid_t pid);

/**
 * Return true if the caller received a SIGHUP, false otherwise.
 * @return true if the caller received a SIGHUP, false otherwise.
 */
extern int diminuto_hungup(void);

/**
 * Install a SIGHUP signal handler in the caller.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_hangupable(void);

#endif
