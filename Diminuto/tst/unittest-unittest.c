/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2009-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Unit Test feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Unit Test feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void * function(void * arg)
{
    EXPECT(arg != (void *)0);
    TEXIT();
}

int main(int argc, char ** argv)
{
    int status;

    SETLOGMASK();

    {
        TEST();

        CHECKPOINT();
        COMMENT();
        EXPECT(!0);
        ASSERT(!0);
        ADVISE(!0);

        STATUS();
    }

    {
        pthread_t thread;
        void * status;

        TEST("Thread 1");

        ASSERT(pthread_create(&thread, (const pthread_attr_t *)0, function, (void *)(intptr_t)0) == 0);
        status = (void *)(intptr_t)256;
        ASSERT(pthread_join(thread, &status) == 0);
        EXPECT(status == (void *)(intptr_t)0);

        STATUS();
    }

    {
        pthread_t thread;
        void * status;

        TEST("Thread 2");

        diminuto_unittest_total = diminuto_unittest_errors = 0;
        ASSERT(pthread_create(&thread, (const pthread_attr_t *)0, function, (void *)(intptr_t)255) == 0);
        status = (void *)(intptr_t)256;
        ASSERT(pthread_join(thread, &status) == 0);
        EXPECT(status == (void *)(intptr_t)1);
        diminuto_unittest_total = diminuto_unittest_errors = 0;

        STATUS();
    }

    {
        TEST("Test");

        CHECKPOINT("%s", argv[0]);
        COMMENT("%s", argv[0]);
    
        if (fork() == 0) {
            COMMENT();
            ADVISE(0);
            EXPECT(0);
            ASSERT(0);
            STATUS("Status");
            EXIT("Exit");
            exit(0);
        }

        if (fork() == 0) {
            FATAL("Fatal");
            FAILURE();
            EXIT();
            exit(0);
        }

        if (fork() == 0) {
            FATAL();
            FAILURE();
            EXIT();
            exit(0);
        }

        if (fork() == 0) {
            PANIC("Panic");
            FAILURE();
            EXIT();
            exit(0);
        }

        if (fork() == 0) {
            PANIC();
            FAILURE();
            EXIT();
            exit(0);
        }

        if (fork() == 0) {
            FAILURE();
            STATUS();
            EXIT();
            exit(0);
        }

        if (fork() == 0) {
            FAILURE("Failure");
            STATUS("Status");
            EXIT("Exit");
            exit(0);
        }

        while (!0) {
            status = 0;
            if (wait(&status) < 0) {
                break;
            }
            EXPECT(status != 0);
        }

        STATUS();
    }

    EXIT();
}
