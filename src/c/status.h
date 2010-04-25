/*
 * $Id$
 */
#ifndef STATUS_H
#define STATUS_H

#include <sys/types.h>
#include "lib/s75.h"

struct novra_status_st {
  int model;
  struct S75D_Status_V3 s75;
  struct S75Plus_Status s75p;
  int signal_strength_percentage;
  int status;	/* return code from get_status() */
};

int get_status(S75_Handle s75h, struct novra_status_st *s75status);
void print_status(struct novra_status_st *s75status, int f_longdisplay);
void print_statusw(struct novra_status_st *s75status, int f_longdisplay);
void log_status(const char *fname,
		struct novra_status_st *s75status,
		int f_longdisplay);
#endif
