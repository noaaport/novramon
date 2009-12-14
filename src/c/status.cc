/*
 * $Id$
 */
#include <string.h>
#include "err.h"
#include "s75.h"
#include "s75p.h"
#include "status.h"

int get_status(S75_Handle s75h, struct novra_status_st *s75status){
  /*
   * Returns:
   * -1 => error from getStatus
   *  1 => unsupported device
   */
  int status = 0;

  if(s75status->model == S75D_V3){
    memset(&s75status->s75, 0, sizeof(struct S75D_Status_V3));
    if(getStatus(s75h, (BYTE*)&s75status->s75) != S75_OK)
      status = -1;
  }else if((s75status->model == S75PLUS) || (s75status->model == S75PLUSPRO)){
    memset(&s75status->s75p, 0, sizeof(struct S75Plus_Status));
    if(getStatus(s75h, (BYTE*)&s75status->s75p) != S75_OK)
      status = -1;
    /*
     * The SignalStrength in this model is deprecated and is returned
     * as 0, and likewise with RFStatusValid.
     */
    s75status->signal_strength_percentage =
      calcSignalStrengthPercentage(s75h,
				   s75status->s75p.AGCA,
				   s75status->s75p.DemodulatorGain,
				   s75status->s75p.SignalLock,
				   s75status->s75p.DataLock);
  }else{
    status = 1;
  }

  return(status);
}

void print_status(struct novra_status_st *s75status, int f_longdisplay){

  if(s75status->model == S75D_V3)
    print_status_s75(s75status, f_longdisplay);
  else if((s75status->model == S75PLUS) || (s75status->model == S75PLUSPRO))
    print_status_s75p(s75status, f_longdisplay);
  else{
    warnx("Invalid value of s75status->model: %d", s75status->model);
  }
}

void print_statusw(struct novra_status_st *s75status, int f_longdisplay){

  if(s75status->model == S75D_V3)
    print_statusw_s75(s75status, f_longdisplay);
  else if((s75status->model == S75PLUS) || (s75status->model == S75PLUSPRO))
    print_statusw_s75p(s75status, f_longdisplay);
  else{
    warnx("Invalid value of s75status->model: %d", s75status->model);
  }
}

void log_status(const char *fname, struct novra_status_st *s75status,
		int f_longdisplay){

  if(s75status->model == S75D_V3)
    log_status_s75(fname, s75status, f_longdisplay);
  else if((s75status->model == S75PLUS) || (s75status->model == S75PLUSPRO))
    log_status_s75p(fname,s75status, f_longdisplay);
  else{
    log_warnx("Invalid value of s75status->model: %d", s75status->model);
  }
}
