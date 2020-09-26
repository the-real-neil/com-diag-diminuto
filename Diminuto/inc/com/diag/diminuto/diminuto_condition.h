/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CONDITION_
#define _H_COM_DIAG_DIMINUTO_CONDITION_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <pthread.h>
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_mutex.h"

static const diminuto_ticks_t DIMINUTO_CONDITION_INFINITY = ~(diminuto_ticks_t)0;

typedef struct DiminutoCondition {
    diminuto_mutex_t mutex;
    pthread_condition_t condition;
} diminuto_mutex_t;

#define DIMINUTO_CONDITION_INITIALIZER \
    { \
        DIMINUTO_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
    }

extern diminuto_condition_t * diminuto_condition_init(diminuto_condition_t * cp);

static inline int diminuto_condition_lock(diminuto_condition_t * mp)
{
    return diminuto_mutex_lock(&(mp->mutex));
}

static inline int diminuto_condition_lock_try(diminuto_condition_t * mp)
{
    return diminuto_mutex_lock_try(&(mp->mutex));
}

static inline int diminuto_condition_unlock(diminuto_condition_t * mp)
{
    return diminuto_mutex_unlock(&(mp->mutex));
}

extern int diminuto_condition_wait_try(diminuto_condition_t * cp, diminuto_ticks_t timeout);

static inline int diminuto_condition_wait(diminuto_condition_t * cp)
{
    return diminuto_condition_wait_try(cp, DIMINUTO_CONDITION_INFINITY);
}

extern int diminuto_condition_signal(diminuto_condition_t * cp);

extern diminuto_condition_t * diminuto_condition_fini(diminuto_condition_t * cp);

#endif
