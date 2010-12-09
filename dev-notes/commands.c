
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include "commands.h"

#include "RecComLib.h"
 

extern "C" {
#include "libcli.h"
}

#define build64long( dest, highDWORD, lowDWORD ) \
		memcpy( &dest, &highDWORD, sizeof(long) ) ; \
	memcpy( (LPBYTE)(&dest)+sizeof(long), &lowDWORD, sizeof(long) ) ; 
	
#define PrintIP(Mac) printf("%d.%d.%d.%d", Mac[0],Mac[1],Mac[2],Mac[3]);
#define PrintMAC(Mac) printf("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X", \
				Mac[0],Mac[1],Mac[2],Mac[3],Mac[4],Mac[5]);

#define CONNECT_TIMEOUT 4000

#define LOGIN_CHECK \
	if ( !loggedIn || hdev == NULL ) { \
       	printf("\nError: Not logged in to any device.  Use LOGIN command.\n"); \
	return CLI_ERROR ;}

#define CONNECT_CHECK \
	if ( !hdev->connected(CONNECT_TIMEOUT) ) { \
        printf("\nDevice is not responding.\n") ; \
        return CLI_ERROR ;}
			
extern  Receiver *hdev ;
extern  char deviceIPStr[20] ;
extern  bool loggedIn ;
//extern  Cmdline cmd ;

int getProgramNumber( int pid ) ;
int programIndex( int progNum ) ;
int vprogramIndex( int progNum ) ;
int camIndex( int progNum ) ;
int destinationIndex( const std::string destIP, int destPort ) ;
int vdestinationIndex( const std::string destIP, int destPort ) ;

const std::string rclFalse = "0" ;
const std::string rclTrue = "1" ;
const std::string nullPIDstr = "8191" ;
const std::string nullDestination("0.0.0.0") ;

bool validIP( const char *ipAddressString ) {
  unsigned int ip1, ip2, ip3, ip4, numAssigned = 0 ;

  if ( ipAddressString ) {
    numAssigned = sscanf( ipAddressString,"%d.%d.%d.%d", 
                          &ip1, &ip2, &ip3, &ip4 );
  }

  if ( numAssigned == 4 && ip1 > 0 && ip1 < 256 &&
       ip2 < 256 && ip3 < 256 && 
       ip4 > 0 && ip4 < 256 ) 
    return true ;

  return false ;
}

bool validMask( const char *ipMask ) {
  unsigned int m1, m2, m3, m4, numAssigned = 0 ;

  if ( ipMask ) {
    numAssigned = sscanf( ipMask,"%d.%d.%d.%d", 
                          &m1, &m2, &m3, &m4 );
  }

  if ( numAssigned == 4 && m1 > 0 && m1 < 256 &&
       m2 < 256 && m3 < 256 && m4 <= 252 ) 
    return true ;

  return false ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int pidIndex( int pid ) {

  CONNECT_CHECK ;
  int index = -1 ;
  int pidListSize = hdev->getParameter( SIZE_PID_LIST ).asShort() ;
 
  for ( int i = 0 ; i < pidListSize ; i++ ) {
    //printf("DEBUG: pid entry %d = %d\n", i, hdev->getIndexedParameter( PID, i ).asShort() ) ; 
    if ( pid == hdev->getIndexedParameter( PID, i, PENDING ).asShort() ) {
      index = i ;
      break ;
    }
  }
  return index ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int programGuideIndex( int pgm ) {

  CONNECT_CHECK ;
  int index = -1 ;
  int programGuideSize = hdev->getParameter( SIZE_PROGRAM_GUIDE ).asShort() ; 

  for ( int i = 0 ; i < programGuideSize ; i++ ) {
    if ( pgm == hdev->getIndexedParameter( PROGRAM_GUIDE_PROGRAM_NUMBER, i, PENDING ).asShort() ) {
      index = i ;
      break ;
    }
  }
  return index ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Login processing
int login_command( const char *ipAddressString, int timeout,
                   bool promptOK, const char *loginPassword ) {

  int retry = 3 ;
  int retValue = CLI_ERROR ;
  char password[9] ;
  ReceiverSearch rs ;
  NOVRA_ERRORS errCode = N_ERROR_FAILED ;

  if ( hdev != NULL ) {
    hdev->disconnect() ;
    delete hdev ;
    hdev = NULL ;
  }
  loggedIn = false ;

  deviceIPStr[0] = '\0' ;

  if ( !validIP( ipAddressString ) ) {
    printf("Invalid receiver IP address specified: %s.\n", ipAddressString) ;
    return CLI_ERROR ;  // no point in continuing processing switches
  }
  hdev = rs.getRemote( string(ipAddressString), timeout ) ;

  if ( hdev && hdev->connect( CONNECT_TIMEOUT, errCode ) ) {

    if ( loginPassword ) {
      // check if password is OK
      if ( hdev->login(string(loginPassword)) ) 
        loggedIn = true ;
      else
        if ( promptOK ) printf("\nInvalid password. %d attempts left\n", retry);
    }

    if (promptOK && !loggedIn) { // prompt for password
      while ( !loggedIn && retry -- ) {
        strncpy( password, getpass("Password: "), 8 ) ;
        password[8] = '\0' ;

        // check if password is OK
        if ( !(hdev->login(string(password))) ) 
          printf("\nInvalid password. %d attempts left\n", retry) ;
        else 
          loggedIn = true ;
      }
    }
    if ( loggedIn ) {
      std::string recIP = hdev->getParameter( RECEIVER_IP ).asString() ;
      strncpy(deviceIPStr, recIP.c_str(), 19) ;
      deviceIPStr[19] = '\0' ;
      hdev->getStatus() ;
      hdev->updateStatus() ;
      hdev->readCurrentSettings() ;
      retValue = CLI_OK ;
    }
  }
  else {
    printf("Unable to communicate with receiver at IP address %s\n.", ipAddressString) ;
    printf("Error code: %d\n.", errCode ) ;
  }

  return retValue ;
}


//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Change the IP Address of the receiver
int setip_command( const char *ipAddrStr, const char *netMaskStr ) {

  LOGIN_CHECK ;

  if ( !validIP( ipAddrStr ) || !validMask( netMaskStr ) ) {
    printf("Invalid receiver IP address or net mask specified.\n") ;
    return CLI_ERROR ;
  }
  else {

    CONNECT_CHECK ;

    hdev->getStatus() ;
    hdev->updateStatus() ;

    if ( ipAddrStr && netMaskStr &&
         hdev->setParameter( RECEIVER_IP, string(ipAddrStr) ) &&
         hdev->setParameter( SUBNET_MASK, string(netMaskStr) ) &&
         hdev->apply() ) {
      printf("\nIP address configuration OK \n");
      hdev->disconnect() ;
      delete hdev ;
      hdev = NULL ;
      loggedIn = false ;
    }
    else 
      printf("\nIP address configuration failed.\a \n");
  }
  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Set the default route (gateway) of receiver 
int gateway_command( const char * gatewayString ) {

  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;

  if ( !validIP( gatewayString ) ) {
    printf("Invalid gateway IP address specified.\n") ;
  }
  else {

    CONNECT_CHECK ;

    if ( hdev->setParameter( DEFAULT_GATEWAY_IP, string(gatewayString) ) &&
         hdev->apply() ) {
      printf("\nDefault gateway configuration OK \a \n");
      hdev->getStatus() ;
      hdev->updateStatus() ;
      retValue = CLI_OK ;
    }
    else {
      printf("\nDefault gateway configuration failed.\n");
    }
  }
  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Enable IGMP mode of operation on LAN interface 
int igmp_command( const char *indicator ) {

  int retValue = CLI_ERROR ;
// indicator should have value of "on" or "off"

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  string parmValue = "";
  bool on ;

  if ( strncasecmp(indicator, "on", 2) == 0 ) {
    parmValue = "1" ;
    on = true ;
  }
  else if ( strncasecmp(indicator, "off", 3) == 0 ) {
    parmValue = "0" ;
    on = false ;
  }

  if ( parmValue.length() && hdev->setParameter( IGMP_ENABLE, parmValue ) &&
       hdev->apply() ) {
    if ( on )
      printf("\nIGMP ON. \a \n");
    else
      printf("\nIGMP OFF. \a \n");

    hdev->getStatus() ;
    hdev->updateStatus() ;
    retValue = CLI_OK ;
  }
  else {
    printf("\nIGMP configuration failed.\n");
  }
  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Display current LAN interface settings including default gateway
int showlaninterface_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  printf("\n\tNetwork Interface Settings:\n");
  printf("\n\tReceiver MAC Address:\t\t%s", hdev->getParameter( RECEIVER_MAC ).asString().c_str() ) ;
  printf("\n\tReceiver IP:\t\t\t%s", hdev->getParameter( RECEIVER_IP ).asString().c_str() ) ;
  printf("\n\tReceiver Subnet Mask:\t\t%s", hdev->getParameter( SUBNET_MASK ).asString().c_str() ) ;
  printf("\n\tDefault Gateway IP address:\t%s", hdev->getParameter( DEFAULT_GATEWAY_IP ).asString().c_str() ) ;
  printf("\n\tUnicast Status Destination:\t%s:%s", hdev->getParameter( UNICAST_STATUS_IP ).asString().c_str(),
                                                   hdev->getParameter( UNICAST_STATUS_PORT ).asString().c_str() ) ;
  printf("\n\tBroadcast Status Port:\t\t%s", hdev->getParameter( BROADCAST_STATUS_PORT ).asString().c_str() ) ;

  if ( hdev->getParameter( IGMP_ENABLE ).asFlag() ) {
    printf("\n\tIGMP:\t\t\t\tON ");
  }
  else {
    printf("\n\tIGMP:\t\t\t\tOFF ");
  }

  printf("\n\n\tEthernet Packets out since boot: %s", hdev->getParameter( TOTAL_ETHERNET_PACKETS_OUT ).asString().c_str() ) ;


  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Display device information
int showdevice_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  //printf("\n\tDevice Information:\n");
  printf("\n\tDevice Type:\t%s", hdev->enumToString( DEVICE_TYPE, hdev->getParameter( DEVICE_TYPE ).asShort() ).c_str() ) ;
  printf("\n\tMAC Address:\t%s", hdev->getParameter( RECEIVER_MAC ).asString().c_str() ) ;
  //printf("\n\tUnit ID:\t\t%s", hdev->getParameter( UNIT_ID_CODE ).asString().c_str() ) ;
  printf("\n\tDSP Firmware:\tVer. %s, Rev. %s", 
         hdev->getParameter( DSP_VERSION ).asString().c_str(),
         hdev->getParameter( DSP_REVISION ).asString().c_str() ) ;
  //printf("\n\tRF Firmware Ver:\t\t%s", hdev->getParameter( RF_FIRMWARE_VERSION ).asString().c_str() ) ;
  if ( hdev->hasParameter( CAM_VERSION ) ) {
    printf("\n\tCAM Firmware:\tVer. %s, Rev. %s", 
           hdev->getParameter( CAM_VERSION ).asString().c_str(),
           hdev->getParameter( CAM_REVISION ).asString().c_str() ) ;
  }
  if ( hdev->hasParameter( FPGA_VERSION ) ) {
    printf("\n\tFPGA Firmware:\tVer. %s, Rev. %s\n", 
           hdev->getParameter( FPGA_VERSION ).asString().c_str(),
           hdev->getParameter( FPGA_REVISION ).asString().c_str() ) ;
  }

  return CLI_OK ;
}


//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Display current settings and signal quality measures for the sat interface
int showsatelliteinterface_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  int S2mode = hdev->getParameter( DVB_SIGNAL_TYPE ).asShort() ;

  printf("\tSatellite Interface Settings:\n");
  printf("\n\tReceiver MAC Address:\t%s", hdev->getParameter( RECEIVER_MAC ).asString().c_str() ) ;
  printf("\n\tReceiver Mode:\t\t%s", hdev->getParameter( DVB_SIGNAL_TYPE ).asString().c_str() ) ;
  printf("\n\tFrequency:\t\t%.1f MHz", hdev->getParameter( LBAND_FREQUENCY ).asFloat() ) ;
  printf("\n\tSymbol Rate:\t\t%.3f Msps", hdev->getParameter( SYMBOL_RATE ).asFloat()/1000.0 ) ;

/*
  int vrate = hdev->getParameter( VITERBI_RATE ).asShort() ;
  const char *fecString ;
  switch (vrate) {
    case 1: fecString = "1/2" ; break ;
    case 2: fecString = "2/3" ; break ;
    case 3: fecString = "3/4" ; break ;
    case 5: fecString = "5/6" ; break ;
    case 7: fecString = "7/8" ; break ;
    default: fecString = "Unknown" ;
  }
  printf("\n\tViterbi Rate:\t\t%s\n", fecString ) ;
*/

  if ( S2mode )
    printf("\n\tModCod:\t\t\t%s\n", hdev->getParameter( MODCOD ).asString().c_str() ) ;
  else
    printf("\n\tViterbi Rate:\t\t%s\n", hdev->getParameter( VITERBI_RATE ).asString().c_str() ) ;
  printf("\n\tSignal Lock:\t\t%s\n\tData Lock:\t\t%s\n", 
         hdev->getParameter( SIGNAL_LOCK ).asFlag() ? "On" : "Off",
         hdev->getParameter( DATA_LOCK ).asFlag() ? "On" : "Off" ) ;

  int uncorrectablesRate = (int)(hdev->getParameter( UNCORRECTABLES ).asLong() * 2 );
  if ( uncorrectablesRate < 254 )
    printf("\tUncorrectable Rate:\t%d/Second\n", uncorrectablesRate ) ;
  else
    printf("\tUncorrectable Rate:\t>255/Second\n") ;

  if ( S2mode )
    printf("\tPacket Error Rate:\t%-#4.4e\n", hdev->getParameter( PER ).asFloat() ) ;
  else
    printf("\tViterbi Bit Error Rate:\t%-#4.4e\n", hdev->getParameter( VBER ).asFloat() ) ;

  //printf("\n\tAGC: %ld, Demodulator Gain %ld\n", hdev->getParameter( AGC ).asLong(), hdev->getParameter( DEMODULATOR_GAIN ).asLong() ) ;

  if ( hdev->hasParameter( CARRIER_TO_NOISE ) ) {
    if ( hdev->getParameter( SIGNAL_LOCK ).asFlag() ) {
      float tempcnr = hdev->getParameter( CARRIER_TO_NOISE ).asFloat();
      if ( tempcnr >= 100.0 )
        printf("\n\tCarrier to Noise C/N:\t> 20dB\n") ;
      else
        printf("\n\tCarrier to Noise C/N:\t%.1fdB\n", tempcnr ) ;
    }
    else
        printf("\n\tCarrier to Noise C/N:\tN/A\n") ;
  }
  if ( hdev->hasParameter( SIGNAL_STRENGTH_AS_DBM ) )
    printf("\tSignal Strength:\t%ld dBm\n", hdev->getParameter( SIGNAL_STRENGTH_AS_DBM ).asLong() ) ;
  else
    printf("\tSignal Strength:\t%ld percent\n", hdev->getParameter( SIGNAL_STRENGTH_AS_PERCENTAGE ).asLong() ) ;
  // Front End Lock Loss: %d, Data Sync Loss: %d, DVB Accepted: %d", status.FrontEndLockLoss, status.DataSyncLoss, status.DVBAccepted );

  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Display current LNB settings and status
int showlnb_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

//  bool lnbPowered = hdev->getParameter( LNB_POWER ).asFlag() ;

//  if ( lnbPowered ) printf("LNB Settings\n");

  if ( hdev->getParameter( LNB_POWER ).asFlag() ) {
    printf("\n\tLNB Power:\t\tOn");

    if ( hdev->getParameter( LNB_FAULT ).asFlag() )
      printf("\n\tLNB Status:\t\tFault - short detected \a\a\n");
    else	
      printf("\n\tLNB Status:\t\tNormal\n");



    int hv = hdev->getParameter( HI_VOLTAGE_SWITCH ).asShort() ; 
    int psv = hdev->getParameter( POLARITY_SWITCHING_VOLTAGE).asShort() ;
    const char *vRangeStr = "Unknown" ;

    if ( hv == 0 ) {
      if ( psv == 0 ) vRangeStr = "13-18v" ; 
      else if ( psv == 1 ) vRangeStr = "11-15v" ;
    }
    else 
      if ( hv == 1 && psv == 0 ) vRangeStr = "21v" ;

    printf("\tVoltage Range:\t\t%s\n", vRangeStr ) ;
    printf("\tLong Line:\t\t%s\n", hdev->getParameter( LONG_LINE_COMPENSATION_SWITCH ).asFlag() ? "On" : "Off" ) ;
    //printf("\tHigh Voltage Mode:\t\t%d\n", status.Hi_VoltageMode ) ;
    printf("\tPolarization:\t\t%s\n", hdev->getParameter( POLARITY ).asShort() ? "Horizontal/Left" : "Vertical/Right") ;
    //if ( status.Polarity ) 
      //printf("\tLNB Power:\t\t15V\n") ;
    //else
      //printf("\tLNB Power:\t\t11V\n") ;

    printf("\t%s Tone:\t\t%s\n", hdev->getParameter( BAND_SWITCHING_TONE_FREQUENCY ).asShort() ? "44KHz" : "22KHz",
                               hdev->getParameter( BAND ).asFlag() ? "On" : "Off" ) ;
  }
  else {
    printf("\n\tLNB Power:\t\tOFF \n");
  }

  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Set destination port for broadcast status packets */
int bsp_command( int broadcastPort ) {

  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  bool portValid = false ;

  for ( int i = 0 ; i < ReceiverSearch::numAutoDetectPorts() ; i++ ) {
    if ( broadcastPort == ReceiverSearch::autoDetectPort(i) ) {
      portValid = true ;
      break ;
    }
  }

  if ( portValid ) {

    std::ostringstream parm ;
    parm << broadcastPort ;

    if ( hdev->setParameter( BROADCAST_STATUS_PORT, parm.str() ) &&
       hdev->apply() ) {
      hdev->getStatus() ;
      hdev->updateStatus() ;
      retValue = CLI_OK ;
      printf("\nBroadcast status port set.\n");
    }
    else 
      printf("\nBroadcast status port configuration failed.\n");
  }
  else {
    printf("\nInvalid port specified.  Must be one of:\n\t") ;
    for ( int i = 0 ; i < ReceiverSearch::numAutoDetectPorts() ; i++ ) 
      printf("%d ", ReceiverSearch::autoDetectPort(i) ) ;
    printf("\n") ;
  }

  return retValue ;
}


//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Set destination port for unicast status packets */
int usp_command( int unicastPort ) {

  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  bool portValid = false ;

  for ( int i = 0 ; i < ReceiverSearch::numAutoDetectPorts() ; i++ ) {
    if ( unicastPort == ReceiverSearch::autoDetectPort(i) ) {
      portValid = true ;
      break ;
    }
  }

  if ( portValid ) {

    std::ostringstream parm ;
    parm << unicastPort ;

    if ( hdev->setParameter( UNICAST_STATUS_PORT, parm.str() ) &&
         hdev->apply() ) {
      hdev->getStatus() ;
      hdev->updateStatus() ;
      retValue = CLI_OK ;
      printf("\nUnicast status port set.\n");
    }
    else
      printf("\nUnicast status port configuration failed.\n");
  }
  else {
    printf("\nInvalid port specified.  Must be one of:\n\t") ;
    for ( int i = 0 ; i < ReceiverSearch::numAutoDetectPorts() ; i++ ) 
      printf("%d ", ReceiverSearch::autoDetectPort(i) ) ;
    printf("\n") ;
  }

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
//Set destination address for unicast status packets 
int usa_command( const char *unicastAddr ) {

  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if ( !validIP( unicastAddr ) ) {
    printf("Invalid unicast status IP address specified.\n") ;
  }
  else {
    if ( hdev->setParameter( UNICAST_STATUS_IP, string(unicastAddr) ) && 
         hdev->apply() ) {
      printf("\nUnicast IP address set.\a \n");
      retValue = CLI_OK ;
    }
    else 
      printf("\nUnicast status address configuration failed.\a \n");
  }
  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Set symbol rate in mega symbols per second (MSPS) 
int sym_command( float symbolRate ) {

  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

// valid range is receiver model specific????
  if ( symbolRate < 1.5 || symbolRate > 45.0 ) {
    printf("\nInvalid symbol rate specified.  Valid range is 1.5 to 45.0 MSPS.\n") ;
    return CLI_ERROR ;
  }

  symbolRate = symbolRate * 1000.0 ;
  std::ostringstream parm ;
  parm << symbolRate ;

  if ( hdev->setParameter( SYMBOL_RATE, parm.str() ) &&
       hdev->apply() ) {
    hdev->getStatus() ;
    hdev->updateStatus() ;
    retValue = CLI_OK ;
    printf("\nSymbol rate set.\n");
  }
  else
    printf("\nSymbol rate configuration failed.\n");

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Sets the receive frequency in megahertz (MHz) 
int rfreq_command( float rfrequency ) {

  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if ( rfrequency < 950.0 || rfrequency > 2150.0 ) {
    printf("\nInvalid frequency specified.  Valid range is 950 to 2150 MHz.\n") ;
    return CLI_ERROR ;
  }

  std::ostringstream parm ;
  parm << rfrequency ;

  if ( hdev->setParameter( LBAND_FREQUENCY, parm.str() ) &&
       hdev->apply() ) {
    hdev->getStatus() ;
    hdev->updateStatus() ;
    retValue = CLI_OK ;
    printf("\nReceive frequency set.\n");
  }
  else
    printf("\nReceive frequency configuration failed.\n");

  return retValue ;

}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Deletes a DVB packet stream identifier (PID) from the list to be processed by the receiver 
int delpidmpe_command( int delpids[], int delpidCount ) {

  int pidIdx = -1 ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( PID ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  for ( int p = 0 ; p < delpidCount ; p++ ) {
    pidIdx = pidIndex( delpids[p] ) ;
    // if the pid is in the list and is not a raw pid, remove it
    if ( pidIdx >= 0 ) {
      if ( hdev->getIndexedParameter( RAW_FORWARD_FLAG, pidIdx ).asFlag() ) 
        printf("\nPID %d not deleted as it is not an MPE PID.\n", delpids[p] ) ;
      else {
        hdev->setIndexedParameter( PID, pidIdx, nullPIDstr ) ;
        hdev->setIndexedParameter( RAW_FORWARD_FLAG, pidIdx, rclTrue ) ;
        hdev->setIndexedParameter( DESTINATION_MASK, pidIdx, string("0") ) ;
        printf("\nRemoved pid %d\n", delpids[p] ) ;
      }
    }
    else {
      printf("\nCould not remove pid %d\n", delpids[p] ) ;
    }
  }
  return hdev->apply() ? CLI_OK : CLI_ERROR ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Specifies a DVB packet stream identifier (PID) to be processed by the S75 
int addpidmpe_command( int *newpid, int newpidCount ) {

  int retValue = CLI_ERROR ;
  int pidIdx = -1 ;
  bool cmdStatus ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( PID ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  for ( int p = 0 ; p < newpidCount ; p++ ) {
    pidIdx = pidIndex( newpid[p] ) ;
    if ( pidIdx >= 0 ) {
      printf("\nPID %d already in list.\n", newpid[p] ) ;
      continue ;
    }

    // add pid to next available slot
    pidIdx = pidIndex(8191) ;
    if ( pidIdx < 0 ) {
      printf("\nPID list is full.  %d not added.\n", newpid[p] ) ;
      break ;
    }

    if ( newpid[p] < 0 || newpid[p] > 8190 ) {
      printf("\nPID %d is an invalid identifier.  It must be between 0 and 8190.\n", newpid[p] ) ;
    }
    else { // add the PID to the the next available entry
      //printf("\nAdding pid %d\n", newpid[p] ) ;
      std::ostringstream parm ;
      parm << newpid[p] ;
      cmdStatus = hdev->setIndexedParameter( PID, pidIdx, parm.str() ) &&
                  hdev->setIndexedParameter( RAW_FORWARD_FLAG, pidIdx, rclFalse ) &&
                  hdev->setIndexedParameter( DESTINATION_MASK, pidIdx, string("0") ) ;
    }
    if ( cmdStatus && hdev->apply() ) {
      printf("PID list saved successfully.\n") ;
      retValue = CLI_OK ;
    }
    else {
      printf("\nError saving PID list.\n") ;
    }
  }
  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Display list of packet stream identifiers to be process by the Novra receiver
int showpids_command() {

  int pidCount, sizePidList ; 
  int pid ;
  bool mpe ;
  int pidsPerLine = 5 ;
  list<int> mpidList ;
  list<int> rpidList ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( PID ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  //printf("\n\tReceiver MAC Address:\t\t%s", hdev->getParameter( RECEIVER_MAC ).asString().c_str() ) ;
  //printf("\n\tReceiver IP:\t\t\t%s", hdev->getParameter( RECEIVER_IP ).asString().c_str() ) ;

  pidCount = hdev->getParameter( PID_COUNT ).asShort() ;
  sizePidList = hdev->getParameter( SIZE_PID_LIST ).asShort() ;

  if ( pidCount <= 0 )
    printf("\n\tPacket Stream Identifier (PID) list is empty.\n");
  else {
    printf("\n\tMPE PIDs being processed:");
    for ( int i = 0 ; i < sizePidList; i++ ) {
      pid = hdev->getIndexedParameter( PID, i ).asShort() ;
      mpe = !(hdev->getIndexedParameter( RAW_FORWARD_FLAG, i ).asFlag()) ;
      if ( mpe && pid != 8191 ) 
        mpidList.push_back( pid ) ;
    }
    mpidList.sort() ;
    for ( list<int>::iterator p = mpidList.begin() ; p != mpidList.end() ; p++ ) {
      if ( pidsPerLine-- <= 0 ) {
        pidsPerLine = 4 ; 
        printf("\n\t\t\t\t") ;
      }
      printf("\t%d", *p) ;
    }
    printf("\n");
    printf("\n\tPIDs being forwarded raw:");
    pidsPerLine = 5 ;
    for ( int i = 0 ; i < sizePidList; i++ ) {
      pid = hdev->getIndexedParameter( PID, i ).asShort() ;
      mpe = !(hdev->getIndexedParameter( RAW_FORWARD_FLAG, i ).asFlag()) ;
      if ( !mpe && pid != 8191 ) 
        rpidList.push_back( pid ) ;
    }
    rpidList.sort() ;
    for ( list<int>::iterator p = rpidList.begin() ; p != rpidList.end() ; p++ ) {
      if ( pidsPerLine-- <= 0 ) {
        pidsPerLine = 4 ; 
        printf("\n\t\t\t\t") ;
      }
      printf("\t%d", *p ) ;
    }
    printf("\n");
  }
  return CLI_OK ;
}



//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Display LAN and satellite interface statistics 
int showtraffic_command() {

  LOGIN_CHECK ; 
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  printf("\nLAN interface statistics\n") ;
  printf("\tTX:\t\t%ld/sec\n", 2 * hdev->getParameter( ETHERNET_TRANSMIT ).asLong() ) ;
  printf("\tRX:\t\t%ld/sec\n", 2 * hdev->getParameter( ETHERNET_RECEIVE ).asLong() ) ;
  printf("\tDropped:\t%ld/sec\n", 2 * hdev->getParameter( ETHERNET_PACKET_DROPPED ).asLong() ) ;
  printf("\tTXErr:\t\t%ld/sec\n", 2 * hdev->getParameter( ETHERNET_TRANSMIT_ERROR ).asLong() ) ;
  //printf("\tRXErr: %ld/sec\n", 2 * hdev->getParameter( ETHERNET_RECEIVE_ERROR ).asLong() ) ;

  printf("\n\tCumulative Ethernet Packets out: %s", hdev->getParameter( TOTAL_ETHERNET_PACKETS_OUT ).asString().c_str() ) ;

  printf("\n\nSatellite interface statistics\n") ;
  printf("\tCumulative DVB Packets Accepted:\t\t%s \n", hdev->getParameter( TOTAL_DVB_PACKETS_ACCEPTED ).asString().c_str() ) ;
  //printf("\tCumulative DVB Packets Received in Error:\t%s\n", hdev->getParameter( TOTAL_DVB_PACKETS_RX_IN_ERROR ).asString().c_str() );
  printf("\tCumulative Uncorrectable TS Packets:\t\t%s\n\n", hdev->getParameter( TOTAL_UNCORRECTABLE_TS_PACKETS ).asString().c_str() ) ;

  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Change the login password for the S75 to a new value 
int setpassword_command( const char *newpassword ) {

  int retValue = CLI_ERROR ;
  char password1[10], password2[10] ;
  std::string newPass = "";
  unsigned char key[] = {  0xfe, 0xc5, 0x5d, 0x1f,
                           0xe0, 0x67, 0x71, 0xa4,
                           0x28, 0xcd, 0x0c, 0x8e,
                           0x6d, 0x6e, 0xd0, 0x8c };


  LOGIN_CHECK ; 
  CONNECT_CHECK ;

  if ( newpassword )
    newPass = string(newpassword) ;
  else {
    strncpy( password1, getpass("New password: "), 9 ) ;
    strncpy( password2, getpass("Re-enter new password: "), 9 ) ;
    if ( strncmp( password1, password2, 8 ) != 0 ) 
      printf("\nPasswords do not match.") ;
    else
      newPass = string(password1) ;
  }

  if ( newPass.length() > 0 && hdev->changePassword( (char *)(newPass.c_str()), key ) ) {
      retValue = CLI_OK ;
      printf("\nPassword changed successfully.\n");
  }
  else 
      printf("\n\aError changing password.\n" );

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Tell the S75 to reboot 
int reboot_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if ( hdev->reset() )
    printf("\nReceiver reboot is complete.\n");
  else
    printf("\nReceiver is not responding.\n") ;

  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int logout_command() {

  if ( hdev ) {
    hdev->disconnect() ;
    delete hdev ;
    hdev = NULL ;
    loggedIn = false ;
  }
  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int lnbvoltage_command( const char *vrange ) { 

  int retValue = CLI_ERROR ;
  string psvString ;
  string highVoltageString = "0" ;
  

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if ( strncasecmp(vrange, "11-15v", 6) == 0 )
    psvString = "1" ;
  else if ( strncasecmp(vrange, "13-18v", 6) == 0 )
    psvString = "0" ;
  else if ( strncasecmp(vrange, "21v", 3) == 0 ) {
    psvString = "0" ;
    highVoltageString = "1" ;
  }
  else
    printf("\nInvalid voltage range.  Must be one of: 11-15v, 13-18v or 21v\n") ;
  
  if ( psvString.length() > 0 ) {
    if ( hdev->setParameter( HI_VOLTAGE_SWITCH, highVoltageString ) &&
         hdev->setParameter( POLARITY_SWITCHING_VOLTAGE, psvString ) &&
         hdev->apply() ) {
      retValue = CLI_OK ;
      printf("\nLNB voltage range changed to %s.\n", vrange) ;
    }
    else 
      printf("\nError changing LNB voltage range.\n") ;
  }

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int lnbtonefreq_command( const char *tf ) {

  int retValue = CLI_ERROR ;
  string parmStr ;
  
  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if ( strncasecmp(tf, "22KHz", 5) == 0 )
    parmStr = "0" ;
  else if ( strncasecmp(tf, "44KHz", 5) == 0 )
    parmStr = "1" ;
  else
    printf("\nInvalid tone frequency given.  Must be 22KHz or 44KHz.\n") ;
  
  if ( parmStr.length() > 0 ) {
    if ( hdev->setParameter( BAND_SWITCHING_TONE_FREQUENCY, parmStr ) &&
         hdev->apply() ) {
      retValue = CLI_OK ;
      printf("\nLNB tone frequency changed to %s\n", tf ) ;
    }
    else
      printf("\nError changing LNB tone frequency.\n") ;
  }
  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int lnbpower_command( bool on ) { 

  int retValue = CLI_ERROR ;
  string parmStr ;
  
  LOGIN_CHECK ;
  CONNECT_CHECK ;

  parmStr = on ? "1" : "0" ;
  if ( hdev->setParameter( LNB_POWER, parmStr ) &&
       hdev->apply() ) {
    retValue = CLI_OK ;
    printf("\nLNB powered %s.\n", on ? "on" : "off" ) ;
  }
  else
    printf("\nError changing LNB power.\n") ;

  return retValue ;
}
  
//000000000000000000000000000000000000000000000000000000000000000000000000000000
int linecompensation_command( bool on ) {

  int retValue = CLI_ERROR ;
  string parmStr ;
  
  LOGIN_CHECK ;
  CONNECT_CHECK ;

  parmStr = on ? "1" : "0" ;
  
  if ( hdev->setParameter( LONG_LINE_COMPENSATION_SWITCH , parmStr ) &&
       hdev->apply() ) {
      retValue = CLI_OK ;
      printf("\nLong line compensation turned %s.\n", on ? "on" : "off" ) ;
  }
  else 
      printf("\nError changing long line compensation.\n") ;

  return retValue ;
} 

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int lnbtone_command( bool on ) {

  int retValue = CLI_ERROR ;
  string parmStr ;
  
  LOGIN_CHECK ;
  CONNECT_CHECK ;

  parmStr = on ? "1" : "0" ;

  if ( hdev->setParameter( BAND, parmStr ) &&
       hdev->apply() ) {
      retValue = CLI_OK ;
      printf("\nLNB tone turned %s.\n", on ? "on" : "off" ) ;
  }
  else 
      printf("\nError changing LNB tone setting.\n") ;
  
  return retValue ;
} 

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int lnbpolarization_command( const char *polarization ) {

  int retValue = CLI_ERROR ;
  string parmStr("") ;
  
  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if ( strncasecmp(polarization, "vertical", 8) == 0  ||
       strncasecmp(polarization, "right", 5) == 0 )
    parmStr = "0" ;
  else if ( strncasecmp(polarization, "horizontal", 10) == 0 ||
            strncasecmp(polarization, "left", 4) == 0 )
    parmStr = "1" ;
  else
    printf("\nInvalid parameter.  Must specify one of: horizontal, vertical, left or right.\n") ;
  
  if ( parmStr.length() > 0 ) {
    if ( hdev->setParameter( POLARITY, parmStr ) &&
         hdev->apply() ) {
      retValue = CLI_OK ;
      printf("LNB polarization changed successfully.\n") ;
    }
    else
      printf("Error changing LNB polarization.\n") ;
  }
  return retValue ;
}
 
//000000000000000000000000000000000000000000000000000000000000000000000000000000
int list_command() {
  ReceiverSearch rs ;
  ReceiverList rl ;
  Receiver *r ;

  rs.discoverLocal( &rl ) ;

  if ( rl.count() == 0 ) 
    printf("No local receivers found.\n") ;
  else {
    for ( int i = 0 ; i < rl.count(); i++ ) {
      r = rl.getReceiver( i ) ;
      printf("%s	IP address: %s	MAC: %s\n", 
             (r->enumToString( DEVICE_TYPE, r->getParameter( DEVICE_TYPE ).asShort() )).c_str(),
             r->getParameter( RECEIVER_IP ).asString().c_str(),
             r->getParameter( RECEIVER_MAC ).asString().c_str() ) ;
    }
  }
  return CLI_OK ; 
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int receivermode_command( const char *recMode ) {

  int retValue = CLI_ERROR ;
  string parmStr("") ;
  char buf[10] ;
  
  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( DVB_SIGNAL_TYPE_CONTROL ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  if ( strncasecmp(recMode, "DVB-S2", 6) == 0 ||
       strncasecmp(recMode, "DVBS2", 5) == 0 )
    parmStr = itoa(DVBS2,buf,10) ; 
  else if ( strncasecmp(recMode, "DVB-S", 5) == 0 ||
            strncasecmp(recMode, "DVBS", 4) == 0 )
    parmStr = itoa(DVBS,buf,10) ;
  else if ( strncasecmp(recMode, "AUTO", 4) == 0 )
    parmStr = itoa(AUTO,buf,10) ;
  else
    printf("\nError: Invalid receiver mode specified.  Must one of DVB-S, \nDVBS, DVB-S2, DVBS2, or AUTO.\n") ;
  
  if ( parmStr.length() > 0 ) {
    if ( hdev->setParameter( DVB_SIGNAL_TYPE_CONTROL, parmStr ) &&
         hdev->apply() ) {
      retValue = CLI_OK ;
      printf("Receiver mode changed successfully.\n") ;
    }
    else
      printf("\nError changing receiver mode.\n") ;
  }
  return retValue ;
} 

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int showmap_command() {

  int numDestinations ;
  int pidListSize ;
  unsigned short destMask ;
  int pidsPerLine = 7 ;
  std::string destString ;
  int destPort ;
  list<int> pidList ;
  char buf[30] ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( PID_DESTINATION_IP ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  numDestinations = hdev->getParameter( SIZE_DESTINATION_LIST ).asShort() ;
  pidListSize = hdev->getParameter( SIZE_PID_LIST ).asShort() ;

  printf("\n\t   Destination\t\t\t\tPIDs\n\n") ;
  for ( int d = 0 ; d < numDestinations ; d++ ) {
    pidsPerLine = 7 ;
    destPort = hdev->getIndexedParameter( PID_DESTINATION_PORT, d ).asShort() ;
    destString = hdev->getIndexedParameter( PID_DESTINATION_IP, d ).asString() ;
    if ( destPort > 0 ) {
      sprintf(buf,"%s:%d", destString.c_str(), destPort ) ;
      printf("\t%-22s",buf) ;
      destMask = 0x8000 >> d ;
      pidList.clear() ;
      for ( int p = 0 ; p < pidListSize ; p++ ) {
        if ( destMask & hdev->getIndexedParameter( DESTINATION_MASK, p ).asShort() ) 
          pidList.push_back( hdev->getIndexedParameter( PID, p ).asShort() ) ;
      }
      pidList.sort() ;
      for ( list<int>::iterator pid = pidList.begin() ; pid != pidList.end() ; pid++ ) {
        if ( pidsPerLine-- <= 0 ) {
          pidsPerLine = 6 ; 
          printf("\n\t\t\t") ;
        }
        printf("%4d\t", *pid ) ;
      }
      printf("\n") ;
    }
  }
  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int delvideoprogram_command( const char* destIPAddr, int destPort, int pgmNum ) {

  int pidListSize ;
  int mappedPID  ;
  int destIdx ;
  int progNum ;
  unsigned short videoDestMask ;
  unsigned short pmtPIDDestMask = 0 ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( PID_DESTINATION_IP ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if ( !validIP( destIPAddr ) ) {
    printf("\nInvalid IP address destination specified for delete video commands.\n") ;
    return CLI_ERROR ;
  }

  if ( destPort < 1 || destPort > 65535 ) {
    printf("\nInvalid port destination specified for delete video command.\n" ) ;
    return CLI_ERROR ;
  }


  // get destination mask
  destIdx = destinationIndex( destIPAddr, destPort ) ;
  if ( destIdx < 0 ) {
    printf("\nError deleting video program.  IP destination not currently mapped.\n") ;
    return CLI_ERROR ;
  }

  videoDestMask = 0x8000 >> destIdx ;
  pidListSize = hdev->getParameter( SIZE_PID_LIST ).asShort() ;
  for ( int p = 0 ; p < pidListSize ; p++ ) {
    if ( videoDestMask & hdev->getIndexedParameter( DESTINATION_MASK, p ).asShort() ) {
      mappedPID = hdev->getIndexedParameter( PID, p ).asShort() ;
      progNum = getProgramNumber(mappedPID) ;
      if ( progNum > -1 ) {  // found a PMT pid, so this mapped destination is for video
        pmtPIDDestMask = hdev->getIndexedParameter( DESTINATION_MASK, p ).asShort() ;
      }
      unmappid_command( mappedPID, destIPAddr, destPort, false ) ;
    }
  }
  // if there are no more destinations for the program, delete the PAT entry and
  // CAM entry if it exists.
  if ( pmtPIDDestMask > 0 && (pmtPIDDestMask ^ videoDestMask) == 0 ) {
    // delete program from CAM table if an entry exists
    if ( camIndex( pgmNum ) > -1 ) 
      delcam_command( pgmNum, false ) ;

    // delete PAT entry for program if an entry exists
    if ( programIndex( pgmNum ) > -1 ) 
      delpat_command( pgmNum, false ) ;
  }
  return hdev->apply() ? CLI_OK : CLI_ERROR ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int showvideo_command() {

  int numDestinations ;
  int pidListSize ;
  unsigned short destMask ;
  int pidsPerLine = 7 ;
  std::string destString ;
  int destPort ;
  int progNum = -1 ;
  int mappedPID  ;
  list<int> pidList ;
  map<int,unsigned short> pgmMaskMap ;
  map<int,unsigned short>::iterator pMask ;

  map<int,string> pgmDestIPMap ;
  map<int,string>::iterator pDestIP ;

  map<int,unsigned short> pgmDestPortMap ;
  map<int,unsigned short>::iterator pDestPort ;

  multimap<int,int> pgmVideoIDMap ;
  multimap<int,int>::iterator pgm ;

  int camIdx ;
  string pgmStatus ;
  std::ostringstream pidPrintStream ;
  char buf[30] ;
  int videoID = 0 ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( PID_DESTINATION_IP ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  numDestinations = hdev->getParameter( SIZE_DESTINATION_LIST ).asShort() ;
  pidListSize = hdev->getParameter( SIZE_PID_LIST ).asShort() ;

  // Only print destinations if they forward a PID used by a program

  printf("\nProgram\t  Destination\t\t\t PIDs\t\t       CA   Status\n\n") ;

  for ( int d = 0 ; d < numDestinations ; d++ ) {
    destPort = hdev->getIndexedParameter( PID_DESTINATION_PORT, d ).asShort() ;
    destString = hdev->getIndexedParameter( PID_DESTINATION_IP, d ).asString() ;
    if ( destPort > 0 ) {
      destMask = 0x8000 >> d ;
      for ( int p = 0 ; p < pidListSize ; p++ ) {
        if ( destMask & hdev->getIndexedParameter( DESTINATION_MASK, p ).asShort() ) {
          mappedPID = hdev->getIndexedParameter( PID, p ).asShort() ;
          progNum = getProgramNumber(mappedPID) ;
          if ( progNum > -1 ) {  // found a PMT pid, so this mapped destination is for video
            pgmMaskMap.insert( make_pair(videoID, destMask) ) ;
            pgmDestIPMap.insert( make_pair(videoID, destString) ) ;
            pgmDestPortMap.insert( make_pair(videoID, destPort) ) ;
            pgmVideoIDMap.insert( make_pair(progNum, videoID++) ) ;
            break ;
          }
        }
      }
    }
  }

  for ( pgm = pgmVideoIDMap.begin(); pgm != pgmVideoIDMap.end() ; pgm++ ) {
    pidList.clear() ;
    printf("  %4d", pgm->first ) ;
    pDestIP = pgmDestIPMap.find(pgm->second) ;
    pDestPort = pgmDestPortMap.find(pgm->second) ;
    destMask = pgmMaskMap.find(pgm->second)->second ;
    sprintf(buf,"%s:%d", pDestIP->second.c_str(), pDestPort->second ) ;
    printf("\t%-22s",buf) ;
    pidsPerLine = 7 ;
    for ( int p = 0 ; p < pidListSize ; p++ ) {
      if ( destMask & hdev->getIndexedParameter( DESTINATION_MASK, p ).asShort() ) 
        pidList.push_back( hdev->getIndexedParameter( PID, p ).asShort() ) ;
    }

    pidList.sort() ;
    pidPrintStream.str("") ; // empty stream
    for ( list<int>::iterator p = pidList.begin() ; p != pidList.end() ; p++ ) {
      pidPrintStream.width( 6 ) ;
      pidPrintStream << *p ;
    }
    printf("%-28s", pidPrintStream.str().c_str() ) ;
    // check if the program is scrambled
    if ( (camIdx = camIndex( pgm->first )) > -1 ) {
      pgmStatus = hdev->getIndexedParameter( CA_PROGRAM_STATUS, camIdx ).asString() ;
      printf("\tY  %s", pgmStatus.c_str() ) ;
    }
    else
      printf("\tN  N/A  ") ;

    printf("\n") ;
  }
  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int vdestinationIndex( const std::string destIP, int destPort ) {

  int index = -1 ;
  int numDestinations = hdev->getParameter( SIZE_VPROGRAM_LIST ).asShort() ;

  if (!hdev->hasParameter( VPROGRAM_DESTINATION_IP ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  for ( int i = 0 ; i < numDestinations ; i++ ) {
    if ( destPort == 0 && hdev->getIndexedParameter( VPROGRAM_DESTINATION_PORT, i, PENDING ).asShort() <= 0 ) {
      index = i ;
      break ;
    }
    else if ( destIP == hdev->getIndexedParameter( VPROGRAM_DESTINATION_IP, i, PENDING ).asString() &&
         destPort == hdev->getIndexedParameter( VPROGRAM_DESTINATION_PORT, i, PENDING ).asShort() ) {
      index = i ;
      break ;
    }
  }
  return index ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int destinationIndex( const std::string destIP, int destPort ) {

  int index = -1 ;
  int numDestinations = hdev->getParameter( SIZE_DESTINATION_LIST ).asShort() ;

  for ( int i = 0 ; i < numDestinations ; i++ ) {
    if ( destPort == 0 && hdev->getIndexedParameter( PID_DESTINATION_PORT, i, PENDING ).asShort() <= 0 ) {
      index = i ;
      break ;
    }
    else if ( destIP == hdev->getIndexedParameter( PID_DESTINATION_IP, i, PENDING ).asString() &&
         destPort == hdev->getIndexedParameter( PID_DESTINATION_PORT, i, PENDING ).asShort() ) {
      index = i ;
      break ;
    }
  }
  return index ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int destinationCount() {

  int count = 0 ;
  int numDestinations = hdev->getParameter( SIZE_DESTINATION_LIST ).asShort() ;

  for ( int i = 0 ; i < numDestinations ; i++ ) {
    if (hdev->getIndexedParameter( PID_DESTINATION_PORT, i, PENDING ).asShort() > 0) {
      count++ ;
    }
  }
  return count ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int mappid_command( int pid, const char *ipAddr, int port, bool doApply ) {

  int maxDestinations ;
  std::string destString = ipAddr ;
  bool mpe = false ;
  int pidIdx ;
  int destIdx ;
  bool cmdStatus = true ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( PID_DESTINATION_IP ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if ( pid < 0 || pid > 8190 ) {
    printf("\nInvalid PID value specified for map pid command.\n" ) ;
    return CLI_ERROR ;
  }

  if ( !validIP( ipAddr ) ) {
    printf("\nInvalid IP address destination specified for map pid command.\n" ) ;
    return CLI_ERROR ;
  }

  if ( port < 1 || port > 65535 ) {
    printf("\nInvalid port destination specified for map pid command.\n" ) ;
    return CLI_ERROR ;
  }

  maxDestinations = hdev->getParameter( SIZE_DESTINATION_LIST ).asShort() ;

  if ( (destIdx = destinationIndex( string(ipAddr), port ))  < 0 &&
       destinationCount() >= maxDestinations ) {
      printf("\nDestination IP map table is full (max %d entries).\nCan't create map to new destination.\n", maxDestinations ) ;
      return CLI_ERROR ;
  }

  // check if pid is already in PID table
  // and is not an MPE pid
  if ( (pidIdx = pidIndex( pid )) >= 0 ) {
    mpe = !(hdev->getIndexedParameter( RAW_FORWARD_FLAG, pidIdx ).asFlag() ) ;
    if ( mpe ) {
      printf("\nPID %d is already specified as an MPE PID and cannot be mapped.\n\a", pid ) ;
      return CLI_ERROR ;
    }
  }

  // check if pid needs to be added to pid table
  if ( pidIdx < 0 ) { // add the PID to the the next available entry
    pidIdx = pidIndex( 8191 ) ;  // find first slot containing null pid

    if ( pidIdx < 0 ) {
      printf("\nCannot map another PID.  Maximum number PIDs already in use.\n" );
      return CLI_ERROR ;
    }
    else { // add the pid to pid list
      //printf("\nAdding pid %d at index %d.\n", pid, pidIdx );
      std::ostringstream parm ;
      parm << pid ;
      cmdStatus = hdev->setIndexedParameter( PID, pidIdx, parm.str() ) &&
      		  hdev->setIndexedParameter( RAW_FORWARD_FLAG, pidIdx, rclTrue ) ;
    }
  }

  // add new destination to destination table if necessary
  if ( cmdStatus && destIdx < 0 ) {
    destIdx = destinationIndex( string("0.0.0.0"), 0 ) ;
    std::ostringstream parm ;
    parm << port ;
    cmdStatus = hdev->setIndexedParameter( PID_DESTINATION_IP, destIdx, destString ) &&
    		hdev->setIndexedParameter( PID_DESTINATION_PORT, destIdx, parm.str() ) ;
  }

  unsigned short int destMask = 0x8000 >> destIdx ;
  unsigned short int currentDestMask = hdev->getIndexedParameter( DESTINATION_MASK, pidIdx, PENDING ).asShort() ;
  destMask = destMask | currentDestMask ;
  std::ostringstream parm ;
  parm << destMask ;
  //printf("\nSetting destination mask for destIdx %d pidIdx %d as %d\n", destIdx, pidIdx, destMask ) ;
  if ( cmdStatus )
    cmdStatus = hdev->setIndexedParameter( DESTINATION_MASK, pidIdx, parm.str() ) ;

  if ( (cmdStatus && !doApply) || (cmdStatus && hdev->apply()) ) {
    printf("PID mapped successfully.\n") ;
    return CLI_OK ;
  }
  else 
    printf("\nError mapping PID.\n") ;

  return CLI_ERROR ;
}


//000000000000000000000000000000000000000000000000000000000000000000000000000000
int unmappid_command( int pid, const char *ipAddr, int port, bool doApply ) { 

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if (!hdev->hasParameter( PID_DESTINATION_IP ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  if ( pid < 0 || pid > 8190 ) {
    printf("\nInvalid PID value specified for unmap pid command.\n" ) ;
    return CLI_ERROR ;
  }

  if ( !validIP( ipAddr ) ) {
    printf("\nInvalid IP address destination specified for unmap pid command: %s.\n", ipAddr ) ;
    return CLI_ERROR ;
  }

  if ( port < 0 || port > 65535 ) {
    printf("\nInvalid port destination specified for unmap pid command.\n" ) ;
    return CLI_ERROR ;
  }

  std::string destString = ipAddr ;
  int destIdx = destinationIndex( destString, port ) ;

  if ( destIdx < 0 ) {
    printf("\nNo destination for the given IP address and port exists.\n" ) ;
    return CLI_ERROR ;
  }
  
  int pidIdx = pidIndex( pid ) ;
  unsigned short destMask = 0x8000 >> destIdx ;
  unsigned short pidDestMask ;
  bool cmdStatus ;

  if ( pidIdx >= 0 )
    pidDestMask = hdev->getIndexedParameter( DESTINATION_MASK, pidIdx ).asShort() ;

  if ( pidIdx < 0 || 
       !(hdev->getIndexedParameter( RAW_FORWARD_FLAG, pidIdx ).asFlag()) ||
       !(destMask & pidDestMask) ) { 
    printf("\nPID not currently mapped to given destination.\n") ;
    return CLI_ERROR ;
  }

  // clear the PID's destination mask bit for the given destination
  
  unsigned short newMask = pidDestMask & ~destMask ; 
  //printf("\nNew mask is: %d\n", newMask ) ;

  if ( newMask == 0 ) { // delete the PID as it has no other destinations
    cmdStatus = hdev->setIndexedParameter( PID, pidIdx, nullPIDstr ) &&
                hdev->setIndexedParameter( RAW_FORWARD_FLAG, pidIdx, rclTrue ) &&
    		hdev->setIndexedParameter( DESTINATION_MASK, pidIdx, string("0") ) ; 
  }
  else { // clear the bit corresponding to the destination being unmapped
    std::ostringstream parm ;
    parm << newMask ;
    cmdStatus = hdev->setIndexedParameter( DESTINATION_MASK, pidIdx, parm.str() ) ;
  }

  // delete the destination if there are no more pids mapped to it

  bool stillMapped = false ;
  int pidListSize = hdev->getParameter( SIZE_PID_LIST ).asShort() ;

  for ( int p = 0 ; p < pidListSize ; p++ ) {
    //printf("pid %d (DM %d)\n", p, hdev->getIndexedParameter( DESTINATION_MASK, p ).asShort() ) ;
    if ( destMask & hdev->getIndexedParameter( DESTINATION_MASK, p, PENDING ).asShort() ) {
      stillMapped = true ;
      break ;
    }
  }
  if ( cmdStatus && !stillMapped ) {
    cmdStatus = hdev->setIndexedParameter( PID_DESTINATION_IP, destIdx, string("0.0.0.0") ) &&
    		hdev->setIndexedParameter( PID_DESTINATION_PORT, destIdx, string("0") ) ;
  }
  
  if ( (cmdStatus && !doApply) || ( cmdStatus && hdev->apply() ) ) {
    printf("Map entry successfully removed.\n") ;
    return CLI_OK ;
  }
  else
    printf("\nError: Map entry not removed successfully.\n") ;

  return CLI_ERROR ;
}


//000000000000000000000000000000000000000000000000000000000000000000000000000000
int programIndex( int progNum ) {

  CONNECT_CHECK ;
  int pgmIdx = -1 ;
  int patSize = hdev->getParameter( SIZE_MINI_PAT_LIST ).asShort() ;


  for ( int i = 0 ; i < patSize ; i++ ) {
    if ( progNum == hdev->getIndexedParameter( PROGRAM_NUMBER, i, PENDING ).asShort() ) {
      pgmIdx = i ;
      break ;
    }
  }
  return pgmIdx ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int vprogramIndex( int progNum ) {

  CONNECT_CHECK ;
  int pgmIdx = -1 ;
  int listSize = hdev->getParameter( SIZE_VPROGRAM_LIST ).asShort() ;


  for ( int i = 0 ; i < listSize ; i++ ) {
    if ( progNum == hdev->getIndexedParameter( VPROGRAM_NUMBER, i, PENDING ).asShort() ) {
      pgmIdx = i ;
      break ;
    }
  }
  return pgmIdx ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Get the program number for a given PMT PID.
int getProgramNumber( int pid ) {

  CONNECT_CHECK ;
  int pgmNum = -1 ;
  int patSize = hdev->getParameter( SIZE_MINI_PAT_LIST ).asShort() ;


  for ( int i = 0 ; i < patSize ; i++ ) {
    if ( pid == hdev->getIndexedParameter( PMT_PID, i, PENDING ).asShort() ) {
      pgmNum = hdev->getIndexedParameter( PROGRAM_NUMBER, i, PENDING ).asShort() ;
      break ;
    }
  }
  return pgmNum ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int showpat_command() { 

  int pgmNumber ;
  list<int> pgmList ;
  map<int,int> pmtPIDMap ;
  map<int,int>::iterator pgmEntry ;
  int pmtPID ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_MINI_PAT_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  int patSize = hdev->getParameter( SIZE_MINI_PAT_LIST ).asShort() ;

//printf("\nDEBUG:  patSize = %d\n", patSize ) ;

  printf("\nProgram Association Table Contents:\n\n") ;
  printf("\tProgram\t\t  PMT PID\n") ;
  for ( int i = 0 ; i < patSize ; i++ ) {
    //printf("\nDEBUG: Program number = %d", hdev->getIndexedParameter( PROGRAM_NUMBER, i ).asShort() ) ;
    pgmNumber = hdev->getIndexedParameter( PROGRAM_NUMBER, i ).asShort() ;
    if ( pgmNumber > 0 ) {
      pgmList.push_back( pgmNumber ) ;
      pmtPID = hdev->getIndexedParameter( PMT_PID, i ).asShort() ;
      pmtPIDMap.insert( make_pair(pgmNumber, pmtPID) ) ;
    }
  }
  pgmList.sort() ;
  for ( list<int>::iterator pgm = pgmList.begin() ; pgm != pgmList.end() ; pgm++ ) {
    pgmEntry = pmtPIDMap.find(*pgm) ;
    printf( "\t   %d\t\t   %d\n", *pgm, pgmEntry->second ) ;
  }

  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int showcam_command() { 

  int pgmNumber ;
  string pgmStatus ;
  list<int> pgmList ;
  map<int,string> pgmStatusMap ;
  map<int,string>::iterator pgmEntry ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  if (!hdev->hasParameter( CA_PROGRAM_NUMBER ) ) {
    printf("\nDevice does not support CAM functions.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  int camListSize = hdev->getParameter( SIZE_CA_PROGRAM_LIST ).asShort() ;

  printf("\nConditional Access Module\n\n") ;
  printf("    CAM Card Status: %s\n\n", hdev->getParameter( CAM_STATUS ).asString().c_str() ) ;
  printf("    CA Processed\tStatus\n") ;
  printf("      Programs\n\n") ;
  for ( int i = 0 ; i < camListSize ; i++ ) {
    pgmNumber = hdev->getIndexedParameter( CA_PROGRAM_NUMBER, i ).asShort() ;
    pgmStatus = hdev->getIndexedParameter( CA_PROGRAM_STATUS, i ).asString() ;
    if ( pgmNumber > 0 ) {
      pgmList.push_back( pgmNumber ) ;
      pgmStatusMap.insert( make_pair(pgmNumber, pgmStatus) ) ;
    }
  }
  pgmList.sort() ;
  for ( list<int>::iterator pgm = pgmList.begin() ; pgm != pgmList.end() ; pgm++ ) {
    pgmStatus = pgmStatusMap.find(*pgm)->second ;
    printf("\t%4d\t\t%s\n", *pgm, pgmStatus.c_str() ) ;
  }
  printf("\n") ;

  return CLI_OK ; 
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int addpat_command( int progNum, int pid, bool doApply ) {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_MINI_PAT_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;
 
  int pgmIdx = programIndex( progNum ) ;
 
  if ( pid < 0 || pid > 8190 ) {
    printf("\nPMT PID %d is an invalid identifier.  It must be between 0 and 8190.\n", pid ) ;
    return CLI_ERROR ;
  }

  if ( pgmIdx >= 0 ) {  // entry already exists for given program number 
    printf("\nProgram Association Table already has an entry for program number %d.\n", progNum ) ;
    return CLI_OK ;
  }
  pgmIdx = programIndex( 0 ) ;  // get next available slot for new entry
  if ( pgmIdx < 0 ) {  // table is full
    printf("\nProgram Association Table is full.  New entry not created.\n") ;
    return CLI_ERROR ;
  }

  std::ostringstream parm ;
  parm << progNum ;
  hdev->setIndexedParameter( PROGRAM_NUMBER, pgmIdx, parm.str() ) ;
  parm.str("") ;  // empty the string stream
  parm << pid ;

  bool cmdStatus = hdev->setIndexedParameter( PMT_PID, pgmIdx, parm.str() ) ;

  if ( (cmdStatus && !doApply) || (cmdStatus && hdev->apply()) ) {
    printf("Program entry added to PAT\n") ;
    return CLI_OK ; 
  }
  else {
    printf("\nError adding program entry to PAT.\n") ;
  }

  return CLI_ERROR ; 
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int delpat_command( int progNum, bool doApply ) { 
  
  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_MINI_PAT_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  int pgmIdx = programIndex( progNum ) ;

  if ( pgmIdx < 0 ) {
    printf("\nNo entry in PAT for program number %d.\n", progNum) ;
    return CLI_ERROR ;
  }

  bool cmdStatus = hdev->setIndexedParameter( PROGRAM_NUMBER, pgmIdx, string("0") ) &&
                   hdev->setIndexedParameter( PMT_PID, pgmIdx, string("0") ) ;
  if ( (cmdStatus && !doApply) || ( cmdStatus && hdev->apply()) ) {
    printf("Program entry deleted from PAT\n") ;
    return CLI_OK ;
  }
  else 
    printf("\nError deleting PAT entry.\n") ;
 
  return CLI_ERROR ; 
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int camIndex( int progNum ) {

  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  int camIdx = -1 ;
  int camListSize = hdev->getParameter( SIZE_CA_PROGRAM_LIST ).asShort() ;

  for ( int i = 0 ; i < camListSize ; i++ ) {
    if ( progNum == hdev->getIndexedParameter( CA_PROGRAM_NUMBER, i, PENDING ).asShort() ) {
      camIdx = i ;
      break ;
    }
  }

  return camIdx ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int addcam_command( int progNum, bool doApply ) {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if (!hdev->hasParameter( CA_PROGRAM_NUMBER ) ) {
    printf("\nError adding program number.  Device does not support CAM functions.\n") ;
    return CLI_ERROR ;
  }

  int camIdx = camIndex( progNum ) ;
  if ( camIdx >= 0 ) {
    printf("\nProgram number %d already in CAM list.\n", progNum) ;
    return CLI_OK ;
  }

  camIdx = camIndex( 0 ) ;  // find next available entry in CAM table.
  if ( camIdx < 0 ) {
    printf("\nCAM processing list is full.  Program number %d not added.\n", progNum ) ;
    return CLI_ERROR ;
  }

  std::ostringstream parm ;
  parm << progNum ;

  bool cmdStatus = hdev->setIndexedParameter( CA_PROGRAM_NUMBER, camIdx, parm.str() ) ;

  if ( (cmdStatus && !doApply) || (cmdStatus && hdev->apply() ) ) {
    printf("Program number added to CAM processing list.\n") ;
    return CLI_OK ;
  }
  else
    printf("\nError adding program number to CAM processing list.\n") ;

  return CLI_ERROR ; 
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int delcam_command( int progNum, bool doApply ) { 

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if (!hdev->hasParameter( CA_PROGRAM_NUMBER ) ) {
    printf("\nError deleting program number.  Device does not support CAM functions.\n") ;
    return CLI_ERROR ;
  }

  int camIdx = camIndex( progNum ) ;
  if ( camIdx < 0 ) {
    printf("\nProgram number %d not in CAM list.\n", progNum ) ;
    return CLI_ERROR ;
  }
  bool cmdStatus = hdev->setIndexedParameter( CA_PROGRAM_NUMBER, camIdx, string("0") ) ;

  if ( (cmdStatus && !doApply) || (cmdStatus && hdev->apply()) ) {
    printf("Program number removed from CAM processing list.\n") ;
    return CLI_OK ;
  }
  else
    printf("\nError removing program number from CAM processing list.\n") ;

  return CLI_ERROR ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int save_command( const char *fileName ) { 

  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if ( fileName ) {
    hdev->saveConfigToXML( string(fileName) ) ;
    retValue = CLI_OK ;
    printf("\nConfiguration saved to %s.\n", fileName) ;
  }
  else 
    printf("\nError saving configuration.\n");

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int load_command( const char *fileName ) {
  int retValue = CLI_ERROR ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if ( fileName && (hdev->loadConfigFromXML( string(fileName), true ) == 0 ) ) {
    hdev->setParameter( RECEIVER_IP, hdev->getParameter( RECEIVER_IP, CURRENT ).asString() ) ;
    hdev->setParameter( SUBNET_MASK, hdev->getParameter( SUBNET_MASK, CURRENT ).asString() ) ;
    hdev->setParameter( DEFAULT_GATEWAY_IP, hdev->getParameter( DEFAULT_GATEWAY_IP, CURRENT ).asString() ) ;
    hdev->apply() ;
    hdev->getStatus() ;
    hdev->updateStatus() ;
    retValue = CLI_OK ;
    printf("\nConfiguration loaded.\n") ; 
  }
  else {
    printf("\nError loading configuration from file.\n") ; 
  }

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int addvidwiz_command( const char* ipAddr, int port, int pgmNum, bool scrambled, 
                       int pmtPID, int videoPID, int audioPID, int pcrPID, 
                       int telexPID ) {

  int retValue = CLI_ERROR ;

  if (!hdev->hasParameter( PMT_PID ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  if ( scrambled ) {
    retValue = addcam_command( pgmNum, false ) ;
    if ( retValue != CLI_OK ) {
      printf("Error adding program to CAM processing list.\n") ;
      goto addvidwizfail ;
    }
  }

  retValue = addpat_command( pgmNum, pmtPID, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error creating PAT entry.\n") ;
    goto addvidwizfail ;
  }

  // map PMT pid
  retValue = mappid_command( pmtPID, ipAddr, port, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error mapping PMT PID.\n") ;
    goto addvidwizfail ;
  }

  // map video PID
  retValue = mappid_command( videoPID, ipAddr, port, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error mapping video PID.\n") ;
    goto addvidwizfail ;
  }

  // map audio PID
  retValue = mappid_command( audioPID, ipAddr, port, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error mapping audio PID.\n") ;
    goto addvidwizfail ;
  }

  // map PCR pid if it is not the same as the video PID
  if ( pcrPID != videoPID ) {
    retValue = mappid_command( pcrPID, ipAddr, port, false ) ;
    if ( retValue != CLI_OK ) {
      printf("Error mapping PCR PID.\n") ;
      goto addvidwizfail ;
    }
  }

  // map telex PID if it was specified
  if ( telexPID > 0 ) {
    retValue = mappid_command( telexPID, ipAddr, port, false ) ;
    if ( retValue != CLI_OK ) {
      printf("Error mapping telex PID.\n") ;
      goto addvidwizfail ;
    }
  }

  // try to apply the changes

  retValue = hdev->apply() ? CLI_OK : CLI_ERROR ;
  if ( retValue == CLI_OK ) {
    printf("Add Video command processed successfully.\n") ;
    return CLI_OK ;
  }
  else
    printf("\nError processing Add Video command.\n") ;

addvidwizfail:

  //delcam_command( pgmNum, true ) ;
  //delpat_command( pgmNum, true ) ;
  //unmappid_command( pmtPID, ipAddr, port, true ) ;
  //unmappid_command( videoPID, ipAddr, port, true ) ;
  //unmappid_command( audioPID, ipAddr, port, true ) ;
  //unmappid_command( pcrPID, ipAddr, port, true ) ;
  //unmappid_command( telexPID, ipAddr, port, true ) ;

  hdev->getStatus() ;
  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
int delvidwiz_command( const char* ipAddr, int port, int pgmNum, bool scrambled, 
                       int pmtPID, int videoPID, int audioPID, int pcrPID, 
                       int telexPID ) {

  int retValue = CLI_ERROR ;

  if (!hdev->hasParameter( PMT_PID ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  if ( scrambled ) {
    retValue = delcam_command( pgmNum, false ) ;
    if ( retValue != CLI_OK ) {
      printf("Error deleting program from CAM processing list.\n") ;
      goto delvidwizfail ;
    }
  }

  retValue = delpat_command( pgmNum, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error deleting PAT entry.\n") ;
    goto delvidwizfail ;
  }

  // map PMT pid
  retValue = unmappid_command( pmtPID, ipAddr, port, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error ummapping PMT PID.\n") ;
    goto delvidwizfail ;
  }

  // map video PID
  retValue = unmappid_command( videoPID, ipAddr, port, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error unmapping video PID.\n") ;
    goto delvidwizfail ;
  }

  // map audio PID
  retValue = unmappid_command( audioPID, ipAddr, port, false ) ;
  if ( retValue != CLI_OK ) {
    printf("Error unmapping audio PID.\n") ;
    goto delvidwizfail ;
  }

  // map PCR pid if it is not the same as the video PID
  if ( pcrPID != videoPID ) {
    retValue = unmappid_command( pcrPID, ipAddr, port, false ) ;
    if ( retValue != CLI_OK ) {
      printf("Error unmapping PCR PID.\n") ;
      goto delvidwizfail ;
    }
  }

  // map telex PID if it was specified
  if ( telexPID > 0 ) {
    retValue = unmappid_command( telexPID, ipAddr, port, false ) ;
    if ( retValue != CLI_OK ) {
      printf("Error unmapping telex PID.\n") ;
      goto delvidwizfail ;
    }
  }

  // try to apply the changes

  retValue = hdev->apply() ? CLI_OK : CLI_ERROR ;
  if ( retValue == CLI_OK )
    printf("Delete Video command processed successfully.\n") ;
  else
    printf("\nError processing Delete Video command.\n") ;

delvidwizfail:

  hdev->getStatus() ;
  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Output receiver status in an XML stream
int xmlStatus_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  hdev->statusToXML( stdout ) ;
  return CLI_OK ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Show contents of program guide
int showGuide_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;
 
  unsigned short program_number ;

  if (!hdev->hasParameter( SIZE_PROGRAM_GUIDE ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  // make sure we have a fresh copy of the program guide
  hdev->readStreamInfo() ;
  // probably should sort these by ascending program number
  if ( hdev->hasParameter( SIZE_PROGRAM_GUIDE ) ) {
    printf("\n\t\tProgram Guide Contents\n\n");
    //printf("\n\tProgram\n\n") ;
    for ( int i = 0; i < hdev->getParameter( SIZE_PROGRAM_GUIDE ).asShort(); i++ ) {
      program_number = hdev->getIndexedParameter( PROGRAM_GUIDE_PROGRAM_NUMBER, i, PENDING ).asShort();
      if ( program_number != 0 ) {
        printf("%d\t%-40s\t\n",
          program_number,
          hdev->getIndexedParameter(PROGRAM_GUIDE_PROGRAM_NAME,i,PENDING).asString().c_str() ) ;
      }
    }
    printf("\n");
  }
  else
    printf("\n\tProgram Guide not available on this receiver.\n");

  return CLI_OK ;
}


//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Show programs selected for processing
int showPrograms_command() {

  LOGIN_CHECK ;
  CONNECT_CHECK ;
 
  unsigned short program_number ;
  char buf[30] ;

  if (!hdev->hasParameter( SIZE_VPROGRAM_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->getStatus() ;
  hdev->updateStatus() ;

  if ( hdev->hasParameter( SIZE_VPROGRAM_LIST ) ) {
    //printf("\n\tVideo Programs\n");
    //printf("\n\tDestination\t\tProgram\n\t\t\t\tNumber\n");
    printf("\n\tDestination\t\tProgram\t\tCA Status\n\n");
    for ( int i = 0; i < hdev->getParameter( SIZE_VPROGRAM_LIST ).asShort(); i++ ) {
        program_number = hdev->getIndexedParameter( VPROGRAM_NUMBER, i ).asShort();
        if ( program_number != 0 ) {
          if ( hdev->getIndexedParameter( VPROGRAM_DESTINATION_PORT, i).asShort() == 0 )
            strcpy(buf, "Data") ;
          else {
            sprintf(buf,"%s:%d", 
              hdev->getIndexedParameter( VPROGRAM_DESTINATION_IP, i).asString().c_str(),
              hdev->getIndexedParameter( VPROGRAM_DESTINATION_PORT, i).asShort() ) ;
          }
          printf("\t%-22s\t%5d\t\t%s\n",
            buf,
            program_number,
            hdev->getIndexedParameter( CA_PROGRAM_SCRAMBLED_FLAG, i ).asString().c_str() ) ;
        }
    }
    printf("\n");
  }
  else
    printf("\n\tVideo program details not available on this receiver.\n");
  return CLI_OK ;
}


//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Specifies a video program to be forwarded by the receiver 
int addVProgram_command( int newPgm, const char *ipAddr, int port ) {

  int retValue = CLI_ERROR ;
  int destIdx = -1 ;
  bool cmdStatus ;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_VPROGRAM_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  if ( programGuideIndex( newPgm ) < 0 ) {
    printf("\nWARNING: Program number %d is not in program guide.\n", newPgm ) ;
    //return CLI_ERROR ;
  }
  // port will = 0 for a data program
  if ( port < 0 || port > 65535 ) {
    printf("\nInvalid port destination specified.\n" ) ;
    return CLI_ERROR ;
  }

  if ( port > 0 && !validIP( ipAddr ) ) {
    printf("\nInvalid IP address destination specified.\n" ) ;
    return CLI_ERROR ;
  }

  if ( port > 0 ) {  // video program being added
    destIdx = vdestinationIndex( ipAddr, port ) ;
    if ( destIdx >= 0 ) {
      unsigned short destPgm = hdev->getIndexedParameter( VPROGRAM_NUMBER, destIdx ).asShort() ;
      if ( newPgm == destPgm ) {
        printf("\nProgram is already being forwarded to the given destination.\n") ;
        retValue = CLI_OK ;
      }
      else {
        printf("\nDestination is already being used for program %d.\n", destPgm ) ;
        retValue = CLI_ERROR ;
      }
      return retValue ;
    }
  }
  else if ( vprogramIndex( newPgm ) >= 0 ) {
    printf("\nData program %d already being processed.\n", newPgm) ;
    return CLI_ERROR ;
  }

  // add Program to next available slot
  destIdx = vprogramIndex( 0 ) ;
  if ( destIdx < 0 ) {
    printf("\nVideo Program destination list is full.  %d not added.\n", newPgm ) ;
  }
  else { // add the destination and video program to the the next available entry
    std::ostringstream portString ;
    portString << port ;
    std::ostringstream pgmString ;
    pgmString << newPgm ;
    cmdStatus = hdev->setIndexedParameter( VPROGRAM_DESTINATION_IP, destIdx, string(ipAddr) ) &&
                hdev->setIndexedParameter( VPROGRAM_DESTINATION_PORT, destIdx, portString.str()) &&
                hdev->setIndexedParameter( VPROGRAM_NUMBER, destIdx, pgmString.str()) ;
  }
  if ( cmdStatus && hdev->apply() ) {
    printf("Program added successfully.\n") ;
    retValue = CLI_OK ;
  }
  else {
    printf("\nError adding program.\n") ;
  }

  return retValue ;
}

//000000000000000000000000000000000000000000000000000000000000000000000000000000
// Removes entries associated with forwarding a video program to a given
// destination.
int delVProgram_command( int pgm, const char *ipAddr, int port ) {

  int retValue = CLI_ERROR ;
  int destIdx = -1 ;
  bool cmdStatus = false;

  LOGIN_CHECK ;
  CONNECT_CHECK ;

  if (!hdev->hasParameter( SIZE_VPROGRAM_LIST ) ) {
    printf("\nCommand not available for this receiver model.\n") ;
    return CLI_ERROR ;
  }

  hdev->updateStatus() ;
  hdev->readCurrentSettings() ;

  //if ( programGuideIndex( newPgm ) < 0 ) {
  //  printf("\nProgram number %d is not in program guide.\n", newPgm ) ;
  //  return CLI_ERROR ;
  //}

  if ( port < 0 || port > 65535 ) {
    printf("\nInvalid port destination specified.\n" ) ;
    return CLI_ERROR ;
  }

  if ( port > 0 && !validIP( ipAddr ) ) {
    printf("\nInvalid IP address destination specified.\n" ) ;
    return CLI_ERROR ;
  }

  if ( port > 0 )  // deleting a video program
    destIdx = vdestinationIndex( ipAddr, port ) ;
  else  // deleting a data program
    destIdx = vprogramIndex( pgm ) ;

  if ( destIdx >= 0 ) {
    short destPgm = hdev->getIndexedParameter( VPROGRAM_NUMBER, destIdx ).asShort() ;
    if ( pgm == destPgm ) {

      cmdStatus = hdev->setIndexedParameter( VPROGRAM_NUMBER, destIdx, string("0") ) &&
                  hdev->setIndexedParameter( VPROGRAM_DESTINATION_IP, destIdx, string("0.0.0.0") ) &&
                  hdev->setIndexedParameter( VPROGRAM_DESTINATION_PORT, destIdx, string("0") ) ;
      if ( cmdStatus && hdev->apply() ) {
        if ( port == 0 ) // deleting a data program
          printf("\nData program %d removed.\n", pgm) ;
        else
          printf("\nProgram %d removed from given destination.\n", pgm) ;
        retValue = CLI_OK ;
      }
      else
        printf("\nError removing program %d.\n", pgm) ;
    }
    else {
      printf("\nProgram %d not being forwarded to %s:%d.\n", pgm, ipAddr, port ) ;
      retValue = CLI_ERROR ;
    }
  }

  return retValue ;
}
