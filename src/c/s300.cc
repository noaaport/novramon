/*
 * $Id$
 */
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <inttypes.h>
#include "err.h"
#include "logfile.h"
#include "s75_private.h"
#include "s300.h"

/*
 * See the file "check_param.txt" in dev-notes.
 */

void print_status_s300(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s300 = &nvstatus->param;

  fprintf(stdout, "      Frequency: %d\n", s300->carrier_frequency);
  fprintf(stdout, "     SymbolRate: %d\n", s300->symbol_rate);
  fprintf(stdout, "       LNBFault: %s\n", lnbstr[s300->lnb_fault]);
  fprintf(stdout, "     SignalLock: %s\n", lockstr[s300->signal_lock]);
  fprintf(stdout, "       DataLock: %s\n", lockstr[s300->data_lock]);
  fprintf(stdout, "      Signal(%%): %d\n",
	  s300->signal_strength_as_percentage);
  fprintf(stdout, "    Signal(dbm): %d\n",
	  s300->signal_strength_as_dbm);
  fprintf(stdout, "           VBER: %.2e\n", s300->vber);
  fprintf(stdout, "      ErrorRate: %d\n", s300->uncorrectables);
  fprintf(stdout, " CarrierToNoise: %.2f\n", s300->carrier_to_noise);
  fprintf(stdout, "   DataSyncLoss: %d\n", s300->data_sync_loss);
  fprintf(stdout, "FrequencyOffset: %d\n", s300->frequency_offset);

  /*
  fprintf(stdout, " FrequencyError: %d\n", s300->frequency_error);
  fprintf(stdout, "FrontEndLckLoss: %d\n", s300->front_end_lock_loss);
  fprintf(stdout, "DemodulatorGain: %d\n", s300->demodulator_gain);
  fprintf(stdout, "  NoiseEstimate: %d\n", s300->nest);
  fprintf(stdout, "    ClockOffset: %d\n", s300->clock_offset);
  fprintf(stdout, "    DigitalGain: %d\n", s300->digital_gain);
  */
 
  if(f_longdisplay != 0){    
    fprintf(stdout, "    EthTransmit: %d\n", s300->ethernet_transmit);
    fprintf(stdout, "     EthReceive: %d\n", s300->ethernet_receive);
    fprintf(stdout, "EthReceiveError: %d\n", s300->ethernet_receive_error);
    fprintf(stdout, "  EthPacketDrop: %d\n", s300->ethernet_packet_dropped);
    
    fprintf(stdout, "    DVBAccepted: %d\n", s300->dvb_accepted);
    fprintf(stdout, "   DVBScrambled: %d\n", s300->dvb_scrambled);
  }

  fprintf(stdout, "\n");
}

void print_statusw_s300(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s300 = &nvstatus->param;
  static unsigned int uncorrectables = 0;

  uncorrectables += s300->uncorrectables;

  move(0,0);

  printw("      Frequency: %d\n", s300->carrier_frequency);
  printw("     SymbolRate: %d\n", s300->symbol_rate);
  printw("       LNBFault: %s\n", lnbstr[s300->lnb_fault]);
  printw("     SignalLock: %s\n", lockstr[s300->signal_lock]); 
  printw("       DataLock: %s\n", lockstr[s300->data_lock]);
  printw("      Signal(%%): %d\n", s300->signal_strength_as_percentage);
  printw("    Signal(dbm): %d\n", s300->signal_strength_as_dbm);
  printw("           VBER: %.2e\n", s300->vber);
  printw("         Errors: %u\n", uncorrectables);
  printw("      ErrorRate: %d\n", s300->uncorrectables);
  printw(" CarrierToNoise: %.2f\n", s300->carrier_to_noise);
  printw("   DataSyncLoss: %d\n", s300->data_sync_loss);
  printw("FrequencyOffset: %d\n", s300->frequency_offset);

  /*
  printw(" FrequencyError: %d\n", s300->frequency_error);
  printw("FrontEndLckLoss: %d\n", s300->front_end_lock_loss);
  printw("DemodulatorGain: %d\n", s300->demodulator_gain);
  printw("  NoiseEstimate: %d\n", s300->nest);
  printw("    ClockOffset: %d\n", s300->clock_offset);
  printw("    DigitalGain: %d\n", s300->digital_gain);
  */

  if(f_longdisplay != 0){
    printw("    EthTransmit: %d\n", s300->ethernet_transmit);
    printw("     EthReceive: %d\n", s300->ethernet_receive);
    printw("EthReceiveError: %d\n", s300->ethernet_receive_error);
    printw("  EthPacketDrop: %d\n", s300->ethernet_packet_dropped);

    printw("    DVBAccepted: %d\n", s300->dvb_accepted);
    printw("   DVBScrambled: %d\n", s300->dvb_scrambled);
  }

  refresh();
}

void log_status_s300(const char *fname, struct novra_status_st *nvstatus,
		     int f_longdisplay, int logperiod){

  struct novra_param_st *s300 = &nvstatus->param;
  FILE *f;
  time_t now;

  /* Update the derived (min, max) parameters */
  update_status_minmax(nvstatus);

  now = time(NULL);
  if((logperiod != 0) && (now < nvstatus->last + logperiod))
    return;

  nvstatus->last = now;

  f = logfile_fopen(fname);
  if(f == NULL)
    log_errn_open(fname);

  fprintf(f, "%" PRIuMAX " %d %d %d %d %.2e %u %u %d %.1f %d %d",
	  (uintmax_t)now,
	  s300->lnb_fault,
	  s300->signal_lock,
	  s300->data_lock,
	  s300->signal_strength_as_percentage,
	  s300->vber,
	  s300->uncorrectables,
	  nvstatus->uncorrectables_total,
	  s300->signal_strength_as_dbm,
	  s300->carrier_to_noise,
	  s300->data_sync_loss,
	  s300->frequency_offset);

  if(f_longdisplay != 0){
    fprintf(f, " %d %d %d %d %d %d",
	    s300->ethernet_transmit,
	    s300->ethernet_receive,
	    s300->ethernet_receive_error,
	    s300->ethernet_packet_dropped,
	    s300->dvb_accepted,
	    s300->dvb_scrambled);
  }

  if(logperiod != 0){
    fprintf(f, " %d %d %.2e %.2e %d %d %.1f %.1f",
	    nvstatus->signal_strength_as_percentage_min,
	    nvstatus->signal_strength_as_percentage_max,
	    nvstatus->vber_min,
	    nvstatus->vber_max,
	    nvstatus->signal_strength_as_dbm_min,
	    nvstatus->signal_strength_as_dbm_max,
	    nvstatus->carrier_to_noise_min,
	    nvstatus->carrier_to_noise_max);
  }
  fprintf(f, "\n");
  logfile_fclose();

  reinit_novra_status(nvstatus);
}
