/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_pipe.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

int diminuto_pipe_debug = 0; /* Not part of the public API. */

static int signaled = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int diminuto_pipe_signal(pid_t pid)
{
    int rc = 0;

    if (kill(pid, SIGPIPE) < 0) {
        diminuto_perror("diminuto_pipe_signal: kill");
        rc = -1;
    } else if (diminuto_pipe_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_pipe_signal: SIGPIPE");
    } else {
        /* Do nothing. */
    }

    return rc;
}

static void diminuto_pipe_handler(int signum)
{
    if (signum != SIGPIPE) {
    	/* Do nothing. */
    } else if (signaled < (~(((int)1) << ((sizeof(signaled) * 8) - 1)))) {
        signaled += 1;
    } else {
    	/* Do nothing. */
    }
}

int diminuto_pipe_check(void)
{
    int mysignaled;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
    	DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGPIPE);

        	mysignaled = signaled;
        	signaled = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;
    DIMINUTO_CRITICAL_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (diminuto_pipe_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_pipe_check: SIGPIPE");
    } else {
        /* Do nothing. */
    }

    return mysignaled;
}

int diminuto_pipe_install(int restart)
{
    int rc = 0;
    struct sigaction alarm = { 0 };

    alarm.sa_handler = diminuto_pipe_handler;
    alarm.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGPIPE, &alarm, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_pipe_install: sigaction");
        rc = -1;
    } else if (diminuto_pipe_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_pipe_install: SIGPIPE");
    } else {
        /* Do nothing. */
    }

    return rc;
}