#include "stdafx.h"
#include "EmojiDlg.h"


CEmojiDlg::CEmojiDlg(IListener* pListner)
	: SHostWnd(_T("LAYOUT:XML_DLG_EMOJI"))
{
	m_pListner = pListner;
}

CEmojiDlg::~CEmojiDlg()
{
}

BOOL CEmojiDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	SetMsgHandled(FALSE);
	STileView* pTileView = FindChildByName2<STileView>(L"emoji_titleview");
	SASSERT(pTileView);
	CAdapter_EmotionTileView* pAdapter = new CAdapter_EmotionTileView(this);
	pTileView->SetAdapter(pAdapter);
	std::map<std::string, std::string>::iterator iter = CGlobalUnits::GetInstance()->m_mapEmojisIndex.begin();
	for (; iter != CGlobalUnits::GetInstance()->m_mapEmojisIndex.end(); iter++)
	{
		pAdapter->AddItem(iter->first.c_str());
	}
	pAdapter->Release();
	pTileView->SetSel(-1);

	return FALSE;
}

void CEmojiDlg::OnSize(UINT nType, SOUI::CSize size)
{
	SetMsgHandled(FALSE);
}

void CEmojiDlg::OnLButtonDown(UINT nFlags, SOUI::CPoint pt)
{
	SetMsgHandled(FALSE);
}

void CEmojiDlg::OnTimer(UINT_PTR idEvent)
{
	SetMsgHandled(FALSE);
}
void CEmojiDlg::OnEmotionItemClick(const std::string& strID)
{
	if ( m_pListner)
	{
		m_pListner->EmotionTileViewItemClick(strID);
	}
}
void CEmojiDlg::OnSysCommand(UINT nID, SOUI::CPoint pt)
{
	switch (nID)
	{
		case SC_CLOSE:
		{
			ShowWindow(SW_HIDE);
			return;
		}
		break;
	}
	SetMsgHandled(FALSE);
}