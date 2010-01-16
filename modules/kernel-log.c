/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <linux/module.h>
#include "diminuto_log.h"

#include "unittest-log.h"

static int diminuto_log_subsystem_mask = -1;

static void unittest(void)
{
    printk("BEGIN\n");
    yes();
    no();
    maybe();
    printk("PUBLIC DEFAULT\n");
    all();
    none();
    mine();
    printk("PUBLIC ALL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    mine();
    printk("PUBLIC EMERGENCY\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    mine();
    printk("PUBLIC ALERT\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    mine();
    printk("PUBLIC CRITICAL\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    mine();
    printk("PUBLIC ERROR\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    mine();
    printk("PUBLIC WARNING\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    mine();
    printk("PUBLIC NOTICE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    mine();
    printk("PUBLIC INFORMATION\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    mine();
    printk("PUBLIC DEBUG\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    mine();
    printk("PUBLIC NONE\n");
    diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    printk("PRIVATE DEFAULT\n");
    all();
    none();
    mine();
    printk("PRIVATE ALL\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALL;
    all();
    none();
    mine();
    printk("PRIVATE NONE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    printk("PRIVATE EMERGENCY\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_EMERGENCY;
    all();
    none();
    mine();
    printk("PRIVATE ALERT\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALERT;
    all();
    none();
    mine();
    printk("PRIVATE CRITICAL\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_CRITICAL;
    all();
    none();
    mine();
    printk("PRIVATE ERROR\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ERROR;
    all();
    none();
    mine();
    printk("PRIVATE WARNING\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_WARNING;
    all();
    none();
    mine();
    printk("PRIVATE NOTICE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NOTICE;
    all();
    none();
    mine();
    printk("PRIVATE INFORMATION\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_INFORMATION;
    all();
    none();
    mine();
    printk("PRIVATE DEBUG\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEBUG;
    all();
    none();
    mine();
    printk("PRIVATE NONE\n");
    diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
    all();
    none();
    mine();
    if (diminuto_log_subsystem_mask != -1) {
        diminuto_log_subsystem[0] = diminuto_log_subsystem_mask;
        printk("PRIVATE SET\n");
        all();
        none();
        mine();
    }
    printk("END\n");
}

int init_log_unittest(void)
{
    unittest();
    return 0;
}

void cleanup_log_unittest(void)
{
}

EXPORT_SYMBOL(diminuto_log_subsystem);
module_param(diminuto_log_subsystem_mask, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(diminuto_log_subsystem_mask, "Diminuto Log Mask");
MODULE_LICENSE("LGPL");
MODULE_AUTHOR("coverclock@diag.com");
MODULE_DESCRIPTION("diminuto log kernel unit test");
module_init(init_log_unittest);
module_exit(cleanup_log_unittest);
