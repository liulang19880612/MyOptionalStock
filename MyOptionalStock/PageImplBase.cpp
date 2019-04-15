#include "stdafx.h"
#include "PageImplBase.h"


CPageImplBase::CPageImplBase()
{
	m_pPageRoot = NULL;
}


CPageImplBase::~CPageImplBase()
{
}
void CPageImplBase::OnInit(SWindow*pRoot)
{
	SASSERT(pRoot);
	if (pRoot)
	{
		m_pPageRoot = pRoot->FindChildByName(GetPageName());
		SASSERT(m_pPageRoot);
	}
}