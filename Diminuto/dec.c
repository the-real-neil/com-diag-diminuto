/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * dec [ number ]
 * cat number | dec
 *
 * EXAMPLES
 *
 * dec 0xa
 * dec 012
 * dec 12
 * hex 0xa
 * hex 012
 * hex 12
 * oct 0xa
 * oct 012
 * oct 12
 * echo 0xa | dec
 * echo 012 | dec
 * echo 12 | dec
 * echo 0xa | hex
 * echo 012 | hex
 * echo 12 | hex
 * echo 0xa | oct
 * echo 012 | oct
 * echo 12 | oct
 *
 * ABSTRACT
 *
 * Allows input and output in bases in other than decimal in scripts.
 */

#include "com/diag/diminuto/diminuto_number.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>

int main(int argc, char * argv[])
{
    const char * name;
    const char * end;
    char buffer[64];
    const char * string;
    const char * next;
    uint64_t value;
    int file;
    int argn = 1;

    name = strrchr(argv[0], '/');
    name = (name == (char *)0) ? argv[0] : name + 1;

    file = (argc < 2) || ((argc == 2) && (strcmp(argv[1], "-") == 0));

    if (file) {
        string = fgets(buffer, sizeof(buffer), stdin);
    } else {
        string = argv[argn++];
    }

    if (string == (const char *)0) {
        errno = EINVAL;
        perror(name);
        return 2;
    }

    while (!0) {

		next = diminuto_number(string, &value);
		if ((*next != '\0') && (!isspace(*next))) {
			perror(string);
			return 3;
		}

		if (strcmp(name, "hex") == 0) {
			printf("0x%llx\n", value);
		} else if (strcmp(name, "oct") == 0) {
			printf("0%llo\n", value);
		} else {
			printf("%llu\n", value);
		}

	    if (file) {
	        string = fgets(buffer, sizeof(buffer), stdin);
	    } else if (argn < argc) {
	        string = argv[argn++];
	    } else {
	    	string = (const char *)0;
	    }

        if (string == (const char *)0) {
        	break;
        }

    }

    return 0;
}
