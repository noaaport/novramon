/*
 * $Id: s75p.cc 40 2010-12-09 22:03:38Z jfnieves $
 */
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include "err.h"
#include "logfile.h"
#include "s75_private.h"
#include "s75p.h"

void print_status_s75p(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s75p = &nvstatus->param;

  fprintf(stdout, "      Frequency: %d\n", s75p->carrier_frequency);
  fprintf(stdout, "     SymbolRate: %d\n", s75p->symbol_rate);
  fprintf(stdout, "       LNBFault: %s\n", lnbstr[s75p->lnb_fault]);
  fprintf(stdout, "     SignalLock: %s\n", lockstr[s75p->signal_lock]);
  fprintf(stdout, "       DataLock: %s\n", lockstr[s75p->data_lock]);
  fprintf(stdout, " SignalStrength: %d\n",
	  s75p->signal_strength_as_percentage);
  fprintf(stdout, "           VBER: %.2e\n", s75p->vber);
  fprintf(stdout, "      ErrorRate: %d\n", s75p->uncorrectables);
  fprintf(stdout, "   DataSyncLoss: %d\n", s75p->data_sync_loss);
  fprintf(stdout, "FrequencyOffset: %d\n", s75p->frequency_offset);
  
  if(f_longdisplay != 0){
    fprintf(stdout, "    EthTransmit: %d\n", s75p->ethernet_transmit);
    fprintf(stdout, "     EthReceive: %d\n", s75p->ethernet_receive);
    fprintf(stdout, "EthReceiveError: %d\n", s75p->ethernet_receive_error);
    fprintf(stdout, "  EthPacketDrop: %d\n", s75p->ethernet_packet_dropped);
    
    fprintf(stdout, "    DVBAccepted: %d\n", s75p->dvb_accepted);
    fprintf(stdout, "   DVBScrambled: %d\n", s75p->dvb_scrambled);
  }

  fprintf(stdout, "\n");
}

void print_statusw_s75p(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s75p = &nvstatus->param;
  static unsigned int uncorrectables = 0;

  uncorrectables += s75p->uncorrectables;

  move(0,0);

  printw("      Frequency: %d\n", s75p->carrier_frequency);
  printw("     SymbolRate: %d\n", s75p->symbol_rate);
  printw("       LNBFault: %s\n", lnbstr[s75p->lnb_fault]);
  printw("     SignalLock: %s\n", lockstr[s75p->signal_lock]); 
  printw("       DataLock: %s\n", lockstr[s75p->data_lock]);
  printw("SignalStrength%%: %d\n", s75p->signal_strength_as_percentage);
  printw("           VBER: %.2e\n", s75p->vber);
  printw("         Errors: %d\n", uncorrectables);
  printw("      ErrorRate: %d\n", s75p->uncorrectables);
  printw("   DataSyncLoss: %d\n", s75p->data_sync_loss);
  printw("FrequencyOffset: %d\n", s75p->frequency_offset);

  if(f_longdisplay != 0){
    printw("    EthTransmit: %d\n", s75p->ethernet_transmit);
    printw("     EthReceive: %d\n", s75p->ethernet_receive);
    printw("EthReceiveError: %d\n", s75p->ethernet_receive_error);
    printw("  EthPacketDrop: %d\n", s75p->ethernet_packet_dropped);

    printw("    DVBAccepted: %d\n", s75p->dvb_accepted);
    printw("   DVBScrambled: %d\n", s75p->dvb_scrambled);
  }

  refresh();
}

void log_status_s75p(const char *fname, struct novra_status_st *nvstatus,
		     int f_longdisplay, int logperiod){

  struct novra_param_st *s75p = &nvstatus->param;
  FILE *f;
  time_t now;

  now = time(NULL);

  /*
   * This is called from the parent log_status()
   *
   * update_status_minmax(nvstatus);
   *
   * if((logperiod != 0) && (now < nvstatus->last + logperiod))
   *   return;
   */

  nvstatus->last = now;

  f = logfile_fopen(fname);
  if(f == NULL)
    log_errn_open(fname);

  fprintf(f, "%" PRIuMAX " %d %d %d %d %.2e %u %u %d %d",
	  (uintmax_t)now,
	  s75p->lnb_fault,
	  s75p->signal_lock,
	  s75p->data_lock,
	  s75p->signal_strength_as_percentage,
	  s75p->vber,
	  s75p->uncorrectables,
	  nvstatus->uncorrectables_total,
	  s75p->data_sync_loss,
	  s75p->frequency_offset);

  if(f_longdisplay != 0){
    fprintf(f, " %d %d %d %d %d %d",
	    s75p->ethernet_transmit,
	    s75p->ethernet_receive,
	    s75p->ethernet_receive_error,
	    s75p->ethernet_packet_dropped,
	    s75p->dvb_accepted,
	    s75p->dvb_scrambled);
  }

  if(logperiod != 0){
    fprintf(f, " %d %d %.2e %.2e",
	    nvstatus->signal_strength_as_percentage_min,
	    nvstatus->signal_strength_as_percentage_max,
	    nvstatus->vber_min,
	    nvstatus->vber_max);
  }
  fprintf(f, "\n");
  logfile_fclose();

  reinit_novra_status(nvstatus);
}
