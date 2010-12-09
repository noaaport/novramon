#include <err.h>
#include "RecComLib.h"

/*
  hdev->getStatus() ;
  hdev->updateStatus() ;
  hdev->readCurrentSettings();

  Line 350 - commands.c
*/
#define CONNECT_TIMEOUT 4000

int main(void){

  char deviceIPStr[20];
  ReceiverSearch rs;
  ReceiverList rl;
  Receiver *r;
  int device_type;
  NOVRA_ERRORS err_code = N_ERROR_FAILED;

  rs.discoverLocal(&rl);

  if(rl.count() == 0)
    errx(1, "No local receivers found.\n");
  else if(rl.count() > 1) 
    errx(1, "Only one receiver is supported.\n");

  r = rl.getReceiver(0);
  device_type = r->getParameter(DEVICE_TYPE).asShort();

  fprintf(stdout, "%d %d\n", device_type, R_S200);
  fprintf(stdout, "%s\n", r->enumToString(DEVICE_TYPE, device_type).c_str());

  fprintf(stdout, "IP address: %s\n",
	  r->getParameter(RECEIVER_IP).asString().c_str());
  fprintf(stdout, "MAC: %s\n", 
         r->getParameter( RECEIVER_MAC ).asString().c_str());

  if(r->connect(CONNECT_TIMEOUT, err_code) == false)
    errx(1, "Cannot connect to device.");

  r->getStatus();
  r->updateStatus();

  fprintf(stdout, "%d\n", r->getParameter(CARRIER_FREQUENCY).asLong());
  fprintf(stdout, "%d\n", r->getParameter(SYMBOL_RATE).asLong());

  fprintf(stdout, "%d\n", r->getParameter(LNB_FAULT).asFlag());
  fprintf(stdout, "%d\n", r->getParameter(SIGNAL_LOCK).asFlag());
  fprintf(stdout, "%d\n", r->getParameter(DATA_LOCK).asFlag());

  fprintf(stdout, "%d\n",
	  r->getParameter(SIGNAL_STRENGTH_AS_PERCENTAGE).asLong());
  fprintf(stdout, "%d\n", r->getParameter(SIGNAL_STRENGTH_AS_DBM).asLong());
  fprintf(stdout, "dbm: %s\n", r->getParameter(SIGNAL_STRENGTH_AS_DBM).asString().c_str());
  fprintf(stdout, "%.1f\n",
	  r->getParameter(CARRIER_TO_NOISE).asFloat());

  fprintf(stdout, "%.2e\n", r->getParameter(VBER).asFloat());
  fprintf(stdout, "%d\n", r->getParameter(UNCORRECTABLES).asLong());

  fprintf(stdout, "%d\n", r->getParameter(FREQUENCY_OFFSET).asLong());
  fprintf(stdout, "%d\n", r->getParameter(CLOCK_OFFSET).asLong());
  fprintf(stdout, "%d\n", r->getParameter(DATA_SYNC_LOSS).asLong());
  fprintf(stdout, "%d\n", r->getParameter(FRONT_END_LOCK_LOSS).asLong());
  fprintf(stdout, "%d\n", r->getParameter(FREQUENCY_ERROR).asLong());

  r->disconnect();

  return(0) ; 
}
