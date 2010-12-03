//#include <stdafx.h>

#include<stdio.h>
#ifdef FREEBSD
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include <string.h>
//#include "linux_windows_types.h"
//#include <curses.h>
#include"s75.h"
#define PrintMAC(Mac) printf("%0.2X-%0.2X-%0.2X-%0.2X-%0.2X-%0.2X", Mac[0],Mac[1],Mac[2],Mac[3],Mac[4],Mac[5]);
#define PrintIP(Mac) printf("%d.%d.%d.%d", Mac[0],Mac[1],Mac[2],Mac[3]);

int logon(S75_Handle hdev);
int userconfigureNetwork(S75_Handle hdev);
int userconfigureRF(S75_Handle hdev);
int DisplayStatus(S75_Handle hdev);
int userSetPID(S75_Handle hdev);
int userGetPID(S75_Handle hdev);
int userSetDVBMAC(S75_Handle hdev);
int userGetPrograms(S75_Handle hdev);
int userSetPrograms(S75_Handle hdev);
int userGetPAT(S75_Handle hdev);
int userSetPAT(S75_Handle hdev);
int userGetTraps(S75_Handle hdev);
int userSetTraps(S75_Handle hdev);
int userChangePassword(S75_Handle hdev);


int main()
{
	Novra_DeviceList list ;
	Novra_DeviceEntry remote_device;
	int Errcode,i, v2index;
	S75_Handle hdev;
	char userinput[120];
	int b1, b2, b3, b4, b5, b6;
	int choice;
	int port;
	int timeout;
	//S75D_NetSetting netset;
	char ch;
	int result;

	discoverDevices(&list);

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
			case PesMaker:
				printf(" PesMaker ");
				break;
			case A75PesMaker:
				printf(" A75PesMaker ");
				break;
			case A75:
				printf(" A75 ");
				break;
			case S75_JLEO:
				printf(" S75 JLeo ");
				break;
			case S75PLUS:
				printf(" S75+ ");
				break;
			case S75PLUSPRO:
				printf(" S75+ Pro ");
				break;
			case S75CA:
				printf(" S75CA ");
				break;
			case S75CAPRO:
				printf(" S75CA Pro ");
				break;
			case S75FK:
				printf(" S75FK ");
				break;
			case S75FKPRO:
				printf(" S75FK Pro ");
				break;
			default:
				printf(" UNKOWN ");
				break;
		}
		printf("\n MAC Address --> ");
		PrintMAC(list.Device[i].DeviceMAC);
//		printf(" <<<--->>> IP Address: ");
		printf(", IP Address --> ");
		PrintIP(list.Device[i].DeviceIP);
		printf(" Sending status on 0x%X \n", list.Device[i].StatusPort);
		if(list.Device[i].DeviceType == S75D_V21) v2index = i;
	}

	printf(" \n Select Device to Control or Enter 0 to Specify a Remote Device: ");
	gets(userinput);
	sscanf(userinput,"%d", &v2index);
	if(v2index > list.num_devices ) {
		printf("\n Invalid Selction.. Exiting");
		return -1;
		}
	if ( v2index ) {
		v2index --;

		if( (hdev =openDevice(&(list.Device[v2index]),1000, Errcode ))== NULL ){
			printf("Open Device Failed, Error Code 0x%X ", Errcode );
			return -1;
		}
	} else {
		printf("\n IP Address of Remote Device --> " );
		gets(userinput);
		sscanf( userinput, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );
		remote_device.DeviceIP[0] = (unsigned char)b1;
		remote_device.DeviceIP[1] = (unsigned char)b2;
		remote_device.DeviceIP[2] = (unsigned char)b3;
		remote_device.DeviceIP[3] = (unsigned char)b4;
		printf("\n MAC Address of Remote Device --> " );
		gets(userinput);
		sscanf( userinput, "%x%c%x%c%x%c%x%c%x%c%x", &b1, &ch, &b2, &ch, &b3, &ch, &b4, &ch, &b5, &ch, &b6 );
		remote_device.DeviceMAC[0] = (unsigned char)b1;
		remote_device.DeviceMAC[1] = (unsigned char)b2;
		remote_device.DeviceMAC[2] = (unsigned char)b3;
		remote_device.DeviceMAC[3] = (unsigned char)b4;
		remote_device.DeviceMAC[4] = (unsigned char)b5;
		remote_device.DeviceMAC[5] = (unsigned char)b6;
		printf( "\n Select device type ...\n\n\t1) S75_JLEO\n\t2) S75CA\n\n Enter Choice --> " );
		scanf( "%d", &choice );
		if ( choice == 1 )
			remote_device.DeviceType = S75_JLEO;
		else if ( choice == 2 )
			remote_device.DeviceType = S75CA;
		else return -1;
		printf( "\n Enter the status port --> " );
		scanf( "%d", &port );
		if ( ( port > 0 ) && ( port < 65535 ) )
			remote_device.StatusPort = port;
		else
			return -1;
		timeout = 5000;
		if( ( hdev = openRemoteDevice( &remote_device, timeout, Errcode ) )== NULL ){
			printf("Open Device Failed, Error Code 0x%X ", Errcode );
			return -1;
		}
	}

	GetDeviceType(hdev, (WORD &)Errcode);
//	printf("GetDeviceType returned %d", Errcode);
	do{
		result = S75_FAILED;
		printf("\n------------------------------------------------------------------------------");
		printf("\n-------------------------------S75 Console------------------------------------");
		printf("\n------------------------------------------------------------------------------");
		if((Errcode == S75_JLEO)||
                   (Errcode == S75PLUS)||
                   (Errcode == S75PLUSPRO)||
                   (Errcode == S75CA)||
                   (Errcode == S75CAPRO)||
                   (Errcode == S75FK)||
                   (Errcode == S75FKPRO) )
		printf("\n\t\t (0) Log in to the Device.");
		printf("\n\t\t (1) Display Device Status Message.");
		printf("\n\t\t (2) Configure the Network Settings of the Device.");
		printf("\n\t\t (3) Configure RF Parameters.");
		printf("\n\t\t (4) Configure Content Forwarding (PID Routing).");
		printf("\n\t\t (5) Display Content Forwarding (PID Routing).");

		if ( ( Errcode == S75CA ) || ( Errcode == S75CAPRO ) ) {
	        printf("\n\t\t (6) Configure Conditional Access Programs.");
	        printf("\n\t\t (7) Display Conditional Access Programs.");
                }

		if ( ( Errcode == S75PLUSPRO ) ||
                     ( Errcode == S75CAPRO ) ||
                     ( Errcode == S75FKPRO ) ) {
	        printf("\n\t\t (8) Configure PAT.");
	        printf("\n\t\t (9) Display PAT.");
//	        printf("\n\t\t (10) Set Traps.");
//	        printf("\n\t\t (11) Get Traps.");
                }

/*
                if ( ( Errcode == S75PLUS ) ||
                     ( Errcode == S75PLUSPRO ) ||
                     ( Errcode == S75CA ) ||
                     ( Errcode == S75CAPRO ) ||
                     ( Errcode == S75FK ) ||
                     ( Errcode == S75FKPRO ) )  {
	        printf("\n\t\t (12) Change Password.");
		}
*/

                printf("\n\t\t (13) Reset the Device.");
		printf("\n\t\t (14) Quit the console ");
		if(Errcode == S75D_V3)
         	printf("\n\t\t (15) Set DVB MAC.");
		printf("\n\n\t\t Select an Action: ");

		scanf("%d",&i);
		getchar();

		switch(i){
			case 0:	if((Errcode == S75_JLEO)||
                                   (Errcode==S75PLUS)||
                                   (Errcode==S75PLUSPRO)||
                                   (Errcode==S75CA)||
                                   (Errcode==S75CAPRO)||
                                   (Errcode==S75FK)||
                                   (Errcode==S75FKPRO))
                			result = logon(hdev);
				else
                			printf(" \n\n \t\t Invalid Selection \a\a\n\n");
				break;
			case 1:
				result = DisplayStatus(hdev);
				break;
			case 2:
				result = userconfigureNetwork(hdev);
				break;
			case 3:
				result = userconfigureRF(hdev);
				break;
			case 4:
				result = userSetPID(hdev);
				break;
			case 5:
				result = userGetPID(hdev);
				break;
			case 6:{
      			       if ( (Errcode == S75CA) ||
      			            (Errcode == S75CAPRO) )
					result = userSetPrograms(hdev);
				else
            		        	printf(" \n\n \t\t Invalid Selection \a\a\n\n");
				break;
				}
			case 7:{
      			       if ( (Errcode == S75CA) ||
      			            (Errcode == S75CAPRO) )
					result = userGetPrograms(hdev);
				else
       			     		printf(" \n\n \t\t Invalid Selection \a\a\n\n");

				break;
				}
			case 8: {
				if ( ( Errcode == S75PLUSPRO )||
				     ( Errcode == S75CAPRO )||
			             ( Errcode == S75FKPRO ) )
					result = userSetPAT(hdev);
				else
            				printf(" \n\n \t\t Invalid Selection \a\a\n\n");

				break;
				}
			case 9: {
				if ( ( Errcode == S75PLUSPRO )||
				     ( Errcode == S75CAPRO )||
			             ( Errcode == S75FKPRO ) )
					result = userGetPAT(hdev);
				else
		            		printf(" \n\n \t\t Invalid Selection \a\a\n\n");

					break;
				}
			case 10: {
				if ( Errcode == S75CA )
					result = userSetTraps(hdev);
				else
            		printf(" \n\n \t\t Invalid Selection \a\a\n\n");

					break;
				}
/*
			case 11: {
				if ( Errcode == S75CA )
					result = userGetTraps(hdev);
				else
            		printf(" \n\n \t\t Invalid Selection \a\a\n\n");

					break;
				}
*/
/*
			case 12: {
				if ( Errcode == S75CA )
					result = userChangePassword(hdev);
				else
            		printf(" \n\n \t\t Invalid Selection \a\a\n\n");

					break;
				}
*/
    		case 13:
	         	result = resetS75(hdev);
				break;
			case 14:
					result = S75_OK;
					break;
    		case 15:{
		   		if(Errcode == S75D_V3)
			    	result = userSetDVBMAC(hdev);
				else
					printf(" \n\n \t\t Invalid Selection \a\a\n\n");

				break;
				}
			default:
				printf(" \n\n \t\t Invalid Selection \a\a\n\n");
				break;
		}

		if ( result != S75_OK ) {
			printf( "\nOPERATION FAILED!\n" );
		}

	}	while(i!=14);

	closeS75(hdev);

	return 1;

}


int logon( S75_Handle hdev )
{
	char userinput[120];

	printf( "\nEnter Password : " );
	gets( userinput );
//	specifyPassword( hdev, "++++++++" );
	specifyPassword( hdev, userinput );

	return S75_OK;
}


int userconfigureNetwork(S75_Handle hdev)
{
	char userinput[120];
	S75D_NetSetting netset;
	S75D_NetSetting_V3 netset3;
	int result = S75_FAILED;

	WORD devType;
	int i;
	GetDeviceType(hdev,devType);

	printf("\n Network Configuration\n ------------------------------- \n ");
//	printf("\tIf you would like to use the Default value for a parameter, \n Hit Enter instead of inputting a value");

	if( 	( devType==S75D_V3 ) ||
		( devType==A75) ||
		(devType==S75CA) || 
		(devType==A75PesMaker) || 
		(devType==PesMaker) || 
		(devType==S75_JLEO) ||
		(devType==S75PLUS) ||
		(devType==S75PLUSPRO) ||
		(devType==S75CA) ||
		(devType==S75CAPRO) ||
		(devType==S75FK) ||
		(devType==S75FKPRO) ) { 

//			printf("\n-> Destination value for Unicast Status --Default 255.255.255.255(no unicast)--\n\t\t Enter new Value " );
			printf("\n Destination value for Unicast Status --> " );
			gets(userinput);
//			if(strlen(userinput) < 6){
//				netset3.UniCastStatusDestIP[0] = 0xff;		netset3.UniCastStatusDestIP[1] = 0xff;
//				netset3.UniCastStatusDestIP[2] = 0xff;		netset3.UniCastStatusDestIP[3] = 0xff;
//			}
//			else{
			sscanf(userinput,"%d.%d.%d.%d",&netset3.UniCastStatusDestIP[0],&netset3.UniCastStatusDestIP[1],&netset3.UniCastStatusDestIP[2],&netset3.UniCastStatusDestIP[3]);
//			}

//			printf("\n-> Device Subnet Mask --Default 255.255.255.0--\n\t\t Enter new Value ");
			printf("\n Device Subnet Mask --> ");
			gets(userinput);

//			if(strlen(userinput) < 6){
//				netset3.SubnetMask[0]= 0xff;		netset3.SubnetMask[1]= 0xff;
//				netset3.SubnetMask[2]= 0xff;		netset3.SubnetMask[3]= 0x00;
//			} else{
				sscanf(userinput,"%d.%d.%d.%d",&netset3.SubnetMask[0],&netset3.SubnetMask[1],&netset3.SubnetMask[2],&netset3.SubnetMask[3]);
//			}

//			printf("\n-> Device Default Gatway IP address --Default 192.168.170.55--\n\t\t Enter new Value ");
			printf("\n Device Default Gatway IP address --> ");
			gets(userinput);

//			if(strlen(userinput) < 6){
//				netset3.DefaultGatewayIP[0] = 192;		netset3.DefaultGatewayIP[1] = 168;
//				netset3.DefaultGatewayIP[2] = 170;		netset3.DefaultGatewayIP[3] = 55;
//			} else{
				sscanf(userinput,"%hd.%hd.%hd.%hd",&netset3.DefaultGatewayIP[0],&netset3.DefaultGatewayIP[1],&netset3.DefaultGatewayIP[2],&netset3.DefaultGatewayIP[3]);
//			}

//			printf("\n-> Device Receiver IP Address --Default 192.168.170.54--\n\t\t Enter new Value ");
			printf("\n Device Receiver IP Address --> ");
			gets(userinput);

//			if(strlen(userinput) < 6){
//				netset3.ReceiverIP[0] = 192;		netset3.ReceiverIP[1] = 168;
//				netset3.ReceiverIP[2] = 170;		netset3.ReceiverIP[3] = 54;
//			}else{
					sscanf(userinput,"%d.%d.%d.%d",&netset3.ReceiverIP[0],&netset3.ReceiverIP[1],&netset3.ReceiverIP[2],&netset3.ReceiverIP[3]);
//			}

//			printf("\n->  IGMP Filtering Enable/Disable (1/0)  \n\t\t Enter new Value ");
			printf("\n IGMP Filtering Enable/Disable (1/0) --> ");
			scanf("%d",&i);
			if(i>0) netset3.IGMP_Enable = 1;
			else	netset3.IGMP_Enable = 0;


			if(GetAvailablePort(netset3.BCastStatusPort)== S75_FAILED)
					printf("\n Get available Port Failed \n\n");
			else
					netset3.BCastStatusPort = 0x1974;

			if(GetAvailablePort(netset3.UniCastStatusDestUDP)== S75_FAILED)
					printf("\n Get available Port Failed \n\n");
			else
					netset3.UniCastStatusDestUDP = 0x2075;
			result=configureNetwork(hdev, (LPBYTE)&netset3);
//			if((result=configureNetwork(hdev, (LPBYTE)&netset3))!= S75_OK)
//				printf("\nNetwork Configuration failed : %x \a \n", result);
//			else
//				printf("\nNetwork Configuration OK \a \n");

	} else if ( devType==S75D_V21 ) {
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

			if((result = configureNetwork(hdev, (LPBYTE)&netset))!= S75_OK)
				printf("\n Network Configuration failed \a \n");
			else
				printf("\n Network Configuration OK \a \n");
	}

	return result;
}

int userconfigureRF(S75_Handle hdev)
{
	WORD devType;
	char userinput[120];
	S75D_RFSetting RFSet;
	S75_RFSetting_JLEO S75JLeoRFSet;
	A75_RFSetting A75RFSet;
	S75Plus_RFSetting S75PlusRFSet;
	int result = S75_FAILED;
	WORD temp; 

	GetDeviceType(hdev,devType);

	printf("\n\n RF Configuration Section \n ---------------------------\n");

	if ( (devType==S75D_V21) || (devType==S75D_V3) || (devType==PesMaker) ) {
		printf("\n\t\t Enter L-Band Frequency (MHz) [Default 1000]: ");
		gets(userinput);
		if(strlen(userinput)==0) RFSet.frequency=1000.0;
		else
			sscanf(userinput,"%f",&RFSet.frequency);
		if((RFSet.frequency > 2150) || (RFSet.frequency < 950) )
		{
			RFSet.frequency = 1000;
		}
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
	
		if((result = configureRF(hdev, (LPBYTE) &RFSet))== S75_OK)
			printf("\n\n RF configuration success \n");
		else
			printf("\n\n RF configuration Failed \n");
	} else if ( (devType==A75) || (devType==A75PesMaker) ) {
		printf("\n\t\t Enter Channel Number [Default 1]: ");
		gets(userinput);
		if(strlen(userinput)==0)
			A75RFSet.ChannelNumber=1;
		else
			sscanf(userinput,"%d",&A75RFSet.ChannelNumber);
		if(result=(configureRF(hdev, (LPBYTE) &A75RFSet))== S75_OK)
			printf("\n\n RF configuration success \n");
		else
			printf("\n\n RF configuration Failed \n");
	} else if ( devType==S75_JLEO ) {
		printf("\n Enter L-Band Frequency (MHz) --> ");
		gets(userinput);
		sscanf(userinput,"%f",&S75JLeoRFSet.frequency);
		if((S75JLeoRFSet.frequency > 2150) || (S75JLeoRFSet.frequency < 950) ){
			printf("Value out of range, default 1000.0 MHz will be used") ;
			S75JLeoRFSet.frequency = 1000;
		}
		printf("\n Enter LO Frequency (MHz) --> ");
		gets(userinput);
		sscanf(userinput,"%d",&S75JLeoRFSet.LocalOscFrequency);
		printf("\n Enter SympolRate (MSPS) --> ");
		gets(userinput);
		sscanf(userinput,"%f",&S75JLeoRFSet.symbolRate);
		if((S75JLeoRFSet.symbolRate > 45) || (S75JLeoRFSet.symbolRate < 2) ) {
			printf("Value out of range, default 20 Msps will be used") ;
			S75JLeoRFSet.symbolRate = 20;
		}
	
		printf("\n LNB Power on or off (1== on, 0== off) --> ");
		gets(userinput);
		sscanf(userinput,"%d",&S75JLeoRFSet.powerOn);
		if(S75JLeoRFSet.powerOn>0) S75JLeoRFSet.powerOn = 1;
		else S75JLeoRFSet.powerOn = 0;
		
		if(S75JLeoRFSet.powerOn == 1){
			printf("\n Transponder Polarization (1== H/LC, 0== V/RC) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&S75JLeoRFSet.polarity);
			if(S75JLeoRFSet.polarity>0) S75JLeoRFSet.polarity = 1;
			else S75JLeoRFSet.polarity = 0;
		}

		if(S75JLeoRFSet.powerOn == 1){
			printf("\n LNB Band switching (1== High, 0== Low) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75JLeoRFSet.band = temp ;
			if(S75JLeoRFSet.band>0) S75JLeoRFSet.band = 1;
			else S75JLeoRFSet.band = 0;
		}

		if(S75JLeoRFSet.powerOn == 1){
			printf("\n Long Line Compensation ( 1 = On, 0 = Off ) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75JLeoRFSet.LongLineCompensation = temp;
			if(S75JLeoRFSet.LongLineCompensation>0) S75JLeoRFSet.LongLineCompensation = 1;
			else S75JLeoRFSet.LongLineCompensation = 0;
		}

		if(S75JLeoRFSet.powerOn == 1){
			printf("\n High Voltage Mode ( 1 = On, 0 = Low) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75JLeoRFSet.HighVoltage = temp ;
			if(S75JLeoRFSet.HighVoltage>0) S75JLeoRFSet.HighVoltage = 1;
			else S75JLeoRFSet.HighVoltage = 0;
		}

		if(S75JLeoRFSet.powerOn == 0){
			S75JLeoRFSet.band=0;
			S75JLeoRFSet.polarity = 0;
			S75JLeoRFSet.HighVoltage = 0;
			S75JLeoRFSet.LongLineCompensation = 0;
		}
	
		result=configureRF(hdev, (LPBYTE) &S75JLeoRFSet);
//		if((result=configureRF(hdev, (LPBYTE) &S75JLeoRFSet))== S75_OK)
//			printf("\n\n RF configuration success \n");
//		else
//			printf("\n\n RF configuration Failed \n");

	} else if ( 	( devType==S75PLUS ) ||
			( devType==S75PLUSPRO ) ||
			( devType==S75CA ) ||
			( devType==S75CAPRO ) ||
			( devType==S75FK ) ||
			( devType==S75FKPRO ) ) {
		printf("\n Enter L-Band Frequency (MHz) --> ");
		gets(userinput);
		sscanf(userinput,"%f",&S75PlusRFSet.frequency);
		if((S75PlusRFSet.frequency > 2150) || (S75PlusRFSet.frequency < 950) ){
			printf("Value out of range, default 1000.0 MHz will be used") ;
			S75PlusRFSet.frequency = 1000;
		}
		printf("\n Enter LO Frequency (MHz) --> ");
		gets(userinput);
		sscanf(userinput,"%d",&S75PlusRFSet.LocalOscFrequency);
		printf("\n Enter SympolRate (MSPS) --> ");
		gets(userinput);
		sscanf(userinput,"%f",&S75PlusRFSet.symbolRate);
		if((S75PlusRFSet.symbolRate > 45) || (S75PlusRFSet.symbolRate < 2) ) {
			printf("Value out of range, default 20 Msps will be used") ;
			S75PlusRFSet.symbolRate = 20;
		}
	
		printf("\n LNB Power on or off (1== on, 0== off) --> ");
		gets(userinput);
		sscanf(userinput,"%d",&S75PlusRFSet.powerOn);
		if(S75PlusRFSet.powerOn>0) S75PlusRFSet.powerOn = 1;
		else S75PlusRFSet.powerOn = 0;
		
		if(S75PlusRFSet.powerOn == 1){
			printf("\n Transponder Polarization (1== H/LC, 0== V/RC) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&S75PlusRFSet.polarity);
			if(S75PlusRFSet.polarity>0) S75PlusRFSet.polarity = 1;
			else S75PlusRFSet.polarity = 0;
		}

		if(S75PlusRFSet.powerOn == 1){
			printf("\n LNB Band switching (1== High, 0== Low) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75PlusRFSet.band = temp ;
			if(S75PlusRFSet.band>0) S75PlusRFSet.band = 1;
			else S75PlusRFSet.band = 0;
		}

		if(S75PlusRFSet.powerOn == 1){
			printf("\n Long Line Compensation ( 1 = On, 0 = Off ) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75PlusRFSet.LongLineCompensation = temp;
			if(S75PlusRFSet.LongLineCompensation>0) S75PlusRFSet.LongLineCompensation = 1;
			else S75PlusRFSet.LongLineCompensation = 0;
		}

		if(S75PlusRFSet.powerOn == 1){
			printf("\n High Voltage Mode ( 1 = On, 0 = Low) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75PlusRFSet.HighVoltage = temp ;
			if(S75PlusRFSet.HighVoltage>0) S75PlusRFSet.HighVoltage = 1;
			else S75PlusRFSet.HighVoltage = 0;
		}

		if(S75PlusRFSet.PolaritySwitchingVoltage == 1){
			printf("\n Polarity Switching Voltage ( 0 = 11-15V, 0 = 13-18V) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75PlusRFSet.PolaritySwitchingVoltage = temp ;
			if(S75PlusRFSet.PolaritySwitchingVoltage>0) S75PlusRFSet.PolaritySwitchingVoltage = 1;
			else S75PlusRFSet.PolaritySwitchingVoltage = 0;
		}

		if(S75PlusRFSet.powerOn == 1){
			printf("\n Hi/Lo Band Switching Tone ( 0 = 22 kHz, 1 = 44 kHz) --> ");
			gets(userinput);
			sscanf(userinput,"%d",&temp);
			S75PlusRFSet.HiLoBandSwitchingTone = temp ;
			if(S75PlusRFSet.HiLoBandSwitchingTone>0) S75PlusRFSet.HiLoBandSwitchingTone = 1;
			else S75PlusRFSet.HiLoBandSwitchingTone = 0;
		}

		if(S75PlusRFSet.powerOn == 0){
			S75PlusRFSet.band=0;
			S75PlusRFSet.polarity = 0;
			S75PlusRFSet.HighVoltage = 0;
			S75PlusRFSet.LongLineCompensation = 0;
			S75PlusRFSet.PolaritySwitchingVoltage = 0;
			S75PlusRFSet.HiLoBandSwitchingTone = 0;
		}
	
		result=configureRF(hdev, (LPBYTE) &S75PlusRFSet);
//		if((result=configureRF(hdev, (LPBYTE) &S75PlusRFSet))== S75_OK)
//			printf("\n\n RF configuration success \n");
//		else
//			printf("\n\n RF configuration Failed \n");

	}
	return result;
}


// This function displays the status
int DisplayStatus(S75_Handle hdev){

	S75D_Status status;
	S75D_Status_V3 status3;
	A75_Status statusA75;
	PesMaker_Status statusPesMaker;
	A75PesMaker_Status statusA75PesMaker;
	S75_Status_JLEO statusS75JLeo;
	S75Plus_Status statusS75Plus;
	bool line_feed = false;
	int result = S75_FAILED;

	int i, j, k;
	WORD devType;

	GetDeviceType(hdev,devType);

	if(devType==S75D_V3){

		memset( (void *)(&status3), 0, sizeof( status3 ) );

		if((result=getStatus(hdev, (LPBYTE)&status3)) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return result;
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
			printf("\n Network Settings: \n Receiver IP:                "); PrintIP(status3.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(status3.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(status3.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(status3.UniCastStatusDestIP);
			printf("\n Destination UDP Port:       0x%X", status3.UniCastStatusDestUDP);
			printf("\n Receiver MAC Address:       "); PrintMAC(status3.ReceiverMAC);
			printf("\n Receiver DVB MAC Address:  ");PrintMAC(status3.DVB_MACADDRESS);
			if(status3.IGMP_Enable > 0) printf("\n IGMP Filter is  ON ");
			else printf("\n IGMP Filter is  OFF ");
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d", status3.EthernetTransmit, status3.EthernetReceive,status3.EthernetPacketDropped, status3.EthernetTransmitError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Unit Identification Code: \n ");
			for(i=0;i<32;i++) printf("%0.2X ",status3.UID[i]);
			printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",status3.PIDList.NumberofPIDs);
			for(i=0;i<status3.PIDList.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",status3.PIDList.Entry[i],status3.PIDList.Control[i] );

	} else if (devType==A75PesMaker) {

		memset( (void *)(&statusA75PesMaker), 0, sizeof( statusA75PesMaker ) );

		if((result=getStatus(hdev, (LPBYTE)&statusA75PesMaker)) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return result;
			}
			printf("\n Device Firmware Ver %d, Rev. %d  ---   RF Firmware Codes 0x%X", statusA75PesMaker.FirmWareVer,statusA75PesMaker.FirmWareRev, statusA75PesMaker.RF_FIRMWARE);
			printf("\n --------------------------------------------------------------------------");

			printf("\n Channel Number:       %d", statusA75PesMaker.ChannelNumber);
			printf("\n Demod Stats:          0x%x", statusA75PesMaker.DeModStats);
			printf("\n VSB Mode:             0x%x", statusA75PesMaker.VSBMode);
			printf("\n SER 1:                0x%x", statusA75PesMaker.SER_1);
			printf("\n SER 10:               0x%x", statusA75PesMaker.SER_10);
			printf("\n SNR BE:               %f", statusA75PesMaker.SNR_BE);
			printf("\n SNR AE:               %f", statusA75PesMaker.SNR_AE);
			printf("\n Carrier Offset:       %f", statusA75PesMaker.Carrier_Off);
			printf("\n IF AGC:               %f", statusA75PesMaker.IF_AGC);
			printf("\n RF AGC:               %f", statusA75PesMaker.RFAGC);
			printf("\n FEC Unerrored:        %d", statusA75PesMaker.FEC_UnErr);
			printf("\n FEC Corrected:        %d", statusA75PesMaker.FEC_Corr);
			printf("\n FEC Uncorrected:      %d", statusA75PesMaker.FEC_UnCorr);
			if ( statusA75PesMaker.FieldLocked ) {
				printf("\n\n Field Locked, ");
			} else {
				printf("\n\n Field Not Locked, ");
			}
			if ( statusA75PesMaker.EqualizerState ) {
				printf("Equalizer On, " );
			} else {
				printf("Equalizer Off, " );
			}
			switch ( statusA75PesMaker.ChannelState ) {
			case 0 : printf( "Channel is Dynamic, " ); break;
			case 1 : printf( "Channel is Static, " ); break;
			case 2 : printf( "Channel is Fast Dynamic, " ); break;
			}
			if ( statusA75PesMaker.NTSC_Filter ) {
				printf("NTSC Filter is On, " );
			} else {
				printf("NTSC Filter is Off, " );
			}
			if ( statusA75PesMaker.InverseSpectrum ) {
				printf( "Spectrum Inverted\n" );
			} else {
				printf( "Spectrum is not Inverted\n" );
			}
			printf("\n -------------------------------------------------------------------------");
			printf("\n Network Settings: \n Receiver IP:                "); PrintIP(statusA75PesMaker.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(statusA75PesMaker.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(statusA75PesMaker.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(statusA75PesMaker.UniCastStatusDestIP);
			printf("\n Destination UDP Port:       0x%X", statusA75PesMaker.UniCastStatusDestUDP);
			printf("\n Receiver MAC Address:       "); PrintMAC(statusA75PesMaker.ReceiverMAC);
			printf("\n Receiver DVB MAC Address:  ");PrintMAC(statusA75PesMaker.DVB_MACADDRESS);
			if(statusA75PesMaker.IGMP_Enable > 0) printf("\n IGMP Filter is  ON ");
			else printf("\n IGMP Filter is  OFF ");
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d", statusA75PesMaker.EthernetTransmit, statusA75PesMaker.EthernetReceive,statusA75PesMaker.EthernetPacketDropped, statusA75PesMaker.EthernetTransmitError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Unit Identification Code: \n ");
			for(i=0;i<32;i++) printf("%0.2X ",statusA75PesMaker.UID[i]);
			printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",statusA75PesMaker.PIDList.NumberofPIDs);
			for(i=0;i<statusA75PesMaker.PIDList.NumberofPIDs;i++) {
				switch ( statusA75PesMaker.PIDList.Control[i] ) {
					case 0 : printf(" 0x%X Passing Raw\n", statusA75PesMaker.PIDList.Entry[i]); break;
					case 1 : printf(" 0x%X MPE Processing\n", statusA75PesMaker.PIDList.Entry[i]); break;
					case 2 : printf(" 0x%X PES Processing\n", statusA75PesMaker.PIDList.Entry[i]); break;
				}
			}

	} else if (devType==A75) {

		memset( (void *)(&statusA75), 0, sizeof( statusA75 ) );

		if((result=getStatus(hdev, (LPBYTE)&statusA75)) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return result;
			}
			printf("\n Device Firmware Ver %d, Rev. %d  ---   RF Firmware Codes 0x%X", statusA75.FirmWareVer,statusA75.FirmWareRev, statusA75.RF_FIRMWARE);
			printf("\n --------------------------------------------------------------------------");

			printf("\n Channel Number:       %d", statusA75.ChannelNumber);
			printf("\n Demod Stats:          0x%x", statusA75.DeModStats);
			printf("\n VSB Mode:             0x%x", statusA75.VSBMode);
			printf("\n SER 1:                0x%x", statusA75.SER_1);
			printf("\n SER 10:               0x%x", statusA75.SER_10);
			printf("\n SNR BE:               %f", statusA75.SNR_BE);
			printf("\n SNR AE:               %f", statusA75.SNR_AE);
			printf("\n Carrier Offset:       %f", statusA75.Carrier_Off);
			printf("\n IF AGC:               %f", statusA75.IF_AGC);
			printf("\n RF AGC:               %f", statusA75.RFAGC);
			printf("\n FEC Unerrored:        %d", statusA75.FEC_UnErr);
			printf("\n FEC Corrected:        %d", statusA75.FEC_Corr);
			printf("\n FEC Uncorrected:      %d", statusA75.FEC_UnCorr);
			if ( statusA75.FieldLocked ) {
				printf("\n\n Field Locked, ");
			} else {
				printf("\n\n Field Not Locked, ");
			}
			if ( statusA75.EqualizerState ) {
				printf("Equalizer On, " );
			} else {
				printf("Equalizer Off, " );
			}
			switch ( statusA75.ChannelState ) {
			case 0 : printf( "Channel is Dynamic, " ); break;
			case 1 : printf( "Channel is Static, " ); break;
			case 2 : printf( "Channel is Fast Dynamic, " ); break;
			}
			if ( statusA75.NTSC_Filter ) {
				printf("NTSC Filter is On, " );
			} else {
				printf("NTSC Filter is Off, " );
			}
			if ( statusA75.InverseSpectrum ) {
				printf( "Spectrum Inverted\n" );
			} else {
				printf( "Spectrum is not Inverted\n" );
			}

			printf("\n -------------------------------------------------------------------------");
			printf("\n Network Settings: \n Receiver IP:                "); PrintIP(statusA75.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(statusA75.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(statusA75.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(statusA75.UniCastStatusDestIP);
			printf("\n Destination UDP Port:       0x%X", statusA75.UniCastStatusDestUDP);
			printf("\n Receiver MAC Address:       "); PrintMAC(statusA75.ReceiverMAC);
			printf("\n Receiver DVB MAC Address:  ");PrintMAC(statusA75.DVB_MACADDRESS);
			if(statusA75.IGMP_Enable > 0) printf("\n IGMP Filter is  ON ");
			else printf("\n IGMP Filter is  OFF ");
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d", statusA75.EthernetTransmit, statusA75.EthernetReceive,statusA75.EthernetPacketDropped, statusA75.EthernetTransmitError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Unit Identification Code: \n ");
			for(i=0;i<32;i++) printf("%0.2X ",statusA75.UID[i]);
			printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",statusA75.PIDList.NumberofPIDs);
			for(i=0;i<statusA75.PIDList.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",statusA75.PIDList.Entry[i],statusA75.PIDList.Control[i] );

	} else if (devType==PesMaker) {
		
		memset( (void *)(&statusPesMaker), 0, sizeof( statusPesMaker ) );

		if((result=getStatus(hdev, (LPBYTE)&statusPesMaker)) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return result;
			}
			printf("\n Device Firmware Ver %d, Rev. %d  ---   RF Firmware Codes 0x%X", statusPesMaker.FirmWareVer,statusPesMaker.FirmWareRev, statusPesMaker.RF_FIRMWARE);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Symbol Rate %f Msps \t Frequency %f", statusPesMaker.SymbolRate, statusPesMaker.Frequency);
			printf("\n The viterbi Bit Error Rate is %-0#4.4e \t Viterbi Rate %d",statusPesMaker.VBER, statusPesMaker.ViterbiRate);
			if(statusPesMaker.Power != 0){
				printf("\n LNB Power On, Polarization = %d, Band = %d",statusPesMaker.Polarity , statusPesMaker.Band);
			}
			else{
				printf("\n LNB Power OFF ");
			}
			if(statusPesMaker.LNBFault == 1){
				printf("\n\n LNB Fault ... \a\a");
			}
			printf("\n Signal Strength: %d, Signal Lock: %d, Data Lock: %d, Uncorrectables: %d", statusPesMaker.SignalStrength, statusPesMaker.SignalLock, statusPesMaker.DataLock,statusPesMaker.Uncorrectables);

			printf("\n -------------------------------------------------------------------------");
			printf("\n Network Settings: \n Receiver IP:                "); PrintIP(statusPesMaker.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(statusPesMaker.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(statusPesMaker.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(statusPesMaker.UniCastStatusDestIP);
			printf("\n Destination UDP Port:       0x%X", statusPesMaker.UniCastStatusDestUDP);
			printf("\n Receiver MAC Address:       "); PrintMAC(statusPesMaker.ReceiverMAC);
			printf("\n Receiver DVB MAC Address:  ");PrintMAC(statusPesMaker.DVB_MACADDRESS);
			if(statusPesMaker.IGMP_Enable > 0) printf("\n IGMP Filter is  ON ");
			else printf("\n IGMP Filter is  OFF ");
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d", statusPesMaker.EthernetTransmit, statusPesMaker.EthernetReceive,statusPesMaker.EthernetPacketDropped, statusPesMaker.EthernetTransmitError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Unit Identification Code: \n ");
			for(i=0;i<32;i++) printf("%0.2X ",statusPesMaker.UID[i]);
			printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",statusPesMaker.PIDList.NumberofPIDs);
			for(i=0;i<statusPesMaker.PIDList.NumberofPIDs;i++) {
				switch ( statusPesMaker.PIDList.Control[i] ) {
					case 0 : printf(" 0x%X Passing Raw\n", statusPesMaker.PIDList.Entry[i]); break;
					case 1 : printf(" 0x%X MPE Processing\n", statusPesMaker.PIDList.Entry[i]); break;
					case 2 : printf(" 0x%X PES Processing\n", statusPesMaker.PIDList.Entry[i]); break;
				}
			}

/*  INTERIM S75CA
	} else if (devType==S75CA) {
		if(getStatus(hdev, (LPBYTE)&statusS75CA) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return;
			}
			printf("\n Device Firmware Ver %d, Rev. %d  ---   RF Firmware Codes 0x%X", statusS75CA.FirmWareVer,statusS75CA.FirmWareRev, statusS75CA.RF_FIRMWARE);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Symbol Rate %f Msps \t Frequency %f", statusS75CA.SymbolRate, statusS75CA.Frequency);
			printf("\n The viterbi Bit Error Rate is %-0#4.4e \t Viterbi Rate %d",statusS75CA.VBER, statusS75CA.ViterbiRate);
			if(statusS75CA.Power != 0){
				printf("\n LNB Power On, Polarization = %d, Band = %d",statusS75CA.Polarity , statusS75CA.Band);
			}
			else{
				printf("\n LNB Power OFF ");
			}
			if(statusS75CA.LNBFault == 1){
				printf("\n\n LNB Fault ... \a\a");
			}
			printf("\n Signal Strength: %d, Signal Lock: %d, Data Lock: %d, Uncorrectables: %d", statusS75CA.SignalStrength, statusS75CA.SignalLock, statusS75CA.DataLock,statusS75CA.Uncorrectables);

			printf("\n -------------------------------------------------------------------------");
			printf("\n Network Settings: \n Receiver IP:                "); PrintIP(statusS75CA.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(statusS75CA.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(statusS75CA.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(statusS75CA.UniCastStatusDestIP);
			printf("\n Destination UDP Port:       0x%X", statusS75CA.UniCastStatusDestUDP);
			printf("\n Receiver MAC Address:       "); PrintMAC(statusS75CA.ReceiverMAC);
			printf("\n Receiver DVB MAC Address:  ");PrintMAC(statusS75CA.DVB_MACADDRESS);
			if(statusS75CA.IGMP_Enable > 0) printf("\n IGMP Filter is  ON ");
			else printf("\n IGMP Filter is  OFF ");
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d", statusS75CA.EthernetTransmit, statusS75CA.EthernetReceive,statusS75CA.EthernetPacketDropped, statusS75CA.EthernetTransmitError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Program List:\n ------------\n" );
			for(i=0;i<16;i++) {
				if ( statusS75CA.Programs[i] ) {
					printf(" 0x%X\n", statusS75CA.Programs[i]);
				}
			}
			printf("\n --------------------------------------------------------------------------");
			printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",statusS75CA.PIDList.NumberofPIDs);
			for(i=0;i<statusS75CA.PIDList.NumberofPIDs;i++) {
				switch ( statusS75CA.PIDList.Control[i] ) {
					case 0 : printf(" 0x%X Passing Raw\n", statusS75CA.PIDList.Entry[i]); break;
					case 1 : printf(" 0x%X MPE Processing\n", statusS75CA.PIDList.Entry[i]); break;
					case 2 : printf(" 0x%X PES Processing\n", statusS75CA.PIDList.Entry[i]); break;
				}
			}
*/
	} else if ( (devType==S75PLUS) ||
	            (devType==S75PLUSPRO) ||
	            (devType==S75CA) ||
	            (devType==S75CAPRO) ||
	            (devType==S75FK) ||
	            (devType==S75FKPRO) ) {

		memset( (void *)(&statusS75Plus), 0, sizeof( statusS75Plus ) );

		printf( "\n" );
		printf("\n\t\t (0) All Status");
		printf("\n\t\t (1) Device Identification");
		printf("\n\t\t (2) Network Status");
     		printf("\n\t\t (3) RF Status");
       		printf("\n\t\t (4) PID List");
        	printf("\n\t\t (5) PID/Destination Tree");
        	printf("\n\t\t (6) Destination/PID Tree");
        	printf("\n\t\t (7) CA Program Status");

		printf("\n\n\t\t Select an Action: ");

		scanf("%d",&k);
		getchar();

		if((result=getStatus(hdev, (LPBYTE)&statusS75Plus)) != S75_OK){
			printf("\n Failed to get status messages from the device\n");
			return result;
		}
		if ( k == 0 || k == 1 ) {
			printf("\n Unit Identification\n---------------------------------------------------");
			printf("\n Device Firmware\tVer %d, Rev. %d", statusS75Plus.FirmWareVer,statusS75Plus.FirmWareRev );
			printf("\n RF Firmware Code\t0x%X", statusS75Plus.RF_FIRMWARE );
			printf("\n Unit ID Code\t\t");
			for(i=0;i<32;i++) printf("%0.2X ",statusS75Plus.UID[i]);
			printf("\n" );
		}
		if ( k == 0 || k == 2 ) {
			printf("\n Network Configuration\n--------------------------------------------------\n" );
			printf("\n Receiver IP\t\t\t"); PrintIP(statusS75Plus.ReceiverIP);
			printf("\n Receiver Subnet Mask\t\t");PrintIP(statusS75Plus.SubnetMask);
			printf("\n Default Gateway IP address\t"); PrintIP(statusS75Plus.DefaultGatewayIP);
			printf("\n Destination IP address\t\t");PrintIP(statusS75Plus.UniCastStatusDestIP);
			printf("\n Destination UDP Port\t\t0x%X", statusS75Plus.UniCastStatusDestUDP);
			printf("\n Broadcast UDP Port\t\t0x%X", statusS75Plus.BroadcastStatusPort);
			printf("\n Receiver MAC Address\t\t"); PrintMAC(statusS75Plus.ReceiverMAC);
//			printf("\n Receiver DVB MAC Address:  ");PrintMAC(statusS75Plus.DVB_MACADDRESS);
			if(statusS75Plus.IGMP_Enable > 0) printf("\n IGMP Filter\t\t\tON ");
			else printf("\n IGMP Filter\t\t\tOFF ");
			printf("\n\n Network Statistics\n--------------------------------------------------\n" );
			printf("\n Ethernet TX\t\t%d", statusS75Plus.EthernetTransmit );
			printf("\n Ethernet RX\t\t%d", statusS75Plus.EthernetReceive );
			printf("\n Ethernet Drops\t\t%d", statusS75Plus.EthernetPacketDropped );
			printf("\n Ethernet RX Errors\t%d", statusS75Plus.EthernetReceiveError );
			printf("\n\n DVB Statistics\n--------------------------------------------------\n" );
			printf("\n Data Sync Loss\t\t%d", statusS75Plus.DataSyncLoss );
			printf("\n DVB Accepted\t\t%d", statusS75Plus.DVBAccepted );
			printf("\n Scrambled DVB Packets\t%d", statusS75Plus.DVBScrambled);
			printf("\n Descrambled DVB Packets\t%d", statusS75Plus.DVBDescrambled);
			printf("\n Total DVB Accepted\t%u %u", statusS75Plus.TotalDVBPacketsAccepted[0], statusS75Plus.TotalDVBPacketsAccepted[1] );
			printf("\n Total DVB RX Error\t%u %u", statusS75Plus.TotalDVBPacketsRXInError[0], statusS75Plus.TotalDVBPacketsRXInError[1] );
			printf("\n Total DVB Eth Out\t%u %u", statusS75Plus.TotalEthernetPacketsOut[0], statusS75Plus.TotalEthernetPacketsOut[1] );
			printf("\n Total Uncorr TS\t%u %u\n", statusS75Plus.TotalUncorrectableTSPackets[0], statusS75Plus.TotalUncorrectableTSPackets[1] );
		}
		if ( k == 0 || k == 3 ) {
			printf( "\n RF Status\n ---------------------------------------\n" );
			printf("\n Symbol Rate\t\t%f Msps", statusS75Plus.SymbolRate );
			printf("\n Frequency\t\t%f", statusS75Plus.Frequency );
			printf("\n LO Frequency\t\t%d", statusS75Plus.LocalOscFrequency );
			printf("\n VBER\t\t\t%-0#4.4e", statusS75Plus.VBER );
			printf("\n Viterbi Rate\t\t%d", statusS75Plus.ViterbiRate );
			if(statusS75Plus.Power != 0){
				printf("\n LNB Power\t\tON" );
				printf("\n Polarization\t\t%d", statusS75Plus.Polarity );
				printf("\n Band\t\t\t%d", statusS75Plus.Band );
				printf("\n Long Line\t\t%d", statusS75Plus.Long_Line_Comp );
				printf("\n Hi Voltage Mode\t%d", statusS75Plus.Hi_VoltageMode);
			/*	if(statusS75CA.LNBFault == 1){
				printf("\n LNB Fault ... \a\a");
			 } */
			}
			else{
				printf("\n LNB Power\t\tOFF ");
			}
			
			printf("\n Signal Strength\t%d", statusS75Plus.SignalStrength );
			printf("\n DemodGain\t\t%d", statusS75Plus.DemodulatorGain );
			printf("\n C/N\t\t\t%f", statusS75Plus.CarrierToNoise );
			printf("\n Signal Lock\t\t%d", statusS75Plus.SignalLock );
			printf("\n Data Lock\t\t%d", statusS75Plus.DataLock );
			printf("\n Uncorrectables\t\t%d", statusS75Plus.Uncorrectables );
//			printf("\n Signal Strength: %d, Signal Lock: %d, Data Lock: %d, Uncorrectables: %d", statusS75CA.SignalStrength, statusS75CA.SignalLock, statusS75CA.DataLock,statusS75CA.Uncorrectables);
			printf("\n Noise Estimate\t\t%d", statusS75Plus.NEST );
			printf("\n Clock Offset\t\t%d", statusS75Plus.Clock_Off );
			printf("\n Frequency Error\t%d", statusS75Plus.Freq_Err );
			printf("\n ADC Midpoint Crossing\t%d", statusS75Plus.ADC_MID );
			printf("\n ADC Clipping Count\t%d", statusS75Plus.ADC_CLIP );
			printf("\n Digital Gain\t\t%d", statusS75Plus.Digital_Gain );
			printf("\n Clamping Count\t\t%d\n", statusS75Plus.AA_CLAMP );
		}
		if ( k == 0 || k == 4 ) {
			printf("\n PID List\n -------------------\n");
			if ( statusS75Plus.PIDList.Control[0] == 3 ) {
				printf( " Entire Transport Stream\n" );
			} else 
			if ( statusS75Plus.PIDList.Control[0] == 4 ) {
				printf( " Entire Transport Stream Minus NULL Packets\n" );
			} else {
				for(i=0;i<statusS75Plus.PIDList.NumberofPIDs;i++) {
					switch ( statusS75Plus.PIDList.Control[i] ) {
						case 0 : printf(" 0x%X Passing Raw\n", statusS75Plus.PIDList.Entry[i]); break;
						case 1 : printf(" 0x%X MPE Processing\n", statusS75Plus.PIDList.Entry[i]); break;
						case 2 : printf(" 0x%X PES Processing\n", statusS75Plus.PIDList.Entry[i]); break;
					}
				}
				printf("\n There are %d PID(s) in the PID List \n",statusS75Plus.PIDList.NumberofPIDs );
			}
		}
		if ( k == 0 || k == 5 ) {
			printf("\n PID List\n -------------------------\n" );
			if ( statusS75Plus.PIDList.Control[0] == 3 ) {
				printf( " Entire Transport Stream\n" );
				printf( "    %d.%d.%d.%d:%d",
						statusS75Plus.PIDDestinations[0].DestinationIP[0],
						statusS75Plus.PIDDestinations[0].DestinationIP[1],
						statusS75Plus.PIDDestinations[0].DestinationIP[2],
						statusS75Plus.PIDDestinations[0].DestinationIP[3],
						statusS75Plus.PIDDestinations[0].DestinationUDP    );
			} else 
			if ( statusS75Plus.PIDList.Control[0] == 4 ) {
				printf( " Entire Transport Stream Minus NULL Packets\n" );
				printf( "    %d.%d.%d.%d:%d",
						statusS75Plus.PIDDestinations[0].DestinationIP[0],
						statusS75Plus.PIDDestinations[0].DestinationIP[1],
						statusS75Plus.PIDDestinations[0].DestinationIP[2],
						statusS75Plus.PIDDestinations[0].DestinationIP[3],
						statusS75Plus.PIDDestinations[0].DestinationUDP    );
			} else {
				for ( i = 0; i < 32; i++ ) {
					if ( statusS75Plus.PIDRoutes[i].PID != NULL_PID ) {
						printf(" 0x%X\n", statusS75Plus.PIDRoutes[i].PID);
						line_feed = false;
						for ( j = 0; j < 16; j++ ) {
							if ( statusS75Plus.PIDRoutes[i].DestinationMask & ( 0x8000 >> j ) ) {
								printf( "    %d.%d.%d.%d:%d, ",
										statusS75Plus.PIDDestinations[j].DestinationIP[0],
										statusS75Plus.PIDDestinations[j].DestinationIP[1],
										statusS75Plus.PIDDestinations[j].DestinationIP[2],
										statusS75Plus.PIDDestinations[j].DestinationIP[3],
										statusS75Plus.PIDDestinations[j].DestinationUDP    );
								line_feed = true;
							}
						}
						if ( line_feed )
							printf( "\n" );
					}
				}
			}
		}
		if ( k == 0 || k == 6 ) {
			printf("\n Destination List\n ---------------------\n" );
			if ( statusS75Plus.PIDList.Control[0] == 3 ) {
				printf( " %d.%d.%d.%d:%d, Entire Transport Stream\n",
						statusS75Plus.PIDDestinations[0].DestinationIP[0],
						statusS75Plus.PIDDestinations[0].DestinationIP[1],
						statusS75Plus.PIDDestinations[0].DestinationIP[2],
						statusS75Plus.PIDDestinations[0].DestinationIP[3],
						statusS75Plus.PIDDestinations[0].DestinationUDP    );
			} else 
			if ( statusS75Plus.PIDList.Control[0] == 4 ) {
				printf( " %d.%d.%d.%d:%d, Entire Transport Stream Minus NULL Packets\n",
						statusS75Plus.PIDDestinations[0].DestinationIP[0],
						statusS75Plus.PIDDestinations[0].DestinationIP[1],
						statusS75Plus.PIDDestinations[0].DestinationIP[2],
						statusS75Plus.PIDDestinations[0].DestinationIP[3],
						statusS75Plus.PIDDestinations[0].DestinationUDP    );
			} else {
				for ( j = 0; j < 16; j++ ) {
					if ( statusS75Plus.PIDDestinations[j].DestinationUDP ) {
						printf( " %d.%d.%d.%d:%d\n",
								statusS75Plus.PIDDestinations[j].DestinationIP[0],
								statusS75Plus.PIDDestinations[j].DestinationIP[1],
								statusS75Plus.PIDDestinations[j].DestinationIP[2],
								statusS75Plus.PIDDestinations[j].DestinationIP[3],
								statusS75Plus.PIDDestinations[j].DestinationUDP    );
						for ( i = 0; i < 32; i++ ) {
							if ( statusS75Plus.PIDRoutes[i].DestinationMask & ( 0x8000 >> j ) ) {
								printf( "    0x%x",  statusS75Plus.PIDRoutes[i].PID );
							}
						}
						printf( "\n" );
					}
				}
			}
		}
		if ( k == 0 || k == 7 ) {
			printf("\n Program List\n ----------------\n" );	
			for(i=0;i<16;i++) {
				if ( statusS75Plus.ProgramList[i] ) {
					printf(" %d\tstatus=%x\n", statusS75Plus.ProgramList[i], statusS75Plus.ProgramStatus[i]);
				}
			}
			printf( "\n" );
			printf("\n CA Card Status = %x\n", statusS75Plus.CardStatus );
		}
	} else if ( devType==S75D_V21 ) {  //Version 2.1

		memset( (void *)(&status), 0, sizeof( status ) );

		if((result=getStatus(hdev, (LPBYTE)&status)) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return result;
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
			printf("\n Network Settings: \n Receiver IP:                "); PrintIP(status.ReceiverIP);
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
			printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",status.PIDList.NumberofPIDs);
			for(i=0;i<status.PIDList.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",status.PIDList.Entry[i],status.PIDList.Control[i] );

	} else if ( devType==S75_JLEO ) {  // S75 Jleo

		memset( (void *)(&statusS75JLeo), 0, sizeof( statusS75JLeo ) );

		if((result=getStatus(hdev, (LPBYTE)&statusS75JLeo)) != S75_OK){
				printf("\n Failed to get status messages from the device\n");
				return result;
			}
			printf("\n Device Firmware Ver %d, Rev. %d  ---   RF Firmware Codes 0x%X", statusS75JLeo.FirmWareVer,statusS75JLeo.FirmWareRev, statusS75JLeo.RF_FIRMWARE);
			printf("\n --------------------------------------------------------------------------");
			printf("\n Symbol Rate %f Msps \t Frequency %f, LO = %d", statusS75JLeo.SymbolRate, statusS75JLeo.Frequency, statusS75JLeo.LocalOscFrequency);
			printf("\n The viterbi Bit Error Rate is %-0#4.4e \t Viterbi Rate %d",statusS75JLeo.VBER, statusS75JLeo.ViterbiRate);
			if(statusS75JLeo.Power != 0){
				printf("\n LNB Power On, Polarization = %d, Band = %d, Long Line = %d, Hi Voltage Mode = %d",statusS75JLeo.Polarity , statusS75JLeo.Band, statusS75JLeo.Long_Line_Comp, statusS75JLeo.Hi_VoltageMode);
			}
			else{
				printf("\n LNB Power OFF ");
			}
			if(statusS75JLeo.LNBFault == 1){
				printf("\n\n LNB Fault ... \a\a");
			}
			printf("\n AGC: %d, DemodGain: %d, C/N: %f, Signal Lock: %d, Data Lock: %d, Uncorrectables: %d", statusS75JLeo.AutomaticGainControl, statusS75JLeo.DemodulatorGain, statusS75JLeo.CarrierToNoise, statusS75JLeo.SignalLock, statusS75JLeo.DataLock,statusS75JLeo.Uncorrectables);

			printf("\n -------------------------------------------------------------------------");
			printf("\n Network Settings: \n Receiver IP:                "); PrintIP(statusS75JLeo.ReceiverIP);
			printf("\n Receiver Subnet Mask:       ");PrintIP(statusS75JLeo.SubnetMask);
			printf("\n Default Gateway IP address: "); PrintIP(statusS75JLeo.DefaultGatewayIP);
			printf("\n Destination IP address:     ");PrintIP(statusS75JLeo.UniCastStatusDestIP);
			printf("\n Destination UDP Port:       0x%X", statusS75JLeo.UniCastStatusDestUDP);
			printf("\n Broadcast Status Port:      0x%X", statusS75JLeo.BroadcastStatusPort);
			printf("\n Receiver MAC Address:       "); PrintMAC(statusS75JLeo.ReceiverMAC);
			printf("\n Receiver DVB MAC Address:  ");PrintMAC(statusS75JLeo.DVB_MACADDRESS);
			if(statusS75JLeo.IGMP_Enable > 0) printf("\n IGMP Filter is  ON ");
			else printf("\n IGMP Filter is  OFF ");
			printf("\n --------------------------------------------------------------------------");
			printf("\n Ethernet Statistics: TX: %d \t  RX: %d \t Dropped: %d \t TXErr: %d \t RXErr: %d", statusS75JLeo.EthernetTransmit, statusS75JLeo.EthernetReceive,statusS75JLeo.EthernetPacketDropped, statusS75JLeo.EthernetTransmitError, statusS75JLeo.EthernetReceiveError);
			printf("\n --------------------------------------------------------------------------");
			printf("\n DVB Statistics: FEL Loss: %d \t  Data Sync Loss: %d \t DVB Accepted: %d", statusS75JLeo.FrontEndLockLoss, statusS75JLeo.DataSyncLoss, statusS75JLeo.DVBAccepted );
			printf("\n Total DVB Accepted = %d %d\n", statusS75JLeo.TotalDVBPacketsAccepted[0], statusS75JLeo.TotalDVBPacketsAccepted[1] );
			printf("\n Total DVB RX Error = %d %d\n", statusS75JLeo.TotalDVBPacketsRXInError[0], statusS75JLeo.TotalDVBPacketsRXInError[1] );
			printf("\n Total DVB Eth Out  = %d %d\n", statusS75JLeo.TotalEthernetPacketsOut[0], statusS75JLeo.TotalEthernetPacketsOut[1] );
			printf("\n Total Uncorr TS    = %d %d\n", statusS75JLeo.TotalUncorrectableTSPackets[0], statusS75JLeo.TotalUncorrectableTSPackets[1] );
			printf("\n --------------------------------------------------------------------------");
			printf("\n Unit Identification Code: \n ");
			for(i=0;i<32;i++) printf("%0.2X ",statusS75JLeo.UID[i]);
			printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",statusS75JLeo.PIDList.NumberofPIDs);
			for(i=0;i<statusS75JLeo.PIDList.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",statusS75JLeo.PIDList.Entry[i],statusS75JLeo.PIDList.Control[i] );
	}

	return result;

}


void enterDest( BYTE destIP[4], int *port )
{
	char destIPAddrStr[16];
	int dummyInt;

	printf("\n Destination IP --> ");
	scanf("%s", destIPAddrStr);
   	while ( sscanf(destIPAddrStr ,"%d.%d.%d.%d.%d", &destIP[0], &destIP[1], &destIP[2], &destIP[3], &dummyInt) != 4 ) {
		printf("\n Invalid IP address\n");
		printf("\n Enter the Destination IP again --> ");
		scanf("%s", destIPAddrStr);
		continue;
	}
	printf("\n Destination Port --> ");
			
scan_for_dest_port:
	
	while ( scanf("%d", port) != 1 ) {
		printf("\n Invalid port\n");
		printf("\n Enter Destination Port again --> ");
		continue;
	}
	if ( ((*port) <= 1024) || ((*port) > 65535) ) {
		printf("\n Invalid port number\n");
		printf("\n Enter the Destination Port again --> ");
		goto scan_for_dest_port;
	}

}


bool editPIDMappings( S75Plus_PIDList *s75PlusPIDs )
{
	int i, j, k;
	int pid;
	int control;
	bool found;
	BYTE destIP[4];
	int destPort;
	WORD mask;
	int pid_index;
	int destination_index;
	bool write_table = false;


	do{
		printf( "\n------------PIDS------------\n\n" );
		if ( s75PlusPIDs->Control[0] == 3 ) {
			printf( "Entire Transport Stream\n" );
		} else
		if ( s75PlusPIDs->Control[0] == 4 ) {
			printf( "Entire Transport Stream Minus NULL Packets\n" );
		} else {
			for ( i = 0; i < SIZE_PID_LIST; i++ ) {
				if ( s75PlusPIDs->PID[i] != 0x1fff ) {
					printf( "%d) %d\t(0x%x)\t\t", i, s75PlusPIDs->PID[i], s75PlusPIDs->PID[i] );
					switch( s75PlusPIDs->Control[i] ) {
					case 0 : printf( "RAW" ); break;
					case 1 : printf( "MPE" ); break;
					case 2 : printf( "PES" ); break;
					}
					printf( "\n" );
				}
			}
		}
		printf( "\n---------------------DESTINATIONS & MAPPED PIDS---------------------\n\n" );

		if ( s75PlusPIDs->Control[0] == 3 ) {
			printf( "0) %d.%d.%d.%d:%d, Entire Transport Stream",
					s75PlusPIDs->Destination[0].DestinationIP[0],
					s75PlusPIDs->Destination[0].DestinationIP[1],
					s75PlusPIDs->Destination[0].DestinationIP[2],
					s75PlusPIDs->Destination[0].DestinationIP[3],
					s75PlusPIDs->Destination[0].DestinationUDP    );
		} else
		if ( s75PlusPIDs->Control[0] == 4 ) {
			printf( "0) %d.%d.%d.%d:%d, Entire Transport Stream Minus NULL Packets",
					s75PlusPIDs->Destination[0].DestinationIP[0],
					s75PlusPIDs->Destination[0].DestinationIP[1],
					s75PlusPIDs->Destination[0].DestinationIP[2],
					s75PlusPIDs->Destination[0].DestinationIP[3],
					s75PlusPIDs->Destination[0].DestinationUDP    );
		} else {
			for ( j = 0; j < NUM_S75PRO_DESTINATIONS; j++ ) {
				if ( s75PlusPIDs->Destination[j].DestinationUDP != 0 ) {
					printf( "%d) %d.%d.%d.%d:%d, ", j,
							s75PlusPIDs->Destination[j].DestinationIP[0],
							s75PlusPIDs->Destination[j].DestinationIP[1],
							s75PlusPIDs->Destination[j].DestinationIP[2],
							s75PlusPIDs->Destination[j].DestinationIP[3],
							s75PlusPIDs->Destination[j].DestinationUDP    );
					for ( i = 0; i < SIZE_PID_LIST; i++ ) {
						if ( s75PlusPIDs->DestinationMask[i] & ( 0x8000 >> j ) ) { 
							printf( "%d  ", s75PlusPIDs->PID[i] );
						}
					}
					printf( "\n" );
				}
			}
		}
		printf( "\n" );
		printf("\n\t\t (1) Add/Modify PID");
		printf("\n\t\t (2) Add Destination");
		printf("\n\t\t (3) Map PID to Destination");
		printf("\n\t\t (4) Remove PID");
		printf("\n\t\t (5) Remove Destination");
		printf("\n\t\t (6) Remove Mapping");
		printf("\n\t\t (7) Raw Forward Entire Transport Stream");
		printf("\n\t\t (8) Raw Forward Transport Stream Minus NULL PID");
        printf("\n\t\t (9) Write Changes to Receiver");
        printf("\n\t\t (0) Exit Without Writing Changes to the Receiver");
		printf("\n\n\t\t Select an Action: ");

		scanf("%d",&i);
		getchar();

		switch(i){
			case 0: break;
			case 1:
					printf("\n Enter PID to add or modify (Decimal value) --> ");
					scanf("%d",&pid);
					if(pid > 8191){ printf("\n PID value exceeds 8191; Entry Terminated "); break ;}
					printf("\n MPE Processing On, OFF (1= On, 0= Off) --> ");
					scanf("%d",&control);
					found = false;
					j = 0;
					while ( (!found) && ( j < SIZE_PID_LIST ) ) {
						if ( s75PlusPIDs->PID[j] == pid )
							found = true;
						else
							j++;
					}
					if ( found ) {
						s75PlusPIDs->PID[j] = pid;
						s75PlusPIDs->Control[j] = control;
						if ( control == 1 )
							s75PlusPIDs->DestinationMask[j] = 0; 
						break;
					}
					j =0;
					while ( (!found) && ( j < SIZE_PID_LIST ) ) {
						if ( s75PlusPIDs->PID[j] == 0x1fff )
							found = true;
						else
							j++;
					}
					if ( found ) {
						s75PlusPIDs->PID[j] = pid;
						s75PlusPIDs->Control[j] = control;
						s75PlusPIDs->DestinationMask[j] = 0;
					} else {
						printf( "\n PID list is full; Entry Terminated!\n" );
					}
					break;
			case 2:
					enterDest( destIP, &destPort );
					found = false;
					j = 0;
					while ( (!found) && ( j < 16 ) ) {
						if (	( s75PlusPIDs->Destination[j].DestinationIP[0] == destIP[0] ) &&
								( s75PlusPIDs->Destination[j].DestinationIP[1] == destIP[1] ) &&
								( s75PlusPIDs->Destination[j].DestinationIP[2] == destIP[2] ) &&
								( s75PlusPIDs->Destination[j].DestinationIP[3] == destIP[3] ) &&
								( s75PlusPIDs->Destination[j].DestinationUDP == destPort ) 		)
							found = true;
						else
							j++;
					}
					if ( found ) {
						printf( "\n Destination already in the list!\n" );
						break;
					}
					j =0;
					while ( (!found) && ( j < 16 ) ) {
						if ( s75PlusPIDs->Destination[j].DestinationUDP == 0 )
							found = true;
						else
							j++;
					}
					if ( found ) {
						memcpy( s75PlusPIDs->Destination[j].DestinationIP, destIP, 4 );
						s75PlusPIDs->Destination[j].DestinationUDP = destPort;
					} else {
						printf( "\n Destination list is full; Entry Terminated!\n" );
					}
					break;
			case 3:
					printf("\n Enter PID Index --> ");
					scanf("%d",&pid_index );
					printf("\n Enter Destination Index --> ");
					scanf("%d",&destination_index );
					if (	( pid_index < 0 ) || ( pid_index >= SIZE_PID_LIST ) ||
						( destination_index < 0 ) || ( destination_index >= 16 ) ) {
						printf( "\n PID Index must be 0-31, Destination Index must be 0-15!\n" );
						break;
					} else {
						if ( s75PlusPIDs->PID[pid_index] != 0x1fff ) {
							if ( s75PlusPIDs->Control[pid_index] != 1 ) {
								if ( s75PlusPIDs->Destination[destination_index].DestinationUDP != 0 ) {
									s75PlusPIDs->DestinationMask[pid_index] = s75PlusPIDs->DestinationMask[pid_index] | (0x8000 >> destination_index);
								} else {
									printf( "\n Destination not in list!\n" );
								}
							} else {
								printf( "\n MPE PIDs cannot be mapped!\n" );
							}
						} else {
							printf( "\n PID not in list!\n" );
						}
					}
					break;
			case 4:
					printf("\n Enter Index of the PID you want to remove --> ");
					scanf("%d",&pid_index);
					if (	( pid_index < 0 ) || ( pid_index >= SIZE_PID_LIST ) ) {
						printf( "\n PID Index must be 0-31 !\n" );
						break ;
					}
					if ( s75PlusPIDs->PID[pid_index] < 0x1fff ) {
						s75PlusPIDs->PID[pid_index] = 0x1fff;
						s75PlusPIDs->DestinationMask[pid_index] = 0;
					} else {
						printf( "\n PID was not in the list!\n" );
					}
					break;
			case 5:
					printf("\n Enter Index of the Destination you want to remove --> ");
					scanf("%d",&destination_index);
					if (	( destination_index < 0 ) || ( destination_index >= 16 ) ) {
						printf( "\n Destination Index must be 0-15 !\n" );
						break ;
					}
					if ( 1) {
						memset( s75PlusPIDs->Destination[destination_index].DestinationIP, 0, 4 );
						s75PlusPIDs->Destination[destination_index].DestinationUDP = 0;
						mask = ~(0x8000 >> destination_index);
						for ( k = 0; k < SIZE_PID_LIST; k++ ) {
							s75PlusPIDs->DestinationMask[k] = s75PlusPIDs->DestinationMask[k] & mask;
						}
						break;
					} else {
						printf( "\n Destination not in the list!\n" );
					}
					break;
    		case 6:
					printf("\n Enter PID Index --> ");
					scanf("%d",&pid_index );
					printf("\n Enter Destination Index --> ");
					scanf("%d",&destination_index );
					if (	( pid_index < 0 ) || ( pid_index >= SIZE_PID_LIST ) ||
						( destination_index < 0 ) || ( destination_index >= 16 ) ) {
						printf( "\n PID Index must be 0-31, Destination Index must be 0-15!\n" );
						break;
					} else {
							mask = (~(0x8000 >> destination_index)) ;
						if ( s75PlusPIDs->PID[pid_index] != 0x1fff ) {
							if ( s75PlusPIDs->Destination[destination_index].DestinationUDP != 0 ) {
								s75PlusPIDs->DestinationMask[pid_index] = s75PlusPIDs->DestinationMask[pid_index] & mask;
							} else {
								printf( "\n Destination not in list!\n" );
							}
						} else {
							printf( "\n PID not in list!\n" );
						}
					}
					break;
			case 7: 
			case 8:
					enterDest( destIP, &destPort );
					for ( j = 0; j < SIZE_PID_LIST; j++ ) {
						s75PlusPIDs->PID[j] = 0x1fff;
					}
					if ( i == 7 ) 
						s75PlusPIDs->Control[0] = 3;
					else
						s75PlusPIDs->Control[0] = 4;
					for ( j = 0; j < 32; j++ ) {
						s75PlusPIDs->DestinationMask[0] = 0;
					}
					s75PlusPIDs->DestinationMask[0] = 0x8000;
					for ( j = 0; j < 16; j++ ) {
						memset( s75PlusPIDs->Destination[j].DestinationIP, 0, 4 );
						s75PlusPIDs->Destination[j].DestinationUDP = 0;
					}
					memcpy( s75PlusPIDs->Destination[0].DestinationIP, destIP, 4 );
					s75PlusPIDs->Destination[0].DestinationUDP = destPort;
					break;
    		case 9: 
					write_table = true;
					break;
			default:
				printf(" \n\n \t\t Invalid Selection \a\a\n\n");
				break;
		}


	}	while((i!=0)&&(i!=9));

	return write_table;
}


int userSetPID(S75_Handle hdev)
{
	S75D_PIDList PIDs;
	PesMaker_PIDList pesMakerPIDs;
	S75Plus_PIDList s75PlusPIDs;
	WORD devType;
	int i = 0, k;
	char forwIPAddrStr[16];
	BYTE forwIP[4];
	int forwPort;
	int dummyInt;
	char tmpBuf[255];
	int result = S75_FAILED;


	GetDeviceType(hdev,devType);

	if ( 	( devType==S75PLUS ) ||
		( devType==S75PLUSPRO ) ||
		( devType==S75CA ) ||
		( devType==S75CAPRO ) ||
		( devType==S75FK ) ||
		( devType==S75FKPRO ) ) {
		memset( (void *)&s75PlusPIDs, 0, sizeof( s75PlusPIDs ) );
		for ( i = 0; i < SIZE_PID_LIST; i++ ) s75PlusPIDs.PID[i] = 0x1fff;
		result = getPIDList(hdev,(BYTE *)&s75PlusPIDs);
		if ( editPIDMappings( &s75PlusPIDs ) ) {
			result = setPIDList(hdev, (BYTE *)&s75PlusPIDs);
//			printf("\n Sending the PID List returned %X\n", result);
		}
	}
	else if ( (devType==PesMaker) || (devType==A75PesMaker) ) {

		pesMakerPIDs.NumberofPIDs = 0;
		while(pesMakerPIDs.NumberofPIDs < 32){
			printf("\n Enter PID to add to the List (Decimal value) --> ");
			scanf("%d",&pesMakerPIDs.Entry[pesMakerPIDs.NumberofPIDs]);
			if(pesMakerPIDs.Entry[pesMakerPIDs.NumberofPIDs] > 8191){ printf("\n Entry Terminated "); break ;}
			printf("\n MPE Processing On, OFF (1= On, 0= Off) --> ");
			scanf("%d",&k);
			if(k==0) pesMakerPIDs.Control[pesMakerPIDs.NumberofPIDs] = 0;
			else pesMakerPIDs.Control[pesMakerPIDs.NumberofPIDs]=1;


			printf("\n Forwarding Destination IP --> ");
			scanf("%s", forwIPAddrStr);
        	while ( sscanf(forwIPAddrStr ,"%d.%d.%d.%d.%d", &forwIP[0], &forwIP[1], &forwIP[2], &forwIP[3], &dummyInt) != 4 ) {
                printf("\n Invalid IP address\n");
				printf("\n Enter the Forwarding Destination IP again --> ");
				scanf("%s", forwIPAddrStr);
				continue;
        	}
//			sscanf(	forwIPAddrStr, "%d.%d.%d.%d",
//					&(pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[0]),
//					&(pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[1]),
//					&(pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[2]),
//					&(pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[3]));
			pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[0] =  forwIP[0];
			pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[1] =  forwIP[1];
			pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[2] =  forwIP[2];
			pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_IP[3] =  forwIP[3];
			printf("\n Forwarding Destination Port --> ");
			scan_for_forw_dest_port:
        	while ( scanf("%d", &forwPort) != 1 ) {
                printf("\n Invalid port\n");
				printf("\n Enter the Forwarding Destination Port again --> ");
                continue;
        	}
			if ( (forwPort <= 1024) || (forwPort > 65535) ) {
				printf("\n Invalid port number\n");
				printf("\n Enter the Forwarding Destination Port again --> ");
				goto scan_for_forw_dest_port;
			}
			pesMakerPIDs.ForwardInformation[pesMakerPIDs.NumberofPIDs].Destination_Port = forwPort;

			pesMakerPIDs.NumberofPIDs ++;

			if(pesMakerPIDs.NumberofPIDs == 32){ printf("\n Entry Terminated ");break ;}

			//Ask if they want to add another PID
			printf("\nDo you want to add another PID? [y/n]: ");
			scanf("%s", tmpBuf);
			if ( (strncmp(tmpBuf, "y", 1) == 0) || (strncmp(tmpBuf, "Y", 1) == 0) || (strncmp(tmpBuf, "yes", 3) == 0) )
				continue;
			else
				break;
		}
		printf("\n There are %d PID(s) in The entered PID List \n PID List:\n --------\n",pesMakerPIDs.NumberofPIDs);
		for(i=0;i<pesMakerPIDs.NumberofPIDs;i++) {
			printf(" 0x%X MPE Processing %d [1=On, 0=Off] Forward to %d.%d.%d.%d : %d\n",pesMakerPIDs.Entry[i],pesMakerPIDs.Control[i], pesMakerPIDs.ForwardInformation[i].Destination_IP[0], pesMakerPIDs.ForwardInformation[i].Destination_IP[1],  pesMakerPIDs.ForwardInformation[i].Destination_IP[2],  pesMakerPIDs.ForwardInformation[i].Destination_IP[3], pesMakerPIDs.ForwardInformation[i].Destination_Port );
		}

		result = setPIDList(hdev, (BYTE *)&pesMakerPIDs);
		printf("\n Sending the PID List returned %X\n", result);

	}
	else { //V2.1 or V3

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

			//Ask if they want to add another PID
			printf("\nDo you want to add another PID? [y/n]: ");
			scanf("%s", tmpBuf);
			if ( (strncmp(tmpBuf, "y", 1) == 0) || (strncmp(tmpBuf, "Y", 1) == 0) || (strncmp(tmpBuf, "yes", 3) == 0) )
				continue;
			else
				break;
		}
		printf("\n There are %d PID(s) in The entered PID List \n PID List:\n --------\n",PIDs.NumberofPIDs);
		for(i=0;i<PIDs.NumberofPIDs;i++) printf(" 0x%X MPE Processing %d [1=On, 0=Off] \n",PIDs.Entry[i],PIDs.Control[i] );

		result = setPIDList(hdev, (BYTE *)&PIDs);
		printf("\n Sending the PID List returned %X\n", result);

	} //V2.1 or V3

	return result;
}

int userGetPID(S75_Handle hdev)
{
	S75D_PIDList PIDs;
	PesMaker_PIDList pesMakerPIDs;
	S75Plus_PIDList s75PlusPIDs;
	int result = S75_FAILED;

	int i, j;
	WORD devType;


	GetDeviceType(hdev,devType);

	if ( 	( devType==S75PLUS ) ||
		( devType==S75PLUSPRO ) ||
		( devType==S75CA ) ||
		( devType==S75CAPRO ) ||
		( devType==S75FK ) ||
		( devType==S75FKPRO ) ) {
//		printf("Get PID List returned %X\n",getPIDList(hdev,(BYTE *)&s75PlusPIDs));
		result = getPIDList(hdev,(BYTE *)&s75PlusPIDs);
		printf( "\nPIDs & Destinations" );
		printf( "\n---------------------\n" );
		if ( s75PlusPIDs.Control[0] == 3 ) {
			printf( "Entire Transport Stream Forwarding to %d.%d.%d.%d:%d\n",
					s75PlusPIDs.Destination[0].DestinationIP[0],
					s75PlusPIDs.Destination[0].DestinationIP[1],
					s75PlusPIDs.Destination[0].DestinationIP[2],
					s75PlusPIDs.Destination[0].DestinationIP[3],
					s75PlusPIDs.Destination[0].DestinationUDP    );
		} else
		if ( s75PlusPIDs.Control[0] == 4 ) {
			printf( "Entire Transport Stream Minus NULL PID Forwarding to %d.%d.%d.%d:%d\n",
					s75PlusPIDs.Destination[0].DestinationIP[0],
					s75PlusPIDs.Destination[0].DestinationIP[1],
					s75PlusPIDs.Destination[0].DestinationIP[2],
					s75PlusPIDs.Destination[0].DestinationIP[3],
					s75PlusPIDs.Destination[0].DestinationUDP    );
		} else {
			for ( i = 0; i < SIZE_PID_LIST; i++ ) {
				if ( s75PlusPIDs.PID[i] != 0x1fff ) {
					printf( "%d) %d (0x%x)  ", i, s75PlusPIDs.PID[i], s75PlusPIDs.PID[i] );
					switch( s75PlusPIDs.Control[i] ) {
					case 0 : printf( "RAW" ); break;
					case 1 : printf( "MPE" ); break;
					case 2 : printf( "PES" ); break;
					}
					printf( "\n" );
					if ( s75PlusPIDs.Control[i] == 0 ) {
						for ( j = 0; j < NUM_S75PRO_DESTINATIONS; j++ ) {
							if ( s75PlusPIDs.DestinationMask[i] & ( 0x8000 >> j ) ) {
								printf( "    %d.%d.%d.%d:%d, ",
										s75PlusPIDs.Destination[j].DestinationIP[0],
										s75PlusPIDs.Destination[j].DestinationIP[1],
										s75PlusPIDs.Destination[j].DestinationIP[2],
										s75PlusPIDs.Destination[j].DestinationIP[3],
										s75PlusPIDs.Destination[j].DestinationUDP    );
							}
						}
						printf( "\n" );
					}
				}
			}
		}
	}
	else if ( (devType==PesMaker) || (devType==A75PesMaker) || (devType==S75CA) ) {
		result = getPIDList(hdev,(BYTE *)&pesMakerPIDs);
		printf("Get PID List returned %X", result);
		printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",pesMakerPIDs.NumberofPIDs);
		for(i=0;i<pesMakerPIDs.NumberofPIDs;i++)
		printf(" 0x%X   MPE Processing %d [1=On, 0=Off] \n Forwarding IP: %d.%d.%d.%d   Forwarding Port: %d \n\n",pesMakerPIDs.Entry[i],pesMakerPIDs.Control[i],pesMakerPIDs.ForwardInformation[i].Destination_IP[0],pesMakerPIDs.ForwardInformation[i].Destination_IP[1],pesMakerPIDs.ForwardInformation[i].Destination_IP[2],pesMakerPIDs.ForwardInformation[i].Destination_IP[3],pesMakerPIDs.ForwardInformation[i].Destination_Port);
	}
	else { //V2.1 or V3
		result = getPIDList(hdev,(BYTE *)&PIDs);
		printf("\n There are %d PID(s) in the PID List \n PID List:\n --------\n",PIDs.NumberofPIDs);
		for(i=0;i<PIDs.NumberofPIDs;i++) printf(" 0x%X   MPE Processing %d [1=On, 0=Off] \n",PIDs.Entry[i],PIDs.Control[i] );
	}

	return result;

}

int userSetDVBMAC(S75_Handle hdev)
{
   BYTE DVBMAC[6];
   int result = S75_FAILED;

   printf("\n-------------------------------------------------------------\n");
   printf("\n Please Enter the DVB MAC Address to be used (Hex), use ':' to Seperate the bytes");
   scanf("%x:%x:%x:%x:%x:%x", &DVBMAC[0],&DVBMAC[1],&DVBMAC[2],&DVBMAC[3],&DVBMAC[4],&DVBMAC[5]);
   result = SetDVBMACAddress(hdev, DVBMAC);

   return result;

}


int userGetPrograms(S75_Handle hdev)
{
	WORD devType;
	S75CA_ProgramList s75caPrograms;
	int result = S75_FAILED;

	GetDeviceType(hdev,devType);

	if ( ( devType==S75CA ) || ( devType==S75CAPRO ) ) {
		result = getProgramList(hdev,(BYTE *)&s75caPrograms);
		printf( "\n-----Programs-----\n" );
		for ( int i = 0; i < 16; i++ ) {
			if ( s75caPrograms.ProgramNumber[i] ) {
				printf( " %d status=%x\n", s75caPrograms.ProgramNumber[i], s75caPrograms.ProgramStatus[i] );
			}
		}
	}

	return result;
}


bool editPrograms( S75CA_ProgramList *s75caPrograms )
{
	int i, j;
	int program;
	bool found;
	bool write_table = false;


	do{
		printf( "\n--------------Programs--------------\n" );
		for ( i = 0; i < 16; i++ ) {
			if ( s75caPrograms->ProgramNumber[i] ) {
				printf( "%d\t(0x%x)\tstatus=%x\n", s75caPrograms->ProgramNumber[i], s75caPrograms->ProgramNumber[i], s75caPrograms->ProgramStatus[i] );
			}
		}
		printf( "\n" );
		printf("\n\t\t (1) Add Program");
		printf("\n\t\t (2) Remove Program");
        printf("\n\t\t (3) Write Changes to Receiver");
        printf("\n\t\t (0) Exit Without Writing Changes to the Receiver");
		printf("\n\n\t\t Select an Action: ");

		scanf("%d",&i);
		getchar();

		switch(i){
			case 0: break;
			case 1:
					printf("\n Enter Program to add to the List (Decimal value) --> ");
					scanf("%d",&program);
					if(( program == 0)||(program>65535)){ printf("\n Program value must be 1-65535 "); break ;}
					found = false;
					j = 0;
					while ( (!found) && ( j < 16 ) ) {
						if ( s75caPrograms->ProgramNumber[j] == program )
							found = true;
						else
							j++;
					}
					if ( !found ) {
						j =0;
						while ( (!found) && ( j < 16 ) ) {
							if ( s75caPrograms->ProgramNumber[j] == 0 )
								found = true;
							else
								j++;
						}
						if ( found ) {
							s75caPrograms->ProgramNumber[j] = program;
							s75caPrograms->ProgramStatus[j] = 0;
						} else {
							printf( " Program list is full; Entry Terminated " );
						}
					}
					break;
			case 2:
					printf("\n Enter program number to remove from the List (Decimal value) --> ");
					scanf("%d",&program);
					found = false;
					j = 0;
					while ( (!found) && ( j < 16 ) ) {
						if ( s75caPrograms->ProgramNumber[j] == program )
							found = true;
						else
							j++;
					}
					if ( found ) {
						s75caPrograms->ProgramNumber[j] = 0;
						s75caPrograms->ProgramStatus[j] = 0;
					} else {
						printf( "\n Program number was not in the list." );
					}
					break;
    		case 3: 
					write_table = true;
					break;
			default:
				printf(" \n\n \t\t Invalid Selection \a\a\n\n");
				break;
		}


	}	while((i!=0)&&(i!=3));

	return write_table;
}


int userSetPrograms(S75_Handle hdev)
{
	WORD devType;
	S75CA_ProgramList s75caPrograms;
	int i, j;
	int result = S75_FAILED;

	GetDeviceType(hdev,devType);

	if ( ( devType==S75CA ) || ( devType==S75CAPRO ) ) {
		result = getProgramList(hdev,(BYTE *)&s75caPrograms);
		for ( i = 0; i < 16; i++ ) {
			if ( s75caPrograms.ProgramNumber[i] ) {
				for ( j = i+1; j < 16; j++ ) {
					if ( s75caPrograms.ProgramNumber[j] == s75caPrograms.ProgramNumber[i] ) {
						s75caPrograms.ProgramNumber[j] = 0;
						s75caPrograms.ProgramStatus[j] = 0;
					}
				}
			}
		}
		if ( editPrograms( &s75caPrograms ) ) {
//			printf("\n Sending the Program List returned %X\n", setProgramList(hdev, (BYTE *)&s75caPrograms));
			result = setProgramList(hdev, (BYTE *)&s75caPrograms);
		}

	}

	return result;
}


int userGetPAT(S75_Handle hdev)
{
	WORD devType;
	S75PlusPro_PAT s75ProPAT;
	int result = S75_FAILED;

	GetDeviceType(hdev,devType);

	if ( ( devType==S75PLUSPRO ) || ( devType==S75CAPRO ) || ( devType==S75FKPRO ) ) {
		result = getPAT(hdev,(BYTE *)&s75ProPAT);
		printf( "\n----Program Association Table----\n" );
		for ( int i = 0; i < 16; i++ ) {
			if ( s75ProPAT.ProgramNumber[i] ) {
				printf( " Prog # = %d\tPMT PID = %d\n", s75ProPAT.ProgramNumber[i], s75ProPAT.PMTPID[i] );
			}
		}
	}

	return result;
}


bool editPAT( S75PlusPro_PAT *s75ProPAT )
{
	int i, j;
	int program;
	int pmt_pid;
	bool found;
	bool write_table = false;
	bool pid_ok = false;


	do{
		printf( "\n------------Program Association Table------------\n" );
		for ( i = 0; i < 16; i++ ) {
			if ( s75ProPAT->ProgramNumber[i] ) {
				printf( " Prog # = %d\tPMT PID = %d\n", s75ProPAT->ProgramNumber[i], s75ProPAT->PMTPID[i] );
			}
		}
		printf( "\n" );
		printf("\n\t\t (1) Add/Edit Program");
		printf("\n\t\t (2) Remove Program");
        printf("\n\t\t (3) Write Changes to Receiver");
        printf("\n\t\t (0) Exit Without Writing Changes to the Receiver");
		printf("\n\n\t\t Select an Action: ");

		scanf("%d",&i);
		getchar();

		switch(i){
			case 0: break;
			case 1:
					printf("\n Enter Program to add/edit (Decimal value) --> ");
					scanf("%d",&program);
					if(( program == 0)||(program>65535)){ printf("\n Program value must be 1-65535 "); break ;}
					found = false;
					j = 0;
					while ( (!found) && ( j < 16 ) ) {
						if ( s75ProPAT->ProgramNumber[j] == program )
							found = true;
						else
							j++;
					}
					if ( !found ) {
						j =0;
						while ( (!found) && ( j < 16 ) ) {
							if ( s75ProPAT->ProgramNumber[j] == 0 )
								found = true;
							else
								j++;
						}
					}
					if ( found ) {
						s75ProPAT->ProgramNumber[j] = program;
						pid_ok = false;
						while ( !pid_ok ) {
							printf("\n Enter PMT PID (Decimal value 1-8191) --> ");
							scanf("%d",&pmt_pid);
							if ( ( pmt_pid > 0 ) && ( pmt_pid < 8191 ) ) {
								s75ProPAT->PMTPID[j] = pmt_pid;
								pid_ok = true;
							}
						}
					} else {
						printf( " Program list is full; Entry Terminated " );
					}
					break;
			case 2:
					printf("\n Enter program number to remove from the List (Decimal value) --> ");
					scanf("%d",&program);
					found = false;
					j = 0;
					while ( (!found) && ( j < 16 ) ) {
						if ( s75ProPAT->ProgramNumber[j] == program )
							found = true;
						else
							j++;
					}
					if ( found ) {
						s75ProPAT->ProgramNumber[j] = 0;
						s75ProPAT->PMTPID[j] = 0;
					} else {
						printf( "\n Program number was not in the list." );
					}
					break;
    		case 3: 
					write_table = true;
					break;
			default:
				printf(" \n\n \t\t Invalid Selection \a\a\n\n");
				break;
		}


	}	while((i!=0)&&(i!=3));

	return write_table;
}


int userSetPAT(S75_Handle hdev)
{
	WORD devType;
	S75PlusPro_PAT s75ProPAT;
	int i, j;
	int result = S75_FAILED;

	GetDeviceType(hdev,devType);

	if ( ( devType==S75PLUSPRO ) || ( devType==S75CAPRO ) || ( devType==S75FKPRO ) ) {
		result = getPAT(hdev,(BYTE *)&s75ProPAT);
		for ( i = 0; i < 16; i++ ) {
			if ( s75ProPAT.ProgramNumber[i] ) {
				for ( j = i+1; j < 16; j++ ) {
					if ( s75ProPAT.ProgramNumber[j] == s75ProPAT.ProgramNumber[i] ) {
						s75ProPAT.ProgramNumber[j] = 0;
						s75ProPAT.PMTPID[j] = 0;
					}
				}
			} else {
				s75ProPAT.PMTPID[i] = 0;
			}
		}
		if ( editPAT( &s75ProPAT ) ) {
//			printf("\n Sending the PAT returned %X\n", setPAT(hdev, (BYTE *)&s75ProPAT));
			result = setPAT(hdev, (BYTE *)&s75ProPAT);
		}

	}

	return result;
}

int displayTraps( S75PlusPro_TrapSetting *s75ProTraps )
{
	printf( "\n----Traps----\n" );
	printf( " Monitoring Interval is %d\n", s75ProTraps->MonitoringInterval );
	printf( " Notification IP:port is %d.%d.%d.%d:%d\n",
			s75ProTraps->NotificationIP[0], s75ProTraps->NotificationIP[1],
			s75ProTraps->NotificationIP[2], s75ProTraps->NotificationIP[3],
			s75ProTraps->NotificationPort );
	if ( s75ProTraps->DataLockMonitorOn ) {
		printf( " Data Lock Monitor is ON\n" );
	} else {
		printf( " Data Lock Monitor is OFF\n" );
	}
	if ( s75ProTraps->EthernetTxErrorOn ) {
		printf( " Ethernet TX Error Monitor is ON, " );
	} else {
		printf( " Ethernet TX Error Monitor is OFF, " );
	}
	printf( " Threshold is %d\n", s75ProTraps->EthernetTxErrorThreshold );
	if ( s75ProTraps->TSErrorOn ) {
		printf( " Transport Stream Uncorrectables Monitor is ON, " );
	} else {
		printf( " Transport Stream Uncorrectables Monitor is OFF, " );
	}
	printf( " Threshold is %d\n", s75ProTraps->UncorrectableTSErrorThreshold );
	if ( s75ProTraps->VBEROn ) {
		printf( " Viterbi Bit Error Rate Monitor is ON, " );
	} else {
		printf( " Viterbi Bit Error Rate Monitor is OFF, " );
	}
	printf( " Threshold is %d\n", s75ProTraps->VBERThreshold );
}


int userGetTraps(S75_Handle hdev)
{
	WORD devType;
	S75PlusPro_TrapSetting s75ProTraps;
	int result = S75_FAILED;

	GetDeviceType(hdev,devType);

	if ( devType==S75CA ) {
		result = getTraps(hdev,(BYTE *)&s75ProTraps);
		displayTraps( &s75ProTraps );
	}

	return result;
}


int userSetTraps(S75_Handle hdev)
{
	WORD devType;
	S75PlusPro_TrapSetting s75ProTraps;
	bool write_traps = false;
	int i;
	int monitoring_interval;
	char userinput[120];
	int b1, b2, b3, b4;
	int notification_port;
	int flag;
	long threshold;
	int result = S75_FAILED;


	GetDeviceType(hdev,devType);

	if ( devType==S75CA ) {
		result = getTraps(hdev,(BYTE *)&s75ProTraps);
		do {
			displayTraps( &s75ProTraps );
			printf( "\n" );
			printf("\n\t\t (1) Set Monitoring Interval");
			printf("\n\t\t (2) Set Traps Destination");
			printf("\n\t\t (3) Turn Data Lock Monitor ON/OFF");
			printf("\n\t\t (4) Turn Ethernet TX Error Monitor ON/OFF");
			printf("\n\t\t (5) Turn Transport Stream Uncorrectables Monitor ON/OFF");
			printf("\n\t\t (6) Turn Viterbi Bit error Rate Monitor ON/OFF");
			printf("\n\t\t (7) Write Changes to Receiver");
			printf("\n\t\t (0) Exit Without Writing Changes to the Receiver");
			printf("\n\n\t\t Select an Action: ");

			scanf("%d",&i);
			getchar();

			switch(i){
				case 0: break;
				case 1:
						printf("\n Enter Program monitoring interval (Decimal value) --> ");
						scanf("%d",&monitoring_interval);
						if(( monitoring_interval == 0)||(monitoring_interval>65535)){ printf("\n Interval must be 1-65535 "); break ;}
						s75ProTraps.MonitoringInterval = monitoring_interval;
						break;
				case 2:
						printf("\n Destination IP for Traps --> " );
						gets(userinput);
						sscanf(	userinput,"%d.%d.%d.%d", &b1, &b2, &b3, &b4 );
						printf("\n Destination Port for Traps --> " );
						gets(userinput);
						sscanf(	userinput,"%d", &notification_port );
						if ( ( notification_port == 0 ) || ( notification_port > 65535 ) ) {  printf("\n Port must be 1-65535 "); break ;}
						s75ProTraps.NotificationIP[0] = (unsigned char)b1;
						s75ProTraps.NotificationIP[1] = (unsigned char)b2;
						s75ProTraps.NotificationIP[2] = (unsigned char)b3;
						s75ProTraps.NotificationIP[3] = (unsigned char)b4;
						s75ProTraps.NotificationPort = (WORD)notification_port;
						break;
				case 3:
						printf( "\n Enter 0 (Off) or 1 (On) --> " );
						scanf( "%d", &flag );
						s75ProTraps.DataLockMonitorOn = (bool)flag;
						break;
				case 4:
						printf( "\n Enter 0 (Off) or 1 (On) --> " );
						scanf( "%d", &flag );
						printf( "\n Enter Threshold --> " );
						scanf("%d",&threshold);
						if(threshold>65535){ printf("\n Interval must be 0-65535 "); break ;}
						s75ProTraps.EthernetTxErrorOn = (bool)flag;
						s75ProTraps.EthernetTxErrorThreshold = threshold;
						break;
				case 5:
						printf( "\n Enter 0 (Off) or 1 (On) --> " );
						scanf( "%d", &flag );
						printf( "\n Enter Threshold --> " );
						scanf("%d",&threshold);
						s75ProTraps.TSErrorOn = (bool)flag;
						s75ProTraps.UncorrectableTSErrorThreshold = threshold;
						break;
				case 6:
						printf( "\n Enter 0 (Off) or 1 (On) --> " );
						scanf( "%d", &flag );
						printf( "\n Enter Threshold --> " );
						scanf("%d",&threshold);
						s75ProTraps.VBEROn = (bool)flag;
						s75ProTraps.VBERThreshold = threshold;
						break;
    			case 7: 
						write_traps = true;
						break;
				default:
					printf(" \n\n \t\t Invalid Selection \a\a\n\n");
					break;
			}
		}	while((i!=0)&&(i!=7));

		if ( write_traps ) {
//			printf("\n Sending the Trap configuration returned %X\n", setTraps(hdev, (BYTE *)&s75ProTraps));
			result = setTraps(hdev, (BYTE *)&s75ProTraps);
		}

	}

	return result;
}



int userChangePassword(S75_Handle hdev)
{
	unsigned char key[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	WORD devType;
	char new_password1[120];
	char new_password2[120];
	char password_array[8];
	int i;
	int result = S75_FAILED;


	GetDeviceType(hdev,devType);

	if ( 	( devType==S75PLUS ) || 
		( devType==S75PLUSPRO ) ||
		( devType==S75CA ) ||
		( devType==S75CAPRO ) ||
		( devType==S75FK ) ||
		( devType==S75FKPRO ) ) {

		printf( "\n New Password : " );
		gets( new_password1 );
		printf( "\n New Password : " );
		gets( new_password2 );
		if ( strcmp( new_password1, new_password2 ) == 0 ) {
			if ( strlen( new_password1 ) <= 8 ) {
				memset( password_array, 0, 8 );
				for ( i = 0; ( i < 8 ) && ( i < (int)strlen( new_password1) ); i++ ) {
					password_array[i] = new_password1[i];
				}
				result = changePassword( hdev, password_array, key );
			} else {
				printf( "\n\n Password too long. Password remains unchanged!\n" );
			}
		} else {
			printf( "\n\n You must enter the new password the same way twice.  Password remains unchanged!\n" );
		}

	}

	return result;
}
