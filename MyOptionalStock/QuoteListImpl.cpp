#include "stdafx.h"
#include "QuoteListImpl.h"
#include "ExternCtrls/GridCtrl.h"


struct SORTCTX
{
	int iCol;
	SHDSORTFLAG stFlag;
};

CQuoteListImpl::CQuoteListImpl()
{
}


CQuoteListImpl::~CQuoteListImpl()
{
}

void CQuoteListImpl::OnInit(SWindow*pRoot)
{
	__super::OnInit(pRoot);
	//多列listview
	CGridCtrl * pMcListView = m_pPageRoot->FindChildByName2<CGridCtrl>("quote_grid_view");
	if (pMcListView)
	{
		pMcListView->SetAdapter(this);
	}
}


//////////////////////////////////////////////////////////////////////////

SStringW CQuoteListImpl::GetColumnName(int iCol) const 
{
	return SStringW().Format(L"col%d", iCol + 1);
}
int CQuoteListImpl::getCount()
{
	return  10;
}
void CQuoteListImpl::getView(int position, SOUI::SWindow * pItem, pugi::xml_node xmlTemplate)
{
	if (pItem->GetChildrenCount() == 0)
	{
		pItem->InitFromXml(xmlTemplate);
	}
	pItem->FindChildByName(L"txt_code_id")->SetWindowText(L"600600");
}
bool CQuoteListImpl::OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols)
{
	if (iCol == 5) //最后一列“操作”不支持排序
		return false;

	SHDSORTFLAG stFlag = stFlags[iCol];
	switch (stFlag)
	{
		case ST_NULL:stFlag = ST_UP; break;
		case ST_DOWN:stFlag = ST_UP; break;
		case ST_UP:stFlag = ST_DOWN; break;
	}
	for (int i = 0; i < nCols; i++)
	{
		stFlags[i] = ST_NULL;
	}
	stFlags[iCol] = stFlag;

	SORTCTX ctx = { iCol, stFlag };
	//qsort_s(m_softInfo.GetData(), m_softInfo.GetCount(), sizeof(SOFTINFO), SortCmp, &ctx);
	return true;
}