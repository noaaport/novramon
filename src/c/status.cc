/*
 * $Id: status.cc 40 2010-12-09 22:03:38Z jfnieves $
 */
#include <string.h>
#include "err.h"
#include "s75.h"
#include "s75p.h"
#include "s200.h"
#include "s300.h"
#include "status.h"

static void fill_novra_param(Receiver *r, struct novra_param_st *p);

void check_param(Receiver *r){

  if(r->hasParameter(CARRIER_FREQUENCY)){
      fprintf(stdout, "OK: CARRIER_FREQUENCY\n");
  }else{
      fprintf(stdout, "NO: CARRIER_FREQUENCY\n");
  }

  if(r->hasParameter(SYMBOL_RATE)){
      fprintf(stdout, "OK: SYMBOL_RATE\n");
  }else{
      fprintf(stdout, "NO: SYMBOL_RATE\n");
  }

  if(r->hasParameter(LNB_FAULT)){
      fprintf(stdout, "OK: LNB_FAULT\n");
  }else{
      fprintf(stdout, "NO: LNB_FAULT\n");
  }

  if(r->hasParameter(SIGNAL_LOCK)){
      fprintf(stdout, "OK: SIGNAL_LOCK\n");
  }else{
      fprintf(stdout, "NO: SIGNAL_LOCK\n");
  }

  if(r->hasParameter(DATA_LOCK)){
      fprintf(stdout, "OK: DATA_LOCK\n");
  }else{
      fprintf(stdout, "NO: DATA_LOCK\n");
  }

  if(r->hasParameter(SIGNAL_STRENGTH_AS_PERCENTAGE)){
      fprintf(stdout, "OK: SIGNAL_STRENGTH_AS_PERCENTAGE\n");
  }else{
      fprintf(stdout, "NO: SIGNAL_STRENGTH_AS_PERCENTAGE\n");
  }

  if(r->hasParameter(SIGNAL_STRENGTH_AS_DBM)){
      fprintf(stdout, "OK: SIGNAL_STRENGTH_AS_DBM\n");
  }else{
      fprintf(stdout, "NO: SIGNAL_STRENGTH_AS_DBM\n");
  }

  if(r->hasParameter(CARRIER_TO_NOISE)){
      fprintf(stdout, "OK: CARRIER_TO_NOISE\n");
  }else{
      fprintf(stdout, "NO: CARRIER_TO_NOISE\n");
  }

  if(r->hasParameter(VBER)){
      fprintf(stdout, "OK: VBER\n");
  }else{
      fprintf(stdout, "NO: VBER\n");
  }

  if(r->hasParameter(UNCORRECTABLES)){
      fprintf(stdout, "OK: UNCORRECTABLES\n");
  }else{
      fprintf(stdout, "NO: UNCORRECTABLES\n");
  }

  if(r->hasParameter(FREQUENCY_OFFSET)){
      fprintf(stdout, "OK: FREQUENCY_OFFSET\n");
  }else{
      fprintf(stdout, "NO: FREQUENCY_OFFSET\n");
  }

  if(r->hasParameter(CLOCK_OFFSET)){
      fprintf(stdout, "OK: CLOCK_OFFSET\n");
  }else{
      fprintf(stdout, "NO: CLOCK_OFFSET\n");
  }

  if(r->hasParameter(DATA_SYNC_LOSS)){
      fprintf(stdout, "OK: DATA_SYNC_LOSS\n");
  }else{
      fprintf(stdout, "NO: DATA_SYNC_LOSS\n");
  }

  if(r->hasParameter(FRONT_END_LOCK_LOSS)){
      fprintf(stdout, "OK: FRONT_END_LOCK_LOSS\n");
  }else{
      fprintf(stdout, "NO: FRONT_END_LOCK_LOSS\n");
  }

  if(r->hasParameter(FREQUENCY_ERROR)){
      fprintf(stdout, "OK: FREQUENCY_ERROR\n");
  }else{
      fprintf(stdout, "NO: FREQUENCY_ERROR\n");
  }

  if(r->hasParameter(ETHERNET_TRANSMIT)){
      fprintf(stdout, "OK: ETHERNET_TRANSMIT\n");
  }else{
      fprintf(stdout, "NO: ETHERNET_TRANSMIT\n");
  }

  if(r->hasParameter(ETHERNET_TRANSMIT_ERROR)){
      fprintf(stdout, "OK: ETHERNET_TRANSMIT_ERROR\n");
  }else{
      fprintf(stdout, "NO: ETHERNET_TRANSMIT_ERROR\n");
  }

  if(r->hasParameter(ETHERNET_RECEIVE)){
      fprintf(stdout, "OK: ETHERNET_RECEIVE\n");
  }else{
      fprintf(stdout, "NO: ETHERNET_RECEIVE\n");
  }

  if(r->hasParameter(ETHERNET_RECEIVE_ERROR)){
      fprintf(stdout, "OK: ETHERNET_RECEIVE_ERROR\n");
  }else{
      fprintf(stdout, "NO: ETHERNET_RECEIVE_ERROR\n");
  }

  if(r->hasParameter(ETHERNET_PACKET_DROPPED)){
      fprintf(stdout, "OK: ETHERNET_PACKET_DROPPED\n");
  }else{
      fprintf(stdout, "NO: ETHERNET_PACKET_DROPPED\n");
  }

  if(r->hasParameter(TOTAL_ETHERNET_PACKETS_OUT)){
      fprintf(stdout, "OK: TOTAL_ETHERNET_PACKETS_OUT\n");
  }else{
      fprintf(stdout, "NO: TOTAL_ETHERNET_PACKETS_OUT\n");
  }

  if(r->hasParameter(DVB_ACCEPTED)){
      fprintf(stdout, "OK: DVB_ACCEPTED\n");
  }else{
      fprintf(stdout, "NO: DVB_ACCEPTED\n");
  }

  if(r->hasParameter(DVB_SCRAMBLED)){
      fprintf(stdout, "OK: DVB_SCRAMBLED\n");
  }else{
      fprintf(stdout, "NO: DVB_SCRAMBLED\n");
  }

  if(r->hasParameter(TOTAL_DVB_PACKETS_ACCEPTED)){
      fprintf(stdout, "OK: TOTAL_DVB_PACKETS_ACCEPTED\n");
  }else{
      fprintf(stdout, "NO: TOTAL_DVB_PACKETS_ACCEPTED\n");
  }

  if(r->hasParameter(TOTAL_DVB_PACKETS_RX_IN_ERROR)){
      fprintf(stdout, "OK: TOTAL_DVB_PACKETS_RX_IN_ERROR\n");
  }else{
      fprintf(stdout, "NO: TOTAL_DVB_PACKETS_RX_IN_ERROR\n");
  }

  if(r->hasParameter(TOTAL_UNCORRECTABLE_TS_PACKETS)){
      fprintf(stdout, "OK: TOTAL_UNCORRECTABLE_TS_PACKETS\n");
  }else{
      fprintf(stdout, "NO: TOTAL_UNCORRECTABLE_TS_PACKETS\n");
  }

  if(r->hasParameter(NEST)){
      fprintf(stdout, "OK: NEST\n");
  }else{
      fprintf(stdout, "NO: NEST\n");
  }

  if(r->hasParameter(DEMODULATOR_GAIN)){
      fprintf(stdout, "OK: DEMODULATOR_GAIN\n");
  }else{
      fprintf(stdout, "NO: DEMODULATOR_GAIN\n");
  }

  if(r->hasParameter(DIGITAL_GAIN)){
      fprintf(stdout, "OK: DIGITAL_GAIN\n");
  }else{
      fprintf(stdout, "NO: DIGITAL_GAIN\n");
  }

  if(r->hasParameter(AGC)){
      fprintf(stdout, "OK: AGC\n");
  }else{
      fprintf(stdout, "NO: AGC\n");
  }

  if(r->hasParameter(AGCA)){
      fprintf(stdout, "OK: AGCA\n");
  }else{
      fprintf(stdout, "NO: AGCA\n");
  }

  if(r->hasParameter(AGCN)){
      fprintf(stdout, "OK: AGCN\n");
  }else{
      fprintf(stdout, "NO: AGCN\n");
  }

  if(r->hasParameter(GNYQA)){
      fprintf(stdout, "OK: GNYQA\n");
  }else{
      fprintf(stdout, "NO: GNYQA\n");
  }

  if(r->hasParameter(GFARA)){
      fprintf(stdout, "OK: GFARA\n");
  }else{
      fprintf(stdout, "NO: GFARA\n");
  }

  exit(0);
}

void init_novra_status(struct novra_status_st *nvstatus){
  /*
   * Initialize the global counters and the min, max values.
   */
  nvstatus->last = time(NULL);
  nvstatus->uncorrectables_total = 0;
  reinit_novra_status(nvstatus);
}

void reinit_novra_status(struct novra_status_st *nvstatus){
  /*
   * Initialize the only the the min, max values.
   */
  nvstatus->signal_strength_as_percentage_min = 255;
  nvstatus->signal_strength_as_percentage_max = 0;
  nvstatus->signal_strength_as_dbm_min = 100;
  nvstatus->signal_strength_as_dbm_max = -100;
  nvstatus->carrier_to_noise_min = 100.0;
  nvstatus->carrier_to_noise_max = 0.0;
  nvstatus->vber_min = 1.0;
  nvstatus->vber_max = 0.0;
}

void update_status(struct novra_status_st *nvstatus){
  /*
   * This is called after get_status() to update the min,max
   * values in the loging interval (when the loging interal
   * spans several device report cycles).
   */

  if(nvstatus->param.signal_strength_as_percentage <
     nvstatus->signal_strength_as_percentage_min){
    nvstatus->signal_strength_as_percentage_min =
      nvstatus->param.signal_strength_as_percentage;
  }

  if(nvstatus->param.signal_strength_as_percentage >
     nvstatus->signal_strength_as_percentage_max){
    nvstatus->signal_strength_as_percentage_max =
      nvstatus->param.signal_strength_as_percentage;
  }

  if(nvstatus->param.signal_strength_as_dbm <
     nvstatus->signal_strength_as_dbm_min){
    nvstatus->signal_strength_as_dbm_min =
      nvstatus->param.signal_strength_as_dbm;
  }

  if(nvstatus->param.signal_strength_as_dbm >
     nvstatus->signal_strength_as_dbm_max){
    nvstatus->signal_strength_as_dbm_max =
      nvstatus->param.signal_strength_as_dbm;
  }

  if(nvstatus->param.carrier_to_noise < nvstatus->carrier_to_noise_min)
    nvstatus->carrier_to_noise_min = nvstatus->param.carrier_to_noise;

  if(nvstatus->param.carrier_to_noise > nvstatus->carrier_to_noise_max)
    nvstatus->carrier_to_noise_max = nvstatus->param.carrier_to_noise;

  if(nvstatus->param.vber < nvstatus->vber_min)
    nvstatus->vber_min = nvstatus->param.vber;

  if(nvstatus->param.vber > nvstatus->vber_max)
    nvstatus->vber_max = nvstatus->param.vber;

  nvstatus->uncorrectables_total += nvstatus->param.uncorrectables;
}

int get_status(Receiver *r, struct novra_status_st *nvstatus){
  /*
   * Returns:
   * -1 => error from getStatus
   *  1 => unsupported device
   */

  if(r->getStatus() == false){
    nvstatus->status = -1;
    return(-1);
  }

  (void)r->updateStatus();	/* always return "true" */
  nvstatus->status = 0;
  memset(&nvstatus->param, 0, sizeof(struct novra_param_st));
  fill_novra_param(r, &nvstatus->param);

  return(0);
}

void print_status(struct novra_status_st *nvstatus, int f_longdisplay){

  if(isdevice_s75(nvstatus))
    print_status_s75(nvstatus, f_longdisplay);
  else if(isdevice_s75p(nvstatus))
    print_status_s75p(nvstatus, f_longdisplay);
  else if(isdevice_s200(nvstatus))
    print_status_s200(nvstatus, f_longdisplay);
  else if(isdevice_s300(nvstatus))
    print_status_s300(nvstatus, f_longdisplay);
  else
    warnx("Invalid value of nvstatus->device_type: %d", nvstatus->device_type);
}

void print_statusw(struct novra_status_st *nvstatus, int f_longdisplay){

  if(isdevice_s75(nvstatus))
    print_statusw_s75(nvstatus, f_longdisplay);
  else if(isdevice_s75p(nvstatus))
    print_statusw_s75p(nvstatus, f_longdisplay);
  else if(isdevice_s200(nvstatus))
    print_statusw_s200(nvstatus, f_longdisplay);
  else if(isdevice_s300(nvstatus))
    print_statusw_s300(nvstatus, f_longdisplay);
  else
    warnx("Invalid value of nvstatus->device_type: %d", nvstatus->device_type);
}

void log_status(const char *fname, struct novra_status_st *nvstatus,
		int f_longdisplay, int logperiod){

  if(isdevice_s75(nvstatus))
    log_status_s75(fname, nvstatus, f_longdisplay, logperiod);
  else if(isdevice_s75p(nvstatus))
    log_status_s75p(fname, nvstatus, f_longdisplay, logperiod);
  else if(isdevice_s200(nvstatus))
    log_status_s200(fname, nvstatus, f_longdisplay, logperiod);
  else if(isdevice_s300(nvstatus))
    log_status_s300(fname, nvstatus, f_longdisplay, logperiod);
  else
    log_warnx("Invalid value of nvstatus->device_type: %d",
	      nvstatus->device_type);
}

int isdevice_s75(struct novra_status_st *nvstatus){

  int t = nvstatus->device_type;

  if((t == R_S75_V1) || (t == R_S75_V2_1) || (t == R_S75_V3))
    return(1);

  return(0);
}

int isdevice_s75p(struct novra_status_st *nvstatus){

  int t = nvstatus->device_type;

  if((t == R_S75PLUS) || (t == R_S75PLUSPRO))
    return(1);

  return(0);
}

int isdevice_s200(struct novra_status_st *nvstatus){

  int t = nvstatus->device_type;

  if(t == R_S200)
    return(1);

  return(0);
}

int isdevice_s300(struct novra_status_st *nvstatus){

  int t = nvstatus->device_type;

  if(t == R_SJ300)
    return(1);

  return(0);
}

static void fill_novra_param(Receiver *r, struct novra_param_st *p){

  if(r->hasParameter(CARRIER_FREQUENCY)){
    p->carrier_frequency = r->getParameter(CARRIER_FREQUENCY).asLong();
  }

  if(r->hasParameter(SYMBOL_RATE)){
    p->symbol_rate = r->getParameter(SYMBOL_RATE).asLong();
  }

  if(r->hasParameter(LNB_FAULT)){
    p->lnb_fault = r->getParameter(LNB_FAULT).asLong();
  }

  if(r->hasParameter(SIGNAL_LOCK)){
    p->signal_lock = r->getParameter(SIGNAL_LOCK).asLong();
  }

  if(r->hasParameter(DATA_LOCK)){
    p->data_lock = r->getParameter(DATA_LOCK).asLong();
  }

  if(r->hasParameter(SIGNAL_STRENGTH_AS_PERCENTAGE)){
    p->signal_strength_as_percentage = 
      r->getParameter(SIGNAL_STRENGTH_AS_PERCENTAGE).asLong();
  }

  if(r->hasParameter(SIGNAL_STRENGTH_AS_DBM)){
    p->signal_strength_as_dbm = 
      r->getParameter(SIGNAL_STRENGTH_AS_DBM).asLong();

    /* See note in check_param.txt */
    if(r->hasParameter(SIGNAL_STRENGTH_AS_PERCENTAGE) == 0){
      if(p->signal_strength_as_dbm > -25){
	p->signal_strength_as_percentage = 100;
      }else{
	p->signal_strength_as_percentage = 2*(p->signal_strength_as_dbm + 75);
      }
    }
  }

  if(r->hasParameter(CARRIER_TO_NOISE)){
    p->carrier_to_noise = r->getParameter(CARRIER_TO_NOISE).asFloat();
  }

  if(r->hasParameter(VBER)){
    p->vber = r->getParameter(VBER).asFloat();
  }

  if(r->hasParameter(UNCORRECTABLES)){
    p->uncorrectables = r->getParameter(UNCORRECTABLES).asLong();
  }

  if(r->hasParameter(FREQUENCY_OFFSET)){
    p->frequency_offset = r->getParameter(FREQUENCY_OFFSET).asLong();
  }

  if(r->hasParameter(CLOCK_OFFSET)){
    p->clock_offset = r->getParameter(CLOCK_OFFSET).asLong();
  }

  if(r->hasParameter(DATA_SYNC_LOSS)){
    p->data_sync_loss = r->getParameter(DATA_SYNC_LOSS).asLong();
  }

  if(r->hasParameter(FRONT_END_LOCK_LOSS)){
    p->front_end_lock_loss = r->getParameter(FRONT_END_LOCK_LOSS).asLong();
  }

  if(r->hasParameter(FREQUENCY_ERROR)){
    p->frequency_error = r->getParameter(FREQUENCY_ERROR).asLong();
  }

  /*
   * Optional
   */

  if(r->hasParameter(ETHERNET_TRANSMIT)){
    p->ethernet_transmit = r->getParameter(ETHERNET_TRANSMIT).asLong();
  }

  if(r->hasParameter(ETHERNET_TRANSMIT_ERROR)){
    p->ethernet_transmit_error = 
      r->getParameter(ETHERNET_TRANSMIT_ERROR).asLong();
  }

  if(r->hasParameter(ETHERNET_RECEIVE)){
    p->ethernet_receive = r->getParameter(ETHERNET_RECEIVE).asLong();
  }

  if(r->hasParameter(ETHERNET_RECEIVE_ERROR)){
    p->ethernet_receive_error = 
      r->getParameter(ETHERNET_RECEIVE_ERROR).asLong();
  }

  if(r->hasParameter(ETHERNET_PACKET_DROPPED)){
    p->ethernet_packet_dropped = 
      r->getParameter(ETHERNET_PACKET_DROPPED).asLong();
  }

  if(r->hasParameter(TOTAL_ETHERNET_PACKETS_OUT)){
    p->total_ethernet_packets_out = 
      r->getParameter(TOTAL_ETHERNET_PACKETS_OUT).asLong();
  }

  if(r->hasParameter(DVB_ACCEPTED)){
    p->dvb_accepted = r->getParameter(DVB_ACCEPTED).asLong();
  }

  if(r->hasParameter(DVB_SCRAMBLED)){
    p->dvb_scrambled = r->getParameter(DVB_SCRAMBLED).asLong();
  }

  if(r->hasParameter(TOTAL_DVB_PACKETS_ACCEPTED)){
    p->total_dvb_packets_accepted = 
      r->getParameter(TOTAL_DVB_PACKETS_ACCEPTED).asLong();
  }

  if(r->hasParameter(TOTAL_DVB_PACKETS_RX_IN_ERROR)){
    p->total_dvb_packets_rx_in_error = 
      r->getParameter(TOTAL_DVB_PACKETS_RX_IN_ERROR).asLong();
  }

  if(r->hasParameter(TOTAL_UNCORRECTABLE_TS_PACKETS)){
    p->total_uncorrectable_ts_packets = 
      r->getParameter(TOTAL_UNCORRECTABLE_TS_PACKETS).asLong();
  }

  /*
   * Unused
   */

  if(r->hasParameter(NEST)){
    p->nest = r->getParameter(NEST).asLong();
  }

  if(r->hasParameter(DEMODULATOR_GAIN)){
    p->demodulator_gain = r->getParameter(DEMODULATOR_GAIN).asLong();
  }

  if(r->hasParameter(DIGITAL_GAIN)){
    p->digital_gain = r->getParameter(DIGITAL_GAIN).asLong();
  }

  if(r->hasParameter(AGC)){
    p->agc = r->getParameter(AGC).asLong();
  }

  if(r->hasParameter(AGCA)){
    p->agca = r->getParameter(AGCA).asLong();
  }

  if(r->hasParameter(AGCN)){
    p->agcn = r->getParameter(AGCN).asLong();
  }

  if(r->hasParameter(GNYQA)){
    p->gnyqa = r->getParameter(GNYQA).asLong();
  }

  if(r->hasParameter(GFARA)){
    p->gfara = r->getParameter(GFARA).asLong();
  }
}
