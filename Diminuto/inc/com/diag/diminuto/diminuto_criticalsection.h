/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CRITICALSECTION_
#define _H_COM_DIAG_DIMINUTO_CRITICALSECTION_

/**
 * @file
 *
 * Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Critical Section feature implements a critical section of
 * code using a POSIX pthread mutex. The mutex is automatically
 * unlocked if the thread cleanup action is invoked.
 */

#include <pthread.h>

extern void diminuto_criticalsection_cleanup(void * voidp);

/**
 * @def DIMINUTO_CRITICAL_SECTION_BEGIN
 * Begin a code section that is serialized using a pthread mutex specified by
 * the caller as a pointer in the argument @a _MUTEX_P by locking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_BEGIN(_MUTEXP_) \
    do { \
        pthread_mutex_t * _diminuto_criticalsection_mutexp_ = (_MUTEXP_); \
        int _diminuto_criticalsection_rc_ = -1; \
        _diminuto_criticalsection_rc_ = pthread_mutex_lock(_diminuto_criticalsection_mutexp_); \
        if (_diminuto_criticalsection_rc_ == 0) { \
            pthread_cleanup_push(diminuto_criticalsection_cleanup, _diminuto_criticalsection_mutexp_); \
            do { \
                (void)0

/**
 * @def DIMINUTO_CRITICAL_SECTION_TRY
 * Conditionally begin a code section that is serialized using a pthread
 * mutex specified by the caller as a pointer in the argument @a _MUTEX_P
 * by locking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_TRY(_MUTEXP_) \
    do { \
        pthread_mutex_t * _diminuto_criticalsection_mutexp_ = (_MUTEXP_); \
        int _diminuto_criticalsection_rc_ = -1; \
        _diminuto_criticalsection_rc_ = pthread_mutex_trylock(_diminuto_criticalsection_mutexp_); \
        if (_diminuto_criticalsection_rc_ == 0) { \
            pthread_cleanup_push(diminuto_criticalsection_cleanup, _diminuto_criticalsection_mutexp_); \
            do { \
                (void)0

/**
 * @def DIMINUTO_CRITICAL_SECTION_END
 * End a code section that was serialized using the pthread mutex specified at
 * the beginning of the block by unlocking the mutex.
 */
#define DIMINUTO_CRITICAL_SECTION_END \
            } while (0); \
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

#endif
