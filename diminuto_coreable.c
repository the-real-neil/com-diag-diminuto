/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_coreable.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>

int diminuto_coreable()
{
	int result = 0;
    int rc;
	struct rlimit limit;

	rc = getrlimit(RLIMIT_CORE, &limit);
	if (rc < 0) {
		perror("getrlimit");
		result = -1;
	} else {
		limit.rlim_cur = limit.rlim_max;
		rc = setrlimit(RLIMIT_CORE, &limit);
		if (rc < 0) {
			perror("setrlimit");
			result = -2;
		}
	}

	return result;
}
