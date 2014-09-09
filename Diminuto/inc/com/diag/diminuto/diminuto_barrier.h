/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BARRIER_
#define _H_COM_DIAG_DIMINUTO_BARRIER_

/**
 * @file
 *
 * Copyright 2008-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * This is largely experimental.
 */

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)
#           define diminuto_barrier() __sync_synchronize()
#       endif
#   endif
#endif

#if !defined(diminuto_barrier)
#   define diminuto_barrier() ((void)0)
#   warning diminuto_barrier() is a no-op!
#endif

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)
#           define diminuto_acquire() do { int _diminuto_acquire_lock_; __sync_lock_test_and_set(&_diminuto_acquire_lock_, 1); } while (0)
#       endif
#   endif
#endif

#if !defined(diminuto_acquire)
#   define diminuto_acquire() diminuto_barrier()
#   warning diminuto_acquire() is a synonym for diminuto_barrier()!
#endif

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)
#           define diminuto_release() do { int _diminuto_release_lock_ = 1; __sync_lock_release(&_diminuto_release_lock_); } while (0)
#       endif
#   endif
#endif

#if !defined(diminuto_release)
#   define diminuto_release() diminuto_barrier()
#   warning diminuto_release() is a synonym for diminuto_barrier()!
#endif

#endif
