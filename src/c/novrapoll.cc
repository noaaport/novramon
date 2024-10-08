/*
 * $Id: novrapoll.cc 40 2010-12-09 22:03:38Z jfnieves $
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
#define NOVRAMON_CONNECT_TIMEOUT_MS 5000

static struct {
  int opt_d;	/* device id */
  int opt_D;	/* debug mode; do not use syslog */
} g = {0, 0};

static ReceiverSearch g_rs;
static ReceiverList g_rl;
static Receiver *g_r = NULL;

static void parse_args(int argc, char **argv);
static void cleanup(void);

static void update_status_device(struct novra_status_st *nvstatus, char *cmd);
static void update_status_s75(struct novra_status_st *nvstatus, char *cmd);
static void update_status_s75p(struct novra_status_st *nvstatus, char *cmd);
static void update_status_s200(struct novra_status_st *nvstatus, char *cmd);
static void update_status_s300(struct novra_status_st *nvstatus, char *cmd);

int main(int argc, char **argv){

  int status = 0;
  NOVRA_ERRORS novra_error;
  struct novra_status_st nvstatus;
  char inputbuffer[32]; /* the cmd "POLL" or "REPORT" */
  int inputbuffer_len = 32;

  set_progname(NOVRAPOLL_IDENT);	/* init err lib */
  atexit(cleanup);

  parse_args(argc, argv);

  if(g.opt_D != 0)
    set_usesyslog();

  g_rs.discoverLocal(&g_rl);
  if(g_rl.count() == 0)
    log_errx(1, "No local receivers found.\n");
  else if(g_rl.count() > 1) 
    log_errx(1, "Only one receiver is supported.\n");

  g_r = g_rl.getReceiver(0);
  nvstatus.device_type = g_r->getParameter(DEVICE_TYPE).asShort();

  if(g_r->connect(NOVRAMON_CONNECT_TIMEOUT_MS, novra_error) == false){
    g_r = NULL;
    log_errx(1, "Cannot connect to device.");
  }

  fprintf(stdout, "OK:Device poller ready.\n");
  fflush(stdout);

  while((fgets(inputbuffer, inputbuffer_len, stdin) != NULL) && (status == 0)){
    /*
     * This function (in status.cc) gets the status from the device and
     * fills the structure we use to keep the data.
     */
    status = get_status(g_r, &nvstatus);
    if(status != 0)
      log_errx_getstatus(status);

    /*
     * Update the data parameters we keep (e.g., min,max) and handle the
     * POLL or REPORT cmd.
     */
    update_status(&nvstatus, inputbuffer);
  }

  return(status);
}

static void update_status(struct novra_status_st *nvstatus, char *cmd){

  if(isdevice_s75(nvstatus))
    update_status_s75(nvstatus, cmd);
  else if(isdevice_s75p(nvstatus))
    update_status_s75p(nvstatus, cmd);
  else if(isdevice_s200(nvstatus))
    update_status_s200(nvstatus, cmd);
  else if(isdevice_s300(nvstatus))
    update_status_s300(nvstatus, cmd);
  else
    log_warnx("%s\n", "Unsupported device.");
}

static void update_status_s75(struct novra_status_st *nvstatus, char *cmd){

  struct novra_param_st *s75 = &nvstatus->param;
  time_t now;
  /* static time_t last = 0; */
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;
  static double vber_min = 1.0;
  static double vber_max = 0.0;
  static int signal_strength_min = 255;
  static int signal_strength_max = 0;

  /*
   * From s75.h:
   * RFStatusValid == 0 means next 12 structure members
   * DO NOT contain valid data.
   *
   * However, that is deprecated in newer models and therefore
   * we don't use it.
   */

  if(nvstatus->status == 0){
    if(s75->vber < vber_min)
      vber_min = s75->vber;

    if(s75->vber > vber_max)
      vber_max = s75->vber;

    if(s75->signal_strength_as_percentage < signal_strength_min)
      signal_strength_min = s75->signal_strength_as_percentage;

    if(s75->signal_strength_as_percentage > signal_strength_max)
      signal_strength_max = s75->signal_strength_as_percentage;

    uncorrectables_period += s75->uncorrectables;
    uncorrectables += s75->uncorrectables;
  }

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

  if(nvstatus->status != 0){
      fprintf(stdout, "ERROR:Cannot get device status.\n");
      fflush(stdout);
      return;
  }

  now = time(NULL);
  fprintf(stdout, "OK:%u,%d,%d,%d,%d,%.2e,%.2e,%u\n",
	  (unsigned int)now,
	  s75->data_lock, s75->signal_lock,
	  signal_strength_min, signal_strength_max,
	  vber_min, vber_max, uncorrectables_period);

  fflush(stdout);

  /* last = now; */
  uncorrectables_period = 0;
  vber_min = 1.0;
  vber_max = 0.0;
  signal_strength_min = 255;
  signal_strength_max = 0;
}

static void update_status_s75p(struct novra_status_st *nvstatus, char *cmd){

  struct novra_param_st *s75p = &nvstatus->param;
  time_t now;
  /* static time_t last = 0; */
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;
  static double vber_min = 1.0;
  static double vber_max = 0.0;
  static int signal_strength_min = 255;
  static int signal_strength_max = 0;
  /*
  static int demodulator_gain_min = 255;
  static int demodulator_gain_max = 0;
  */

  /*
   * Use nvstatus->valid_data since s75p->RFStatusValid is always 0
   * in s75+.
   */

  if(nvstatus->status == 0){
    if(s75p->vber < vber_min)
      vber_min = s75p->vber;

    if(s75p->vber > vber_max)
      vber_max = s75p->vber;

    if(s75p->signal_strength_as_percentage < signal_strength_min)
      signal_strength_min = s75p->signal_strength_as_percentage;

    if(s75p->signal_strength_as_percentage > signal_strength_max)
      signal_strength_max = s75p->signal_strength_as_percentage;

    /*
    if(s75p->demodulator_gain < demodulator_gain_min)
      demodulator_gain_min = s75p->demodulator_gain;

    if(s75p->demodulator_gain > demodulator_gain_max)
      demodulator_gain_max = s75p->demodulator_gain;
    */

    uncorrectables_period += s75p->uncorrectables;
    uncorrectables += s75p->uncorrectables;
  }

  if(strcmp(cmd, "POLL\n") == 0)
    return;
  else if(strcmp(cmd, "REPORT\n") != 0)
    log_errx(1, "Unrecognized command");

  /*
  fprintf(stdout, "%u %hhu %.2e %.2e %hhu %hhu %u %u\n",
	  (unsigned int)now, nvstatus->signal_strength_percentage,
	  vber_min, vber_max,
	  s75p->SignalLock, s75p->DataLock,
	  uncorrectables_period, uncorrectables);
  */

  if(nvstatus->status != 0){
      fprintf(stdout, "ERROR:Cannot get device status.\n");
      fflush(stdout);
      return;
  }

  now = time(NULL);

  fprintf(stdout, "OK:%d,%d,%d,%d,%d,%.2e,%.2e,%u\n",
	  (unsigned int)now,
	  s75p->data_lock, s75p->signal_lock,
	  signal_strength_min, signal_strength_max,
	  vber_min, vber_max, uncorrectables_period);

  fflush(stdout);

  /* last = now; */
  uncorrectables_period = 0;
  vber_min = 1.0;
  vber_max = 0.0;
  signal_strength_min = 255;
  signal_strength_max = 0;
  /*
  demodulator_gain_min = 255;
  demodulator_gain_max = 0;
  */
}

static void update_status_s200(struct novra_status_st *nvstatus, char *cmd){

  struct novra_param_st *s200 = &nvstatus->param;
  time_t now;
  /* static time_t last = 0; */
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;
  static unsigned int ethernet_transmit_period = 0;   /* packets */
  static unsigned int dvb_accepted_period = 0;        /* packets */
  static double vber_min = 1.0;
  static double vber_max = 0.0;
  static double carrier_to_noise_min = 100.0;
  static double carrier_to_noise_max = 0.0;
  static int signal_strength_min = 255;
  static int signal_strength_max = 0;
  static int signal_strength_min_dbm = 100;
  static int signal_strength_max_dbm = -100;

  if(nvstatus->status == 0){
    if(s200->vber < vber_min)
      vber_min = s200->vber;

    if(s200->vber > vber_max)
      vber_max = s200->vber;

    if(s200->signal_strength_as_percentage < signal_strength_min)
      signal_strength_min = s200->signal_strength_as_percentage;

    if(s200->signal_strength_as_percentage > signal_strength_max)
      signal_strength_max = s200->signal_strength_as_percentage;

    if(s200->signal_strength_as_dbm < signal_strength_min_dbm)
      signal_strength_min_dbm = s200->signal_strength_as_dbm;

    if(s200->signal_strength_as_dbm > signal_strength_max_dbm)
      signal_strength_max_dbm = s200->signal_strength_as_dbm;

    if(s200->carrier_to_noise < carrier_to_noise_min)
      carrier_to_noise_min = s200->carrier_to_noise;

    if(s200->carrier_to_noise > carrier_to_noise_max)
      carrier_to_noise_max = s200->carrier_to_noise;

    uncorrectables_period += s200->uncorrectables;
    uncorrectables += s200->uncorrectables;

    ethernet_transmit_period += s200->ethernet_transmit;
    dvb_accepted_period += s200->dvb_accepted;
  }

  if(strcmp(cmd, "POLL\n") == 0)
    return;
  else if(strcmp(cmd, "REPORT\n") != 0)
    log_errx(1, "Unrecognized command");

  /*
  fprintf(stdout, "%u %hhu %.2e %.2e %hhu %hhu %u %u\n",
	  (unsigned int)now, nvstatus->signal_strength_percentage,
	  vber_min, vber_max,
	  s75p->SignalLock, s75p->DataLock,
	  uncorrectables_period, uncorrectables);
  */

  if(nvstatus->status != 0){
      fprintf(stdout, "ERROR:Cannot get device status.\n");
      fflush(stdout);
      return;
  }

  now = time(NULL);

  fprintf(stdout, "OK:%d,%d,%d,%d,%d,%.2e,%.2e,%u,"
	  "%.1f,%.1f,%d,%d,%d,%d,%u,%u\n",
	  (unsigned int)now,
	  s200->data_lock, s200->signal_lock,
	  signal_strength_min, signal_strength_max,
	  vber_min, vber_max, uncorrectables_period,
	  carrier_to_noise_min, carrier_to_noise_max,
	  signal_strength_min_dbm, signal_strength_max_dbm,
	  s200->data_sync_loss, s200->frequency_offset,
	  ethernet_transmit_period, dvb_accepted_period);

  fflush(stdout);

  /* last = now; */
  uncorrectables_period = 0;
  ethernet_transmit_period = 0;
  dvb_accepted_period = 0;
  vber_min = 1.0;
  vber_max = 0.0;
  carrier_to_noise_min = 100.0;
  carrier_to_noise_max = 0.0;
  signal_strength_min = 255;
  signal_strength_max = 0;
  signal_strength_min_dbm = 100;
  signal_strength_max_dbm = -100;
}

static void update_status_s300(struct novra_status_st *nvstatus, char *cmd){

  struct novra_param_st *s300 = &nvstatus->param;
  time_t now;
  /* static time_t last = 0; */
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;
  static unsigned int ethernet_transmit_period = 0;   /* packets */
  static unsigned int dvb_accepted_period = 0;        /* packets */
  static double vber_min = 1.0;
  static double vber_max = 0.0;
  static double carrier_to_noise_min = 100.0;
  static double carrier_to_noise_max = 0.0;
  static int signal_strength_min = 255;
  static int signal_strength_max = 0;
  static int signal_strength_min_dbm = 100;
  static int signal_strength_max_dbm = -100;

  if(nvstatus->status == 0){
    if(s300->vber < vber_min)
      vber_min = s300->vber;

    if(s300->vber > vber_max)
      vber_max = s300->vber;

    if(s300->signal_strength_as_percentage < signal_strength_min)
      signal_strength_min = s300->signal_strength_as_percentage;

    if(s300->signal_strength_as_percentage > signal_strength_max)
      signal_strength_max = s300->signal_strength_as_percentage;

    if(s300->signal_strength_as_dbm < signal_strength_min_dbm)
      signal_strength_min_dbm = s300->signal_strength_as_dbm;

    if(s300->signal_strength_as_dbm > signal_strength_max_dbm)
      signal_strength_max_dbm = s300->signal_strength_as_dbm;

    if(s300->carrier_to_noise < carrier_to_noise_min)
      carrier_to_noise_min = s300->carrier_to_noise;

    if(s300->carrier_to_noise > carrier_to_noise_max)
      carrier_to_noise_max = s300->carrier_to_noise;

    uncorrectables_period += s300->uncorrectables;
    uncorrectables += s300->uncorrectables;

    ethernet_transmit_period += s300->ethernet_transmit;
    dvb_accepted_period += s300->dvb_accepted;
  }

  if(strcmp(cmd, "POLL\n") == 0)
    return;
  else if(strcmp(cmd, "REPORT\n") != 0)
    log_errx(1, "Unrecognized command");

  /*
  fprintf(stdout, "%u %hhu %.2e %.2e %hhu %hhu %u %u\n",
	  (unsigned int)now, nvstatus->signal_strength_percentage,
	  vber_min, vber_max,
	  s75p->SignalLock, s75p->DataLock,
	  uncorrectables_period, uncorrectables);
  */

  if(nvstatus->status != 0){
      fprintf(stdout, "ERROR:Cannot get device status.\n");
      fflush(stdout);
      return;
  }

  now = time(NULL);

  fprintf(stdout, "OK:%d,%d,%d,%d,%d,%.2e,%.2e,%u,"
	  "%.1f,%.1f,%d,%d,%d,%d,%u,%u\n",
	  (unsigned int)now,
	  s300->data_lock, s300->signal_lock,
	  signal_strength_min, signal_strength_max,
	  vber_min, vber_max, uncorrectables_period,
	  carrier_to_noise_min, carrier_to_noise_max,
	  signal_strength_min_dbm, signal_strength_max_dbm,
	  s300->data_sync_loss, s300->frequency_offset,
	  ethernet_transmit_period, dvb_accepted_period);

  fflush(stdout);

  /* last = now; */
  uncorrectables_period = 0;
  ethernet_transmit_period = 0;
  dvb_accepted_period = 0;
  vber_min = 1.0;
  vber_max = 0.0;
  carrier_to_noise_min = 100.0;
  carrier_to_noise_max = 0.0;
  signal_strength_min = 255;
  signal_strength_max = 0;
  signal_strength_min_dbm = 100;
  signal_strength_max_dbm = -100;
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

  if(g_r == NULL)
    return;

  g_r->disconnect();
  g_r = NULL;
}
