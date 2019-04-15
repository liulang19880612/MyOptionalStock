#include "stdafx.h"
#include "IMChartDlg.h"
#include "LastTalkImpl.h"
#include "EmojiDlg.h"
#include "ExternCtrls/imre/SImRichEdit.h"
#include "../utils.h"
#include "ExternCtrls/imre/ImgProvider.h"
#include "ExternSkins/SAntialiasSkin.h"
#include "Snapshot/SnapshotDlg.h"
#include "Snapshot/CWindowEnumer.h"

#define  CHAT_TAB_NAME L"chattab"

CIMChartDlg::CIMChartDlg(): SHostWnd(_T("LAYOUT:XML_IMCHART")),
m_LastTalkImpl(this),
m_ContactImpl(this)
{
	m_pEmojiDlg = NULL;
	m_bEmotionShow = FALSE;
	m_bSnapshotHideWindow = FALSE;
	SNotifyCenter::getSingleton().addEvent(EVENTID(EventSnapshotFinish));
}


CIMChartDlg::~CIMChartDlg()
{
}
BOOL CIMChartDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	if (!ImageProvider::IsExist(L"default_portrait"))
	{
		SAntialiasSkin* pSkin = new SAntialiasSkin();
		pSkin->SetRound(TRUE);

		if (pSkin->LoadFromFile(L"uires\\default_res\\default_portrait.png"))
			ImageProvider::Insert(L"default_portrait", pSkin);
		else
			delete pSkin;
	}

	m_LastTalkImpl.OnInit(this);
	m_ContactImpl.OnInit(this);
	SImageButton* pBtnMessage = FindChildByName2<SImageButton>(L"btn_message");
	SImageButton* pBtnContact = FindChildByName2<SImageButton>(L"btn_contact");
	SImageButton* pBtnFavorites = FindChildByName2<SImageButton>(L"btn_favorites");
	SASSERT(pBtnMessage);
	SASSERT(pBtnContact);
	SASSERT(pBtnFavorites);

	pBtnMessage->SetCheck(TRUE);
	pBtnContact->SetCheck(FALSE);
	pBtnFavorites->SetCheck(FALSE);

	// 表情对话框
	if (!m_pEmojiDlg)
	{
		m_pEmojiDlg = new CEmojiDlg(this);
		m_pEmojiDlg->Create(m_hWnd, 0, 0, 0, 0);
		m_pEmojiDlg->SendMessage(WM_INITDIALOG);
	}
	m_bLayoutInited = TRUE;
	return 0;
}
void CIMChartDlg::OnClose()
{
	__super::DestroyWindow();
}


void CIMChartDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CIMChartDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CIMChartDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CIMChartDlg::OnSize(UINT nType, SOUI::CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;

	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if (!pBtnMax || !pBtnRestore) return;

	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}
void CIMChartDlg::OnInitGroup(SOUI::EventArgs *e)
{
	SOUI::EventGroupListInitGroup *e2 = SOUI::sobj_cast<SOUI::EventGroupListInitGroup>(e);
	SOUI::SToggle *pTgl = e2->pItem->FindChildByName2<SOUI::SToggle>(L"tgl_switch");
	pTgl->SetToggle(!e2->pGroupInfo->bCollapsed);
	e2->pItem->FindChildByName(L"txt_label")->SetWindowText(e2->pGroupInfo->strText);
}

void CIMChartDlg::OnInitItem(SOUI::EventArgs *e)
{
	SOUI::EventGroupListInitItem *e2 = SOUI::sobj_cast<SOUI::EventGroupListInitItem>(e);
	e2->pItem->FindChildByName(L"txt_label")->SetWindowText(e2->pItemInfo->strText);
	e2->pItem->FindChildByName2<SOUI::SImageWnd>(L"img_indicator")->SetIcon(e2->pItemInfo->iIcon);
}

void CIMChartDlg::OnGroupStateChanged(SOUI::EventArgs *e)
{
	SOUI::EventGroupStateChanged *e2 = SOUI::sobj_cast<SOUI::EventGroupStateChanged>(e);
	SOUI::SToggle *pTgl = e2->pItem->FindChildByName2<SOUI::SToggle>(L"tgl_switch");
	pTgl->SetToggle(!e2->pGroupInfo->bCollapsed);
}
void CIMChartDlg::OnBnClickMessage()
{
	OnLeftBtnClick(0);
}
void CIMChartDlg::OnBnClickContact()
{
	OnLeftBtnClick(1);
}
void CIMChartDlg::OnBnClickFavorites()
{
	OnLeftBtnClick(2);
}
void CIMChartDlg::OnBnClickMenu()
{

}
void CIMChartDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{
	if (uNotifyCode == 0)
	{
		if (nID == 1)
		{
			SOUI::SMessageBox(m_hWnd, _T("导出配置"), _T("导出配置"), MB_OKCANCEL);
		}
		if (nID == 51)
		{
			SOUI::SMessageBox(m_hWnd, _T("访问官网"), _T("访问官网"), MB_OKCANCEL);
		}
	}
}
void CIMChartDlg::OnLeftBtnClick(int nBtnIndex)
{
	SOUI::SButton *pBtnMsg = FindChildByName2<SOUI::SButton>(L"btn_message");
	SASSERT(pBtnMsg);
	SOUI::SButton *pBtnContact = FindChildByName2<SOUI::SButton>(L"btn_contact");
	SASSERT(pBtnContact);
	SOUI::SButton *pBtnFavor = FindChildByName2<SOUI::SButton>(L"btn_favorites");
	SASSERT(pBtnFavor);
	SOUI::SButton *pBtnMenu = FindChildByName2<SOUI::SButton>(L"btn_menu");
	SASSERT(pBtnMenu);
	if (nBtnIndex < 3)
	{
		pBtnMsg->SetCheck(nBtnIndex == 0);
		pBtnContact->SetCheck(nBtnIndex == 1);
		pBtnFavor->SetCheck(nBtnIndex == 2);
	}
	SOUI::STabCtrl *pLeftListTab = FindChildByName2<SOUI::STabCtrl>(L"leftlist_tabctrl");
	SASSERT(pLeftListTab);
	if (pLeftListTab)
	{
		LPCTSTR leftListPageName = NULL;
		switch (nBtnIndex)
		{
			case 0: leftListPageName = L"lasttalk_page"; break;
			case 1: leftListPageName = L"contact_page";  break;
			case 2: leftListPageName = L"favorites_page"; break;
		}
		if (leftListPageName)
			pLeftListTab->SetCurSel(leftListPageName, TRUE);
	}
}
void CIMChartDlg::OnCtrlPageClick(SOUI::EventArgs *e)
{

}
void CIMChartDlg::OnAddItem(MessageListItemData* pItemData)
{
	STabCtrl* pChatTab = FindChildByName2<STabCtrl>(CHAT_TAB_NAME);
	SASSERT(pChatTab);
	if ( pChatTab)
	{
		SStringW sstrPage;
		switch (pItemData->nType)
		{
			case LTMT_FILE_TRANS_VX:
			{
				sstrPage.Format(L"<page title='%s'><include src='layout:XML_PAGE_COMMON_FILEHELPER'/></page>", L"file_helper");
			}
			break;
		}
		if (!sstrPage.IsEmpty())
			pChatTab->InsertItem(sstrPage);
	}
}
void CIMChartDlg::MessageListItemClick(const MessageListItemData* pMsgItemData)
{
	SASSERT(pMsgItemData);
	if (!pMsgItemData)return;

	STabCtrl* pChatTab = FindChildByName2<STabCtrl>(CHAT_TAB_NAME);
	SASSERT(pChatTab);
	if (!pChatTab)return;
	SStringW sstrID = S_CA2W(pMsgItemData->strID.c_str());
	pChatTab->SetCurSel(sstrID, TRUE);
}
void CIMChartDlg::MessageListItemRClick(const MessageListItemData* pMsgItemData)
{
	STabCtrl* pChatTab = FindChildByName2<STabCtrl>(CHAT_TAB_NAME);
	SASSERT(pChatTab);
}
void CIMChartDlg::OnBnClickChatEmotion()
{
	SetMsgHandled(FALSE);
	SImageButton* pBtn = FindChildByName2<SImageButton>(L"btn_emotion");
	SASSERT(pBtn);
	CRect rcEmotionBtn = pBtn->GetWindowRect();
	ClientToScreen(&rcEmotionBtn);
	::SetWindowPos(m_pEmojiDlg->m_hWnd, NULL, rcEmotionBtn.left - 10, rcEmotionBtn.top - 250, 0, 0, SWP_NOSIZE);

	if (m_bEmotionShow)
	{
		m_pEmojiDlg->ShowWindow(SW_HIDE);
		m_bEmotionShow = FALSE;
	}
	else
	{
		m_pEmojiDlg->ShowWindow(SW_SHOW);
		m_bEmotionShow = TRUE;
	}
}
void CIMChartDlg::ContactItemClick(int nGID, const std::string& strID)
{

}
void CIMChartDlg::ContactItemDBClick(int nGID, const std::string& strID)
{

}
void CIMChartDlg::ContactItemRClick(int nGID, const std::string& strID)
{

}
void CIMChartDlg::EmotionTileViewItemClick(const std::string& strID)
{
	if (m_bEmotionShow)
	{
		m_pEmojiDlg->ShowWindow(SW_HIDE);
		m_bEmotionShow = false;
	}
	else
	{
		m_pEmojiDlg->ShowWindow(SW_SHOW);
		m_bEmotionShow = true;
	}
	auto iter = CGlobalUnits::GetInstance()->m_mapEmojisIndex.find(strID);
	if (iter != CGlobalUnits::GetInstance()->m_mapEmojisIndex.end())
	{
		std::string strEmotionName = iter->second;
		std::string strPath = S_CW2A(GetExePath()) + "uires\\emojis\\" + strEmotionName.c_str();

		SStringW sstrPath = S_CA2W(strPath.c_str());
		SStringW sstrEmotion;
		sstrEmotion.Format(L"<RichEditContent>"
			L"<para break=\"0\" disable-layout=\"1\">"
			L"<img type=\"smiley_img\" path=\"%s\" id=\"zzz\" max-size=\"\" minsize=\"\" scaring=\"1\" cursor=\"hand\" />"
			L"</para>"
			L"</RichEditContent>", sstrPath);

		STabCtrl* pChatTab = FindChildByName2<STabCtrl>(L"chattab");
		SASSERT(pChatTab);
		SWindow* pPage = pChatTab->GetPage(pChatTab->GetCurSel());
		SASSERT(pPage);
		SImRichEdit* pSendRichedit = pPage->FindChildByName2<SImRichEdit>(L"send_richedit");
		SASSERT(pSendRichedit);
		pSendRichedit->InsertContent(sstrEmotion, RECONTENT_CARET);
	}
}
void CIMChartDlg::OnBnClickSend()
{
	std::ostringstream os;
	STabCtrl* pChatTab = FindChildByName2<STabCtrl>(L"chattab");
	SASSERT(pChatTab);
	SWindow* pPage = pChatTab->GetPage(pChatTab->GetCurSel());
	SASSERT(pPage);
	SImRichEdit* pRecvRichedit = pPage->FindChildByName2<SImRichEdit>(L"recv_richedit");
	SImRichEdit* pSendRichedit = pPage->FindChildByName2<SImRichEdit>(L"send_richedit");
	SASSERT(pRecvRichedit);
	SASSERT(pSendRichedit);

	CHARRANGE chr = { 0, -1 };
	SStringT strContent = pSendRichedit->GetSelectedContent(&chr);
	pugi::xml_document doc;
	if (!doc.load_buffer(strContent, strContent.GetLength() * sizeof(WCHAR)))
		return;
	strContent.Empty();

	std::vector<SStringW> vecContent;
	pugi::xml_node node = doc.child(L"RichEditContent").first_child();
	for (; node; node = node.next_sibling())
	{
		const wchar_t* pNodeName = node.name();
		if (wcscmp(RichEditText::GetClassName(), pNodeName) == 0)			//文本
		{
			SStringW sstrContent = node.text().get();
			SStringW sstrText;
			sstrText.Format(L"<text font-size=\"10\" font-face=\"微软雅黑\" color=\"#000000\"><![CDATA[%s]]></text>", sstrContent);
			vecContent.push_back(sstrText);
		}
		else if (wcscmp(RichEditImageOle::GetClassName(), pNodeName) == 0)	//图片
		{
			SStringW sstrImgPath = node.attribute(L"path").as_string();
			SStringW sstrImg;
			sstrImg.Format(L"<img subid=\"%s\" id=\"%s\" type=\"normal_img\" encoding=\"\" show-magnifier=\"1\" path=\"%s\"/>", L"", L"", sstrImgPath);
			vecContent.push_back(sstrImg);
		}
		else if (wcscmp(RichEditMetaFileOle::GetClassName(), pNodeName) == 0)	//文件
		{
			SStringW sstrFilePath = node.attribute(L"file").as_string();
			AddBackRightFileMessage(pRecvRichedit, sstrFilePath);

			time_t local;
			time(&local);
			os.str("");
			os << local;
 			std::string strTimestamp = os.str();
// 			auto iterTime = CGlobalUnits::GetInstance()->m_mapLasttalkTime.find(m_LasttalkCurSel.m_strID);
//  			if (iterTime != CGlobalUnits::GetInstance()->m_mapLasttalkTime.end())
//  			{
//  				CGlobalUnits::GetInstance()->m_mapLasttalkTime.erase(m_LasttalkCurSel.m_strID);
//  				CGlobalUnits::GetInstance()->m_mapLasttalkTime.insert(std::make_pair(m_LasttalkCurSel.m_strID, strTimestamp));
//  			}
//  			else
//  				CGlobalUnits::GetInstance()->m_mapLasttalkTime.insert(std::make_pair(m_LasttalkCurSel.m_strID, strTimestamp));
		}
	}

	if (!vecContent.empty())
	{
		AddBackRightGeneralMessage(pRecvRichedit, vecContent);

		time_t local;
		time(&local);
		os.str("");
		os << local;
// 		std::string strTimestamp = os.str();
// 		auto iterTime = CGlobalUnits::GetInstance()->m_mapLasttalkTime.find(m_LasttalkCurSel.m_strID);
// 		if (iterTime != CGlobalUnits::GetInstance()->m_mapLasttalkTime.end())
// 		{
// 			CGlobalUnits::GetInstance()->m_mapLasttalkTime.erase(m_LasttalkCurSel.m_strID);
// 			CGlobalUnits::GetInstance()->m_mapLasttalkTime.insert(std::make_pair(m_LasttalkCurSel.m_strID, strTimestamp));
// 		}
// 		else
// 			CGlobalUnits::GetInstance()->m_mapLasttalkTime.insert(std::make_pair(m_LasttalkCurSel.m_strID, strTimestamp));
	}

	pSendRichedit->Clear();
}

void CIMChartDlg::AddBackRightGeneralMessage(SImRichEdit* pRecvRichEdit, const std::vector<SStringW>& vecContents)
{
	//统一使用右侧布局
	LPCWSTR pEmpty;
	pEmpty = L"<para id=\"msgbody\" margin=\"0,0,0,0\" break=\"1\" simulate-align=\"1\">"
		L""
		L"</para>";

	//插入消息时间
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	SStringW sstrTempTime;
	if (t->tm_hour > 12)
		sstrTempTime.Format(L"%02d月%02d日  下午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	else
		sstrTempTime.Format(L"%02d月%02d日  上午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	SStringW sstrTime;
	sstrTime.Format(L"<text font-size=\"8\" color=\"#808080\">%s</text>", sstrTempTime);

	SStringW sstrTimeContent;
	sstrTimeContent.Format(
		L"<RichEditContent type=\"ContentCenter\" >"
		L"<para margin=\"100,15,100,0\" align=\"center\" break=\"1\" >"
		L"%s"
		L"</para>"
		L"</RichEditContent>", sstrTime);
	pRecvRichEdit->InsertContent(sstrTimeContent, RECONTENT_LAST);

	SStringW sstrResend;
	sstrResend = L"<bkele id=\"resend\" name=\"BkEleSendFail\" data=\"resend\" right-skin=\"skin.richedit_resend\" right-pos=\"{-15,[-16,@12,@12\" cursor=\"hand\" interactive=\"1\"/>";

	SStringW sstrMsg;
	for (size_t i = 0; i < vecContents.size(); i++)
	{
		sstrMsg += vecContents[i];
	}

	SStringW sstrContent;
	sstrContent.Format(
		L"<RichEditContent  type=\"ContentRight\" align=\"right\" auto-layout=\"1\">"
		L"<para break=\"1\" align=\"left\" />"
		L"<bkele data=\"avatar\" id=\"%s\" skin=\"%s\" left-pos=\"0,]-6,@40,@40\" right-pos=\"-50,]-9,@40,@40\" cursor=\"hand\" interactive=\"1\"/>"
		L"<para id=\"msgbody\" margin=\"65,0,35,0\" break=\"1\" simulate-align=\"1\">"
		L"%s"
		L"</para>"
		L"<bkele data=\"bubble\" left-skin=\"skin_left_bubble\" right-skin=\"skin_right_otherbubble\" left-pos=\"50,{-9,[10,[10\" right-pos=\"{-10,{-9,-55,[10\" />"
		L"%s"
		L"</RichEditContent>",
		L"default_portrait", L"default_portrait", sstrMsg, pEmpty);

	pRecvRichEdit->InsertContent(sstrContent, RECONTENT_LAST);
	pRecvRichEdit->ScrollToBottom();

	RichEditBkElement* pResendEleObj = sobj_cast<RichEditBkElement>(pRecvRichEdit->GetElementById(L"resend"));
	if (pResendEleObj)
	{
		pResendEleObj->SetVisible(FALSE);
		pRecvRichEdit->Invalidate();
	}

	AddBackLeftGeneralMessage(pRecvRichEdit, vecContents);
}
void CIMChartDlg::AddBackLeftGeneralMessage(SImRichEdit* pRecvRichEdit, const std::vector<SStringW>& vecContents)
{
	LPCWSTR pEmpty;
	pEmpty = L"<para id=\"msgbody\" margin=\"0,0,0,0\" break=\"1\" simulate-align=\"1\">"
		L""
		L"</para>";

	//插入消息时间
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	SStringW sstrTempTime;
	if (t->tm_hour > 12)
		sstrTempTime.Format(L"%02d月%02d日  下午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	else
		sstrTempTime.Format(L"%02d月%02d日  上午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	SStringW sstrTime;
	sstrTime.Format(L"<text font-size=\"8\" color=\"#808080\">%s</text>", sstrTempTime);

	SStringW sstrTimeContent;
	sstrTimeContent.Format(
		L"<RichEditContent type=\"ContentCenter\" >"
		L"<para margin=\"100,15,100,0\" align=\"center\" break=\"1\" >"
		L"%s"
		L"</para>"
		L"</RichEditContent>", sstrTime);
	pRecvRichEdit->InsertContent(sstrTimeContent, RECONTENT_LAST);

	SStringW sstrMsg;
	for (size_t i = 0; i < vecContents.size(); i++)
	{
		sstrMsg += vecContents[i];
	}

	SStringW sstrContent;
	sstrContent.Format(
		L"<RichEditContent  type=\"ContentLeft\" align=\"left\">"
		L"<para break=\"1\" align=\"left\" />"
		L"<bkele data=\"avatar\" id=\"%s\" skin=\"%s\" pos=\"0,]-10,@40,@40\" cursor=\"hand\" interactive=\"1\"/>"
		L"<para id=\"msgbody\" margin=\"65,0,45,0\" break=\"1\" simulate-align=\"1\">"
		L"%s"
		L"</para>"
		L"<bkele data=\"bubble\" left-skin=\"skin_left_bubble\" left-pos=\"50,{-9,[10,[10\" />"
		L"%s"
		L"</RichEditContent>",
		L"default_portrait", L"default_portrait", sstrMsg, pEmpty);

	pRecvRichEdit->InsertContent(sstrContent, RECONTENT_LAST);
	pRecvRichEdit->ScrollToBottom();
}
void CIMChartDlg::AddBackRightFileMessage(SImRichEdit* pRecvRichEdit, const SStringW& sstrFilePath)
{
	//统一使用右侧布局
	LPCWSTR pEmpty;
	pEmpty = L"<para id=\"msgbody\" margin=\"0,0,0,0\" break=\"1\" simulate-align=\"1\">"
		L""
		L"</para>";

	//添加时间
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	SStringW sstrTempTime;
	if (t->tm_hour > 12)
		sstrTempTime.Format(L"%02d月%02d日  下午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	else
		sstrTempTime.Format(L"%02d月%02d日  上午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	SStringW sstrTime;
	sstrTime.Format(L"<text font-size=\"8\" color=\"#808080\">%s</text>", sstrTempTime);

	SStringW sstrTimeContent;
	sstrTimeContent.Format(
		L"<RichEditContent type=\"ContentCenter\" >"
		L"<para margin=\"100,15,100,0\" align=\"center\" break=\"1\" >"
		L"%s"
		L"</para>"
		L"</RichEditContent>", sstrTime);
	pRecvRichEdit->InsertContent(sstrTimeContent, RECONTENT_LAST);

	SStringW sstrResend;
	sstrResend = L"<bkele id=\"resend\" name=\"BkEleSendFail\" data=\"resend\" right-skin=\"skin.richedit_resend\" right-pos=\"{-15,[-16,@12,@12\" cursor=\"hand\" interactive=\"1\"/>";

	std::string strFilePath = S_CW2A(sstrFilePath);
	//获取文件大小
	FILE* fp = fopen(strFilePath.c_str(), "rb");
	SASSERT(fp);
	int nFileSize = 0;
	fseek(fp, 0L, SEEK_END);
	nFileSize = ftell(fp);
	rewind(fp);
	fclose(fp);

	//获取文件后缀名
	std::string strFileSuffix;
	std::string::size_type pos = strFilePath.find_last_of(".");
	if (pos != std::string::npos)
	{
		strFileSuffix = strFilePath.substr(pos + 1);
	}

	SStringW sstrFileType = S_CA2W(strFileSuffix.c_str());

	SStringW sstrFile;
	int nVisableLinks = 0x0010 | 0x0020;
	sstrFile.Format(L"<file id=\"file_file\" file-size=\"%d\" file-state=\"上传成功\" file-suffix=\"%s\" file-path=\"%s\" links=\"%d\"/>",
		nFileSize,
		sstrFileType,
		sstrFilePath,
		nVisableLinks);

	SStringW sstrContent;
	sstrContent.Format(
		L"<RichEditContent  type=\"ContentRight\" align=\"right\" auto-layout=\"1\">"
		L"<para break=\"1\" align=\"left\" />"
		L"<bkele data=\"avatar\" id=\"%s\" skin=\"%s\" left-pos=\"0,]-6,@40,@40\" right-pos=\"-50,]-10,@40,@40\" cursor=\"hand\" interactive=\"1\"/>"
		L"<para data=\"file_file\" id=\"msgbody\" margin=\"65,0,45,0\" break=\"1\" simulate-align=\"1\">"
		L"%s"
		L"</para>"
		L"<bkele data=\"bubble\" left-skin=\"skin_left_bubble\" right-skin=\"skin_right_otherbubble\" left-pos=\"50,{-9,[10,[10\" right-pos=\"{-10,{-9,-55,[10\" />"
		L"%s"
		L"</RichEditContent>",
		L"default_portrait", L"default_portrait", sstrFile, pEmpty);

	pRecvRichEdit->InsertContent(sstrContent, RECONTENT_LAST);
	pRecvRichEdit->ScrollToBottom();

	RichEditBkElement* pResendEleObj = sobj_cast<RichEditBkElement>(pRecvRichEdit->GetElementById(L"resend"));
	if (pResendEleObj)
	{
		pResendEleObj->SetVisible(FALSE);
		pRecvRichEdit->Invalidate();
	}

	AddBackLeftFileMessage(pRecvRichEdit, sstrFilePath);
}
void CIMChartDlg::AddBackLeftFileMessage(SImRichEdit* pRecvRichEdit, const SStringW& sstrFilePath)
{
	LPCWSTR pEmpty;
	pEmpty = L"<para id=\"msgbody\" margin=\"0,0,0,0\" break=\"1\" simulate-align=\"1\">"
		L""
		L"</para>";

	//添加时间
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	SStringW sstrTempTime;
	if (t->tm_hour > 12)
		sstrTempTime.Format(L"%02d月%02d日  下午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	else
		sstrTempTime.Format(L"%02d月%02d日  上午%02d:%02d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	SStringW sstrTime;
	sstrTime.Format(L"<text font-size=\"8\" color=\"#808080\">%s</text>", sstrTempTime);

	SStringW sstrTimeContent;
	sstrTimeContent.Format(
		L"<RichEditContent type=\"ContentCenter\" >"
		L"<para margin=\"100,15,100,0\" align=\"center\" break=\"1\" >"
		L"%s"
		L"</para>"
		L"</RichEditContent>", sstrTime);
	pRecvRichEdit->InsertContent(sstrTimeContent, RECONTENT_LAST);

	std::string strFilePath = S_CW2A(sstrFilePath);
	//获取文件大小
	FILE* fp = fopen(strFilePath.c_str(), "rb");
	SASSERT(fp);
	int nFileSize = 0;
	fseek(fp, 0L, SEEK_END);
	nFileSize = ftell(fp);
	rewind(fp);
	fclose(fp);

	//获取文件后缀名
	std::string strFileSuffix;
	std::string::size_type pos = strFilePath.find_last_of(".");
	if (pos != std::string::npos)
	{
		strFileSuffix = strFilePath.substr(pos + 1);
	}

	SStringW sstrFileType = S_CA2W(strFileSuffix.c_str());

	SStringW sstrFile;
	int nVisableLinks = 0x0010 | 0x0020;
	sstrFile.Format(L"<file id=\"file_file\" file-size=\"%d\" file-state=\"上传成功\" file-suffix=\"%s\" file-path=\"%s\" links=\"%d\"/>",
		nFileSize,
		sstrFileType,
		sstrFilePath,
		nVisableLinks);

	SStringW sstrContent;
	sstrContent.Format(
		L"<RichEditContent  type=\"ContentLeft\" align=\"left\">"
		L"<para break=\"1\" align=\"left\" />"
		L"<bkele data=\"avatar\" id=\"%s\" skin=\"%s\" pos=\"0,]-10,@40,@40\" cursor=\"hand\" interactive=\"1\"/>"
		L"<para id=\"msgbody\" margin=\"65,0,45,0\" break=\"1\" simulate-align=\"1\">"
		L"%s"
		L"</para>"
		L"<bkele data=\"bubble\" left-skin=\"skin_left_bubble\" left-pos=\"50,{-9,[10,[10\" />"
		L"%s"
		L"</RichEditContent>",
		L"default_portrait", L"default_portrait", sstrFile, pEmpty);

	pRecvRichEdit->InsertContent(sstrContent, RECONTENT_LAST);
	pRecvRichEdit->ScrollToBottom();
}
void CIMChartDlg::OnBnClickChatCapture()
{
	if (m_bSnapshotHideWindow)
	{
		this->ShowWindow(SW_HIDE);
	}

	CSnapshotDlg dlg;
	CWindowEnumer::EnumAllTopWindow();
	dlg.DoModal(NULL);
}
bool CIMChartDlg::OnEventSnapshotFinish(EventArgs* e)
{
	if (m_bSnapshotHideWindow)
		this->ShowWindow(SW_SHOWNORMAL);

	STabCtrl* pChatTab = FindChildByName2<STabCtrl>(L"chattab");
	SASSERT(pChatTab);
	SWindow* pPage = pChatTab->GetPage(pChatTab->GetCurSel());
	SImRichEdit* pSendRichedit = pPage->FindChildByName2<SImRichEdit>(L"send_richedit");
	SASSERT(pSendRichedit);

	pSendRichedit->Paste();
	pSendRichedit->SetFocus();
	return true;
}