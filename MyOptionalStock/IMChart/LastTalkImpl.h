#pragma once


//////////////////////////////////////////////////////////////////////////
//  ��������-����
//////////////////////////////////////////////////////////////////////////


#include "../PageImplBase.h"
enum  LASTTALK_MSG_TYPE
{
	LTMT_FILE_TRANS_VX =0, // �ļ���������
	LTMT_PERSONAL_TALK,    // ��������
	LTMT_GROUP_TALK,       // ��������
	LTMT_DYH,              // ���ĺ�
	LTMT_NEWS,             // ����
	LTMT_GZH               // ���ں�
};
struct MessageListItemData
{
	LASTTALK_MSG_TYPE nType;
	std::string       strID;
};
struct IListener
{
	virtual void MessageListItemClick( const MessageListItemData* pMsgItemData) = 0;
	virtual void MessageListItemRClick(const MessageListItemData* pMsgItemData) = 0;
	virtual void OnAddItem(MessageListItemData* pItemData) = 0;
};
class CLastTalkImpl : public CPageImplBase, public SAdapterBase
{
public:
	CLastTalkImpl(IListener*pListener);
	~CLastTalkImpl();
public:
	virtual  void OnInit(SWindow*pRoot);
	virtual  SStringW GetPageName(){ return L"lasttalk_page"; }
public:
	// Adapter
public:
	virtual void getView(int position, SWindow* pItem, pugi::xml_node xmlTemplate); 
	virtual int getCount();
	virtual SStringT getColunName(int iCol) const;
	void AddItem(const LASTTALK_MSG_TYPE& nType, const std::string& strID);
	void MoveItemToTop(const std::string& strID);
	void EnsureVisable(const std::string& strID);
	void SetCurSel(const std::string& strID);
	int GetItemIndexByID(const std::string& strID);

	bool OnEventLvSelChangeing(EventLVSelChanging* pEvt);
	bool OnEventItemPanelClick(EventArgs* e);
	bool OnEventItemPanelRClick(EventArgs* e);
	std::vector<MessageListItemData*> m_vecItemInfo;
	SListView*						  m_pOwner;

protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pPageRoot)
		CHAIN_EVENT_MAP(CPageImplBase)
	EVENT_MAP_END()
	IListener*  m_pListener;
};

