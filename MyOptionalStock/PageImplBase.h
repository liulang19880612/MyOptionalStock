#pragma once
class CPageImplBase
{
public:
	CPageImplBase();
	virtual ~CPageImplBase();
public:
	virtual  void OnInit(SWindow*pRoot);
	virtual  SStringW GetPageName(){ return L""; }
protected:
	EVENT_MAP_BEGIN()
	EVENT_MAP_BREAK()
protected:
	SWindow*   m_pPageRoot;
};

