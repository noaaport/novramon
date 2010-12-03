#include <stdio.h>
#include <unistd.h>
#include <string>

using namespace std;

unsigned short poi[6] = { 0, 1, 0x7fff, 0x8000, 0x8001, 0xffff };

int main( int arc, char *argv[] )
{
	unsigned long long big_number = 0;
	unsigned short w[4];
	int i3, i2, i1, i0;
	char temp[100];
	string s;

	for ( i3 = 0; i3 < 6; i3++ ) {
	for ( i2 = 0; i2 < 6; i2++ ) {
	for ( i1 = 0; i1 < 6; i1++ ) {
	for ( i0 = 0; i0 < 6; i0++ ) {
		w[3] = poi[i3];
		w[2] = poi[i2];
		w[1] = poi[i1];
		w[0] = poi[i0];
		big_number = w[3];
		big_number = big_number << 16;
		big_number += w[2];
		big_number = big_number << 16;
		big_number += w[1];
		big_number = big_number << 16;
		big_number += w[0];
	        sprintf( temp, "%Lu", big_number );
		s = string( temp );
		sscanf( s.c_str(), "%Lu", &big_number );
	        printf( "%Lu\t%x\t%x\t%x\t%x\n", big_number, poi[i3], poi[i2], poi[i1], poi[i0] );
	}
	}
	}
	}

	return 0;
}

