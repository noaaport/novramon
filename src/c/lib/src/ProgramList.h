// ProgramList.h: interface for the ProgramList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMLIST_H__D1F670A0_1216_4F15_BD6D_ECD7CDA9BF8D__INCLUDED_)
#define AFX_PROGRAMLIST_H__D1F670A0_1216_4F15_BD6D_ECD7CDA9BF8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s75.h"


class Program_List
{
public:
	Program_List();
	Program_List( void *Container ) { mProgramList = (S75CA_ProgramList *)Container; }
	~Program_List();
	BYTE* DataPointer()	{ return (BYTE *)mProgramList; }
//	WORD* EntryPointer() { return mProgramList->Entry; }
	void DeleteProgramList() { delete mProgramList; }
protected:
	S75CA_ProgramList *mProgramList;
};

#endif // !defined(AFX_PROGRAMLIST_H__D1F670A0_1216_4F15_BD6D_ECD7CDA9BF8D__INCLUDED_)

