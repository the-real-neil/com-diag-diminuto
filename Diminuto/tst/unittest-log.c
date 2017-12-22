/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "unittest-log.h"
#include <stdlib.h>

int main(int argc, char ** argv)
{
    char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
    int ii;
    diminuto_log_mask = 0;
    ASSERT(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "~0", !0) == 0);
    diminuto_log_setmask();
    ASSERT(diminuto_log_mask == ~(int)0);
    ASSERT(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0", !0) == 0);
    diminuto_log_setmask();
    ASSERT(diminuto_log_mask == 0);
    ASSERT(setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0xff", !0) == 0);
    diminuto_log_setmask();
    ASSERT(diminuto_log_mask == 255);
    diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_DEFAULT, DIMINUTO_LOG_HERE "HERE");
    for (ii = 0; ii < sizeof(buffer) - 1; ++ii) {
        buffer[ii] = (ii % ('~' - ' ' + 1)) + ' ';
    }
    buffer[sizeof(buffer) - 1] = '\0';
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_DEFAULT, "%s%s%s", DIMINUTO_LOG_HERE, "THERE", buffer);
    if (argc > 1) {
        int rc;
        diminuto_log_emit("DAEMONIZING\n");
        if ((rc = diminuto_daemon(argv[0])) == 0) {
            diminuto_log_emit("DAEMONIZED\n");
        } else {
            diminuto_perror("diminuto_daemon");
            exit(1);
        }
    }
    diminuto_log_emit("BEGIN\n");
    yes();
    no();
    maybe();
    diminuto_log_emit("PUBLIC DEFAULT\n");
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC ALL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC EMERGENCY\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC ALERT\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC CRITICAL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC ERROR\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC WARNING\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC NOTICE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC INFORMATION\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC DEBUG\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    mine();
    diminuto_log_emit("PUBLIC NONE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE DEFAULT\n");
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE ALL\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE NONE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE EMERGENCY\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE ALERT\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE CRITICAL\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE ERROR\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE WARNING\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE NOTICE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE INFORMATION\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE DEBUG\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    mine();
    diminuto_log_emit("PRIVATE NONE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    diminuto_log_emit("END\n");
    EXIT();
}
