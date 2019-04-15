#include "stdafx.h"
#include "ContactImpl.h"

CContactImpl::CContactImpl(IContactListener*pListener)
{
	m_pContactListener = pListener;
	SASSERT(m_pContactListener);
}
CContactImpl::~CContactImpl()
{

}

void CContactImpl::OnInit(SWindow*pRoot)
{
	__super::OnInit(pRoot);
	ContactItemData data;
	data.strName = L"ÐÂÅóÓÑ";
	data.gid = 1;
	data.bGroup = true;

	HSTREEITEM hRoot = InsertItem(data);
	SetItemExpanded(hRoot, TRUE);

	data.bGroup = FALSE;
	data.strID = L"new_friends";
	data.strName = L"ÐÂµÄÅóÓÑ";
	data.strImg = L"skin_personal";
	InsertItem(data, hRoot);


	ContactItemData data1;
	data1.strName = L"¹«ÖÚºÅ";
	data1.gid = 2;
	data1.bGroup = true;
	HSTREEITEM hRoot1 = InsertItem(data1);
	SetItemExpanded(hRoot1, TRUE);

	data1.bGroup = false;
	data1.strID = L"page_gzh";
	data1.strName = L"¹«ÖÚºÅ²âÊÔ";
	data1.strImg = L"skin_gzh";
	InsertItem(data1, hRoot1);

	ContactItemData data2;
	data2.strName = L"¶©ÔÄºÅ";
	data2.gid = 3;
	data2.bGroup = TRUE;
	HSTREEITEM hRoot2 = InsertItem(data2);
	SetItemExpanded(hRoot2, TRUE);

	data2.bGroup = false;
	data2.strID = L"page_dyh";
	data2.strName = L"¶©ÔÄºÅ²âÊÔ";
	data2.strImg = L"skin_dyh";
	InsertItem(data2, hRoot2);

	ContactItemData data3;
	data3.strName = L"ÈºÁÄ";
	data3.gid = 4;
	data3.bGroup = TRUE;
	HSTREEITEM hRoot3 = InsertItem(data3);
	SetItemExpanded(hRoot3, TRUE);

	ContactItemData data4;
	data4.strName = L"ºÃÓÑ";
	data4.gid = 5;
	data4.bGroup = TRUE;
	HSTREEITEM hRoot4 = InsertItem(data4);
	SetItemExpanded(hRoot4, TRUE);

	STreeView * pTreeView = m_pPageRoot->FindChildByName2<STreeView>("tv_Friend");
	SASSERT(pTreeView);
	if (pTreeView)
	{
		pTreeView->SetAdapter(this);
	}
}

void CContactImpl::getView(SOUI::HTREEITEM loc, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)loc);
		int itemType = getViewType(loc);
		if (pItem->GetChildrenCount() == 0)
		{
			switch (itemType)
			{
				case 0:xmlTemplate = xmlTemplate.child(L"item_group");
					break;
				case 1:xmlTemplate = xmlTemplate.child(L"item_data");
					break;
			}
			pItem->InitFromXml(xmlTemplate);
			if (itemType == 0)
			{//²»ÈÃËüµã	
				pItem->GetEventSet()->setMutedState(true);
			}
		}
		if (itemType == 1)
		{
		}
		else
		{
			pItem->FindChildByName(L"hr")->SetVisible(ii.data.gid != 1);
		}
		pItem->FindChildByName(L"name")->SetWindowText(ii.data.strName);
	}