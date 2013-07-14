/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_well.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct Object {
	int a;
	int b;
	int c;
} object_t;

static int is_a_power_of_two(unsigned long value) {
	int bits = 0;

	if (value == 0) {
		/* Do nothing. */
	} else if ((value & 1) != 0) {
		/* Do nothing. */
	} else {
		value >>= 1;
		while (value > 0) {
			if ((value & 1) != 0) {
				++bits;
			}
			value >>= 1;
		}
	}

	return (bits == 1);
}

int main(void)
{
	ssize_t pagesize;
	ssize_t linesize;
    diminuto_well_t * wellp;
    object_t * pa[5];
    size_t ii;
    size_t jj;
    int kk;
    int rc;
    unsigned long value;

    ASSERT(!is_a_power_of_two(0));
    ASSERT(!is_a_power_of_two(1));
    ASSERT(!is_a_power_of_two(3));
    ASSERT(!is_a_power_of_two(6));
	for (value = 2; value > 0; value <<= 1) {
    	ASSERT(is_a_power_of_two(value));
    }

    pagesize = diminuto_well_pagesize();
    printf("pagesize=0x%x=%d\n", pagesize, pagesize);
    ASSERT(is_a_power_of_two(pagesize));

    linesize = diminuto_well_linesize();
    printf("linesize=0x%x=%d\n", linesize, linesize);
    ASSERT(is_a_power_of_two(linesize));

    wellp = diminuto_well_init(sizeof(object_t), countof(pa));
    ASSERT(wellp != (diminuto_well_t *)0);

    for (kk = 0; kk < 4; ++kk) {

		for (ii = 0; ii < countof(pa); ++ii) {
			pa[ii] = diminuto_well_alloc(wellp);
			ASSERT(pa[ii] != (void *)0);
		}

		for (ii = 0; ii < (countof(pa) - 1); ++ii) {
			ASSERT(((char *)pa[ii] - (char *)pa[ii + 1]) == sizeof(object_t));
		}

		ASSERT(diminuto_well_alloc(wellp) == (void *)0);

		for (ii = 0; ii < countof(pa); ++ii) {
			for (jj = 0; jj < countof(pa); ++jj) {
				if (jj != ii) {
					ASSERT(pa[ii] != pa[jj]);
				}
			}
		}

		for (ii = 0; ii < countof(pa); ++ii) {
			rc = diminuto_well_free(wellp, pa[ii]);
			ASSERT(rc == 0);
		}

    }

    diminuto_well_fini(wellp);
}
