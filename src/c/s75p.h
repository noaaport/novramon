/*
 * $Id$
 */
#ifndef S75P_H
#define S75P_H

#include "status.h"

void print_status_s75p(struct novra_status_st *nvstatus, int f_longdisplay);
void print_statusw_s75p(struct novra_status_st *nvstatus, int f_longdisplay);
void log_status_s75p(const char *fname,
		     struct novra_status_st *nvstatus,
		     int f_longdisplay);

#endif
