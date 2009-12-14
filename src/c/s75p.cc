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
#include "s75p.h"

void print_status_s75p(struct novra_status_st *s75status, int f_longdisplay){

  struct S75Plus_Status *s75p = &s75status->s75p;

  fprintf(stdout, "      Frequency: %f\n", s75p->Frequency);
  fprintf(stdout, "     SymbolRate: %f\n", s75p->SymbolRate);
  fprintf(stdout, "       LNBFault: %s\n", lnbstr[s75p->LNBFault]);
  fprintf(stdout, "     SignalLock: %s\n", lockstr[s75p->SignalLock]);
  fprintf(stdout, "       DataLock: %s\n", lockstr[s75p->DataLock]);
  /* SignalStrength is deprecated in s75+ */
  fprintf(stdout, "SignalStrength%%: %hhu\n",
	  s75status->signal_strength_percentage);
  fprintf(stdout, "           VBER: %.2e\n", s75p->VBER);
  fprintf(stdout, "      ErrorRate: %hhu\n", s75p->Uncorrectables);
  fprintf(stdout, "DemodulatorGain: %hhu\n", s75p->DemodulatorGain);
  fprintf(stdout, "  NoiseEstimate: %hhu\n", s75p->NEST);
  fprintf(stdout, "   DataSyncLoss: %hu\n", s75p->DataSyncLoss);  
  fprintf(stdout, "    ClockOffset: %hhu\n", s75p->Clock_Off);
  /* The Freq_Err should really be a signed quantity */
  fprintf(stdout, " FrequencyError: %hd\n", s75p->Freq_Err);
  
  if(f_longdisplay != 0){
    /* fprintf(stdout, " CarrierToNoise: %.2f\n", s75p->CarrierToNoise); */
    /* fprintf(stdout, "    DigitalGain: %hhu\n", s75p->Digital_Gain); */
    
    fprintf(stdout, "    EthTransmit: %hu\n", s75p->EthernetTransmit);
    fprintf(stdout, "     EthReceive: %hu\n", s75p->EthernetReceive);
    fprintf(stdout, "EthReceiveError: %hu\n", s75p->EthernetReceiveError);
    fprintf(stdout, "  EthPacketDrop: %hu\n", s75p->EthernetPacketDropped);
    
    fprintf(stdout, "    DVBAccepted: %hu\n", s75p->DVBAccepted);
    fprintf(stdout, "   DVBScrambled: %hu\n", s75p->DVBScrambled);
    fprintf(stdout, " DVBDescrambled: %hu\n", s75p->DVBDescrambled);
    
    fprintf(stdout, "   NyquistFGain: %hhu\n", s75p->GNYQA);
    fprintf(stdout, " AntiAliasFGain: %hhu\n", s75p->GFARA);
    
    fprintf(stdout, "ADCMidPointCross: %hhu\n", s75p->ADC_MID);
    fprintf(stdout, "   ADCClipCount: %hhu\n", s75p->ADC_CLIP);
    fprintf(stdout, "  ClampingCount: %hhu\n", s75p->AA_CLAMP);
  }

  fprintf(stdout, "\n");
}

void print_statusw_s75p(struct novra_status_st *s75status, int f_longdisplay){

  struct S75Plus_Status *s75p = &s75status->s75p;
  static unsigned int uncorrectables = 0;

  uncorrectables += s75p->Uncorrectables;

  move(0,0);

  printw("      Frequency: %f\n", s75p->Frequency);
  printw("     SymbolRate: %f\n", s75p->SymbolRate);
  printw("       LNBFault: %s\n", lnbstr[s75p->LNBFault]);
  printw("     SignalLock: %s\n", lockstr[s75p->SignalLock]); 
  printw("       DataLock: %s\n", lockstr[s75p->DataLock]);
  printw("SignalStrength%%: %hhu\n", s75status->signal_strength_percentage);
  printw("           VBER: %.2e\n", s75p->VBER);
  printw("         Errors: %u\n", uncorrectables);
  printw("      ErrorRate: %hhu\n", s75p->Uncorrectables);
  printw("DemodulatorGain: %hhu\n", s75p->DemodulatorGain);
  printw("  NoiseEstimate: %hhu\n", s75p->NEST);
  printw("   DataSyncLoss: %hu\n", s75p->DataSyncLoss);
  printw("    ClockOffset: %hhu\n", s75p->Clock_Off);
  printw(" FrequencyError: %hd\n", s75p->Freq_Err);

  if(f_longdisplay != 0){
    /* printw(" CarrierToNoise: %.2f\n", s75p->CarrierToNoise); */
    /* printw("    DigitalGain: %hhu\n", s75p->Digital_Gain); */

    printw("    EthTransmit: %hu\n", s75p->EthernetTransmit);
    printw("     EthReceive: %hu\n", s75p->EthernetReceive);
    printw("EthReceiveError: %hu\n", s75p->EthernetReceiveError);
    printw("  EthPacketDrop: %hu\n", s75p->EthernetPacketDropped);

    printw("    DVBAccepted: %hu\n", s75p->DVBAccepted);
    printw("   DVBScrambled: %hu\n", s75p->DVBScrambled);
    printw(" DVBDescrambled: %hu\n", s75p->DVBDescrambled);

    printw("   NyquistFGain: %hhu\n", s75p->GNYQA);
    printw(" AntiAliasFGain: %hhu\n", s75p->GFARA);

    printw("ADCMidPointCross: %hhu\n", s75p->ADC_MID);
    printw("   ADCClipCount: %hhu\n", s75p->ADC_CLIP);
    printw("  ClampingCount: %hhu\n", s75p->AA_CLAMP);
  }

  refresh();
}

void log_status_s75p(const char *fname, struct novra_status_st *s75status,
		     int f_longdisplay){

  struct S75Plus_Status *s75p = &s75status->s75p;
  FILE *f;
  time_t now;
  static unsigned int uncorrectables = 0;
  static unsigned int uncorrectables_period = 0;

  uncorrectables_period += s75p->Uncorrectables;
  uncorrectables += s75p->Uncorrectables;

  now = time(NULL);

  f = logfile_fopen(fname);
  if(f == NULL)
    log_errn_open(fname);

  /*
   * Freq_Err should really be a _signed_ quantity.
   */
  fprintf(f, "%u %hhu %hhu %hhu %hhu %.2e %u %u %hhu %hhu %hu %hhu %hd",
	  (unsigned int)now,
	  s75p->LNBFault,
	  s75p->SignalLock,
	  s75p->DataLock,
	  s75status->signal_strength_percentage,
	  s75p->VBER,
	  uncorrectables_period, uncorrectables,
	  s75p->DemodulatorGain,
	  s75p->NEST,
	  s75p->DataSyncLoss, 
	  s75p->Clock_Off,
	  s75p->Freq_Err);

  if(f_longdisplay != 0){
    fprintf(f, " %hu %hu %hu %hu %hu %hu %hu %hhu %hhu %hhu %hhu %hhu",
	    s75p->EthernetTransmit,
	    s75p->EthernetReceive,
	    s75p->EthernetReceiveError,
	    s75p->EthernetPacketDropped,
	    s75p->DVBAccepted,
	    s75p->DVBScrambled,
	    s75p->DVBDescrambled,
	    s75p->GNYQA,
	    s75p->GFARA,
	    s75p->ADC_MID,
	    s75p->ADC_CLIP,
	    s75p->AA_CLAMP);
  }
  fprintf(f, "\n");
  logfile_fclose();

  uncorrectables_period = 0;
}
