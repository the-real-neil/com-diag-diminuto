/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_serializedsection.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include <stdint.h>
#include <errno.h>

/*
 * This really isn't an adequate test. At best it just tests that the feature
 * doesn't break a simple algorithm.
 */

static int shared = 0;
static const int LIMIT = 100;

static void * body(void * arg)
{
    static volatile diminuto_spinlock_t lock = 0;
    int done = 0;

    while (!done) {

        DIMINUTO_SERIALIZED_SECTION_BEGIN(&lock);

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d\n", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                diminuto_yield();
            }

        DIMINUTO_SERIALIZED_SECTION_END;

    }

    return (void *)arg;
}
int main(void)
{
    {
        volatile diminuto_spinlock_t lock1 = 0;
        volatile diminuto_spinlock_t lock2 = 0;
        int zero = 0;

        TEST();

        ASSERT(lock1 == zero);
        ASSERT(lock2 == zero);

        DIMINUTO_SERIALIZED_SECTION_BEGIN(&lock1);

            int zero = 0;

            ASSERT(lock1 != zero);
            ASSERT(lock2 == zero);

            DIMINUTO_SERIALIZED_SECTION_BEGIN(&lock2);

                int zero = 0;

                ASSERT(lock1 != zero);
                ASSERT(lock2 != zero);

            DIMINUTO_SERIALIZED_SECTION_END;

            ASSERT(lock1 != zero);
            ASSERT(lock2 == zero);

        DIMINUTO_SERIALIZED_SECTION_END;

        ASSERT(lock1 == zero);
        ASSERT(lock2 == zero);

        STATUS();
    }
    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        diminuto_thread_t * tp;
        void * final;

        TEST();

        ASSERT(shared == 0);

        tp = diminuto_thread_init(&odd, body);
        ASSERT(tp == &odd);
        tp = diminuto_thread_init(&even, body);
        ASSERT(tp == &even);

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);
        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        tp = diminuto_thread_fini(&odd);
        ASSERT(tp == (diminuto_thread_t *)0);
        tp = diminuto_thread_fini(&even);
        ASSERT(tp == (diminuto_thread_t *)0);

        STATUS();
    }

    EXIT();
}
