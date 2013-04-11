#include "DUIWinVer.h"
#include "DUITooltip.h"
#include <commctrl.h>

DUI_BGN_NAMESPCE

CDUITooltip::CDUITooltip()
{
	m_hWndTooltip = NULL;
}

CDUITooltip::~CDUITooltip()
{
	Uninstall();
}

BOOL CDUITooltip::Setup()
{
	if(m_hWndTooltip != NULL
		&& ::IsWindow(m_hWndTooltip))
	{
		return TRUE;
	}

	m_hWndTooltip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL
		, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP
		, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
		, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);

	DUI_ASSERT(m_hWndTooltip != NULL);
	
	return m_hWndTooltip != NULL;
}

VOID CDUITooltip::Uninstall()
{
	if(m_hWndTooltip != NULL && ::IsWindow(m_hWndTooltip))
	{
		DestroyWindow(m_hWndTooltip);
	}
	
	m_hWndTooltip = NULL;
}

BOOL CDUITooltip::AddTool(HWND hWndTool)
{
	if(m_hWndTooltip == NULL) return FALSE;

	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO	);
	ti.hinst = GetModuleHandle(NULL);
	ti.hwnd = hWndTool;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	ti.uId = (UINT)hWndTool;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	
	::SendMessage(m_hWndTooltip, TTM_ADDTOOL, 0, (LPARAM) &ti);
	::SendMessage(m_hWndTooltip, TTM_ACTIVATE, TRUE, 0);
	
	return TRUE;
}

BOOL CDUITooltip::RemoveTool(HWND hWndTool)
{
	if(m_hWndTooltip == NULL) return FALSE;

	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO	);
	ti.hinst = GetModuleHandle(NULL);
	ti.hwnd = hWndTool;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	ti.uId = (UINT)hWndTool;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	
	::SendMessage(m_hWndTooltip, TTM_DELTOOL, 0, (LPARAM) &ti);

	return TRUE;
}	

BOOL CDUITooltip::Activate(BOOL bActivate)
{
	if(m_hWndTooltip == NULL) return FALSE;

	::SendMessage(m_hWndTooltip, TTM_ACTIVATE, bActivate, 0);

	return TRUE;
}

HWND CDUITooltip::GetTooltipWnd()
{
	return m_hWndTooltip;
}


DUI_END_NAMESPCE