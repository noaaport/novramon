// PesMakerPIDList.h: interface for the PesMakerPIDList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PESMAKERPIDLIST_H__8D084B68_FDCD_4EDF_83A0_46C433D6B919__INCLUDED_)
#define AFX_PESMAKERPIDLIST_H__8D084B68_FDCD_4EDF_83A0_46C433D6B919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PIDList.h"
#include "s75.h"

class PesMakerPIDList : public PIDList  
{
public:
	PesMakerPIDList( void *Container ) { mPIDList = (PesMaker_PIDList *)Container; }
	virtual ~PesMakerPIDList();
	BYTE* DataPointer()	{ return (BYTE *)mPIDList; }
	WORD* EntryPointer() { return mPIDList->Entry; }
	BYTE* ControlPointer() { return mPIDList->Control; }
	void DeletePIDList() { delete mPIDList; }
protected:
	PesMaker_PIDList *mPIDList;
};

#endif // !defined(AFX_PESMAKERPIDLIST_H__8D084B68_FDCD_4EDF_83A0_46C433D6B919__INCLUDED_)
