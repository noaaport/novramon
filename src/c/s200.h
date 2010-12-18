/*
 * $Id: s75p.h 1 2009-12-14 01:44:00Z nieves $
 */
#ifndef S200_H
#define S200_H

#include "status.h"

void print_status_s200(struct novra_status_st *nvstatus, int f_longdisplay);
void print_statusw_s200(struct novra_status_st *nvstatus, int f_longdisplay);
void log_status_s200(const char *fname,
		     struct novra_status_st *nvstatus,
		     int f_longdisplay,
		     int logperiod);
#endif
