/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_well.h"
#include "com/diag/diminuto/diminuto_memory.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

diminuto_well_t * diminuto_well_init(size_t size, size_t count, size_t alignment, size_t pagesize, size_t linesize)
{
    diminuto_well_t * wellp = (diminuto_well_t *)0;
    diminuto_well_t * well;
    void * control = (void *)0;
    void * data = (void *)0;
    size_t ii;

    do {

        if (pagesize == 0) {
            pagesize = diminuto_memory_pagesize(0);
        }

        control = diminuto_memory_aligned(pagesize, sizeof(diminuto_list_t) * (DIMINUTO_WELL_NODE + count));
        if (control == (void *)0) {
            break;
        }

        if (linesize == 0) {
            linesize = diminuto_memory_linesize(0);
        }

        if (alignment == 0) {
            alignment = linesize;
        }

        alignment = diminuto_memory_power(alignment);
        size = diminuto_memory_alignment(size, alignment);

        data = diminuto_memory_aligned(pagesize, size * count);
        if (data == (void *)0) {
            free(control);
            break;
        }

        well = (diminuto_well_t *)control;
        diminuto_list_datainit(&well[DIMINUTO_WELL_FREE], control);
        diminuto_list_datainit(&well[DIMINUTO_WELL_USED], data);
        for (ii = 0; ii < count; ++ii) {
            diminuto_list_insert(diminuto_list_prev(&well[DIMINUTO_WELL_FREE]), diminuto_list_datainit(&well[DIMINUTO_WELL_NODE + ii], data));
            data += size;
        }

        wellp = &well[0];

    } while (0);

    return wellp;
}

void diminuto_well_fini(diminuto_well_t * wellp)
{
    free(diminuto_list_data(&wellp[DIMINUTO_WELL_USED])); /* data */
    free(diminuto_list_data(&wellp[DIMINUTO_WELL_FREE])); /* control */
}

void * diminuto_well_alloc(diminuto_well_t * wellp)
{
    void * pointer = (void *)0;
    diminuto_list_t * nodep;

    wellp = diminuto_list_root(wellp);
    if (!diminuto_list_isempty(&wellp[DIMINUTO_WELL_FREE])) {
        nodep = diminuto_list_remove(diminuto_list_next(&wellp[DIMINUTO_WELL_FREE]));
        if (nodep != (diminuto_well_t *)0) {
            pointer = diminuto_list_data(nodep);
            diminuto_list_insert(diminuto_list_prev(&wellp[DIMINUTO_WELL_USED]), nodep);
        } else {
            errno = EFAULT;
        }
    } else {
        errno = ENOMEM;
    }

    return pointer;
}

int diminuto_well_free(diminuto_well_t * wellp, void * pointer)
{
    int rc = -1;
    diminuto_list_t * nodep;

    wellp = diminuto_list_root(wellp);
    if (!diminuto_list_isempty(&wellp[DIMINUTO_WELL_USED])) {
        nodep = diminuto_list_remove(diminuto_list_next(&wellp[DIMINUTO_WELL_USED]));
        if (nodep != (diminuto_well_t *)0) {
            diminuto_list_insert(diminuto_list_prev(&wellp[DIMINUTO_WELL_FREE]), diminuto_list_datainit(nodep, pointer));
            rc = 0;
        } else {
            errno = EFAULT;
        }
    } else {
        errno = ENOMEM;
    }

    return rc;
}
