// Source: Kernighan & Ritchie, "The C Programming Language", 2nd Edition, p.64
#include <iostream>
#include <string.h>


void reverse( char s[] )
{
    int i, j;
    char c;

    for ( i = 0, j = strlen(s)-1; i<j; i++, j-- ) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }

}


char* itoa( int n, char s[], int radix )
{

    int i, sign;
    int digit;

    if ( ( sign = n ) < 0 ) {
        n = -n;
    }

    i = 0;
    do {
        digit = n % radix;
        if ( digit < 10 ) {
            s[i++] = digit + '0';
        } else {
            s[i++] = (digit-10) + 'a';
        }
    } while ( ( n /= radix ) > 0 );
    
    if ( sign < 0 ) {
        s[i++] = '-';
    }

    s[i] = '\0';

    reverse( s );

    return s;
}

