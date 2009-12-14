// StandardPIDList.h: interface for the StandardPIDList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STANDARDPIDLIST_H__1ECF5A91_3EE7_4F4B_8B06_2167EE476966__INCLUDED_)
#define AFX_STANDARDPIDLIST_H__1ECF5A91_3EE7_4F4B_8B06_2167EE476966__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PIDList.h"
#include "s75.h"

class StandardPIDList : public PIDList  
{
public:
	StandardPIDList( void *Container ) { mPIDList = (S75D_PIDList *)Container; }
	virtual ~StandardPIDList();
	BYTE* DataPointer()	{ return (BYTE *)mPIDList; }
	WORD* EntryPointer() { return mPIDList->Entry; }
	BYTE* ControlPointer() { return mPIDList->Control; }
	void DeletePIDList() { delete mPIDList; }
protected:
	S75D_PIDList *mPIDList;
};

#endif // !defined(AFX_STANDARDPIDLIST_H__1ECF5A91_3EE7_4F4B_8B06_2167EE476966__INCLUDED_)
