// EmptyProject.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MyOptionalStock.h"
#include "MainDlg.h"

// ��չƤ��
#include "ExternSkins/SVscrollbar.h"
#include "ExternSkins/ExtendSkins.h"
#include "ExternSkins/SAntialiasSkin.h"
// ��չ�ؼ�
#include "ExternCtrls/GridHeaderCtrl.h"
#include "ExternCtrls/GridCtrl.h"
#include "ExternCtrls/imre/SImRichEdit.h"
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartTitleCtrl.h"
#include "IMChart/Snapshot/SSnapshotCtrl.h"
#include "ExternCtrls/miniblink/SMiniBlink.h"
#include "ExternCtrls/miniblink/wkedefine.h"


#include "GlobalUnits.h"


#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

//��ʾ�첽����
class CAsyncTaskObj
{
public:
	void task1(int a)
	{
		SLOG_INFO("task1,a:" << a);
	}

	void task2(int a, const std::string & b)
	{
		SLOG_INFO("task2,a:" << a << " b:" << b.c_str());
	}
};

#include <iostream>
using namespace std;
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	// �ٻ�����̨
#ifdef DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
#endif 
	Sleep(4000);
	printf("Hello World111111111111/r/rn");
	std::cout << "Hello World" << std::endl;
	// OLE��ʼ��
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	// ���õ�ǰ·��
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
	_tcscpy(lpInsertPos + 1, _T("\0"));
	SetCurrentDirectory(szCurrentDir);

	int nRet = 0;
	// Com��������ʼ��
	SComMgr *pComMgr = new SComMgr;
	{
		BOOL bLoaded = FALSE;

		// ������������ʼ��
		SOUI::CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;     
			bLoaded = pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));

		// ��Ⱦ��������ʼ��
		SOUI::CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;         
		bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));

		// ��Ⱦ�����ý�����
		pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
		
// 		SOUI::CAutoRefPtr<SOUI::ITranslatorMgr> trans;
// 		bLoaded = pComMgr->CreateTranslator((IObjRef**)&trans);
// 		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("translator"));
		// ����App����
		SOUI::SApplication *theApp = new SOUI::SApplication(pRenderFactory, hInstance);

		// ע����չƤ��
		theApp->RegisterSkinClass<SSkinVScrollbar>();
		theApp->RegisterSkinClass<SColorMask>();
		theApp->RegisterSkinClass<SAntialiasSkin>();
		// ע����չ�ؼ�
		theApp->RegisterWindowClass<CGridHeaderCtrl>();
		theApp->RegisterWindowClass<CGridCtrl>();
		theApp->RegisterWindowClass<CChartCtrl>();
		theApp->RegisterWindowClass<CChartTitleCtrl>();
		theApp->RegisterWindowClass<SSnapshotCtrl>();

		// ���ı�����ص�ע��
		theApp->RegisterWindowClass<SImRichEdit>();
		theApp->RegisterWindowClass<SImageView>(); // ��ע��png����ʾ

		std::string strPYMapPath = S_CW2A(szCurrentDir);
		strPYMapPath += "\\default_res\\PYmap\\mapfile.txt";
		//������ƴ�����ձ�
		CGlobalUnits::GetInstance()->OperatePinyinMap(strPYMapPath);
		CGlobalUnits::GetInstance()->OperateSerachIndex();
		// ��ʼ�������
		CGlobalUnits::GetInstance()->OperateEmojis();
		
		
		// ��ʼ����־ģ��
		CAutoRefPtr<ILog4zManager>  pLogMgr;                //log4z����
		pComMgr->CreateLog4z((IObjRef**)&pLogMgr);
		SASSERT("��־ģ���ʼ��ʧ��");
		if (pLogMgr)
		{
			pLogMgr->createLogger("MyOptionalStock");
			pLogMgr->start();
			theApp->SetLogManager(pLogMgr);
			// ���ﲻ�����Ͻ�����־���   ��־�߳�û������������ 
			// SLOG_INFO("test= 10000123123");
			// SLOGFMTE("log output using ansi format,str=%s, tick=%u", "test", GetTickCount());
			// SLOGFMTE(L"log output using unicode format,str=%s, tick=%u", L"������־", GetTickCount());
		}


		// mb
		SWkeLoader wkeLoader;
		theApp->RegisterWindowClass<SWkeWebkit>();


		//��ʾ�첽����
		CAutoRefPtr<ITaskLoop>  pTaskLoop;
		if (pComMgr->CreateTaskLoop((IObjRef**)&pTaskLoop))
		{
			CAsyncTaskObj obj;
			pTaskLoop->start("test", ITaskLoop::Low);
			STaskHelper::post(pTaskLoop, &obj, &CAsyncTaskObj::task1, 100, true);
			STaskHelper::post(pTaskLoop, &obj, &CAsyncTaskObj::task2, 100, "abc", true);
		}
// 		if (trans)
// 		{//�������Է����
// 			theApp->SetTranslator(trans);
// 			pugi::xml_document xmlLang;
// 			if (theApp->LoadXmlDocment(xmlLang, _T("lang_cn"), _T("translator")))
// 			{
// 				SOUI::CAutoRefPtr<SOUI::ITranslator> langCN;
// 				trans->CreateTranslator(&langCN);
// 				langCN->Load(&xmlLang.child(L"language"), 1);//1=LD_XML
// 				trans->InstallTranslator(langCN);
// 			}
// 		}

		HMODULE hModSysResource = ::LoadLibrary(SYS_NAMED_RESOURCE);
		if (hModSysResource)
		{
			SOUI::CAutoRefPtr<SOUI::IResProvider> sysResProvider;
			SOUI::CreateResProvider(SOUI::RES_PE, (IObjRef**)&sysResProvider);
			sysResProvider->Init((WPARAM)hModSysResource, 0);
			theApp->LoadSystemNamedResource(sysResProvider);
			FreeLibrary(hModSysResource);
		}
		else
		{
			SASSERT(0);
		}

		SOUI::CAutoRefPtr<SOUI::IResProvider>   pResProvider;
		SOUI::CreateResProvider(SOUI::RES_FILE, (IObjRef**)&pResProvider);
		bLoaded = pResProvider->Init((LPARAM)_T("uires"), 0);
		SASSERT(bLoaded);
		theApp->AddResProvider(pResProvider);
		
		// �¼�֪ͨ
		SNotifyCenter *pNotifyCenter = new SNotifyCenter;
		{
			CMainDlg dlgMain;
			dlgMain.Create(GetActiveWindow());
			dlgMain.SendMessage(WM_INITDIALOG);
			dlgMain.CenterWindow(dlgMain.m_hWnd);
			dlgMain.ShowWindow(SW_SHOWNORMAL);
			nRet = theApp->Run(dlgMain.m_hWnd);
		}
		delete theApp;
		if (pLogMgr)
		{
			pLogMgr->stop();
		}
		if (pTaskLoop)
		{
			pTaskLoop->stop();		
			pTaskLoop = NULL;
		}
	}
	delete pComMgr;
	OleUninitialize();
	return nRet;
}