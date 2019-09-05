/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int diminuto_interrupter_debug = 0; /* Not part of the public API. */

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int signaled = 0;

int diminuto_interrupter_signal(pid_t pid)
{
    int rc = 0;

    if (kill(pid, SIGINT) < 0) {
        diminuto_perror("diminuto_interrupter_signal: kill");
        rc = -1;
    } else if (diminuto_interrupter_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_interrupter_signal: SIGINT");
    } else {
        /* Do nothing. */
    }

    return rc;
}


static void diminuto_interrupter_handler(int signum)
{
    if (signum != SIGINT) {
        /* Do nothing. */
    } else if (signaled < (~(((int)1) << ((sizeof(signaled) * 8) - 1)))) {
        signaled += 1;
    } else {
        /* Do nothing. */
    }
}

int diminuto_interrupter_check(void)
{
    int mysignaled;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGINT);

            mysignaled = signaled;
            signaled = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;
    DIMINUTO_CRITICAL_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (diminuto_interrupter_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_interrupter_check: SIGINT");
    } else {
        /* Do nothing. */
    }

    return mysignaled;
}

int diminuto_interrupter_install(int restart)
{
    int rc = 0;
    struct sigaction interrupter = { 0 };

    interrupter.sa_handler = diminuto_interrupter_handler;
    interrupter.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGINT, &interrupter, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_interrupter_install: sigaction");
        rc = -1;
    } else if (diminuto_interrupter_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_interrupter_install: SIGINT");
    } else {
        /* Do nothing. */
    }

    signaled = 0;

    return rc;
}
