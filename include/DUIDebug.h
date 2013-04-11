#ifndef __DUI_DEBUG_H__
#define __DUI_DEBUG_H__

#include "DUIDef.h"
#include <crtdbg.h>

DUI_BGN_NAMESPCE

#define DUI_ASSERT _ASSERTE

enum DUI_TRACE_LEVEL
{
	DUI_TRACE_INFO = 1,
	DUI_TRACE_WARNING,
	DUI_TRACE_ERROR
};

class DUILIB_API CTrace
{
public:
	static VOID _cdecl TraceInfo(LPCTSTR lpszFormat, ...);
	static VOID _cdecl TraceWarning(LPCTSTR lpszFormat, ...);
	static VOID _cdecl TraceError(LPCTSTR lpszFormat, ...);
	
	static VOID _cdecl Trace(DUI_TRACE_LEVEL level, LPCTSTR lpszFormat, ...);

private:
	static VOID OutputTraceInfo(DUI_TRACE_LEVEL level, LPCTSTR lpszText);
};

class DUILIB_API CWinMsgDebug
{
public:
	static VOID OuputMsg(HWND hWnd, UINT nMsgType, WPARAM wParam = NULL, LPARAM lParam = NULL);

protected:
	static	LPCTSTR DUIGetMessageName(UINT uMsg);
};


#define DUI_DEBUG_MAP_MSG			CWinMsgDebug::OuputMsg(hWnd,  uMsg,  wParam,  lParam);
#define DUI_DEBUG_MSG(pMsg)			CWinMsgDebug::OuputMsg(pMsg->hwnd,  pMsg->message,  pMsg->wParam,  pMsg->lParam);

class DUILIB_API CTimeCalculator
{
public:
	CTimeCalculator();
	
	virtual unsigned __int64 Start();
	virtual unsigned __int64 Stop();
	virtual double GetInterval() const;
	
protected:
	unsigned __int64 m_nStartTimer;
	unsigned __int64 m_nStopTimer;
	
	static unsigned __int64 m_nFrequency;
};


class DUILIB_API CTimeCostDebug
{
public:
	CTimeCostDebug(LPCTSTR lpszTopic);
	~CTimeCostDebug();
	
protected:
	CTimeCalculator m_timer;
	TCHAR m_szTopic[256];
};



DUI_END_NAMESPCE

#endif //__DUI_DEBUG_H__