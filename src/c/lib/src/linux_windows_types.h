//************************************************************************
//*
//* Copyright Novra Technologies Incorporated, 2003
//* 1100-330 St. Mary Avenue
//* Winnipeg, Manitoba
//* Canada R3C 3Z5
//* Telephone (204) 989-4724
//*
//* All rights reserved. The copyright for the computer program(s) contained
//* herein is the property of Novra Technologies Incorporated, Canada.
//* No part of the computer program(s) contained herein may be reproduced or
//* transmitted in any form by any means without direct written permission
//* from Novra Technologies Inc. or in accordance with the terms and
//* conditions stipulated in the agreement/contract under which the
//* program(s) have been supplied.
//*
//***************************************************************************
//
// linux_windows_types.h
//

#ifndef __linux_windows_types

    #define __linux_windows_types

    #define BOOL bool
#ifndef TRUE
    #define TRUE true
#endif
#ifndef FALSE
    #define FALSE false
#endif
    typedef unsigned char   BYTE;
    typedef unsigned short  WORD;
    typedef unsigned int    DWORD;
    typedef unsigned int    UINT;
    typedef unsigned char*  LPBYTE;
    typedef char            TCHAR;
    typedef int             SOCKET;
    typedef unsigned long   ULONG;
    typedef char*           LPTSTR;

#endif
