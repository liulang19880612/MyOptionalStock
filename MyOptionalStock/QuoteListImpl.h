#pragma once
#include "PageImplBase.h"

//////////////////////////////////////////////////////////////////////////
//  ���鱨���б� - ����
//////////////////////////////////////////////////////////////////////////

class CQuoteListImpl : public CPageImplBase , public SOUI::SMcAdapterBase
{
public:
	CQuoteListImpl();
	~CQuoteListImpl();
public:
	virtual  SStringW GetPageName(){ return L"quotelist"; }
	virtual  void OnInit(SWindow*pRoot);
public:
	// Adapter �ӿ�
	virtual int  getCount();
	virtual void getView(int position, SOUI::SWindow * pItem, pugi::xml_node xmlTemplate);
	virtual bool OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols);
	SStringW GetColumnName(int iCol) const;
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pPageRoot)
		CHAIN_EVENT_MAP(CPageImplBase)
	EVENT_MAP_END()
};

