#include "stdafx.h"
#include "LastTalkImpl.h"
#include "../MenuWrapper.h"

CLastTalkImpl::CLastTalkImpl(IListener*pListener)
{
	m_pListener = pListener;
	SASSERT(m_pListener);
}


CLastTalkImpl::~CLastTalkImpl()
{

}
void CLastTalkImpl::OnInit(SWindow*pRoot)
{
	__super::OnInit(pRoot);
	
	// 最新聊天列表
	SListView *pLasttalkList = m_pPageRoot->FindChildByName2<SListView>(L"lv_list_lasttalk");
	SASSERT(pLasttalkList);
	pLasttalkList->EnableScrollBar(SB_HORZ, FALSE);
	pLasttalkList->SetAdapter(this);
	pLasttalkList->GetEventSet()->subscribeEvent(&CLastTalkImpl::OnEventLvSelChangeing, this);

	AddItem(LTMT_FILE_TRANS_VX, "file_helper");
	AddItem(LTMT_PERSONAL_TALK, "个人聊天");
	AddItem(LTMT_GROUP_TALK, "群聊天");
	AddItem(LTMT_DYH, "订阅号");
}

void CLastTalkImpl::getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate)
{
	if (0 == pItem->GetChildrenCount())
	{
		pItem->InitFromXml(xmlTemplate);
		pItem->GetEventSet()->subscribeEvent(EventItemPanelClick::EventID, Subscriber(&CLastTalkImpl::OnEventItemPanelClick, this));
		pItem->GetEventSet()->subscribeEvent(EventItemPanelRclick::EventID, Subscriber(&CLastTalkImpl::OnEventItemPanelRClick, this));
	}

	size_t sPos = static_cast<size_t>(position);
	if (sPos >= m_vecItemInfo.size())
		return;

	MessageListItemData* pInfo = m_vecItemInfo[sPos];
	if (NULL == pInfo)
		return;

	//设置可见数据
	// 头像
	SImageWnd* pItemFace = pItem->FindChildByName2<SImageWnd>(L"lasttalk_face");
	SASSERT(pItemFace);
	// 名字
	SStatic* pItemName = pItem->FindChildByName2<SStatic>(L"lasttalk_name");
	SASSERT(pItemName);
	SStringW sstrName, sstrFace;
	switch (pInfo->nType)
	{
		case LTMT_FILE_TRANS_VX:
		{
			sstrFace = L"skin_filehelper";
			sstrName = L"文件传输助手";
			
		}
		break;
		case LTMT_PERSONAL_TALK:
		{
			sstrFace = L"skin_personal";
		}
		break;
		case LTMT_GROUP_TALK:
		{
			sstrFace = L"skin_group";
		}
		break;
		case LTMT_DYH:
		{
			sstrName = L"订阅号";
			sstrFace = L"skin_dyh";
		}
		break;
		case LTMT_NEWS: 
		{
			sstrName = L"新闻";
			sstrFace = L"skin_news";
		}
		break;
		case LTMT_GZH:
		{
			sstrName = L"公众号";
			sstrFace = L"skin_gzh";
		}
		break;
	}

	pItemFace->SetAttribute(L"skin", sstrFace, TRUE);
	pItemName->SetWindowText(sstrName);

	//消息内容
	SStatic* pItemMessageContent = pItem->FindChildByName2<SStatic>(L"lasttalk_message_content");
	SASSERT(pItemMessageContent);
	pItemMessageContent->SetWindowText(L"SOUI_IMDemo");

	//消息时间设置
	SStatic* pItemMessageTime = pItem->FindChildByName2<SStatic>(L"lasttalk_message_time");
	SASSERT(pItemMessageTime);
	pItemMessageTime->SetWindowText(L"2018/12/17");
}
void CLastTalkImpl::AddItem(const LASTTALK_MSG_TYPE& nType, const std::string& strID)
{
	MessageListItemData* pItemData = new MessageListItemData;
	pItemData->nType = nType;
	pItemData->strID = strID;

	m_vecItemInfo.push_back(pItemData);
	notifyDataSetChanged();
	if ( m_pListener)
	{
		m_pListener->OnAddItem(pItemData);
	}
}
int CLastTalkImpl::getCount()
{
	return static_cast<int>(m_vecItemInfo.size());
}
SStringT CLastTalkImpl::getColunName(int iCol) const
{
	return L"col_nick";
}
bool CLastTalkImpl::OnEventLvSelChangeing(EventLVSelChanging* pEvt)
{
	return true;
}
bool CLastTalkImpl::OnEventItemPanelClick(EventArgs* e)
{
	SItemPanel *pItem = static_cast<SItemPanel*>(e->sender);
	if (pItem)
	{
		MessageListItemData* pInfo = m_vecItemInfo[static_cast<int>(pItem->GetItemIndex())];
		if (NULL != pInfo)
		{
			m_pListener->MessageListItemClick(pInfo);
		}
	}
	return true;
}
bool CLastTalkImpl::OnEventItemPanelRClick(EventArgs* e)
{
	SItemPanel *pItem = static_cast<SItemPanel*>(e->sender);
	if ( pItem)
	{
		MessageListItemData* pInfo = m_vecItemInfo[static_cast<int>(pItem->GetItemIndex())];
		if (NULL != pInfo)
		{
			m_pListener->MessageListItemRClick(pInfo);
		}
	}
	return true;
}