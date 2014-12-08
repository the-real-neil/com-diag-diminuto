/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_throttle.h"

diminuto_ticks_t diminuto_throttle_request(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    diminuto_ticks_t delay = 0;
    diminuto_ticks_t elapsed;

    elapsed = (throttlep->now = now) - throttlep->then;
    if (throttlep->expected <= elapsed) {
        throttlep->actual = 0;
        throttlep->alarming = 0;
    } else if ((throttlep->actual = throttlep->expected - elapsed) <= throttlep->limit) {
        throttlep->alarming = 0;
    } else {
        delay = throttlep->actual - throttlep->limit;
        throttlep->alarming = !0;
    }

    return delay;
}

int diminuto_throttle_commitn(diminuto_throttle_t * throttlep, size_t events)
{
    throttlep->then = throttlep->now;
    throttlep->expected = throttlep->actual + (events * throttlep->increment);
    throttlep->alarmed = throttlep->alarming;
    return throttlep->alarmed;
}

int diminuto_throttle_admitn(diminuto_throttle_t * throttlep, diminuto_ticks_t now, size_t events)
{
    throttlep->now = now;
    throttlep->then = now;
    throttlep->expected = events * throttlep->increment;
    throttlep->actual = 0;
    throttlep->alarming = 0;
    throttlep->alarmed = 0;
    return throttlep->alarmed;
}
