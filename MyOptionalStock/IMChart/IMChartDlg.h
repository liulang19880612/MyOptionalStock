#pragma once

//////////////////////////////////////////////////////////////////////////
// IM 聊天工具
//////////////////////////////////////////////////////////////////////////

#include "../ExternCtrls/SGroupList.h"
#include "LastTalkImpl.h"
#include "ContactImpl.h"
#include "EmojiDlg.h"
#include "ExternEvent/ExtendEvents.h"

namespace SOUI{
	class SImRichEdit;
}
class CIMChartDlg : public SOUI::SHostWnd,
	public TAutoEventMapReg<CIMChartDlg>,//通知中心自动注册
	public IListener, public IContactListener,
	public CEmojiDlg::IListener
{
public:
	CIMChartDlg();
	~CIMChartDlg();
public:
	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, SOUI::CSize size);
	void OnInitGroup(SOUI::EventArgs *e);
	void OnInitItem(SOUI::EventArgs *e);
	void OnGroupStateChanged(SOUI::EventArgs *e);
	void OnCtrlPageClick(SOUI::EventArgs *e);
	void OnBnClickMessage();
	void OnBnClickContact();
	void OnBnClickFavorites();
	void OnBnClickMenu();
	void OnLeftBtnClick(int n);
	void OnBnClickSend();
	//DUI菜单响应函数
	void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
public:
	// 表情接口实现
	virtual void EmotionTileViewItemClick(const std::string& strID);

	virtual void MessageListItemClick(const MessageListItemData* pMsgItemData);
	virtual void MessageListItemRClick(const MessageListItemData* pMsgItemData);
	virtual void OnAddItem(MessageListItemData* pItemData);
public:
	virtual void ContactItemClick(int nGID, const std::string& strID);
	virtual void ContactItemDBClick(int nGID, const std::string& strID);
	virtual void ContactItemRClick(int nGID, const std::string& strID);

	void OnBnClickChatEmotion();
	void OnBnClickChatCapture();
	void AddBackRightGeneralMessage(SImRichEdit* pRecvRichEdit, const std::vector<SStringW>& vecContents);
	void AddBackLeftGeneralMessage(SImRichEdit* pRecvRichEdit, const std::vector<SStringW>& vecContents);
	void AddBackRightFileMessage(SImRichEdit* pRecvRichEdit, const SStringW& sstrFilePath);
	void AddBackLeftFileMessage(SImRichEdit* pRecvRichEdit, const SStringW& sstrFilePath);

	// 截图相关事件
	bool OnEventSnapshotFinish(EventArgs* e);  // 截图完成
protected:
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_message", OnBnClickMessage)
		EVENT_NAME_COMMAND(L"btn_contact", OnBnClickContact)
		EVENT_NAME_COMMAND(L"btn_favorites", OnBnClickFavorites)
		EVENT_NAME_COMMAND(L"btn_menu", OnBnClickMenu)
		EVENT_NAME_COMMAND(L"btn_emotion", OnBnClickChatEmotion)
		EVENT_NAME_COMMAND(L"btn_snapshot", OnBnClickChatCapture)
		EVENT_NAME_COMMAND(L"btn_send", OnBnClickSend)

		EVENT_NAME_HANDLER(L"gl_catalog", SOUI::EventGroupListInitGroup::EventID, OnInitGroup)
		EVENT_NAME_HANDLER(L"gl_catalog", SOUI::EventGroupListInitItem::EventID, OnInitItem)
		EVENT_NAME_HANDLER(L"gl_catalog", SOUI::EventGroupStateChanged::EventID, OnGroupStateChanged)
		EVENT_NAME_HANDLER(L"gl_catalog", SOUI::EventGroupListItemCheck::EventID, OnCtrlPageClick)
		EVENT_HANDLER(EVT_SNAPSHOTFINISH, OnEventSnapshotFinish)
		CHAIN_EVENT_MAP_MEMBER(m_LastTalkImpl)
		CHAIN_EVENT_MAP_MEMBER(m_ContactImpl)
	EVENT_MAP_END()
	BEGIN_MSG_MAP_EX(CIMChartDlg)
		MSG_WM_SIZE(OnSize)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
protected:
	BOOL                  m_bLayoutInited;       // 初始化完成标记
	CLastTalkImpl         m_LastTalkImpl;
	CContactImpl          m_ContactImpl;
	CEmojiDlg*            m_pEmojiDlg;
	BOOL                  m_bEmotionShow;
	bool			      m_bSnapshotHideWindow;
};

