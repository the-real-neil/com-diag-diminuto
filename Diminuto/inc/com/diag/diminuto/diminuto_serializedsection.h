/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_
#define _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_

/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements macros to to bracket sections serialized with spin locks.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Serialized Section feature uses the Barrier feature with bracketing
 * macros to implement a code section serialized between multiple processors.
 * EXPERIMENTAL.
 */

#include "com/diag/diminuto/diminuto_barrier.h"

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)

/**
 * @def DIMINUTO_SERIALIZED_SECTION_BEGIN
 * Start a code block that is serialized using a spin lock and busy waiting by
 * blocking on an integer lock variable specified by the caller as a pointer to
 * a volatile variable of type int but be provided as the argument @a _INTP_.
 * Note that the while loop iterates as long as the builtin returns !0 (true,
 * or already locked by someone else), and exits once the builtin returns
 * 0 (false, or unlocked), indicating that the lock has transitioned to !0
 * (true, or locked on the caller's behalf).
 */
#           define DIMINUTO_SERIALIZED_SECTION_BEGIN(_INTP_) \
                do { \
                    volatile diminuto_spinlock_t * diminuto_serialized_section_spinlock_p; \
                    diminuto_serialized_section_spinlock_p = (_INTP_); \
                    while (__sync_lock_test_and_set(diminuto_serialized_section_spinlock_p, !0)); \
                    do { \
                        ((void)0)

/**
 * @def DIMINUTO_SERIALIZED_SECTION_END
 * End a code block that was serialized using a spin lock and busy waiting by
 * releasing the integer lock variable specified at the beginning of the block.
 */
#           define DIMINUTO_SERIALIZED_SECTION_END \
                    } while (0); \
                    __sync_lock_release(diminuto_serialized_section_spinlock_p); \
                } while (0)

#       endif
#   endif
#endif

#if defined(DIMINUTO_SERIALIZED_SECTION_BEGIN) && defined(DIMINUTO_SERIALIZED_SECTION_END)
#   define COM_DIAG_DIMINUTO_SERIALIZED_SECTION (!0)
#else
#   undef COM_DIAG_DIMINUTO_SERIALIZED_SECTION
#   warning DIMINUTO_SERIALISED_SECTION_BEGIN and DIMINUTO_SERIALIZED_SECTION_END are no-ops!
#   define DIMINUTO_SERIALIZED_SECTION_BEGIN(_INTP_) \
    do { \
        ((void)0)
#   define DIMINUTO_SERIALIZED_SECTION_END \
    } while (0)
#endif

#endif
