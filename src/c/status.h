/*
 * $Id: status.h 40 2010-12-09 22:03:38Z jfnieves $
 */
#ifndef STATUS_H
#define STATUS_H

#include <time.h>
#include "RecComLib.h"

/*
 * From RecComLib, these are the "device_type" that novramon supports:
 *
 * R_S75_V1
 * R_S75_V2_1
 * R_S75_V3
 * R_S75PLUS
 * R_S75PLUSPRO
 * R_S200
 */

struct novra_param_st {
  /*
   * This is the subset of the parameters that we will track.
   */
  int carrier_frequency;
  int symbol_rate;
  int lnb_fault;
  int signal_lock;
  int data_lock;
  int signal_strength_as_percentage;
  int signal_strength_as_dbm;
  double carrier_to_noise;
  double vber;
  int uncorrectables;
  int frequency_offset;
  int clock_offset;
  int data_sync_loss;
  int front_end_lock_loss;
  int frequency_error;
  /* optional */
  int ethernet_transmit;
  int ethernet_transmit_error;
  int ethernet_receive;
  int ethernet_receive_error;
  int ethernet_packet_dropped;
  int total_ethernet_packets_out;
  int dvb_accepted;
  int dvb_scrambled;
  int total_dvb_packets_accepted;
  int total_dvb_packets_rx_in_error;
  int total_uncorrectable_ts_packets;
  /* unused */
  int demodulator_gain;
  int nest;
  int digital_gain;
  int agc;
  int agca;
  int agcn;
  int gnyqa;
  int gfara;
};

struct novra_status_st {
  int status;		/* return code from get_status() */
  int device_type;	/* R_S200, ... */
  struct novra_param_st  param;
  /* calculated when loging period spans several device periods */
  time_t last;	/* time of last report */
  int signal_strength_as_percentage_min;
  int signal_strength_as_percentage_max;
  int signal_strength_as_dbm_min;
  int signal_strength_as_dbm_max;
  double carrier_to_noise_min;
  double carrier_to_noise_max;
  double vber_min;
  double vber_max;
  unsigned int uncorrectables_total;
};

void init_novra_status(struct novra_status_st *nvstatus);
void reinit_novra_status(struct novra_status_st *nvstatus);
void update_status(struct novra_status_st *nvstatus);

int get_status(Receiver *r, struct novra_status_st *nvstatus);
void print_status(struct novra_status_st *nvstatus, int f_longdisplay);
void print_statusw(struct novra_status_st *nvstatus, int f_longdisplay);
void log_status(const char *fname,
		struct novra_status_st *nvstatus,
		int f_longdisplay,
		int logperiod);

int isdevice_s75(struct novra_status_st *nvstatus);
int isdevice_s75p(struct novra_status_st *nvstatus);
int isdevice_s200(struct novra_status_st *nvstatus);

#endif
