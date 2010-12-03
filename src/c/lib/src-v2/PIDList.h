// PIDList.h: interface for the PIDList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIDLIST_H__66D38E21_8061_423E_B2C4_98A9A48CB3CA__INCLUDED_)
#define AFX_PIDLIST_H__66D38E21_8061_423E_B2C4_98A9A48CB3CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef LINUX
#include <windows.h>
#else
#include "linux_windows_types.h"
#endif

class PIDList  
{
public:
	PIDList();
	virtual ~PIDList();
	virtual BYTE* DataPointer()=0;
	virtual WORD* EntryPointer()=0;
	virtual BYTE* ControlPointer()=0;
	virtual void DeletePIDList()=0;
};

#endif // !defined(AFX_PIDLIST_H__66D38E21_8061_423E_B2C4_98A9A48CB3CA__INCLUDED_)
