/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_string.h"
#include "diminuto_unittest.h"
#include <string.h>

int main(void)
{
    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };
        char * src = "ABC";
        char * result;
        result = diminuto_strscpy(dest, src, 0);
        EXPECT(result == dest);
        EXPECT(dest[0] == (char)0x01);
        EXPECT(dest[1] == (char)0x23);
        EXPECT(dest[2] == (char)0x45);
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };
        char * src = "ABC";
        char * result;
        result = diminuto_strscpy(dest, src, 1);
        EXPECT(result == dest);
        EXPECT(dest[0] == '\0');
        EXPECT(dest[1] == (char)0x23);
        EXPECT(dest[2] == (char)0x45);
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "") == 0);
        EXPECT(strlen(dest) == 0);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };
        char * src = "ABC";
        char * result;
        result = diminuto_strscpy(dest, src, 2);
        EXPECT(result == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == '\0');
        EXPECT(dest[2] == (char)0x45);
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "A") == 0);
        EXPECT(strlen(dest) == 1);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };
        char * src = "ABC";
        char * result;
        result = diminuto_strscpy(dest, src, 3);
        EXPECT(result == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == 'B');
        EXPECT(dest[2] == '\0');
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "AB") == 0);
        EXPECT(strlen(dest) == 2);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };
        char * src = "ABC";
        char * result;
        result = diminuto_strscpy(dest, src, 4);
        EXPECT(result == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == 'B');
        EXPECT(dest[2] == 'C');
        EXPECT(dest[3] == '\0');
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "ABC") == 0);
        EXPECT(strlen(dest) == 3);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, 0x89 };
        char * src = "ABC";
        char * result;
        result = diminuto_strscpy(dest, src, sizeof(dest));
        EXPECT(result == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == 'B');
        EXPECT(dest[2] == 'C');
        EXPECT(dest[3] == '\0');
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "ABC") == 0);
        EXPECT(strlen(dest) == 3);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    EXIT();
}
