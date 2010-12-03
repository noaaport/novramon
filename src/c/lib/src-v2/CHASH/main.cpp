#include "CHash.h"

int main( int argc, char *argv[] )
{

	CHash ch;

	ch.SetHashString( "HELLO" );
	cerr<<ch.DoHash()<<endl;
}
