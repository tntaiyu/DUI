#ifndef __DUI_ATL_H__
#define __DUI_ATL_H__

#include <atlbase.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#ifdef DUI_USE_WTL
#include "atlapp.h"
extern CAppModule _Module;
#else
extern CComModule _Module;
#endif //DUI_USE_WTL

#include <atlwin.h>

#endif //__DUI_ATL_H__