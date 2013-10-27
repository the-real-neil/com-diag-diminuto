/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIME_
#define _H_COM_DIAG_DIMINUTO_TIME_

/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Important safety tip: although UNIX (and POSIX) professes to keep the time
 * in Coordinated Universal Time (UTC) it doesn't account for periodic leap
 * seconds. This means it isn't strictly UTC, and worse, the time may appear
 * discontinuous if and when the system clock is manually adjusted. This has
 * no effect on the monotonically increasing clock, which is why that's the
 * clock you should use to measure the passage of time. So UNIX doesn't quite
 * do UTC, but it also doesn't quite do International Atomic Time (IAT) either,
 * which also doesn't do leap seconds. (I think GPS time falls into this
 * category as well.) Also: daylight saving time is just an abomination.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Return the resolution of the Diminuto time units in ticks per second (Hertz).
 * @return the resolution in ticks per second.
 */
extern diminuto_ticks_t diminuto_time_resolution(void);

/**
 * Return the system clock time in Coordinated Universal Time (UTC) in
 * ticks since the Epoch (shown here in ISO8601 format)
 * 1970-01-01T00:00:00+0000.
 * @return the number of ticks elapsed since the Epoch or ~0ULL with
 * errno set if an error occurred.
 */
extern diminuto_ticks_t diminuto_time_clock(void);

/**
 * Return the elapsed time in ticks of a monotonically increasing
 * clock. This value is immune to time adjustments in the system clock and
 * hence is usable to measure duration and elapsed time.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_ticks_t diminuto_time_elapsed(void);

/**
 * Return the CPU time in ticks for the calling process.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_ticks_t diminuto_time_process(void);

/**
 * Return the CPU time in ticks for the calling thread.
 * @return the number of ticks elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_ticks_t diminuto_time_thread(void);

/**
 * Return the number of ticks the local time zone is offset _east_ of
 * Coordinated Universal Time (UTC). This will be a _negative_ number that
 * indicates how much earlier the local time zone is from UTC. This offset
 * does _not_ account for Daylight Saving Time (DST) if it is used and in
 * effect. (Since the offset of the time zone doesn't shift over the span
 * of a year like DST does, the argument to this function can probably always
 * be zero.)
 * @param ticks is the number of ticks since the Epoch.
 * @return the number of ticks east of UTC.
 */
extern diminuto_ticks_t diminuto_time_timezone(diminuto_ticks_t ticks);

/**
 * Return the number of ticks the local time zone is offset _west_
 * because of Daylight Saving Time (DST). It DST is not used for local time,
 * or is not in effect for the specified time, then this value will be zero,
 * otherwise it will be a _positive_ number.
 * @param ticks is the number of ticks since the Epoch.
 * @return the number of ticks added to this time zone.
 */
extern diminuto_ticks_t diminuto_time_daylightsaving(diminuto_ticks_t ticks);

/**
 * Return the number of ticks after the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 for the specified date and time. If the
 * specified date or time is invalid, the results are unspecified.
 * @param year is the year including the century in the range [1970..].
 * @param month is the month of the year in the range [1..12].
 * @param day is the day of the month in the range [1..31].
 * @param hour is the hour of the day in the range [0..23].
 * @param minute is the minute of the hour in the range [0..59].
 * @param second is the second of the minute in the range [0..59].
 * @param tick is the fraction of a second in the range [0..(resolution-1)].
 * @param offset is the number of ticks east of the time zone (0 if UTC).
 * @param daylightsaving is the number of ticks offset for DST (0 if UTC).
 * @return the number of ticks since the Epoch.
 */
extern diminuto_ticks_t diminuto_time_epoch(int year, int month, int day, int hour, int minute, int second, int tick, diminuto_ticks_t offset, diminuto_ticks_t daylightsaving);

/**
 * Convert the number in ticks since the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 into individual fields representing the
 * Common Era (CE) date and and Coordinated Universal Time (UTC) time.
 * @param ticks is the number of ticks since the Epoch.
 * @param yearp is where the year including the century will be returned.
 * @param monthp is where the month of the year will be returned.
 * @param dayp is where the day of the month will be returned.
 * @param hourp is where the hour of the day will be returned.
 * @param minutep is where the minute of the hour will be returned.
 * @param secondp is where the second of the minute will be returned.
 * @param tickp is where the fraction of a second will be returned.
 * @return clock for success, -1 otherwise.
 */
extern diminuto_ticks_t diminuto_time_zulu(diminuto_ticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * tickp);

/**
 * Convert the number in ticks since the Epoch (shown here in ISO8601
 * format) 1970-01-01T00:00:00+0000 into individual fields representing the
 * local date and time.
 * @param ticks is the number of ticks since the Epoch.
 * @param yearp is where the year including the century will be returned.
 * @param monthp is where the month of the year will be returned.
 * @param dayp is where the day of the month will be returned.
 * @param hourp is where the hour of the day will be returned.
 * @param minutep is where the minute of the hour will be returned.
 * @param secondp is where the second of the minute will be returned.
 * @param ticksp is where the fraction of a second will be returned.
 * @return clock for success, -1 otherwise.
 */
extern diminuto_ticks_t diminuto_time_juliet(diminuto_ticks_t ticks, int * yearp, int * monthp, int * dayp, int * hourp, int * minutep, int * secondp, int * ticksp);

#endif
