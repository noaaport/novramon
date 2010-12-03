#include "RecComLib.h"
#include <iostream>


int main ( int argc, char *arv[] )
{

    char temp[20];

     
    cerr<<itoa( 4095, temp, 16 )<<endl;
    cerr<<itoa( 4095, temp, 10 )<<endl;
    cerr<<itoa( 4095, temp, 8 )<<endl;
    cerr<<itoa( 4095, temp, 2 )<<endl;


    return 0;

}
