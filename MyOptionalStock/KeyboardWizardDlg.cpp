#include "stdafx.h"
#include "KeyboardWizardDlg.h"
#include "ExternEvent/ExtendEvents.h"


std::vector<StockInfo> _gCodeTable;
CKeyboardWizardDlg::CKeyboardWizardDlg() : SOUI::SHostWnd(_T("LAYOUT:XML_AUTO_PROMPT"))
{
	m_bLayoutInited = FALSE;
	pugi::xml_document doc;
	if ( doc.load_file("xml\\stock_codetb.xml"))
	{
		for (auto child = doc.child(L"contractinfo").first_child(); child; child = child.next_sibling())
		{
			StockInfo info;
			for (auto attr = child.first_attribute(); attr; attr = attr.next_attribute())
			{
				if (_wcsicmp(attr.name(), L"name") == 0)
				{
					info.code_name = attr.value();
				}		
				else if (_wcsicmp(attr.name(), L"symbol") == 0)
				{
					info.code_id = attr.value();
				}
				else if (_wcsicmp(attr.name(), L"exchange") == 0)
				{
					info.code_exchange = attr.value();
				}
			}
			_gCodeTable.push_back(info);
		}
	}
}


CKeyboardWizardDlg::~CKeyboardWizardDlg()
{
	
}
BOOL CKeyboardWizardDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	m_bLayoutInited = TRUE;
	// 响应Edit的EN_CHANGE事件
	SOUI::SEdit *pEditCode = FindChildByName2<SOUI::SEdit>(L"edit_code");
	SASSERT(pEditCode);
	if (pEditCode)
	{
		pEditCode->SSendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE);
	}
	// 隐藏横滚动条
	SOUI::SMCListView *pMCListCode = FindChildByName2<SOUI::SMCListView>(L"mclv_code");
	SASSERT(pMCListCode);
	if (pMCListCode)
	{
		pMCListCode->EnableScrollBar(SOUI::SBST_HOVER,FALSE);
		pMCListCode->SetAdapter((SOUI::IMcAdapter*)this);
	}
	return 0;
}

void CKeyboardWizardDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DOWN || nChar == VK_UP)
	{
		SOUI::SMCListView *pMCListCode = FindChildByName2<SOUI::SMCListView>(L"mclv_code");
		SASSERT(pMCListCode);
		if (pMCListCode && ( (nChar == VK_DOWN && pMCListCode->GetSel()< (int)m_curSelectStock.size()) || 
			(nChar == VK_UP && pMCListCode->GetSel() > 0)))
		{
			int nOldSel = pMCListCode->GetSel();
			int nNewSel = nOldSel + ((nChar == VK_DOWN) ? 1 : -1);
			pMCListCode->SetSel(nNewSel, FALSE);
			pMCListCode->EnsureVisible(nNewSel);
		}
	}else if ( nChar == VK_RETURN)
	{
		SOUI::SMCListView *pMCListCode = FindChildByName2<SOUI::SMCListView>(L"mclv_code");
		SASSERT(pMCListCode);
		if (pMCListCode &&  pMCListCode->GetSel() >= 0)
		{
			if ( pMCListCode->GetSel()< (int)m_curSelectStock.size())
			{
				StockInfo *pInfo = &m_curSelectStock[pMCListCode->GetSel()];
				if ( pInfo)
				{
					ShowWindow(SW_HIDE);
				}
			}
		}
	}
	else if (nChar == VK_BACK)
	{
		SOUI::SEdit *pEditCode = FindChildByName2<SOUI::SEdit>(L"edit_code");
		SASSERT(pEditCode);
		if (pEditCode->GetWindowTextW().GetLength() <=1)
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}
	else
	{
		SetMsgHandled(FALSE);
	}
}

void CKeyboardWizardDlg::OnCodeChange(SOUI::EventArgs * pEvt)
{
	if ( m_bLayoutInited)
	{
		SOUI::EventRENotify *pEvt2 = SOUI::sobj_cast<SOUI::EventRENotify>(pEvt);
		STRACE(_T("OnUrlReNotify,iNotify = %d"), pEvt2->iNotify);
		if (pEvt2->iNotify == EN_CHANGE)
		{
			SOUI::SStringW code;
			SOUI::SEdit *pEditCode = FindChildByName2<SOUI::SEdit>(L"edit_code");
			SASSERT(pEditCode);
			if (pEditCode)
			{
				code = pEditCode->GetWindowText(FALSE);
				m_curSelectStock.clear();
				if ( !code.IsEmpty())
				{
					for (auto stock: _gCodeTable)
					{
						if ( stock.code_id.Find(code) >=0 || stock.code_name.Find(code) >=0 )
						{
							m_curSelectStock.push_back(stock);
						}
					}
				}
				notifyDataSetChanged();
				SOUI::SMCListView *pMCListCode = FindChildByName2<SOUI::SMCListView>(L"mclv_code");
				SASSERT(pMCListCode);
				if (pMCListCode)
				{
					pMCListCode->SetSel(0);
				}
			}
		}
	}
}
void CKeyboardWizardDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	SOUI::SEdit *pEditCode = FindChildByName2<SOUI::SEdit>(L"edit_code");
	SASSERT(pEditCode);
	if ( pEditCode)
	{
		pEditCode->SetWindowTextW(SOUI::SStringW(m_utFirstChar));
		pEditCode->SetFocus();
		pEditCode->SetSel(-1);
	}
}

// 重写 SMcAdapterBase 接口

int  CKeyboardWizardDlg::getCount()
{
	return m_curSelectStock.size();
}
SOUI::SStringW CKeyboardWizardDlg::GetColumnName(int iCol) const
{
	static std::map<int, SOUI::SStringW> _mapColName = {
		{0,L"code_id"},
		{ 1, L"code_name" } ,
		{ 2, L"code_type" },
	};
	auto itFind = _mapColName.find(iCol);
	if ( itFind != _mapColName.end())
	{
		return itFind->second;
	}
	else
	{
		SASSERT(FALSE);
		return L"";
	}
}
void CKeyboardWizardDlg::getView(int position, SOUI::SWindow * pItem, pugi::xml_node xmlTemplate)
{
	if (pItem->GetChildrenCount() == 0) // 确保只调用一次
	{
		// 初始化item模板
		pItem->InitFromXml(xmlTemplate);
	}
	StockInfo *pStockInfo = NULL;
	if (position < (int)m_curSelectStock.size())
	{
		pStockInfo = &m_curSelectStock[position];
	}
	if ( pStockInfo)
	{
		pItem->FindChildByName(L"txt_code_id")->SetWindowText(SOUI::S_CW2T(pStockInfo->code_id));
		pItem->FindChildByName(L"txt_code_name")->SetWindowText(SOUI::S_CW2T(pStockInfo->code_name));
		pItem->FindChildByName(L"txt_code_type")->SetWindowText(SOUI::S_CW2T(pStockInfo->code_exchange));
	}
}