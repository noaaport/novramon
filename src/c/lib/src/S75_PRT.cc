// S75_PRT.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "S75_PRT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include<stdio.h>
#include<malloc.h>
#include <iostream>
#include <string>
#ifdef LINUX
	#include "linux_windows_types.h"
#endif
#include"s75.h"
#define PrintMAC(Mac) printf("%0.2X-%0.2X-%0.2X-%0.2X-%0.2X-%0.2X", Mac[0],Mac[1],Mac[2],Mac[3],Mac[4],Mac[5]);
#define PrintIP(Mac) printf("%d.%d.%d.%d", Mac[0],Mac[1],Mac[2],Mac[3]);


#ifndef LINUX
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;
#endif

using namespace std;



#ifdef LINUX
int main(int argc, char* argv[])
#else
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
#endif
{
	unsigned char key[16] = { 0x4d, 0x69, 0x6b, 0x65, 0x26, 0x4e, 0x61, 0x73, 0x68, 0x61, 0x61, 0x74, 0x44, 0x61, 0x76, 0x65 };
	Novra_DeviceList list ;
	S75_Handle hdev;
	int i;
	int j;
	string choice="L";
	string password;
	char temp[20];
	int index = -1;
	bool no_exit = true;
	bool automatic = true;
	char password_array[8];
	int Errcode;

#ifndef LINUX

	if (AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{

#endif
		cout<<endl;
		cout<<endl;
		cout<<endl;
		cout<<" JSAT / Leo Palace / Novra S75 Password Recovery Tool (V1.2 RC1)"<<endl;
		cout<<"-------------------------------------------------------------------------------"<<endl;
		cout<<endl;

		while ( no_exit ) {
			if ( !automatic ) {
				cout<<endl;
				cout<<" (L)ist Devices, (S)elect Device, (P)assword Change, (E)xit : ";
				cin>>choice;
				cout<<endl;
			}
			if ( ( choice.at(0) == 'l' ) || ( choice.at(0) == 'L' ) ) {		

				printf( " Searching for devices... \n" );

				discoverDevices(&list);

				for ( i = 0; i < list.num_devices; i++ ) {
					if ( list.Device[i].DeviceType != S75_JLEO ) {
						for ( j = i; j < list.num_devices - 1; j++ ) {
							memcpy( (void *)(&(list.Device[j])), (void *)(&(list.Device[j+1])), sizeof( Novra_DeviceEntry ) );
						}
						list.num_devices--;
					}
				}	

				if ( list.num_devices > 0 ) {	

					printf("\n Found %d Device(s) \n",list.num_devices);
					for(i=0; i<list.num_devices;i++){
						printf("\n %d)", i+1 );
						printf(" MAC --> ");
						PrintMAC(list.Device[i].DeviceMAC);
						printf(" , IP --> ");
						PrintIP(list.Device[i].DeviceIP);
					}

				}

				cout<<endl;
				cout<<endl;	

				if ( list.num_devices < 1 ) {
					cout<<endl;	
					cout<<" There are no S75 devices.  Exiting."<<endl;
					cout<<endl;
					choice = "E";
					automatic=true;
				} else 
				if ( list.num_devices == 1 ) {
					index = 0;
					automatic = true;
					choice = "P";
				} else
				if ( ( index > 0 ) && ( index < list.num_devices ) ) {
					automatic = false;
				} else {
					automatic = true;
					choice = "S";
				}

			} else 
			if ( ( choice.at(0) == 's' ) || ( choice.at(0) == 'S' ) ) {
				automatic = false;
				if ( list.num_devices >= 1 ) {
					do {
						cout<<endl;
						cout<<" Select a device (1-"<<list.num_devices<<"): ";
						cin>>choice;
						if ( ( choice.size() > 0 ) && ( choice.find_first_not_of( "0123456789", 0, 10 ) == string::npos ) ) {
							memset( temp, 0, 20 );
							strncpy( temp, choice.c_str(), 20 );
							sscanf( temp, "%d", &index );
							index--;
						} else {
							index = -1;
						}
					} while ( !(( index >= 0 ) && ( index <= list.num_devices-1 )) );
					cout<<endl;
					cout<<" You chose ";
					printf(" MAC --> ");
					PrintMAC(list.Device[index].DeviceMAC);
					printf(" , IP --> ");
					PrintIP(list.Device[index].DeviceIP);
					cout<<endl;
					cout<<endl;
				} else {
					cout<<"There are no devices to choose from"<<endl;
				}
			} else
			if ( ( choice.at(0) == 'p' ) || ( choice.at(0) == 'P' ) ) {
				automatic = false;
				do {
					cout<<endl;
					cout<<" Enter new password: ";
					cin>>password;
					if ( password.size() > 8 ) {
						cout<<endl;
						cout<<" Password must be no more than 8 characters!"<<endl;
					}
				} while ( password.size() > 8 );
				do {
					cout<<endl;
					cout<<" Change password of ";
					PrintMAC(list.Device[index].DeviceMAC);
					printf(" , ");
					PrintIP(list.Device[index].DeviceIP);
					cout<<" to "<<password<<" ? (Y/N): ";
					cin>>choice;
				} while ( !( ( choice.at(0) == 'n' ) || ( choice.at(0) == 'N' ) ||( choice.at(0) == 'y' ) ||( choice.at(0) == 'Y' ) ) );
				if ( ( choice.at(0) == 'y' ) || ( choice.at(0) == 'Y' ) ) {
					if( (hdev =openDevice(&(list.Device[index]),1000, Errcode ))== NULL ){
						printf("Open Device Failed, Error Code 0x%X ", Errcode );
					} else {
						memset( password_array, 0, 8 );
						for ( i = 0; ( i < 8 ) && ( i < (int)password.size() ); i++ ) {
							password_array[i] = password.at(i);
						}
						cout<<endl;
						if ( changePassword( hdev, password_array, key ) == ERR_SUCCESS ) {
							cout<<" Password Changed"<<endl;
						} else {
							cout<<" Password Change Failed!"<<endl;
						}
						closeS75( hdev );
					}
				} else {
					cout<<endl;
					cout<<" Change Aborted!"<<endl;
				}
				if ( list.num_devices == 1 ) {
					automatic = true;
					choice = "E";
				}
			} else
			if ( ( choice.at(0) == 'e' ) || ( choice.at(0) == 'E' ) ) {
//				automatic = false;
				no_exit = false;
			} 
		

		}

		cout<<endl;
		cout<<endl;

#ifndef LINUX
	}
#endif

#ifndef LINUX
	if ( automatic ) {
		getchar();
	}
#endif

	return 0;
}


