#include "stdafx.h"
#include "MyOptionalStock.h"
#include "MainDlg.h"
#include "SMcListViewEx/SMCListViewEx.h"
#include <Wininet.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/stringbuffer.h"
#include "KeyboardWizardDlg.h"
#include "IMChart/IMChartDlg.h"
#include "ExternEvent/ExtendEvents.h"
#include "ExternCtrls/miniblink/SMiniBlink.h"




using namespace rapidjson;

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
	m_pQuoteListImpl = new CQuoteListImpl;
}


CMainDlg::~CMainDlg()
{
	if (m_pQuoteListImpl)delete m_pQuoteListImpl; m_pQuoteListImpl = NULL;
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	m_pAutoPromptDlg = new CKeyboardWizardDlg;
	m_pAutoPromptDlg->Create(m_hWnd, 0, 0, 500, 300);
	m_pAutoPromptDlg->ShowWindow(SW_HIDE);
	m_pAutoPromptDlg->SendMessage(WM_INITDIALOG);
	m_pIMChartDlg = new CIMChartDlg;
	m_pIMChartDlg->Create(m_hWnd, 0, 0, 800,640);
	m_pIMChartDlg->ShowWindow(SW_HIDE);
	m_pIMChartDlg->SendMessage(WM_INITDIALOG);
	return 0;
}
std::string& replace_str(std::string& str, const std::string& to_replaced, const std::string& newchars)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += newchars.length())
	{
		pos = str.find(to_replaced, pos);
		if (pos != std::string::npos)
			str.replace(pos, to_replaced.length(), newchars);
		else
			break;
	}
	return   str;
}std::string Utf8ToMbcs(const char* src);
int urlopen(std::string url, std::string& str)
{
	int BUF_LEN = 1024 * 10;
	std::string ret;
	int nLen = 0;
	HINTERNET hSession = InternetOpenA("urltest", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET hHttp = InternetOpenUrlA(hSession, url.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (hHttp != NULL)
		{
			char* Temp = new char[BUF_LEN];

			ULONG Number = 0;
			while (true)
			{
				if (!InternetReadFile(hHttp, Temp, BUF_LEN - 1, &Number)) break;

				if (Number <= 0) break;
				Temp[Number] = 0;
				ret += Temp;
				nLen += Number;

			}

			str = Utf8ToMbcs(ret.c_str());

			InternetCloseHandle(hHttp);
			hHttp = NULL;
			delete[] Temp; Temp = NULL;
		}

		InternetCloseHandle(hSession);
		hSession = NULL;

	}


	return 0;
}
std::string Utf8ToMbcs(const char* src)
{
	// MFC多字节模式
	std::string str;
	WCHAR* pwchar = NULL; // 16位宽字符串指针
	CHAR*  pchar = NULL; // 8位字符串指针

	// 获取存储宽字符串所需要的空间(单位为WCHAR)
	int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	pwchar = new WCHAR[len]; // 为宽字符串分配内存空间
	if (pwchar == NULL) return str;

	// 将字符串转换为宽字符串
	len = MultiByteToWideChar(CP_UTF8, 0, src, -1, pwchar, len);

	if (len != 0)
	{
		// 判断文件输入输出函数使用的是ANSI代码页还是OEM代码页
		int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

		len = WideCharToMultiByte(codepage, 0, pwchar, -1, NULL, 0, NULL, NULL);
		pchar = new CHAR[len];
		if (pchar != NULL)
		{
			// 将字符串转换为UTF8编码
			len = WideCharToMultiByte(codepage, 0, pwchar, -1, pchar, len, NULL, NULL);
			if (len != 0)
			{
				str = pchar;
			}
			delete[] pchar;
		}
	}

	delete[] pwchar;

	return str;
}
SStringW GetTimeStr(CTime tm)
{
	return SStringW().Format(L"%04d/%02d/%02d %02d:%02d:%02d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	//
	if ( m_pQuoteListImpl)
	{
		m_pQuoteListImpl->OnInit(this);
	}

	SWkeWebkit *pWeb = FindChildByName2<SWkeWebkit>(L"wke_test");
	SASSERT(pWeb);
	if (pWeb)
	{
		// 设置cookie
		wkeSetCookie(pWeb->GetWebView(), "", "");
	}

	std::string ret;
	urlopen("http://data.gtimg.cn/flashdata/hushen/4day/sz/sz300251.js?maxage=43200&visitDstTime=1", ret);
	ret = std::string(ret, std::string("var min_data_4d=").size(), ret.size());
	ret.erase(ret.size()-1);
	ret =replace_str(ret,"\'","\"");
	Document d;
	d.Parse<0>(ret.c_str());

	if (d.IsArray())
	{
		for (size_t i = 0; i < d.Size()&& i==0 ; ++i)
		{
			Value &v = d[i];
			if (v.IsObject())
			{
				int y, m, d;
				if (v.HasMember("date") && v["date"].IsString())
				{
					std::string day = v["date"].GetString();
					if (!day.empty())
					{
						sscanf_s(day.c_str(), "%2d%2d%2d %2d:%2d:%2d", &y, &m, &d);
					}
				}
				if (v.HasMember("data") && v["data"].IsString())
				{
					SArray<SStringW> array;
					SplitString(S_CA2W(v["data"].GetString()), L'^', array);
					for (size_t i = 0; i < array.GetCount(); ++i)
					{
						SArray<SStringW> info;
						SplitString(array[i], L'~', info);
						SASSERT(info.GetCount() == 4);
						if ( info.GetCount() == 4)
						{
							DC_KData data;
							// 时间
							int tmp = _ttoi(info[0]);
							data.time = CTime(y + 2000, m, d,tmp/100,tmp%100,0 ).GetTime();
							// 价格
							data.value = _ttof(info[1]);
							m_arrDynaData.Add(data);
						}
					}
				}
			}
		}
	}

	urlopen("http://data.gtimg.cn/flashdata/hushen/latest/daily/sz000002.js?maxage=43201&visitDstTime=1", ret);
	SArray<SStringW> arrtmp;
	SplitString(S_CA2W(ret.c_str()), L'\n', arrtmp);
	if ( arrtmp.GetCount() >2)
	{
		for (size_t i = 0; i < arrtmp.GetCount(); ++i)
		{
			SArray<SStringW> arrayData;
			SplitString(arrtmp.GetAt(i), L' ', arrayData);
			if ( arrayData.GetCount() == 6U)
			{
				//日期   开盘  最新(收盘)  最高    最低  成交量
				//190404 32.50 32.22      33.12   31.75 692092
				DC_KData data;
				// 时间
				int tmp = _ttoi(arrayData[0]);
				data.time = CTime(tmp / 10000 + 2000, tmp % 10000 / 100, tmp % 100,0,0,0).GetTime();
				auto t1 = GetTimeStr(data.time);
				// 开盘
				data.Open = _ttof(arrayData[1]);
				// 最新 收盘价
				data.Close = _ttof(arrayData[2]);
				// 最高
				data.High = _ttof(arrayData[3]);
				// 最低
				data.Low = _ttof(arrayData[4]);
				// 成交量
				data.Volume = _ttoll(arrayData[5]);
				m_arrKData.Add(data);
			}
		}
	}


	CChartCtrl*pChart = FindChildByName2<CChartCtrl>(L"mychart");
	SASSERT(pChart);
	if ( pChart)
	{
		DC_ContractInfo info;
		strcpy(info.ContractName, "光线传媒");
		strcpy(info.ContractNo, "300251");
		info.pect = 2;
		info.TickPrice = 0.01;
		Period prd;
		prd.kind = DAY_DATA;
		pChart->SetContractInfo(&info, &prd);
		// 分时线测试
		// pChart->UpdateHisData(m_arrDynaData.GetData(), m_arrDynaData.GetCount(), m_arrDynaData.GetCount(), 0, m_arrDynaData.GetAt(0).value);
		// K线测试
		pChart->UpdateHisData(m_arrKData.GetData(), m_arrKData.GetCount(), m_arrKData.GetCount(), 0, 0);
	}
	return 0;
}
void CMainDlg::ShowKeyboardWizard()
{
	SOUI::CRect rcClient = GetClientRect();
	rcClient.top = rcClient.bottom - 310;
	rcClient.left = rcClient.right - 285;
	ClientToScreen(rcClient);
	m_pAutoPromptDlg->MoveWindow(&rcClient);
	m_pAutoPromptDlg->ShowWindow(SW_SHOW);
}
void CMainDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( (nChar >='0' && nChar <='9')||(nChar <='z' && nChar>='a') || (nChar>='A' && nChar <='Z') )
	{
		SetMsgHandled(TRUE);
		if (!m_pAutoPromptDlg->IsWindowVisible())
		{
			m_pAutoPromptDlg->SetFirstChar(nChar);
		}
		ShowKeyboardWizard();
	}
	else
	{
		SetMsgHandled(FALSE);
	}
}
void CMainDlg::OnClose()
{
	__super::DestroyWindow();
}


void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}
void CMainDlg::OnImChart()
{
	if ( m_pIMChartDlg && m_pIMChartDlg->IsWindow())
	{
		m_pIMChartDlg->ShowWindow(SW_SHOW);
	}
}
BOOL CMainDlg::OnEraseBkgnd(HDC dc)
{
	return TRUE;
}

void CMainDlg::OnSize(UINT nType, SOUI::CSize size)
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
	if (m_pAutoPromptDlg && m_pAutoPromptDlg->IsWindow() && m_pAutoPromptDlg->IsWindowVisible())
	{
		ShowKeyboardWizard();
	}
}
