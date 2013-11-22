/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUX_
#define _H_COM_DIAG_DIMINUTO_MUX_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * WORK IN PROGRESS
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <sys/select.h>

typedef struct DiminutoMux {
	int nfds;
	int minrfd;
	int minwfd;
	int minefd;
	int maxrfd;
	int maxwfd;
	int maxefd;
	fd_set reading;
	fd_set writing;
	fd_set excepting;
	fd_set readable;
	fd_Set writeable;
	fd_set exceptional;
	sigset_t mask;
} diminuto_mux_t;

extern int diminuto_mux_register_read(diminuto_mux_t * that, int fd);

extern int diminuto_mux_unregister_read(diminuto_mux_t * that, int fd);

extern int diminuto_mux_register_write(diminuto_mux_t * that, int fd);

extern int diminuto_mux_unregister_write(diminuto_mux_t * that, int fd);

extern int diminuto_mux_register_exception(diminuto_mux_t * that, int fd);

extern int diminuto_mux_unregister_exception(diminuto_mux_t * that, int fd);

extern int diminuto_mux_register_signal(diminuto_mux_t * that, int sig);

extern int diminuto_mux_unregister_signal(diminuto_mux_t * that, int sig);

extern int diminuto_mux_ready_read(diminuto_mux_t * that);

extern int diminuto_mux_ready_write(diminuto_mux_t * that);

extern int diminuto_mux_ready_exception(diminuto_mux_t * that);

extern int diminuto_mux_ready(diminuto_mux_t * that, diminuto_ticks_t timeout);

#endif
