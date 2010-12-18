/*
 * $Id: s75.h 40 2010-12-09 22:03:38Z jfnieves $
 */
#ifndef S75_H
#define S75_H

#include "status.h"

void print_status_s75(struct novra_status_st *nvstatus, int f_longdisplay);
void print_statusw_s75(struct novra_status_st *nvstatus, int f_longdisplay);
void log_status_s75(const char *fname,
		    struct novra_status_st *nvstatus,
		    int f_longdisplay,
		    int logperiod);

#endif
