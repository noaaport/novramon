#ifndef PARAMETERVALUE_H
#define PARAMETERVALUE_H

#include <string>

#ifdef VS6
	#include <iostream.h>
#else
	#include <iostream>
#endif

#ifndef WINDOWS
	#include <cstdio>
#endif


#include "DataTypes.h"

using namespace std;


// This class implements a data value.
/**
  * It allows the programmer to interact with a data entity without having to know the underlying type until it is required.
  *
  * Users of the Novra Receiver Communications API will not typically need to use the constructors, destructors, set or get methods of this class, as this will be done for them by the Receiver class.
  *
  * Programmers will typically use only the member functions asString(), asShort(), asLong(), asI64(), asFloat(), asFlag() and isSet().
  */

class ParameterValue /// This class implements a data value.
{
public:

    // Constructors/Destructors
    //  

	/**
	* Empty Constructor
	*/
	ParameterValue ( );


    /**
	 * Initializing Constructor 
     *
     * @param  dataType the native type of the data item
     * @param  value the initial value of the data item
     */

    ParameterValue( DataType dataType, string value )
	{
		setM_dataType( dataType );
		setM_value( value );
		setM_isSet( true );
	}


    /**
     * Destructor
     */
    virtual ~ParameterValue ( );


    /**
     * Set the value of m_value
     * @param new_var the new value of m_value
     */
    void setM_value ( string new_var )     {
            m_value = new_var;
			m_isSet = true;
    }

    /**
     * Get the value of m_value
     * @return the value of m_value
     */
    string getM_value ( )     {
        return m_value;
    }

    /**
     * Set the value of m_dataType
     * @param new_var the new value of m_dataType
     */
    void setM_dataType ( DataType new_var )     {
            m_dataType = new_var;
    }

    /**
     * Get the value of m_dataType
     * @return the value of m_dataType
     */
    DataType getM_dataType ( )     {
        return m_dataType;
    }

    /**
     * Set the value of m_isSet
     * @param new_var the new value of m_isSet
     */
    void setM_isSet ( bool new_var )     {
            m_isSet = new_var;
    }

    /**
     * Get the value of m_isSet
     * @return the value of m_isSet
     */
    bool getM_isSet ( )     {
        return m_isSet;
    }

    /**
     * @return bool
     */
    bool isSet ( )
    {
		return m_isSet;
    }


//	string validRange() {
//		return string( "" );
//	}


    /**
     * Returns string representation of a parameter value.
     * @return string
     */
    virtual string asString ( )
    {
		return m_value;
    }


    /**
     * Returns parameter value as a short integer if possible, NaN otherwise.
     * @return short
     */
    short asShort ( )
    {
		long s = 0;

		if ( m_isSet ) {
			if ( m_value.find( "." ) == string::npos ) {
				sscanf( m_value.c_str(), "%ld", &s );
			} else {
				s = (short)( asFloat() + 0.5 );
			}
		}

		return s;
    }


    /**
     * Returns parameter value as a long integer if possible, NaN otherwise.
     * @return long
     */
    long asLong ( )
    {
		long l = 0;

		if ( m_isSet ) {
			if ( m_value.find( "." ) == string::npos ) {
				sscanf( m_value.c_str(), "%ld", &l );
			} else {
				l = (long)( asFloat() + 0.5 );
			}
		}

		return l;
    }


    /**
     * Returns parameter value as a 64 bit integer if possible, NaN otherwise.
     * @return unsigned __int64
     */
#ifdef WINDOWS
    unsigned __int64 asUI64 ( )
    {
		unsigned __int64 l = 0;

		if ( m_isSet ) {
			if ( m_value.find( "." ) == string::npos ) {
				sscanf( m_value.c_str(), "%I64u", &l );
			} else {
				l = (unsigned __int64)( asFloat() + 0.5 );
			}
		}

		return l;
    }
#else
    unsigned long long asUI64 ( )
    {
		unsigned long long l = 0;

		if ( m_isSet ) {
			if ( m_value.find( "." ) == string::npos ) {
				sscanf( m_value.c_str(), "%Lu", &l );
			} else {
				l = (unsigned long long)( asFloat() + 0.5 );
			}
		}

		return l;
    }
#endif


    /**
     * Returns parameter value as a double precision floating point value if possible,
     * NaN otherwise.
     * @return double
     */
    double asFloat ( )
    {
		float f = 0.0;

		if ( m_isSet ) {
			sscanf( m_value.c_str(), "%f", &f );
		}

		return f;
    }


    /**
     * Returns parameter value as a boolean value if possible,
     * NaN otherwise.
     * @return bool
     */
    bool asFlag ( )
    {
		int d = 0;
		bool f = false;

		if ( m_isSet ) {
			sscanf( m_value.c_str(), "%d", &d );
		}

	    if ( d != 0 ) f = true;

		return f;
    }

private:


    void initAttributes ( ) ;

    // Private attributes
    //  

    string m_value;
    DataType m_dataType;
    bool m_isSet;

};


typedef ParameterValue* ParameterValuePtr;

#endif // PARAMETERVALUE_H

