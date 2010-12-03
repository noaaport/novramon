#include "RecComLib.h"


int main ( int argc, char *arv[] )
{
    ReceiverSearch rs;
    ReceiverList rl;
    Receiver *r;
    ParameterValue p;
    int i;
    NOVRA_ERRORS e;
    char password[100];
    char temp[100];


    // Discover reporting receivers

    rs.discoverLocal( &rl );

    if ( rl.count() == 0 ) {
        cout<<"No local receivers found."<<endl;
        exit( 0 );
    }

    // List the receivers found

    for ( i = 0; i < rl.count(); i++ ) {

        r = rl.getReceiver( i );

        cout<<"Found "<<r->getParameter( DEVICE_TYPE ).asString()<<endl;

        cout<<"IP address: "<<r->getParameter( RECEIVER_IP ).asString()<<endl;

        cout<<"MAC: "<<r->getParameter( RECEIVER_MAC ).asString()<<endl<<endl;

        cout<<endl;
    }

    // Use the first one on the list

//    r = rl.getReceiver( 0 );

    r = rs.getRemote( "192.168.0.136" );

    if ( r ) {

        r->connect( 4, e );

        // Log on if neccessary

        if ( r->hasParameter( PASSWORD ) ) {

            do {
                cout<<"Password: ";
                cin>>password;
            } while ( !r->login( password ) ); 
  
        }

        if ( r->connected( 2 ) ) {

            cout<<"Ready to go!"<<endl<<endl;

            // Get status and update our copy

            r->getStatus();
            r->updateStatus();

            // Read current settings from receiver

            r->readCurrentSettings();

            // If the receiver has a PID list, list them

            if ( r->hasParameter( SIZE_PID_LIST ) ) {
                cout<<"This receiver supports "<<r->getParameter( SIZE_PID_LIST ).asShort()<<" PIDs"<<endl;
		cout<<r->getParameter( PID_COUNT ).asShort()<<" PIDs are set"<<endl;
                cout<<"PID LIST: ";
                for ( i = 0; i < r->getParameter( SIZE_PID_LIST ).asShort(); i++ ) {
                    cout<<r->getIndexedParameter( PID, i ).asShort()<<"   ";
                }
                cout<<endl;
            }
    
            // If the receiver has a program list, list them

            if ( r->hasParameter( SIZE_VPROGRAM_LIST ) ) {
                cout<<"This receiver supports "<<r->getParameter( SIZE_VPROGRAM_LIST ).asShort()<<" programs"<<endl;
                cout<<"Program List: ";
                for ( i = 0; i < r->getParameter( SIZE_VPROGRAM_LIST ).asShort(); i++ ) {
                    cout<<r->getIndexedParameter( VPROGRAM_NUMBER, i ).asShort()<<"   ";
                }
                cout<<endl;
            }

            cout<<endl;

            // Dump receiver status to XML

	    r->statusToXML( "./status.xml" );

            // Dump receiver definition to XML

	    r->definitionToXML( "./definition.xml" );


            // List the parameter enumeration

	    for ( i = 0; i <= (int)MAX_RECEIVER_PARAM+1; i++ ) {
		cout<<EnumeratedReceiverParameter( (ReceiverParameter)0, itoa( i, temp, 10 ) ).asString().c_str()<<endl;;
    	    }

            cout<<endl;

            // Dump receiver status to XML

            r->statusToXML( stdout );
            r->definitionToXML( stdout );


	    r->statusToXML( "./status.xml" );
            cout<<"Receiver status has been dumped to status.xml"<<endl;

            // Dump receiver definition to XML

	    r->definitionToXML( "./definition.xml" );
            cout<<"Receiver definition has been dumped to definition.xml"<<endl;

            r->disconnect();
            cout<<"Goodbye!"<<endl;

        } else {

            cout<<"Connection Problem! Exiting."<<endl;

        }

    }

    return 0;

}
