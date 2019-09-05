/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * frequency
 *
 * ABSTRACT
 *
 * Prints on standard output the base frequency of the Diminuto library.
 * This is the number of ticks per second used for the basis of all time
 * measurement in the library. The duration of the basic tick unit is the
 * reciprocal of this number.
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_frequency.h"

int main(void)
{
    printf("%lld\n", (long long int)diminuto_frequency());

    return 0;
}
