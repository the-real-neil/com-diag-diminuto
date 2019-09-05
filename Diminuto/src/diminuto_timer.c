/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "../src/diminuto_timer.h"
#include <pthread.h>

diminuto_sticks_t diminuto_timer_generic(int * initializedp, timer_t * timeridp, struct sigevent * eventp, diminuto_ticks_t ticks, int periodic)
{
    diminuto_sticks_t sticks = 0;
    struct itimerspec timer = { 0 };
    struct itimerspec remaining = { 0 };

    /*
     * If we don't have a timer, and are not deleting it, create the timer.
     */

    if (*initializedp) {
        /* Do nothing: already have the timer. */
    } else if (ticks == 0) {
        /* Do nothing: deleting the timer. */
    } else if (timer_create(CLOCK_MONOTONIC, eventp, timeridp) < 0) {
        diminuto_perror("diminuto_timer_generic: timer_create");
        sticks = (diminuto_sticks_t)-1;
    } else {
        *initializedp = !0;
    }

    if (sticks < 0) {
        return sticks;
    }

    /*
     * If we have a timer, and are deleting it, delete the timer.
     */

    if (ticks != 0) {
        /* Do nothing: not deleting the timer. */
    } else if (!(*initializedp)) {
        /* Do nothing: do not have the timer. */
    } else if (timer_gettime(*timeridp, &remaining) < 0) {
        diminuto_perror("diminuto_timer_generic: timer_gettime");
        sticks = (diminuto_sticks_t)-1;
     } else if (timer_delete(*timeridp) < 0) {
        diminuto_perror("diminuto_timer_generic: timer_delete");
        sticks = (diminuto_sticks_t)-1;
    } else {
        sticks = diminuto_frequency_seconds2ticks(remaining.it_value.tv_sec, remaining.it_value.tv_nsec, diminuto_timer_frequency());
        *initializedp = 0;
    }

    if ((sticks < 0) || (!(*initializedp))) {
        return sticks;
    }

    /*
     * If we are not deleting the timer, then set the timer.
     */

    timer.it_value.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
    timer.it_value.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_timer_frequency());

    if (periodic) {
        timer.it_interval = timer.it_value;
    } else {
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = 0;
    }

    remaining = timer;

    if (timer_settime(*timeridp, 0, &timer, &remaining) < 0) {
        diminuto_perror("diminuto_timer_generic: timer_settime");
        sticks = (diminuto_sticks_t)-1;
    } else {
        sticks = diminuto_frequency_seconds2ticks(remaining.it_value.tv_sec, remaining.it_value.tv_nsec, diminuto_timer_frequency());
    }

    return sticks;
}

/*******************************************************************************
 * As much as possible the code below mimics the semantics of setitimer(2)
 * but uses a monotonic POSIX real-time timer to do so. The timer is a
 * singleton and sends a SIGALRM to the calling process, just like setitimer(2).
 ******************************************************************************/

static timer_t timerid = 0;
static int initialized = 0;

/*
 * Exposed just for unit testing.
 */
timer_t diminuto_timer_singleton_get(void)
{
    return initialized ? timerid : (timer_t)-1;
}

diminuto_sticks_t diminuto_timer_singleton(diminuto_ticks_t ticks, int periodic)
{
    diminuto_sticks_t sticks = 0;
    struct sigevent event = { 0 };
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGALRM;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

        sticks = diminuto_timer_generic(&initialized, &timerid, &event, ticks, periodic);

    DIMINUTO_CRITICAL_SECTION_END;

    return sticks;
}
