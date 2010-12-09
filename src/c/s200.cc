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
#include "err.h"
#include "logfile.h"
#include "s75_private.h"
#include "s200.h"

/*
 * See the file "check_param.txt" in dev-notes.
 */

void print_status_s200(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s200 = &nvstatus->param;

  fprintf(stdout, "      Frequency: %d\n", s200->carrier_frequency);
  fprintf(stdout, "     SymbolRate: %d\n", s200->symbol_rate);
  fprintf(stdout, "       LNBFault: %s\n", lnbstr[s200->lnb_fault]);
  fprintf(stdout, "     SignalLock: %s\n", lockstr[s200->signal_lock]);
  fprintf(stdout, "       DataLock: %s\n", lockstr[s200->data_lock]);
  fprintf(stdout, "      Signal(%%): %d\n",
	  s200->signal_strength_as_percentage);
  fprintf(stdout, "    Signal(dbm): %d\n",
	  s200->signal_strength_as_dbm);
  fprintf(stdout, "           VBER: %.2e\n", s200->vber);
  fprintf(stdout, "      ErrorRate: %d\n", s200->uncorrectables);
  fprintf(stdout, " CarrierToNoise: %.2f\n", s200->carrier_to_noise);
  fprintf(stdout, "   DataSyncLoss: %d\n", s200->data_sync_loss);
  fprintf(stdout, "FrequencyOffset: %d\n", s200->frequency_offset);

  /*
  fprintf(stdout, " FrequencyError: %d\n", s200->frequency_error);
  fprintf(stdout, "FrontEndLckLoss: %d\n", s200->front_end_lock_loss);
  fprintf(stdout, "DemodulatorGain: %d\n", s200->demodulator_gain);
  fprintf(stdout, "  NoiseEstimate: %d\n", s200->nest);
  fprintf(stdout, "    ClockOffset: %d\n", s200->clock_offset);
  fprintf(stdout, "    DigitalGain: %d\n", s200->digital_gain);
  */
 
  if(f_longdisplay != 0){    
    fprintf(stdout, "    EthTransmit: %d\n", s200->ethernet_transmit);
    fprintf(stdout, "     EthReceive: %d\n", s200->ethernet_receive);
    fprintf(stdout, "EthReceiveError: %d\n", s200->ethernet_receive_error);
    fprintf(stdout, "  EthPacketDrop: %d\n", s200->ethernet_packet_dropped);
    
    fprintf(stdout, "    DVBAccepted: %d\n", s200->dvb_accepted);
    fprintf(stdout, "   DVBScrambled: %d\n", s200->dvb_scrambled);
  }

  fprintf(stdout, "\n");
}

void print_statusw_s200(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s200 = &nvstatus->param;
  static unsigned int uncorrectables = 0;

  uncorrectables += s200->uncorrectables;

  move(0,0);

  printw("      Frequency: %d\n", s200->carrier_frequency);
  printw("     SymbolRate: %d\n", s200->symbol_rate);
  printw("       LNBFault: %s\n", lnbstr[s200->lnb_fault]);
  printw("     SignalLock: %s\n", lockstr[s200->signal_lock]); 
  printw("       DataLock: %s\n", lockstr[s200->data_lock]);
  printw("      Signal(%%): %d\n", s200->signal_strength_as_percentage);
  printw("    Signal(dbm): %d\n", s200->signal_strength_as_dbm);
  printw("           VBER: %.2e\n", s200->vber);
  printw("         Errors: %u\n", uncorrectables);
  printw("      ErrorRate: %d\n", s200->uncorrectables);
  printw(" CarrierToNoise: %.2f\n", s200->carrier_to_noise);
  printw("   DataSyncLoss: %d\n", s200->data_sync_loss);
  printw("FrequencyOffset: %d\n", s200->frequency_offset);

  /*
  printw(" FrequencyError: %d\n", s200->frequency_error);
  printw("FrontEndLckLoss: %d\n", s200->front_end_lock_loss);
  printw("DemodulatorGain: %d\n", s200->demodulator_gain);
  printw("  NoiseEstimate: %d\n", s200->nest);
  printw("    ClockOffset: %d\n", s200->clock_offset);
  printw("    DigitalGain: %d\n", s200->digital_gain);
  */

  if(f_longdisplay != 0){
    printw("    EthTransmit: %d\n", s200->ethernet_transmit);
    printw("     EthReceive: %d\n", s200->ethernet_receive);
    printw("EthReceiveError: %d\n", s200->ethernet_receive_error);
    printw("  EthPacketDrop: %d\n", s200->ethernet_packet_dropped);

    printw("    DVBAccepted: %d\n", s200->dvb_accepted);
    printw("   DVBScrambled: %d\n", s200->dvb_scrambled);
  }

  refresh();
}

void log_status_s200(const char *fname, struct novra_status_st *nvstatus,
		     int f_longdisplay){

  struct novra_param_st *s200 = &nvstatus->param;
  FILE *f;
  time_t now;
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;

  uncorrectables_period += s200->uncorrectables;
  uncorrectables += s200->uncorrectables;

  now = time(NULL);

  f = logfile_fopen(fname);
  if(f == NULL)
    log_errn_open(fname);

  fprintf(f, "%u %d %d %d %d %.2e %u %u %d %.1f %d %d",
	  (unsigned int)now,
	  s200->lnb_fault,
	  s200->signal_lock,
	  s200->data_lock,
	  s200->signal_strength_as_percentage,
	  s200->vber,
	  uncorrectables_period,
	  uncorrectables,
	  s200->signal_strength_as_dbm,
	  s200->carrier_to_noise,
	  s200->data_sync_loss,
	  s200->frequency_offset);

  if(f_longdisplay != 0){
    fprintf(f, " %d %d %d %d %d %d",
	    s200->ethernet_transmit,
	    s200->ethernet_receive,
	    s200->ethernet_receive_error,
	    s200->ethernet_packet_dropped,
	    s200->dvb_accepted,
	    s200->dvb_scrambled);
  }
  fprintf(f, "\n");
  logfile_fclose();

  uncorrectables_period = 0;
}
