#include "DUIDebug.h"
#include "DUIRef.h"
#include "DUIBase.h"

#pragma warning(disable:4996)

DUI_BGN_NAMESPCE

class CTraceBuffer
{
public:	
	enum 
	{
		TRACE_BUFFER_SIZE = 1024
	};

	static LPTSTR GetBuffer()
	{
		return m_lpszBuffer;
	}

protected:
	 static TCHAR m_lpszBuffer[TRACE_BUFFER_SIZE + 1];
};

TCHAR CTraceBuffer::m_lpszBuffer[TRACE_BUFFER_SIZE + 1] = {0};


/////////////////   CTrace   ///////////////////////////
VOID CTrace::OutputTraceInfo(DUI_TRACE_LEVEL level, LPCTSTR lpszText)
{
#ifdef _DEBUG
	switch(level)
	{
	case DUI_TRACE_INFO:
		OutputDebugString(_T("DUI_Info: "));
		OutputDebugString(lpszText);
		OutputDebugString(_T("\n"));
		break;
		
	case DUI_TRACE_WARNING:
		OutputDebugString(_T("DUI_Warning: "));
		OutputDebugString(lpszText);
		OutputDebugString(_T("\n"));
		break;
		
	case DUI_TRACE_ERROR:
		OutputDebugString(_T("DUI_Error: "));
		OutputDebugString(lpszText);
		OutputDebugString(_T("\n"));
		break;
		
	default:
		DUI_ASSERT(FALSE);
		break;
	}
#else
	switch(level)
	{
	case DUI_TRACE_INFO:
		OutputDebugString(_T("DUI_Info: "));
		OutputDebugString(lpszText);
		OutputDebugString(_T("\n"));
		break;
		
	case DUI_TRACE_WARNING:
		OutputDebugString(_T("DUI_Warning: "));
		OutputDebugString(lpszText);
		OutputDebugString(_T("\n"));
		break;
		
	case DUI_TRACE_ERROR:
		OutputDebugString(_T("DUI_Error: "));
		OutputDebugString(lpszText);
		OutputDebugString(_T("\n"));
		break;
		
	default:
		DUI_ASSERT(FALSE);
		break;
	}	
#endif //_DEBUG
}

VOID __cdecl CTrace::Trace(DUI_TRACE_LEVEL level, LPCTSTR lpszFormat, ...)
{

	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf = _vsntprintf(CTraceBuffer::GetBuffer(), CTraceBuffer::TRACE_BUFFER_SIZE, lpszFormat, args);
	DUI_ASSERT(nBuf < CTraceBuffer::TRACE_BUFFER_SIZE);
	
	va_end(args);
	
	OutputTraceInfo(level, CTraceBuffer::GetBuffer());
}

VOID __cdecl CTrace::TraceInfo(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf = _vsntprintf(CTraceBuffer::GetBuffer(), CTraceBuffer::TRACE_BUFFER_SIZE, lpszFormat, args);
	DUI_ASSERT(nBuf < CTraceBuffer::TRACE_BUFFER_SIZE);
	
	va_end(args);
	
	OutputTraceInfo(DUI_TRACE_INFO, CTraceBuffer::GetBuffer());
}

VOID __cdecl CTrace::TraceWarning(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf = _vsntprintf(CTraceBuffer::GetBuffer(), CTraceBuffer::TRACE_BUFFER_SIZE, lpszFormat, args);
	DUI_ASSERT(nBuf < CTraceBuffer::TRACE_BUFFER_SIZE);
	
	va_end(args);
	
	OutputTraceInfo(DUI_TRACE_WARNING, CTraceBuffer::GetBuffer());
}

VOID __cdecl CTrace::TraceError(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	
	int nBuf = _vsntprintf(CTraceBuffer::GetBuffer(), CTraceBuffer::TRACE_BUFFER_SIZE, lpszFormat, args);
	DUI_ASSERT(nBuf < CTraceBuffer::TRACE_BUFFER_SIZE);
	
	va_end(args);
	
	OutputTraceInfo(DUI_TRACE_ERROR, CTraceBuffer::GetBuffer());
}
/////////////////   CTrace   ///////////////////////////


//////////////////   CWinMsgDebug  /////////////////////////////////////////
VOID CWinMsgDebug::OuputMsg(HWND hWnd, UINT nMsgType, WPARAM wParam /* = NULL */, LPARAM lParam /* = NULL */)
{
//	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));

// 	TCHAR szText[256] = {0};
// 	TCHAR szClass[32] = {0};
// 
// 	::GetWindowText(hWnd, szText, sizeof(szText) / sizeof(TCHAR) - 1);
// 	::GetClassName(hWnd, szClass, sizeof(szClass) / sizeof(TCHAR) - 1);

//	CTrace::TraceInfo(_T("Win Msg: hWnd[0X%04X], text[%s], class[%s], nMsgType[%s], wParam[%u], lParam[%u]")
//		, hWnd, szText, szClass, DUIGetMessageName(nMsgType), wParam, lParam);

	CTrace::TraceInfo(_T("Win Msg: hWnd[0X%04X], nMsgType[%s], wParam[%u], lParam[%u]")
		, hWnd, DUIGetMessageName(nMsgType), wParam, lParam);
}

LPCTSTR CWinMsgDebug::DUIGetMessageName(UINT uMsg)
{
#ifdef _UNICODE
#define MSGDEF(x) if(uMsg==x) return L#x
#else
#define MSGDEF(x) if(uMsg==x) return #x
#endif //_UNICODE
	
	MSGDEF(WM_NULL);
	MSGDEF(WM_SETCURSOR);
	MSGDEF(WM_NCHITTEST);
	MSGDEF(WM_NCPAINT);
	MSGDEF(WM_PAINT);
	MSGDEF(WM_ERASEBKGND);
	MSGDEF(WM_NCMOUSEMOVE);  
	MSGDEF(WM_MOUSEMOVE);
	MSGDEF(WM_MOUSELEAVE);
	MSGDEF(WM_MOUSEHOVER);   
	MSGDEF(WM_NOTIFY);
	MSGDEF(WM_COMMAND);
	MSGDEF(WM_MEASUREITEM);
	MSGDEF(WM_DRAWITEM);   
	MSGDEF(WM_LBUTTONDOWN);
	MSGDEF(WM_LBUTTONUP);
	MSGDEF(WM_LBUTTONDBLCLK);
	MSGDEF(WM_RBUTTONDOWN);
	MSGDEF(WM_RBUTTONUP);
	MSGDEF(WM_RBUTTONDBLCLK);
	MSGDEF(WM_SETFOCUS);
	MSGDEF(WM_KILLFOCUS);  
	MSGDEF(WM_MOVE);
	MSGDEF(WM_SIZE);
	MSGDEF(WM_SIZING);
	MSGDEF(WM_MOVING);
	MSGDEF(WM_GETMINMAXINFO);
	MSGDEF(WM_CAPTURECHANGED);
	MSGDEF(WM_WINDOWPOSCHANGED);
	MSGDEF(WM_WINDOWPOSCHANGING);   
	MSGDEF(WM_NCCALCSIZE);
	MSGDEF(WM_NCCREATE);
	MSGDEF(WM_NCDESTROY);
	MSGDEF(WM_TIMER);
	MSGDEF(WM_KEYDOWN);
	MSGDEF(WM_KEYUP);
	MSGDEF(WM_CHAR);
	MSGDEF(WM_SYSKEYDOWN);
	MSGDEF(WM_SYSKEYUP);
	MSGDEF(WM_SYSCOMMAND);
	MSGDEF(WM_SYSCHAR);
	MSGDEF(WM_VSCROLL);
	MSGDEF(WM_HSCROLL);
	MSGDEF(WM_CHAR);
	MSGDEF(WM_SHOWWINDOW);
	MSGDEF(WM_PARENTNOTIFY);
	MSGDEF(WM_CREATE);
	MSGDEF(WM_NCACTIVATE);
	MSGDEF(WM_ACTIVATE);
	MSGDEF(WM_ACTIVATEAPP);   
	MSGDEF(WM_CLOSE);
	MSGDEF(WM_DESTROY);
	MSGDEF(WM_GETICON);   
	MSGDEF(WM_GETTEXT);
	MSGDEF(WM_GETTEXTLENGTH);  
	
	//unknown message
	static TCHAR szMsg[10] = {0};
	::wsprintf(szMsg, _T("0x%04X"), uMsg);
	
	return szMsg;
}
//////////////////   CWinMsgDebug  /////////////////////////////////////////



///////////////////  CTimeCalculator  ////////////////////////////////////
unsigned __int64 CTimeCalculator::m_nFrequency = 0;
CTimeCalculator::CTimeCalculator()
{
	if(m_nFrequency == 0)
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&m_nFrequency);
	}
	
	Start();
}

unsigned __int64  CTimeCalculator::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_nStartTimer);
	m_nStopTimer = m_nStartTimer;
	
	return  m_nStartTimer;
}

unsigned __int64 CTimeCalculator::Stop()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_nStopTimer);
	
	return m_nStopTimer;
}

double CTimeCalculator::GetInterval() const
{
	return 1000 * ((double)(long)(m_nStopTimer - m_nStartTimer)) / (double)(long)m_nFrequency;
}
///////////////////  CTimeCalculator  ////////////////////////////////////


///////////////////   CTimeCostDebug   ///////////////////////////////////
CTimeCostDebug::CTimeCostDebug(LPCTSTR lpszTopic)
{
	DUI_ASSERT(lpszTopic != NULL);
	lstrcpyn(m_szTopic, lpszTopic, sizeof(m_szTopic) / sizeof(TCHAR) - 1);
	
	m_timer.Start();
}

CTimeCostDebug::~CTimeCostDebug()
{
	m_timer.Stop();
	
	CTrace::TraceInfo( _T("[%s] cost time[ %f ms ]"), m_szTopic, m_timer.GetInterval());
}
///////////////////   CTimeCostDebug   ///////////////////////////////////

DUI_END_NAMESPCE