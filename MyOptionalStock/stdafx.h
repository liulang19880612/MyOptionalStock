#pragma once

#define  _CRT_SECURE_NO_WARNINGS  
#define	 DLL_SOUI

#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include <core/swnd.h>
#include <event/Events.h>
#include <event/NotifyCenter.h>
#include <helper/SAdapterBase.h>
#include <layout/SouiLayout.h>
 
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <io.h>

#include <Imm.h>
#pragma comment(lib, "imm32.lib")

using namespace SOUI;
#include <math.h>
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")
#ifndef _Post_writable_byte_size_
#define _Post_writable_byte_size_(s)
#endif


//<--定一个filter="demo"的slog输出
#undef LOG_FILTER
#define LOG_FILTER "HelloWorld"
#include <helper/slog-def.h>
//-->