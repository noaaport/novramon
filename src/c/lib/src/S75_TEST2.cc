#include<stdio.h>
#ifdef FREEBSD
#include <stdlib.h>
#else
#include<malloc.h>
#endif
#include <string.h>
#include "linux_windows_types.h"
#include <curses.h>
#include"s75.h"
#define PrintMAC(Mac) printf("%0.2X-%0.2X-%0.2X-%0.2X-%0.2X-%0.2X", Mac[0],Mac[1],Mac[2],Mac[3],Mac[4],Mac[5]);
#define PrintIP(Mac) printf("%d.%d.%d.%d", Mac[0],Mac[1],Mac[2],Mac[3]);

void userconfigureNetwork(S75_Handle hdev);
void userconfigureRF(S75_Handle hdev);
void DisplayStatus(S75_Handle hdev);
void userSetPID(S75_Handle hdev);
void userGetPID(S75_Handle hdev);
void userSetDVBMAC(S75_Handle hdev);

int main()
{
	Novra_DeviceList list ;
	int Errcode=0,i, v2index;
	discoverDevices(&list);
	S75_Handle hdev;
	S75D_NetSetting netset;
	char ch;

	printf("\n Found %d Device(s) \n",list.num_devices);
	v2index = -1;
	for(i=0; i<list.num_devices;i++){
		printf("\n Device No. %d, Type ->",i+1);
		switch(list.Device[i].DeviceType){

			case S75D_V21:
				printf(" S75 Version 2.1 ");
				break;
			case S75D_V3:
				printf(" S75 Version 3.0 ");
				break;
			default:
				printf(" UNKIOWN ");
				break;
		}
		printf("\n MAC Address --> ");
		PrintMAC(list.Device[i].DeviceMAC);
		printf(" <<<--->>> IP Address: ");
		PrintIP(list.Device[i].DeviceIP);
		printf(" Blapping status on 0x%X \n", list.Device[i].StatusPort);
		if(list.Device[i].DeviceType == S75D_V21) v2index = i;
	}

	printf(" \n Select Device to Control ");
	scanf("%d", &v2index);
	if(v2index > list.num_devices ) {
		printf("\n Invalid Selction.. Exiting");
		return -1;
		}
	v2index --;

	if( (hdev =openDevice(&(list.Device[v2index]),1000, Errcode ))== NULL ){
		printf("Open Device Failed, Error Code 0x%X ", Errcode );
		return -1;
	}
#ifdef LINUX
	GetDeviceType(hdev, (WORD &)Errcode);
#else
	GetDeviceType(hdev, (WORD)Errcode);
#endif
	//printf("GetDeviceType returned %d", Errcode);
	do{
		printf("\n----------------------------------------------------------------------------------------");
		printf("\n------------------------------------S75 Console-----------------------------------------");
		printf("\n----------------------------------------------------------------------------------------");
		printf("\n\t\t (1) Configure the Network Settings of the Device..");
		printf("\n\t\t (2) Display Device Status Message..");
		printf("\n\t\t (3) Configure RF parameters..");
		printf("\n\t\t (4) Configure contents (PID List)..");
		printf("\n\t\t (5) Display Contents..");
      printf("\n\t\t (6) Reset the Device..");
		if(Errcode == S75D_V3)
      	printf("\n\t\t (7) Set DVB MAC..");

		printf("\n\t\t (8) Quit the console ");
		printf("\n\n\t\t  Select an Action: ");

		scanf("%d",&i);
		getchar();

		switch(i){
			case 1:
				userconfigureNetwork(hdev);
				break;
			case 2:
				DisplayStatus(hdev);
				break;
			case 3:
				userconfigureRF(hdev);
				break;
			case 4:
				userSetPID(hdev);
				break;
			case 5:
				userGetPID(hdev);
				break;
    		case 6:
         	resetS75(hdev);
				break;
    		case 7:{
      		if(Errcode == S75D_V3)
            	userSetDVBMAC(hdev);
            else
            	printf(" \n\n \t\t Invalid Selection \a\a\n\n");

				break;
            }
			case 8:
				break;
			default:
				printf(" \n\n \t\t Invalid Selection \a\a\n\n");
				break;
		}


	}	while(i!=8);

	closeS75(hdev);





	return 1;

}
void userconfigureNetwork(S75_Handle hdev)
{
	char userinput[120];
	S75D_NetSetting netset;
	bool version3  = false;
	S75D_NetSetting_V3 netset3;
	WORD devType;
	int i;
	GetDeviceType(hdev,devType);
	if(devType==S75D_V3) version3 = true;

	printf("-> Network configuration Section, \n   ------------------------------- \n ");
	printf("\tIf you would like to use the Default value for a parameter, \n Hit Enter instead of inputting a value");

	if(!version3){
			printf("\n-> Destination value for Raw traffic --Default 255.255.255.255--\n\t\t Enter new Value " );
			gets(userinput);
			if(strlen(userinput) < 6){
				netset.DestinationIP[0] = 0xff;		netset.DestinationIP[1] = 0xff;
				netset.DestinationIP[2] = 0xff;		netset.DestinationIP[3] = 0xff;
			}
			else{
			sscanf(userinput,"%d.%d.%d.%d",&netset.DestinationIP[0],&netset.DestinationIP[1],&netset.DestinationIP[2],&netset.DestinationIP[3]);
			}

			printf("\n-> Device Subnet Mask --Default 255.255.255.0--\n\t\t Enter new Value ");
			gets(userinput);

			if(strlen(userinput) < 6){
				netset.SubnetMask[0]= 0xff;		netset.SubnetMask[1]= 0xff;
				netset.SubnetMask[2]= 0xff;		netset.SubnetMask[3]= 0x00;
			} else{
				sscanf(userinput,"%d.%d.%d.%d",&netset.SubnetMask[0],&netset.SubnetMask[1],&netset.SubnetMask[2],&netset.SubnetMask[3]);
			}

			printf("\n-> Device Default Gatway IP address --Default 192.168.170.55--\n\t\t Enter new Value ");
			gets(userinput);

			if(strlen(userinput) < 6){
				netset.DefaultGatewayIP[0] = 192;		netset.DefaultGatewayIP[1] = 168;
				netset.DefaultGatewayIP[2] = 170;		netset.DefaultGatewayIP[3] = 55;
			} else{
				sscanf(userinput,"%d.%d.%d.%d",&netset.DefaultGatewayIP[0],&netset.DefaultGatewayIP[1],&netset.DefaultGatewayIP[2],&netset.DefaultGatewayIP[3]);
			}

			printf("\n-> Device Receiver IP Address --Default 192.168.170.54--\n\t\t Enter new Value ");
			gets(userinput);

			if(strlen(userinput) < 6){
				netset.ReceiverIP[0] = 192;		netset.ReceiverIP[1] = 168;
				netset.ReceiverIP[2] = 170;		netset.ReceiverIP[3] = 54;
			}else{
					sscanf(userinput,"%d.%d.%d.%d",&netset.ReceiverIP[0],&netset.ReceiverIP[1],&netset.ReceiverIP[2],&netset.ReceiverIP[3]);
			}
			netset.DestinationUDP = 20000;
			if(GetAvailablePort(netset.StatusPort)== S75_FAILED)
					printf("\n Get available Port Failed \n\n");
			else
					netset.StatusPort = 0x2075;

			printf(" \n\n Returned Port %X\n\n ", netset.StatusPort);

			if(configureNetwork(hdev, (LPBYTE)&netset)!= S75_OK)
				printf("\nNetwork Configuration failed \a \n");
			else
				printf("\nNetwork Configuration OK \a \n");
	}else{


			printf("\n-> Destination value for Unicast Status --Default 255.255.255.255(no unicast)--\n\t\t Enter new Value " );
			gets(userinput);
			if(strlen(userinput) < 6){
				netset3.UniCastStatusDestIP[0] = 0xff;		netset3.UniCastStatusDestIP[1] = 0xff;
				netset3.UniCastStatusDestIP[2] = 0xff;		netset3.UniCastStatusDestIP[3] = 0xff;
			}
			else{
			sscanf(userinput,"%d.%d.%d.%d",&netset3.UniCastStatusDestIP[0],&netset3.UniCastStatusDestIP[1],&netset3.UniCastStatusDestIP[2],&netset3.UniCastStatusDestIP[3]);
			}

			printf("\n-> Device Subnet Mask --Default 255.255.255.0--\n\t\t Enter new Value ");
			gets(userinput);

			if(strlen(userinput) < 6){
				netset3.SubnetMask[0]= 0xff;		netset3.SubnetMask[1]= 0xff;
				netset3.SubnetMask[2]= 0xff;		netset3.SubnetMask[3]= 0x00;
			} else{
				sscanf(userinput,"%d.%d.%d.%d",&netset3.SubnetMask[0],&netset3.SubnetMask[1],&netset3.SubnetMask[2],&netset3.SubnetMask[3]);
			}

			printf("\n-> Device Default Gatway IP address --Default 192.168.170.55--\n\t\t Enter new Value ");
			gets(userinput);

			if(strlen(userinput) < 6){
				netset3.DefaultGatewayIP[0] = 192;		netset3.DefaultGatewayIP[1] = 168;
				netset3.DefaultGatewayIP[2] = 170;		netset3.DefaultGatewayIP[3] = 55;
			} else{
				sscanf(userinput,"%hd.%hd.%hd.%hd",&netset3.DefaultGatewayIP[0],&netset3.DefaultGatewayIP[1],&netset3.DefaultGatewayIP[2],&netset3.DefaultGatewayIP[3]);
			}

			printf("\n-> Device Receiver IP Address --Default 192.168.170.54--\n\t\t Enter new Value ");
			gets(userinput);

			if(strlen(userinput) < 6){
				netset3.ReceiverIP[0] = 192;		netset3.ReceiverIP[1] = 168;
				netset3.ReceiverIP[2] = 170;		netset3.ReceiverIP[3] = 54;
			}else{
					sscanf(userinput,"%d.%d.%d.%d",&netset3.ReceiverIP[0],&netset3.ReceiverIP[1],&netset3.ReceiverIP[2],&netset3.ReceiverIP[3]);
			}

			printf("\n->  IGMP Filtering Enable/Disable (1/0)  \n\t\t Enter new Value ");
			scanf("%d",&i);
			if(i>0) netset3.IGMP_Enable = 1;
			else	netset3.IGMP_Enable = 0;


			if(GetAvailablePort(netset3.BCastStatusPort)== S75_FAILED)
					printf("\n Get available Port Failed \n\n");
			else
					netset3.BCastStatusPort = 0x2075;

			if(GetAvailablePort(netset3.UniCastStatusDestUDP)== S75_FAILED)
					printf("\n Get available Port Failed \n\n");
			else
					netset3.UniCastStatusDestUDP = 0x2075;

			if(configureNetwork(hdev, (LPBYTE)&netset3)!= S75_OK)
				printf("\nNetwork Configuration failed \a \n");
			else
				printf("\nNetwork Configuration OK \a \n");



	}

	return ;
}
void userconfigureRF(S75_Handle hdev)
{
	char userinput[120];
	S75D_RFSetting RFSet;

	printf("\n\n-> RF Configuration Section \n   ---------------------------\n\t To use the Default value for any parameter hit enter.");
	printf("\n\t\t Enter Frequency (MHz) [Default 1000]: ");
	gets(userinput);
	if(strlen(userinput)==0) RFSet.frequency=1000.0;
	else
		sscanf(userinput,"%f",&RFSet.frequency);
	if((RFSet.frequency > 2150) || (RFSet.frequency < 950) )
		RFSet.frequency = 1000;
	printf("\n\t\t Enter SympolRate (MSPS) [Default 20]: ");
	gets(userinput);
	if(strlen(userinput)==0) RFSet.symbolRate=20.0;
	else
		sscanf(userinput,"%f",&RFSet.symbolRate);
	if((RFSet.symbolRate > 45) || (RFSet.symbolRate < 2) )
		RFSet.symbolRate = 20;

	printf("\n\t\t LNB Power on or off (1== on, 0== off) [Default off]: ");
	gets(userinput);
	if(strlen(userinput)==0) RFSet.powerOn = 0;
	else
		sscanf(userinput,"%d",&RFSet.powerOn);
	if(RFSet.powerOn>0) RFSet.powerOn = 1;
	else RFSet.powerOn = 0;

	if(RFSet.powerOn == 1){
		printf("\n\t\t  Transponder Polarization  (1== H/LC, 0== V/RC) [Default V]: ");
		gets(userinput);
		if(strlen(userinput)==0) RFSet.polarity = 0;
		else
			sscanf(userinput,"%d",&RFSet.polarity);
		if(RFSet.polarity>0) RFSet.polarity = 1;
		else RFSet.polarity = 0;
	}

	if(RFSet.powerOn == 1){
		printf("\n\t\t LNB Band switching  (1== High, 0== Low) [Default L]: ");
		gets(userinput);
		if(strlen(userinput)==0) RFSet.band = 0;
		else
			sscanf(userinput,"%d",&RFSet.band);
		if(RFSet.band>0) RFSet.band = 1;
		else RFSet.band = 0;
	}
	if(RFSet.powerOn == 0){
		RFSet.band=0;
		RFSet.polarity = 0;
	}

	if(configureRF(hdev, (LPBYTE) &RFSet)== S75_OK)
		printf("\n\n RF configuration success \n");
	else
		printf("\n\n RF configuration Failed \n");
	return ;
}
// This function displays the status
void DisplayStatus(S75_Handle hdev){
	S75D_Status status;
	S75D_Status_V3 status3;
	int i;
	bool version3  = false;
	WORD devType;

	GetDeviceType(hdev,devType);
	if(devType==S75D_V3) version3 = true;
	if(!version3){
			if(getStatus(hdev, (LPBYTE)&status) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return;
			}
			printf("\n Device Firmware Ver %d, Rev. %d  ---   RF Firmware Codes 0x%X", status.FirmWareVer,status.FirmWareRev, status.RF_FIRMWARE);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Symbol Rate %f Msps \t Frequency %f", status.SymbolRate, status.Frequency);
			printf("\n The viterbi Bit Error Rate is %-0#4.4e \t Viterbi Rate %d",status.VBER, status.ViterbiRate);
			if(status.Power != 0){
				printf("\n LNB Power On\t Polarization = %d\t Band = %d",status.Polarity , status.Band);
			}
			else{
				printf("\n LNB Power OFF ");
			}
			if(status.LNBFault == 1){
				printf("\n\n LNB Fault ... \a\a");
			}
			printf("\n Signal Strength: %d, Signal Lock: %d, Data Lock: %d, Uncorrectables: %d", status.SignalStrength, status.SignalLock, status.DataLock,status.Uncorrectables);

			printf("\n -------------------------------------------------------------------------");
			printf("\n Netowrk Settings: \n Receiver IP:                "); PrintIP(status.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(status.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(status.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(status.DestinationIP);
			printf("\n Destination UDP Port:       0x%X", status.DestinationUDP);
			printf("\n Receiver MAC Address:       "); PrintMAC(status.ReceiverMAC);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d", status.EthernetTransmit, status.EthernetReceive,status.EthernetPacketDropped, status.EthernetTransmitError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Unit Identification Code: \n ");
			for(i=0;i<32;i++) printf("%0.2X ",status.UID[i]);
			printf("\n There is %d PID(s) in the PID List \n PID Lis:\n --------\n",status.PIDList.NumberofPIDs);
			for(i=0;i<status.PIDList.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",status.PIDList.Entry[i],status.PIDList.Control[i] );
	}else{
			if(getStatus(hdev, (LPBYTE)&status3) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return;
			}
			printf("\n Device Firmware Ver %d, Rev. %d  ---   RF Firmware Codes 0x%X", status3.FirmWareVer,status3.FirmWareRev, status3.RF_FIRMWARE);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Symbol Rate %f Msps \t Frequency %f", status3.SymbolRate, status3.Frequency);
			printf("\n The viterbi Bit Error Rate is %-0#4.4e \t Viterbi Rate %d",status3.VBER, status3.ViterbiRate);
			if(status3.Power != 0){
				printf("\n LNB Power On, Polarization = %d, Band = %d",status3.Polarity , status3.Band);
			}
			else{
				printf("\n LNB Power OFF ");
			}
			if(status3.LNBFault == 1){
				printf("\n\n LNB Fault ... \a\a");
			}
			printf("\n Signal Strength: %d, Signal Lock: %d, Data Lock: %d, Uncorrectables: %d", status3.SignalStrength, status3.SignalLock, status3.DataLock,status3.Uncorrectables);

			printf("\n -------------------------------------------------------------------------");
			printf("\n Netowrk Settings: \n Receiver IP:                "); PrintIP(status3.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(status3.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(status3.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(status3.UniCastStatusDestIP);
			printf("\n Destination UDP Port:       0x%X", status3.UniCastStatusDestUDP);
			printf("\n Receiver MAC Address:       "); PrintMAC(status3.ReceiverMAC);
			printf("\n Receiuver DVB MAC Address:  ");PrintMAC(status3.DVB_MACADDRESS);
			if(status3.IGMP_Enable > 0) printf("\n IGMP Filter is  ON ");
			else printf("\n IGMP Filter is  OFF ");
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d", status3.EthernetTransmit, status3.EthernetReceive,status3.EthernetPacketDropped, status3.EthernetTransmitError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Unit Identification Code: \n ");
			for(i=0;i<32;i++) printf("%0.2X ",status3.UID[i]);
			printf("\n There is %d PID(s) in the PID List \n PID Lis:\n --------\n",status3.PIDList.NumberofPIDs);
			for(i=0;i<status3.PIDList.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",status3.PIDList.Entry[i],status3.PIDList.Control[i] );

	}


}

void userSetPID(S75_Handle hdev)
{
	S75D_PIDList PIDs ;
	bool Continue_ = true;
	int i = 0, k;
	PIDs.NumberofPIDs = 0;
	while(PIDs.NumberofPIDs < 32){
		printf("\n Enter PID to add to the List (Decimal value) --> ");
		scanf("%d",&PIDs.Entry[PIDs.NumberofPIDs]);
		if(PIDs.Entry[PIDs.NumberofPIDs] > 8191){ printf("\n Entry Terminated "); break ;}
		printf("\n MPE Processing On, OFF (1= On, 0= Off) --> ");
		scanf("%d",&k);
		if(k==0) PIDs.Control[PIDs.NumberofPIDs] = 0;
		else PIDs.Control[PIDs.NumberofPIDs]=1;
		PIDs.NumberofPIDs ++;
		if(PIDs.NumberofPIDs == 32){ printf("\n Entry Terminated ");break ;}
		}
	printf("\n There is %d PID(s) in The entered PID List \n PID List:\n --------\n",PIDs.NumberofPIDs);
	for(i=0;i<PIDs.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",PIDs.Entry[i],PIDs.Control[i] );



//	printf("\n Sending the PID List returned %X\n", setPIDList(hdev, &PIDs));
}

void userGetPID(S75_Handle hdev)
{
	S75D_PIDList PIDs ;
	int i;
//	getPIDList(hdev,&PIDs);
	printf("\n There is %d PID(s) in the PID List \n PID Lis:\n --------\n",PIDs.NumberofPIDs);
	for(i=0;i<PIDs.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",PIDs.Entry[i],PIDs.Control[i] );


}
void userSetDVBMAC(S75_Handle hdev)
{
	char userinput[120];
   BYTE DVBMAC[6];
   
   printf("\n-------------------------------------------------------------\n");
   printf("\n Please Enter the DVB MAC Address to be used (Hex), use ':' to Seperate the bytes");
   scanf("%x:%x:%x:%x:%x:%x", &DVBMAC[0],&DVBMAC[1],&DVBMAC[2],&DVBMAC[3],&DVBMAC[4],&DVBMAC[5]);
   SetDVBMACAddress(hdev, DVBMAC);








}
