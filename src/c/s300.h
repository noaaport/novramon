/*
 * $Id$
 */
#ifndef S300_H
#define S300_H

#include "status.h"

void print_status_s300(struct novra_status_st *nvstatus, int f_longdisplay);
void print_statusw_s300(struct novra_status_st *nvstatus, int f_longdisplay);
void log_status_s300(const char *fname,
		     struct novra_status_st *nvstatus,
		     int f_longdisplay,
		     int logperiod);
#endif
