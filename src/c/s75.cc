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

void print_status_s75(struct novra_status_st *s75status, int f_longdisplay){

  struct S75D_Status_V3 *s75 = &s75status->s75;

  fprintf(stdout, "      Frequency: %f\n", s75->Frequency);
  fprintf(stdout, "     SymbolRate: %f\n", s75->SymbolRate);
  fprintf(stdout, "       LNBFault: %s\n", lnbstr[s75->LNBFault]);
  fprintf(stdout, "     SignalLock: %s\n", lockstr[s75->SignalLock]);
  fprintf(stdout, "       DataLock: %s\n", lockstr[s75->DataLock]);
  fprintf(stdout, " SignalStrength: %hhu\n", s75->SignalStrength);
  fprintf(stdout, "           VBER: %.2e\n", s75->VBER);
  fprintf(stdout, "      ErrorRate: %hhu\n", s75->Uncorrectables);

  if(f_longdisplay != 0){
    fprintf(stdout, "   EthTxPackets: %hu\n", s75->EthernetTransmit);
    fprintf(stdout, "  EthRcvPackets: %hu\n", s75->EthernetReceive);
    fprintf(stdout, "    EthTxErrors: %hu\n", s75->EthernetTransmitError);
    fprintf(stdout, "  EthPacketDrop: %hu\n", s75->EthernetPacketDropped);
  }
}

void print_statusw_s75(struct novra_status_st *s75status, int f_longdisplay){

  struct S75D_Status_V3 *s75 = &s75status->s75;
  static unsigned int uncorrectables = 0;

  uncorrectables += s75->Uncorrectables;

  move(0,0);

  printw("      Frequency: %f\n", s75->Frequency);
  printw("     SymbolRate: %f\n", s75->SymbolRate);
  printw("       LNBFault: %s\n", lnbstr[s75->LNBFault]);
  printw("     SignalLock: %s\n", lockstr[s75->SignalLock]);
  printw("       DataLock: %s\n", lockstr[s75->DataLock]);
  printw(" SignalStrength: %hhu\n", s75->SignalStrength);
  printw("           VBER: %.2e\n", s75->VBER);
  printw("         Errors: %u\n", uncorrectables);
  printw("      ErrorRate: %hhu\n", s75->Uncorrectables);

  if(f_longdisplay != 0){
    printw("   EthTxPackets: %hu\n", s75->EthernetTransmit);
    printw("  EthRcvPackets: %hu\n", s75->EthernetReceive);
    printw("    EthTxErrors: %hu\n", s75->EthernetTransmitError);
    printw("  EthPacketDrop: %hu\n", s75->EthernetPacketDropped);
  }

  refresh();
}

void log_status_s75(const char *fname, struct novra_status_st *s75status,
		    int f_longdisplay){

  struct S75D_Status_V3 *s75 = &s75status->s75;
  FILE *f;
  time_t now;
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;

  uncorrectables_period += s75->Uncorrectables;
  uncorrectables += s75->Uncorrectables;

  now = time(NULL);

  f = logfile_fopen(fname);
  if(f == NULL)
    log_errn_open(fname);

  fprintf(f, "%u %hhu %hhu %hhu %hhu %.2e %u %u",
	  (unsigned int)now,
	  s75->LNBFault,
	  s75->SignalLock,
	  s75->DataLock,
	  s75->SignalStrength,
	  s75->VBER,
	  uncorrectables_period, uncorrectables);

  if(f_longdisplay != 0){
    fprintf(f, " %hu %hu %hu %hu",
	    s75->EthernetTransmit,
	    s75->EthernetReceive,
	    s75->EthernetTransmitError,
	    s75->EthernetPacketDropped);
  }
  fprintf(f, "\n");

  logfile_fclose();

  uncorrectables_period = 0;
}
