/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNITTEST_
#define _H_COM_DIAG_DIMINUTO_UNITTEST_

/**
 * @file
 *
 * Copyright 2009-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is a dirt simple unit testing framework for C programs.
 * If you are using C++ (or even if you are using C but are
 * comfortable with C++) you would be better off using one of the
 * established C++ unit test frameworks. I particularly like Google
 * Test (a.k.a. gtest). But developers I know and trust have made good
 * use of CxxUnit, CppUnit, and CxxUnitLite. (If you are using Java,
 * go directly to JUnit.)
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include <stdlib.h>
#include <stdio.h>

static int diminuto_unittest_errors = 0;

/**
 * @def SETLOGMASK()
 * Set the log mask from the string value of the environmental variable
 * DIMINUTO_LOG_MASK. The value of the environmental variable can be in decimal,
 * hexadecimal, or octal in strtoul() format.
 */
#define SETLOGMASK() \
	do { \
		const char * diminuto_unittest_log_mask; \
		if ((diminuto_unittest_log_mask = getenv("COM_DIAG_DIMINUTO_LOG_MASK")) != (const char *)0) { \
			DIMINUTO_LOG_MASK = strtoul(diminuto_unittest_log_mask, (char **)0, 0); \
		} \
	} while (0)

/**
 * @def CHECKPOINT(...)
 * Emit a notice message with the current translation unit file and line number
 * and an optional manifest string argument.
 */
#define CHECKPOINT(...) \
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE __VA_ARGS__)

/**
 * @def EXIT()
 * Exit the calling process with a zero exit code if there are no errors,
 * or a non-zero exit code if there are.
 */
#define EXIT() \
	do { \
		fflush(stdout); \
		fflush(stderr); \
		exit(diminuto_unittest_errors > 255 ? 255 : diminuto_unittest_errors); \
	} while (0)

/**
 * @def ADVISE(_COND_)
 * Log a notice message if the specified condition @a _COND_ is not true.
 */
#define ADVISE(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "!ADVISE(" #_COND_ ")!\n", __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * @def EXPECT(_COND_)
 * Log a warning message if the specified condition @a _COND_ is not true
 * and increment the error counter.
 */
#define EXPECT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, DIMINUTO_LOG_HERE "!EXPECT(" #_COND_ ")!\n", __FILE__, __LINE__); \
            ++diminuto_unittest_errors; \
        } \
    } while (0)

/**
 * @def ASSERT(_COND_)
 * Log a warning message if the specified condition @a _COND_ is not true,
 * increment the error counter, and exit immediately.
 */
#define ASSERT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE "!ASSERT(" #_COND_ ")!\n", __FILE__, __LINE__); \
            ++diminuto_unittest_errors; \
            EXIT(); \
        } \
    } while (0)

/**
 * @def FATAL(...)
 * Emit a error message and exit the calling process with a non-zero exit code.
 */
#define FATAL(...) \
        do { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE __VA_ARGS__); \
            ++diminuto_unittest_errors; \
            EXIT(); \
	    } while (0)

/**
 * @def PANIC(...)
 * Emit a error message and try to dump core.
 */
#define PANIC(...) \
        do { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE __VA_ARGS__); \
            fflush(stdout); \
            fflush(stderr); \
            diminuto_core_enable(); \
            FATAL("0x%x", *((volatile char *)0)); \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE __VA_ARGS__); \
            EXIT(); \
        } while (0)

#endif
