#ifndef __DUI_DEF_H__
#define __DUI_DEF_H__

#include <Windows.h>
#include <tchar.h>

#pragma warning(disable:4786)
#pragma warning(disable:4275)
#pragma warning(disable:4251)

//================================
//dui name space
#define DUI_BGN_NAMESPCE namespace DUI {
#define DUI_END_NAMESPCE }

#define DUI_USING_NAMESPACE using namespace DUI;

//=================================
//dll export
#ifdef DUILIB_EXPORTS
#define DUILIB_API __declspec(dllexport)
#else
#define DUILIB_API /*__declspec(dllimport)*/
#endif


#ifndef GET_X_LPARAM
  #define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif //GET_X_LPARAM
#ifndef GET_Y_LPARAM
  #define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif //GET_Y_LPARAM


#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif //WS_EX_LAYERED

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif //AC_SRC_ALPHA

#ifndef  ULW_ALPHA 
#define ULW_ALPHA 0x2
#endif //ULW_ALPHA

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL         0x020A
#endif //WM_MOUSEWHEEL

#ifndef WM_MOUSEHOVER
#define WM_MOUSEHOVER                   0x02A1
#define WM_MOUSELEAVE                   0x02A3
#endif //WM_MOUSEHOVER

#ifndef WS_EX_NOACTIVATE
#define WS_EX_NOACTIVATE 0x08000000L
#endif //WS_EX_NOACTIVATE

//=================================

#define  RICHEDIT_DLL_NAME _T("Riched20.dll")


#define DUI_WND_CLASS _T("DUI_Foundation")


#include "DUIString.h"

class DUILIB_API CStrLessCmp
{
public:
	bool operator()(const CDUIString& str1, const CDUIString& str2) const
	{
		return lstrcmp(str1.c_str(), str2.c_str()) < 0;
	}
};

#endif //end __DUI_DEF_H__