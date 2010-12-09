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
#include "err.h"
#include "logfile.h"
#include "s75_private.h"
#include "s75.h"

void print_status_s75(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s75 = &nvstatus->param;

  fprintf(stdout, "      Frequency: %d\n", s75->carrier_frequency);
  fprintf(stdout, "     SymbolRate: %d\n", s75->symbol_rate);
  fprintf(stdout, "       LNBFault: %s\n", lnbstr[s75->lnb_fault]);
  fprintf(stdout, "     SignalLock: %s\n", lockstr[s75->signal_lock]);
  fprintf(stdout, "       DataLock: %s\n", lockstr[s75->data_lock]);
  fprintf(stdout, " SignalStrength: %d\n", s75->signal_strength_as_percentage);
  fprintf(stdout, "           VBER: %.2e\n", s75->vber);
  fprintf(stdout, "      ErrorRate: %d\n", s75->uncorrectables);

  if(f_longdisplay != 0){
    fprintf(stdout, "   EthTxPackets: %d\n", s75->ethernet_transmit);
    fprintf(stdout, "  EthRcvPackets: %d\n", s75->ethernet_receive);
    fprintf(stdout, "    EthTxErrors: %d\n", s75->ethernet_transmit_error);
    fprintf(stdout, "  EthPacketDrop: %d\n", s75->ethernet_packet_dropped);
  }
}

void print_statusw_s75(struct novra_status_st *nvstatus, int f_longdisplay){

  struct novra_param_st *s75 = &nvstatus->param;
  static unsigned int uncorrectables = 0;

  uncorrectables += s75->uncorrectables;

  move(0,0);

  printw("      Frequency: %d\n", s75->carrier_frequency);
  printw("     SymbolRate: %d\n", s75->symbol_rate);
  printw("       LNBFault: %s\n", lnbstr[s75->lnb_fault]);
  printw("     SignalLock: %s\n", lockstr[s75->signal_lock]);
  printw("       DataLock: %s\n", lockstr[s75->data_lock]);
  printw(" SignalStrength: %d\n", s75->signal_strength_as_percentage);
  printw("           VBER: %.2e\n", s75->vber);
  printw("         Errors: %d\n", uncorrectables);
  printw("      ErrorRate: %d\n", s75->uncorrectables);

  if(f_longdisplay != 0){
    printw("   EthTxPackets: %d\n", s75->ethernet_transmit);
    printw("  EthRcvPackets: %d\n", s75->ethernet_receive);
    printw("    EthTxErrors: %d\n", s75->ethernet_transmit_error);
    printw("  EthPacketDrop: %d\n", s75->ethernet_packet_dropped);
  }

  refresh();
}

void log_status_s75(const char *fname, struct novra_status_st *nvstatus,
		    int f_longdisplay){

  struct novra_param_st *s75 = &nvstatus->param;
  FILE *f;
  time_t now;
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;

  uncorrectables_period += s75->uncorrectables;
  uncorrectables += s75->uncorrectables;

  now = time(NULL);

  f = logfile_fopen(fname);
  if(f == NULL)
    log_errn_open(fname);

  fprintf(f, "%u %d %d %d %d %.2e %u %u",
	  (unsigned int)now,
	  s75->lnb_fault,
	  s75->signal_lock,
	  s75->data_lock,
	  s75->signal_strength_as_percentage,
	  s75->vber,
	  uncorrectables_period, uncorrectables);

  if(f_longdisplay != 0){
    fprintf(f, " %d %d %d %d",
	    s75->ethernet_transmit,
	    s75->ethernet_receive,
	    s75->ethernet_transmit_error,
	    s75->ethernet_packet_dropped);
  }
  fprintf(f, "\n");

  logfile_fclose();

  uncorrectables_period = 0;
}
