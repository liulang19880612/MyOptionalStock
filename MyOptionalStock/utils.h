#pragma once
#include <GdiPlus.h>
#include <atltime.h>
#include "string\tstring.h"


namespace SOUI
{
    SStringW GenGuid();
    SStringW XMLEscape(SStringW& strXml);
    int GetCodecInfo(const WCHAR* format, Gdiplus::ImageCodecInfo* p);
    SStringW GetTempPath();
    double GetZoomRatio(SIZE normalSize, SIZE maxSize);
	CTime IntDate2DateTime(int intDate);

	SStringW GetExePath();
}

#define SUBSCRIBE(x,y,z) (x)->GetEventSet()->subscribeEvent(y,Subscriber(&z,this))

#define UNSUBSCRIBE(x,y,z) (x)->GetEventSet()->unsubscribeEvent(y,Subscriber(&z,this))