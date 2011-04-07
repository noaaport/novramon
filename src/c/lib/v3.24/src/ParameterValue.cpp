//#include "stdafx.h"

#include "ParameterValue.h"

// Constructors/Destructors
//  

ParameterValue::ParameterValue ( ) {

	initAttributes();

}

ParameterValue::~ParameterValue ( ) { }

//  
// Methods
//  


// Accessor methods
//  


// Other methods
//  

void ParameterValue::initAttributes ( ) {

    m_value = "";
    m_dataType = NO_TYPE;
    m_isSet = false;

}

