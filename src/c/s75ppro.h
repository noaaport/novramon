/*
 * $Id: s75ppro.h 1 2009-12-14 01:44:00Z nieves $
 */
#ifndef S75P_H
#define S75P_H

#include "status.h"

void print_status_s75p(struct novra_status_st *s75status, int f_longdisplay);
void print_statusw_s75p(struct novra_status_st *s75status, int f_longdisplay);
void log_status_s75p(const char *fname,
		     struct novra_status_st *s75status,
		     int f_longdisplay);

#endif
