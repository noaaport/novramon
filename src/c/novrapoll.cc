/*
 * $Id$
 */
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include "err.h"
#include "status.h"

#define NOVRAPOLL_IDENT "novrapoll"

static struct {
  int opt_d;	/* device id */
  int opt_D;	/* debug mode; do not use syslog */
} g = {0, 0};

static S75_Handle g_s75h = NULL;

static void parse_args(int argc, char **argv);
static void cleanup(void);

static void update_status(struct novra_status_st *s75status, char *cmd);
static void update_status_s75(struct novra_status_st *s75status, char *cmd);
static void update_status_s75p(struct novra_status_st *s75status, char *cmd);

int main(int argc, char **argv){

  struct Novra_DeviceList devlist;
  int status = 0;
  struct novra_status_st s75status;
  char inputbuffer[32];
  int inputbuffer_len = 32;

  set_progname(NOVRAPOLL_IDENT);	/* init err lib */
  atexit(cleanup);

  parse_args(argc, argv);

  if(g.opt_D != 0)
    set_usesyslog();

  status = discoverDevices(&devlist);
  if(status == 0)
    log_errx(1, "Cannot discoverDevices().");

  s75status.model = devlist.Device[0].DeviceType;

  g_s75h = openDevice(&devlist.Device[0], 2000, status);
  if(g_s75h == NULL)
    log_errx(1, "Cannot openDevice()");

  fprintf(stdout, "OK: Device poller ready.\n");
  fflush(stdout);

  while((fgets(inputbuffer, inputbuffer_len, stdin) != NULL) && (status == 0)){
    status = get_status(g_s75h, &s75status);
    if(status == 0)
      update_status(&s75status, inputbuffer);
    else{
      log_errx_getstatus(status);
      fprintf(stdout, "ERROR: Cannot get device status.\n");
      fflush(stdout);
    }
  }

  return(status);
}

static void update_status(struct novra_status_st *s75status, char *cmd){

  if(s75status->model == S75D_V3)
    update_status_s75(s75status, cmd);
  else if((s75status->model == S75PLUS) || (s75status->model == S75PLUSPRO))
    update_status_s75p(s75status, cmd);
  else{
    fprintf(stdout, "ERROR: Unsupprted device.\n");
    fflush(stdout);
  }
}

static void update_status_s75(struct novra_status_st *s75status, char *cmd){

  struct S75D_Status_V3 *s75 = &s75status->s75;
  time_t now;
  static time_t last = 0;
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;
  static double vber_min = 1.0;
  static double vber_max = 0.0;
  static unsigned char signal_strength_min = 255;
  static unsigned char signal_strength_max = 0;

  /*
   * From s75.h:
   * RFStatusValid == 0 means next 12 structure members
   * DO NOT contain valid data.
   *
   * However, that is deprecated in newer models and therefore
   * we don't use it.
   */

  if(s75->VBER < vber_min)
    vber_min = s75->VBER;

  if(s75->VBER > vber_max)
    vber_max = s75->VBER;

  if(s75->SignalStrength < signal_strength_min)
    signal_strength_min = s75->SignalStrength;

  if(s75->SignalStrength > signal_strength_max)
    signal_strength_max = s75->SignalStrength;

  uncorrectables_period += s75->Uncorrectables;
  uncorrectables += s75->Uncorrectables;

  now = time(NULL);

  if(strcmp(cmd, "POLL\n") == 0)
    return;
  else if(strcmp(cmd, "REPORT\n") != 0)
    log_errx(1, "Unrecognized command");

  /*
  fprintf(stdout, "%u %hhu %.2e %.2e %hhu %hhu %u %u\n",
	  (unsigned int)now, s75->SignalStrength,
	  vber_min, vber_max,
	  s75->SignalLock, s75->DataLock,
	  uncorrectables_period, uncorrectables);
  */

  fprintf(stdout, "OK:%u,%hhu,%hhu,%hhu,%hhu,%.2e,%.2e,%u\n",
	  (unsigned int)now,
	  s75->DataLock, s75->SignalLock,
	  signal_strength_min, signal_strength_max,
	  vber_min, vber_max, uncorrectables_period);

  fflush(stdout);

  last = now;
  uncorrectables_period = 0;
  vber_min = 1.0;
  vber_max = 0.0;
  signal_strength_min = 255;
  signal_strength_max = 0;
}

static void update_status_s75p(struct novra_status_st *s75status, char *cmd){

  struct S75Plus_Status *s75p = &s75status->s75p;
  time_t now;
  static time_t last = 0;
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;
  static double vber_min = 1.0;
  static double vber_max = 0.0;
  static unsigned char signal_strength_min = 255;
  static unsigned char signal_strength_max = 0;
  static unsigned char demodulator_gain_min = 255;
  static unsigned char demodulator_gain_max = 0;

  /*
   * Use s75status->valid_data since s75p->RFStatusValid is always 0
   * in s75+.
   */

  if(s75p->VBER < vber_min)
    vber_min = s75p->VBER;

  if(s75p->VBER > vber_max)
    vber_max = s75p->VBER;

  if(s75status->signal_strength_percentage < signal_strength_min)
    signal_strength_min = s75status->signal_strength_percentage;

  if(s75status->signal_strength_percentage > signal_strength_max)
    signal_strength_max = s75status->signal_strength_percentage;

  if(s75p->DemodulatorGain < demodulator_gain_min)
    demodulator_gain_min = s75p->DemodulatorGain;

  if(s75p->DemodulatorGain > demodulator_gain_max)
    demodulator_gain_max = s75p->DemodulatorGain;

  uncorrectables_period += s75p->Uncorrectables;
  uncorrectables += s75p->Uncorrectables;

  now = time(NULL);

  if(strcmp(cmd, "POLL\n") == 0)
    return;
  else if(strcmp(cmd, "REPORT\n") != 0)
    log_errx(1, "Unrecognized command");

  /*
  fprintf(stdout, "%u %hhu %.2e %.2e %hhu %hhu %u %u\n",
	  (unsigned int)now, s75status->signal_strength_percentage,
	  vber_min, vber_max,
	  s75p->SignalLock, s75p->DataLock,
	  uncorrectables_period, uncorrectables);
  */

  /* The Freq_Err should really be a signed int */
  fprintf(stdout, "OK:%u,%hhu,%hhu,%hhu,%hhu,%.2e,%.2e,%u,"
	  "%hhu,%hhu,%hhu,%hu,%hhu,%hd\n",
	  (unsigned int)now,
	  s75p->DataLock, s75p->SignalLock,
	  signal_strength_min, signal_strength_max,
	  vber_min, vber_max, uncorrectables_period,
	  demodulator_gain_min, demodulator_gain_max,
	  s75p->NEST,
	  s75p->DataSyncLoss,      
	  s75p->Clock_Off,
	  s75p->Freq_Err);

  fflush(stdout);

  last = now;
  uncorrectables_period = 0;
  vber_min = 1.0;
  vber_max = 0.0;
  signal_strength_min = 255;
  signal_strength_max = 0;
  demodulator_gain_min = 255;
  demodulator_gain_max = 0;
}

static void parse_args(int argc, char **argv){

  int c;
  int status = 0;
  const char *optstr = "Dd:";
  const char *usage = "novrapoll [-D] [-d <id>]";

  while((status == 0) && ((c = getopt(argc, argv, optstr)) != -1) ){
    switch(c){
    case 'D':
      g.opt_D = 1;
      break;
    case 'd':
      g.opt_d = atoi(optarg);
      break;
    default:
      status = 1;
      log_errx(1, usage);
      break;
    }
  }
}

static void cleanup(void){

  if(g_s75h == NULL)
    return;

  closeS75(g_s75h);
  g_s75h = NULL;
}
