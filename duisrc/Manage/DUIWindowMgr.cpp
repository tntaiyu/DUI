#include "DUIWindowMgr.h"
#include "DUITooltip.h"
#include "DUIRenderEngine.h"
#include <algorithm>


DUI_BGN_NAMESPCE

static UINT MapKeyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
	
	return uState;
}


CDUIWindowMgr::CDUIWindowMgr()
{
	m_bControlLayouting = FALSE;
	m_bMouseTracking = FALSE;
	m_ptLastCursor.x = m_ptLastCursor.y = -1;

	m_pClickControl = NULL;
	m_pHoverControl = NULL;
	m_pFocusControl = NULL;

	m_bLayoutAgain = FALSE;
	m_bLockLayout = FALSE;

	m_bUseSkinBK = FALSE;

	ZeroMemory(&m_rtClientMargin, sizeof(RECT));
}

CDUIWindowMgr::~CDUIWindowMgr()
{

}

BOOL CDUIWindowMgr::RegisterNotifyHandler(IDUINotifyHandler* pNotifier)
{
	DUI_ASSERT(pNotifier != NULL);
	if(pNotifier == NULL) return FALSE;

	CNotifyVector::iterator itr = std::find(m_arNotifier.begin(), m_arNotifier.end(), pNotifier);
	if(itr == m_arNotifier.end())
	{
		m_arNotifier.push_back(pNotifier);
		return TRUE;
	}

	return FALSE;
}

BOOL CDUIWindowMgr::UnregisterNotifyHandler(IDUINotifyHandler* pNotifier)
{
	m_arNotifier.erase(std::remove(m_arNotifier.begin(), m_arNotifier.end(), pNotifier), m_arNotifier.end());
	return TRUE;
}

VOID CDUIWindowMgr::SendNotify(const DUINotify& info)
{
	DUI_ASSERT(!m_pRootControl.IsNull()); 

	BOOL bHandled(FALSE);
	//send to control self first
	if(info.pSender != NULL)
	{
		info.pSender->OnDUINotify(info, bHandled);
		if(bHandled) return;
	}

	//send to manager here
	OnDUINotify(info, bHandled);
	if(bHandled) return;
	
	//send to other registered notifier
	//copy to temp data, avoid  m_arNotifier changed when notify
	CNotifyVector arTemp = m_arNotifier;
	CNotifyVector::iterator itr = arTemp.begin();
	for(itr; itr != arTemp.end(); ++itr)
	{
		DUI_ASSERT(*itr != NULL);
		if(std::find(m_arNotifier.begin(), m_arNotifier.end(), *itr) != m_arNotifier.end())
		{
			(*itr)->OnDUINotify(info, bHandled);
		}
		if(bHandled) break;
	}
}


VOID CDUIWindowMgr::OnDUINotify(const DUINotify& info, BOOL& bHandled)
{
	switch(info.dwMsgType)
	{
	case WM_NOTIFY_CONTROL_ATTACHED:
		{
			break;
		}

	case WM_NOTIFY_CONTROL_DETACHED:
		{
			if(info.pSender == m_pClickControl) m_pClickControl = NULL;
			if(info.pSender == m_pFocusControl) m_pFocusControl = NULL;
			if(info.pSender == m_pHoverControl) m_pHoverControl = NULL;
			break;
		}
		
	default:
		break;
	}
}

LRESULT CDUIWindowMgr::HandleFocusRequest(const DUINotify& info)
{
	if(info.wParam == REQUEST_WPARAM_GET_FOCUS_CONTROL)
	{
		return (LRESULT)m_pFocusControl;
	}

	IDUIControl* pControl = NULL;
	if(info.wParam == REQUEST_WPARAM_KILL_FOCUS)
	{
		pControl = NULL;
	}
	else if(info.wParam == REQUEST_WPARAM_SET_FOCUS)
	{
		pControl = info.pSender;
	}
	else
	{
		DUI_ASSERT(FALSE);
		return S_OK;
	}
	
	BOOL bHandled(FALSE);

	if(pControl != m_pFocusControl)
	{
		if(m_pFocusControl != NULL)
		{
			DUIEvent info = {0};
			info.dwEventType = DUI_EVENT_KILLFOCUS;
			info.pSender = m_pFocusControl;
			m_pFocusControl->OnDUIEvent(info, bHandled);
			
			m_pFocusControl = NULL;
		}
		
		if(pControl != NULL
			&& (pControl->IsVisible())
			&& (pControl->IsEnabled()))
		{
			m_pFocusControl = pControl;
			
			if(::GetFocus() != m_hWnd 
				&& !(GetExStyle() & WS_EX_NOACTIVATE)) 
			{
				::SetFocus(m_hWnd);
			}

			DUIEvent info = {0};
			info.pSender = pControl;
			info.dwEventType = DUI_EVENT_SETFOCUS;
			m_pFocusControl->OnDUIEvent(info, bHandled);
		}
	}

	return S_OK;
}

UINT CDUIWindowMgr::GetIdleTimerIndex()
{
	int nMaxTimerCount = 1024;
	INT nCount = m_arTimer.size();

	INT nRet(0);
	for(int i=1; i<nMaxTimerCount; ++i)
	{
		BOOL bFound(FALSE);
		for(int j=0; j<nCount; ++j)
		{
			LPDUITimerInfo pTimer = m_arTimer[j];
			DUI_ASSERT(pTimer != NULL);

			if(pTimer->nTimerIndex == i) //timer id is used
			{
				bFound = TRUE;
				break;
			}
		}

		if(!bFound) //not used, then use this id
		{
			nRet = i;
			break;
		}
	}

	return nRet;
}

VOID CDUIWindowMgr::ClearTimer()
{
	INT nCount = m_arTimer.size();
	
	for(int i=0; i<nCount; ++i)
	{
		LPDUITimerInfo pTimer = m_arTimer[i];
		DUI_ASSERT(pTimer != NULL);
		
		if(pTimer != NULL && IsWindow())
		{
			::KillTimer(m_hWnd, pTimer->nTimerIndex);
		}
		delete pTimer;
	}

	m_arTimer.clear();
}

HWND CDUIWindowMgr::GetMostTopParent()
{
	if(!IsWindow()) return NULL;
	
	CWindow win(m_hWnd);
	while(TRUE)
	{
		if(win.GetStyle() & WS_CHILD)
		{
			win = win.GetParent();
		}
		else
		{
			break;
		}
	}

	return win.m_hWnd;
}

DWORD CDUIWindowMgr::GetMostTopParentStatus()
{
	HWND hWnd = GetMostTopParent();
	if(hWnd == NULL) return 0;

	CWindow win(hWnd);

	DWORD dwStatus(0);
	if(win.IsIconic())
	{
		dwStatus |= DUI_WIN_STATUS_MIN;
	}
	else if(win.IsZoomed())
	{
		dwStatus |= DUI_WIN_STATUS_MAX;
	}
	else
	{
		dwStatus |= DUI_WIN_STATUS_NORMAL;
	}

	if(win.GetStyle() & WS_DISABLED)
	{
		dwStatus |= DUI_WIN_STATUS_DISABLE;
	}

	if(!(win.GetStyle() & WS_VISIBLE))
	{
		dwStatus |= DUI_WIN_STATUS_INVISIBLE;
	}
	
	if(IsNcActivate())
	{
		dwStatus |= DUI_WIN_STATUS_ACTIVATE;
	}

	return dwStatus;
}

BOOL CDUIWindowMgr::IsNcActivate()
{
	HWND hWndActive = GetActiveWindow();
	if(hWndActive != NULL
		&& hWndActive == GetMostTopParent())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

LRESULT CDUIWindowMgr::HandleTimerRequest(const DUINotify& info)
{
	LPDUITimerInfo pTimerReq = (LPDUITimerInfo)info.lParam;
	LRESULT lRet(S_FALSE);
	if(!IsWindow()) return lRet;

	LPDUITimerInfo pTimerRet = NULL;
	INT nCount = m_arTimer.size();
	for(int i=0; i<nCount; ++i) //check if the request timer is exist
	{
		LPDUITimerInfo pTemp = m_arTimer[i];
		DUI_ASSERT(pTemp != NULL);

		if(pTemp->pControl == pTimerReq->pControl
			&& pTemp->nTimerID == pTimerReq->nTimerID)
		{
			pTimerRet = pTemp;
			break;
		}
	}

	if(info.wParam == WPARAM_SET_TIMER)
	{
		UINT nTimerIndex = GetIdleTimerIndex();
		if(pTimerRet == NULL && nTimerIndex != 0)
		{
			UINT_PTR nID = ::SetTimer(m_hWnd, nTimerIndex, pTimerReq->nElapse, NULL);
			LPDUITimerInfo pNewTimer = new DUITimerInfo;
			if(nID != NULL && pNewTimer != NULL)
			{
				pNewTimer->hWnd = m_hWnd;
				pNewTimer->nElapse = pTimerReq->nElapse;
				pNewTimer->nTimerID = pTimerReq->nTimerID; 
				pNewTimer->nTimerIndex = nID; 
				pNewTimer->pControl = pTimerReq->pControl;

				m_arTimer.push_back(pNewTimer);

				lRet = S_OK;
			}
			else
			{
				if(nID != NULL) ::KillTimer(m_hWnd, nID);
				if(pNewTimer != NULL) delete pNewTimer;
				lRet = E_FAIL;
			}
		}
		else
		{
			DUI_ASSERT(FALSE);
			lRet = E_FAIL;
		}
	}
	else if(info.wParam == WPARAM_KILL_TIMER)
	{
		if(pTimerRet != NULL)
		{
			::KillTimer(m_hWnd, pTimerRet->nTimerIndex);
			m_arTimer.erase(std::remove(m_arTimer.begin(), m_arTimer.end(), pTimerRet), m_arTimer.end());
			delete pTimerRet;
			lRet = S_OK;
		}
	}	

	return lRet;
}

LRESULT CDUIWindowMgr::OnControlRequest( DUINotify& info)
{
	switch(info.dwMsgType)
	{
	case WM_CONTROL_REQUEST_REDRAW:
		if(!m_bLockLayout)
		{
			if(!m_bControlLayouting)
			{
				if(info.wParam) //need resize
				{
					LayoutRootControl();
				}

				RECT rtControl = info.pSender->GetControlRect();
				if(info.lParam != NULL)
				{
					rtControl = *(RECT*)info.lParam;
				}
				InvalidateRect(&rtControl);
			}
			else
			{
				if(info.wParam) //layout later
				{
					m_bLayoutAgain = TRUE;
				}
			}
		}
		break;

	case WM_CONTROL_REQUEST_LOCK_LAYOUT:
		LockLayout((BOOL)info.wParam);
		break;

	case WM_CONTROL_REQUEST_FOCUS:
		return HandleFocusRequest(info);
		break;


	case WM_CONTROL_REQUEST_TIMER:
		return HandleTimerRequest(info);
		break;

	case WM_CONTROL_REQUEST_GET_HWND:
		if(info.wParam == REQUEST_WPARAM_HWND_MANAGER)
		{
			info.lParam = (LPARAM)m_hWnd;
		}
		else if(info.wParam == REQUEST_WPARAM_HWND_TOP_PARENT)
		{
			info.lParam = (LPARAM)GetMostTopParent();
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
		break;
		
	case WM_CONTROL_REQUEST_GET_CONTROL:
		{
			if(info.wParam == REQUEST_WPARAM_CONTROL_BY_NAME)
			{
				CDUIString strTemp((LPCTSTR)info.lParam);
				return  (LRESULT)GetControlByName(strTemp);
			}
			else if(info.wParam == REQUEST_WPARAM_CONTROL_BY_POINT)
			{
				LPPOINT pPtHit = (LPPOINT)info.lParam;
				return (LRESULT)FindControl(pPtHit);
			}
			else
			{
				DUI_ASSERT(FALSE);
				return NULL;
			}
		}
		break;
	
	case WM_CONTROL_REQUEST_WIN_STATUS:
		info.wParam = GetMostTopParentStatus();
		break;

	default:
		DUI_ASSERT(FALSE);
		return S_FALSE;
	}

	return S_OK;
}

BOOL CDUIWindowMgr::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

BOOL CDUIWindowMgr::OnIdle()
{
	return FALSE;
}

CRefPtr<IDUIControl> CDUIWindowMgr::OnBuilderNewControl(const CDUIString& strClassName)
{
	return NULL;
}

VOID CDUIWindowMgr::OnBuilderControlCreated(CRefPtr<IDUIControl> pControl)
{
	
}

VOID CDUIWindowMgr::OnBuilderWindowAttribute(const CDUIString& strAttriName, const CDUIString& strAttriValue)
{
	if(strAttriName.compare(_T("frame_style")) == 0)
	{
		m_pFrameBkgnd = GetSkinMgr()->GetFrameBK(strAttriValue);
	}
	else if(strAttriName.compare(_T("client_margin")) == 0)
	{
		TCHAR szTextBuffer[128] = {0};
		lstrcpyn(szTextBuffer, strAttriValue.c_str(), sizeof(szTextBuffer) / sizeof(TCHAR) - 1);

		RECT rtTemp = {0};
		LPTSTR pstr = NULL;
		rtTemp.left = _tcstol(szTextBuffer, &pstr, 10);
		rtTemp.top = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.right = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.bottom = _tcstol(pstr + 1, &pstr, 10);
		
		SetClientRectMargin(rtTemp);	
	}
	else if(strAttriName.compare(_T("name")) == 0)
	{
		NULL;
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

VOID CDUIWindowMgr::OnSkinUIDataChanged(CRefPtr<CDUIFrameBK> pSkinUIData)
{
	if(m_bUseSkinBK
		&& !IsIconic())
	{
		PostMessage(WM_SIZE);
	}
}

LRESULT CDUIWindowMgr::OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	
	GetSkinMgr()->AddSkinObserver(this);

	CDUITooltip* pTooltip = CDUITooltip::Instance();
	if(pTooltip != NULL)
	{
		pTooltip->AddTool(m_hWnd);
	}

	if(!m_pRootControl.IsNull())
	{
		m_pRootControl->AttachWindow(this);

		m_pClickControl = NULL;
		m_pHoverControl = NULL;
		m_pFocusControl = NULL;
		m_ptLastCursor.x = m_ptLastCursor.y = -1;
		m_mapNameControl.clear();
		m_mapIDControl.clear();
	}

	return S_OK;
}

LRESULT CDUIWindowMgr::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	
	GetSkinMgr()->RemoveSkinObserver(this);

	CDUITooltip* pTooltip = CDUITooltip::Instance();
	if(pTooltip != NULL)
	{
		pTooltip->RemoveTool(m_hWnd);
	}

	if(!m_pRootControl.IsNull())
	{
		m_pRootControl->DetachWindow();
		m_pRootControl = NULL;

		m_bMouseTracking = FALSE;
		m_mapNameControl.clear();
		m_mapIDControl.clear();

		ClearTimer();
	}
	

	return S_OK;
}


LRESULT CDUIWindowMgr::OnKillFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	if(m_pFocusControl != NULL)
	{
		m_pFocusControl->SetFocus(FALSE);
	}

	bHandled = FALSE;

	return S_OK;
}

LRESULT CDUIWindowMgr::OnSetCursor(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if(m_pRootControl == NULL) return S_OK;
	
	POINT ptCursor = {0};
	GetCursorPos(&ptCursor);
	::ScreenToClient(m_hWnd, &ptCursor);
	
	IDUIControl* pControl = FindControl(&ptCursor);
	if(pControl != NULL
		&& (pControl->GetStyle() & CONTROL_STYLE_SETCURSOR))
	{
		BOOL bEventHandled(FALSE);
		DUIEvent event = { 0 };
		event.dwEventType = DUI_EVENT_SETCURSOR;
		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = ptCursor;
		event.wKeyState = MapKeyState();
		event.pSender = pControl;
		pControl->OnDUIEvent(event, bEventHandled);		
		
		bHandled = TRUE;
		return TRUE;
	}
	
	bHandled = FALSE;
	return S_OK;
}

LRESULT CDUIWindowMgr::OnGetDispInfo(INT, LPNMHDR lpnmtdi, BOOL& bHandled)
{
	bHandled = FALSE;

	LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO)lpnmtdi;
	DUI_ASSERT(pDispInfo != NULL);

	if(m_pHoverControl != NULL)
	{	
		const CDUIString& strTooltip = m_pHoverControl->GetTooltip();
		if(strTooltip.size() > 0)
		{
			lstrcpyn(pDispInfo->szText, strTooltip.c_str(), sizeof(pDispInfo->szText) / sizeof(TCHAR) - 1);
		}
	}

	return S_OK;
}

LRESULT CDUIWindowMgr::OnLButtonDBClick(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	m_ptLastCursor = pt;
	
	IDUIControl* pControl = FindControl(&pt);
	if(pControl == NULL) return S_OK;

	m_pClickControl = pControl;
	::SetCapture(m_hWnd);

	BOOL bEventHandled(FALSE);
	DUIEvent info = {0};
	info.pSender = pControl;
	info.wParam = wParam;
	info.lParam = lParam;
	info.ptMouse = pt;
	info.wKeyState = wParam;
	info.dwEventType = DUI_EVENT_LBUTTONDBCLK;

	pControl->OnDUIEvent(info, bEventHandled);
	
	return S_OK;
}

LRESULT CDUIWindowMgr::OnMouseWheel(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if(m_pFocusControl == NULL) return S_OK;

	BOOL bEventHandled(FALSE);
	DUIEvent info = {0};
	info.pSender = m_pFocusControl;
	info.wParam = wParam;
	info.lParam = lParam;
	info.ptMouse = m_ptLastCursor;
	info.wKeyState = LOWORD(wParam);
	info.dwEventType = DUI_EVENT_MOUSEWHEEL;
	
	m_pFocusControl->OnDUIEvent(info, bEventHandled);

	return S_OK;
}

LRESULT CDUIWindowMgr::OnLButtonDown(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	m_ptLastCursor = pt;
	IDUIControl* pControl = FindControl(&pt);
	if(pControl == NULL) return S_OK;
	
	m_pClickControl = pControl;
	::SetCapture(m_hWnd);
	
	if(pControl->GetStyle() & CONTROL_STYLE_TABSTOP)
	{
		pControl->SetFocus();
	}
	
	BOOL bEventHandled(FALSE);
	DUIEvent info = {0};
	info.pSender = pControl;
	info.wParam = wParam;
	info.lParam = lParam;
	info.ptMouse = pt;
	info.wKeyState = wParam;
	info.dwEventType = DUI_EVENT_LBUTTONDOWN;

	pControl->OnDUIEvent(info, bEventHandled);

	//send message to the focused control
	if(m_pFocusControl != NULL
		&& m_pFocusControl != pControl)
	{
		bHandled = FALSE;
		info.pSender = pControl;
		info.dwEventType = DUI_EVENT_MOUSECLICK;
		m_pFocusControl->OnDUIEvent(info, bHandled);
	}

	return S_OK;
}

LRESULT CDUIWindowMgr::OnLButtonUp(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	m_ptLastCursor = pt;
	if(m_pClickControl == NULL) return S_OK;

	DUIEvent info = {0};
	info.dwEventType = DUI_EVENT_LBUTTONUP;
	info.wParam = wParam;
	info.lParam = lParam;
	info.ptMouse = pt;
	info.wKeyState = wParam;
	info.pSender = m_pClickControl;
	
	BOOL bEventHandled(FALSE);
	m_pClickControl->OnDUIEvent(info, bEventHandled);
	m_pClickControl = NULL;
	
	::ReleaseCapture();

	return S_OK;
}

LRESULT CDUIWindowMgr::OnContextMenu(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	::ScreenToClient(m_hWnd, &pt);
	IDUIControl* pControl = FindControl(&pt);
	if(pControl == NULL) return S_OK;
	
	BOOL bEventHandled(FALSE);
	DUIEvent info = {0};
	info.pSender = pControl;
	info.wParam = wParam;
	info.lParam = lParam;
	info.ptMouse = pt;
	info.wKeyState = MapKeyState();
	info.dwEventType = DUI_EVENT_CONTEXTMENU;
	
	pControl->OnDUIEvent(info, bEventHandled);
	return S_OK;
}

LRESULT CDUIWindowMgr::OnMouseLeave(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled= FALSE;
	
	if(m_bMouseTracking)
	{
		::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM)-1);
		m_bMouseTracking = FALSE;
	}
	
	return S_OK;
}

LRESULT CDUIWindowMgr::OnMouseMove(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	if(!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tm = {0};
		tm.cbSize = sizeof(tm);
		tm.dwFlags = TME_LEAVE;
		tm.hwndTrack = m_hWnd;
		m_bMouseTracking = _TrackMouseEvent(&tm);
	}
	
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	m_ptLastCursor = pt;


	IDUIControl* pNewControl = FindControl(&pt);
	
	DUIEvent info = {0};
	info.wParam = wParam;
	info.lParam = lParam;
	info.wKeyState = wParam;
	info.ptMouse = pt;

	BOOL bEventHandled(FALSE);
	if(m_pClickControl != NULL)
	{
		if(pNewControl != m_pHoverControl 
			&& m_pHoverControl != NULL
			&& pNewControl != m_pClickControl)
		{
			info.dwEventType = DUI_EVENT_MOUSELEAVE;
			info.pSender = pNewControl;
			m_pHoverControl->OnDUIEvent(info, bEventHandled);
			m_pHoverControl = NULL;
		}

		if(pNewControl != m_pHoverControl
			&& pNewControl != NULL
			&& pNewControl == m_pClickControl)
		{
			info.dwEventType = DUI_EVENT_MOUSEENTER;
			info.pSender =  m_pHoverControl;
			pNewControl->OnDUIEvent(info, bEventHandled);
			
			m_pHoverControl = pNewControl;
		}

		info.dwEventType = DUI_EVENT_MOUSEMOVE;
		info.pSender = NULL;
		m_pClickControl->OnDUIEvent(info, bEventHandled);
	}
	else
	{
		if(pNewControl != m_pHoverControl && m_pHoverControl != NULL)
		{
			info.dwEventType = DUI_EVENT_MOUSELEAVE;
			info.pSender = pNewControl;
			m_pHoverControl->OnDUIEvent(info, bEventHandled);
			
			
			if(m_pHoverControl != NULL
				&& (m_pHoverControl->GetStyle() & CONTROL_STYLE_TOOLTIP))
			{
				CDUITooltip::Instance()->Activate(FALSE);
			}
			m_pHoverControl = NULL;
		}

		if(pNewControl != m_pHoverControl && pNewControl != NULL)
		{
			info.dwEventType = DUI_EVENT_MOUSEENTER;
			info.pSender =  m_pHoverControl;
			pNewControl->OnDUIEvent(info, bEventHandled);
			
			m_pHoverControl = pNewControl;

			if(m_pHoverControl != NULL
				&& (m_pHoverControl->GetStyle() & CONTROL_STYLE_TOOLTIP))
			{
				CDUITooltip::Instance()->Activate(TRUE);
			}
		}

		if(pNewControl != NULL)
		{
			info.dwEventType = DUI_EVENT_MOUSEMOVE;
			info.pSender = pNewControl;
			pNewControl->OnDUIEvent(info, bEventHandled);
		}
	}

	return S_OK;
}

LRESULT CDUIWindowMgr::OnKeyDown(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	BOOL bEventHandled(FALSE);
	if(m_pFocusControl != NULL)
	{
		DUIEvent event = { 0 };
		event.dwEventType = DUI_EVENT_KEYDOWN;
		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = m_ptLastCursor;
		event.wKeyState = MapKeyState();
		event.chKey = wParam;
		event.pSender = m_pFocusControl;
		
		m_pFocusControl->OnDUIEvent(event, bEventHandled);
		
		bHandled = TRUE;
	}
	
	return S_OK;
}

LRESULT CDUIWindowMgr::OnKeyUp(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	BOOL bEventHandled(FALSE);
	if(m_pFocusControl != NULL)
	{
		DUIEvent event = { 0 };
		event.dwEventType = DUI_EVENT_KEYUP;
		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = m_ptLastCursor;
		event.wKeyState = MapKeyState();
		event.pSender = m_pFocusControl;
		event.chKey = wParam;
		
		m_pFocusControl->OnDUIEvent(event, bEventHandled);
		
		bHandled = TRUE;
	}
	
	return S_OK;
}

LRESULT CDUIWindowMgr::OnChar(UINT, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	bHandled = FALSE;
	
	BOOL bEventHandled(FALSE);
	if(m_pFocusControl != NULL)
	{
		DUIEvent event = { 0 };
		event.dwEventType = DUI_EVENT_CHAR;
		event.wParam = wParam;
		event.lParam = lParam;
		event.ptMouse = m_ptLastCursor;
		event.wKeyState = MapKeyState();
		event.pSender = m_pFocusControl;
		event.chKey = wParam;
		
		m_pFocusControl->OnDUIEvent(event, bEventHandled);
		
		bHandled = TRUE;
	}
	
	return S_OK;
}


LRESULT CDUIWindowMgr::OnTimer(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nTimerIndex = (UINT)LOWORD(wParam);
	INT nCount = m_arTimer.size();

	LPDUITimerInfo pTimer = NULL;
	BOOL bFound(FALSE);
	for(INT i=0; i<nCount; ++i)
	{
		pTimer = m_arTimer[i];
		DUI_ASSERT(pTimer != NULL);

		if(pTimer->nTimerIndex == nTimerIndex)
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound)
	{
		DUI_ASSERT(pTimer != NULL);
		IDUIControl* pControl = pTimer->pControl;
		DUI_ASSERT(pControl != NULL);

		DUIEvent info = {0};
		info.pSender = pTimer->pControl;
		info.wParam = (WPARAM)(pTimer->nTimerID);
		info.ptMouse = m_ptLastCursor;
		info.wKeyState = MapKeyState();
		info.lParam = lParam;
		info.dwEventType = DUI_EVENT_TIMER;
		
		BOOL bEventHandled(FALSE);
		pControl->OnDUIEvent(info, bEventHandled);		
	}
	else
	{
		DUI_ASSERT(FALSE);
	}


	return S_OK;	
}

VOID CDUIWindowMgr::LayoutRootControl()
{
	if(m_pRootControl.IsNull()) return;
	
	CTimeCostDebug debug(_T("CDUIWindowMgr::LayoutRootControl"));

	RECT rtDUIClient = {0};
	GetDUIClientRect(&rtDUIClient);

	m_bControlLayouting = TRUE;
	m_pRootControl->SetControlRect(rtDUIClient);
	m_bControlLayouting = FALSE;

	if(m_bLayoutAgain)
	{
		m_bLayoutAgain = FALSE;
		LayoutRootControl();
	}
}

LRESULT CDUIWindowMgr::OnSize(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	
	if(!m_pRootControl.IsNull())
	{
		DUIEvent info = {0};
		info.wKeyState = MapKeyState();
		info.dwEventType = DUI_EVENT_WINSIZE;
		info.wParam = wParam;
		info.lParam = lParam;
		info.ptMouse = m_ptLastCursor;
		info.pSender = m_pRootControl.GetPtr();
		m_pRootControl->BroadcastEvent(info);
	}

	if(!IsIconic())
	{
		m_backDib.Reset();
		LayoutRootControl();
		Invalidate(TRUE);
	}

	return S_OK;
}

LRESULT CDUIWindowMgr::OnMove(UINT, WPARAM wParam,  LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if(!m_pRootControl.IsNull())
	{
		DUIEvent info = {0};
		info.wKeyState = MapKeyState();
		info.dwEventType = DUI_EVENT_WINMOVE;
		info.wParam = wParam;
		info.lParam = lParam;
		info.ptMouse = m_ptLastCursor;
		info.pSender = m_pRootControl.GetPtr();
		m_pRootControl->BroadcastEvent(info);
	}

	return S_OK;
}

LRESULT CDUIWindowMgr::OnNcActivate(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;

	if( ::IsIconic(*this) ) bHandled = FALSE;

	if(!(GetStyle()&WS_CAPTION))
		return (wParam == 0) ? TRUE : FALSE;
	else
	{
		bHandled = FALSE;
		return S_OK;
	}
}

HWND CDUIWindowMgr::CreateFromXML(const CDUIString& strXML, HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName/* = NULL*/,
				   DWORD dwStyle /*= 0*/, DWORD dwExStyle/* = 0*/,
				   UINT nID/* = 0*/, LPVOID lpCreateParam/* = NULL*/)
{
	if(IsWindow()) return NULL;

	CDUIControlBuilder builder(this);
	m_pRootControl = builder.CreateFromXML(strXML);
	if(m_pRootControl.IsNull()) 
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}

	return Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	
}

HWND CDUIWindowMgr::CreateFromRootControl(CRefPtr<IDUIControl> pRootControl, HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName /*= NULL*/,
						   DWORD dwStyle/* = 0*/, DWORD dwExStyle/* = 0*/,
						   UINT nID/* = 0*/, LPVOID lpCreateParam/* = NULL*/)
{
	if(IsWindow()) return NULL;
	
	DUI_ASSERT(!pRootControl.IsNull());
	if(pRootControl.IsNull()) return NULL;
	
	m_pRootControl = pRootControl;
	return Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
}

CRefPtr<IDUIControl> CDUIWindowMgr::DetachRootControl()
{
	CRefPtr<IDUIControl> pControlRet = m_pRootControl;
	m_pRootControl = NULL;

	return pControlRet;
}

HWND CDUIWindowMgr::CreateFromResource(const CDUIString& strResourceID, HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName /*= NULL*/,
						 DWORD dwStyle /*= 0*/, DWORD dwExStyle/* = 0*/,
			UINT nID /*= 0*/, LPVOID lpCreateParam /*= NULL*/)
{
	if(IsWindow()) return NULL;

	CDUIControlBuilder builder(this);
	m_pRootControl = builder.CreateFromResource(strResourceID);
	if(m_pRootControl.IsNull()) 
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}
	
	return Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
}

IDUIControl* CDUIWindowMgr::GetControlByName(const CDUIString& strName, BOOL bRefresh /* = FALSE */)
{
	if(strName.size() <= 0) return NULL;
	
	if(bRefresh || m_mapNameControl.empty())
	{
		m_mapNameControl.clear();
		
		if(!m_pRootControl.IsNull())
		{
			m_pRootControl->FindControl(&ControlFromNameCallBack, (LPVOID)&m_mapNameControl, FIND_FLAG_ALL);
		}
	}
	
	CNameControlMap::iterator itr = m_mapNameControl.find(strName);
	if(itr == m_mapNameControl.end())
	{
		//DUI_ASSERT(FALSE);
		return NULL;
	}
	
	return itr->second;
}

IDUIControl* CDUIWindowMgr::GetControlByID(DWORD dwControlID, BOOL bRefresh /* = FALSE */)
{
	if(dwControlID == 0) return NULL;

	if(bRefresh || m_mapIDControl.empty())
	{
		m_mapIDControl.clear();
		if(!m_pRootControl.IsNull())
		{
			m_pRootControl->FindControl(&ControlFromIDCallBack, (LPVOID)&m_mapIDControl, FIND_FLAG_ALL);
		}
	}

	CIDControlMap::iterator itr = m_mapIDControl.find(dwControlID);
	if(itr == m_mapIDControl.end())
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}
	
	return itr->second;
}

CRefPtr<CDUIFrameBK> CDUIWindowMgr::GetCurrentFrameBK()
{
	CRefPtr<CDUIFrameBK> pSkinUIData;
	if(m_bUseSkinBK)
	{
		pSkinUIData = GetSkinMgr()->GetSKinUIData();
	}
	else
	{
		pSkinUIData = m_pFrameBkgnd;
	}

	return pSkinUIData;
}

VOID CDUIWindowMgr::PaintFrameBkgnd(HDC dc)
{
	CRefPtr<CDUIFrameBK> pFrameBK = GetCurrentFrameBK();
	if(!pFrameBK.IsNull() 
		&& !pFrameBK->IsEmpty())
	{
		RECT rtClient = {0};
		GetClientRect(&rtClient);
		pFrameBK->DrawBK(dc, rtClient);
	}
}

VOID CDUIWindowMgr::PaintClientArea(HDC dc)
{
	RECT rtClipBox = {0};
	::GetClipBox(dc, &rtClipBox);

	if(!m_pRootControl.IsNull())
	{
		RECT rtDUIClient = {0};
		GetDUIClientRect(&rtDUIClient);

		CDUIRenderClip clip(dc, rtDUIClient);
		m_pRootControl->DoPaint(dc, rtClipBox);
	}	
}

BOOL CDUIWindowMgr::GetDUIClientRect(LPRECT lpRect)
{
	if(!IsWindow() || lpRect == NULL) return FALSE;

	RECT rtClient = {0};
	GetClientRect(&rtClient);
	
	CRefPtr<CDUIFrameBK> pFrameBK = GetCurrentFrameBK();
	if(!pFrameBK.IsNull()
		&& !pFrameBK->IsEmpty())
	{
		RECT rtFrame = pFrameBK->GetFrameBorder();
		rtClient.left += rtFrame.left;
		rtClient.right -= rtFrame.right;
		rtClient.top += rtFrame.top;
		rtClient.bottom -= rtFrame.bottom;


	}

	rtClient.left += m_rtClientMargin.left;
	rtClient.right -= m_rtClientMargin.right;
	rtClient.top += m_rtClientMargin.top;
	rtClient.bottom -= m_rtClientMargin.bottom;

	if(IsRectEmpty(&rtClient))
	{
		rtClient.right = rtClient.left;
		rtClient.bottom = rtClient.top;
	}
	
	*lpRect = rtClient;

	return TRUE;
}

IDUIControl* CDUIWindowMgr::FindControl(LPPOINT pPtHitTest)
{
	DUI_ASSERT(!m_pRootControl.IsNull());
	if(m_pRootControl.IsNull()) return NULL;
	
	return m_pRootControl->FindControl(ControlFromPointCallBack, pPtHitTest
		, FIND_FLAG_HITTEST | FIND_FLAG_VISIBLE);
}

IDUIControl* WINAPI CDUIWindowMgr::ControlFromPointCallBack(IDUIControl* pControl, LPVOID pData)
{
	LPPOINT pPt = static_cast<LPPOINT>(pData);
	DUI_ASSERT(pPt != NULL);
	
	IDUIControl* pRet = NULL;
	if(PtInRect(&pControl->GetControlRect(), *pPt))
	{
		pRet = pControl;
	}
	
	return pRet;
}

IDUIControl* CALLBACK CDUIWindowMgr::ControlFromNameCallBack(IDUIControl* pControl, LPVOID pData)
{
	DUI_ASSERT(pData != NULL && pControl != NULL);
	CNameControlMap* pNamedControlMap = (CNameControlMap*)pData;
	
	CDUIString strName = pControl->GetName();
	if(strName.size() > 0)
	{
		DUI_ASSERT(pNamedControlMap->find(strName) == pNamedControlMap->end());
		pNamedControlMap->insert(std::make_pair(strName, pControl));
	}
	
	return NULL;
}

IDUIControl* CALLBACK CDUIWindowMgr::ControlFromIDCallBack(IDUIControl* pControl, LPVOID pData)
{
	DUI_ASSERT(pData != NULL && pControl != NULL);
	CIDControlMap* pIDControlMap = (CIDControlMap*)pData;
	
	DWORD dwID = pControl->GetID();
	if(dwID != 0)
	{
		DUI_ASSERT(pIDControlMap->find(dwID) == pIDControlMap->end());
		pIDControlMap->insert(std::make_pair(dwID, pControl));
	}
	
	return NULL;
}

IDUIControl* CALLBACK CDUIWindowMgr::ContorlFromShortcutCallBack(IDUIControl* pControl, LPVOID pData)
{
	DUI_ASSERT(pData != NULL);
	TCHAR cShortcut = *(TCHAR*)(pData);
	if(cShortcut == toupper(pControl->GetShortcut())) return pControl;

	return NULL;
}

IDUIControl* CALLBACK CDUIWindowMgr::ControlFromInterfaceCallBack(IDUIControl* pControl, LPVOID pData)
{
	DUI_ASSERT(pData != NULL);
	LPCTSTR lpszInterface = (LPCTSTR)pData;
	if(pControl->GetInterface(lpszInterface) != NULL) return pControl;

	return NULL;
}

IDUIControl* CALLBACK CDUIWindowMgr::ControlFromTabCallBack(IDUIControl* pThis, LPVOID pData)
{
	DUI_ASSERT(pData != NULL);
	FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
	if( pInfo->pFocus == pThis )
	{
		if( pInfo->bForward ) pInfo->bNextIsIt = true;
		return pInfo->bForward ? NULL : pInfo->pLast;
	}
	
	if( !(pThis->GetStyle() & CONTROL_STYLE_TABSTOP) ) return NULL;
	
	pInfo->pLast = pThis;
	if( pInfo->bNextIsIt ) return pThis;
	if( pInfo->pFocus == NULL) return pThis;
	
	return NULL;
}

IDUIControl* CDUIWindowMgr::GetControlByShortcut(TCHAR chShortcut)
{
	return m_pRootControl->FindControl(ContorlFromShortcutCallBack, &chShortcut
		, FIND_FLAG_VISIBLE | FIND_FLAG_ENABLED | FIND_FLAG_MEFIRST);	
}

IDUIControl* CDUIWindowMgr::GetControlByInterface(LPCTSTR lpszInterfaceName)
{
	if(lpszInterfaceName == NULL)
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}

	return m_pRootControl->FindControl(ControlFromInterfaceCallBack, (VOID*)lpszInterfaceName
		, FIND_FLAG_VISIBLE | FIND_FLAG_ENABLED | FIND_FLAG_MEFIRST);	
}


VOID CDUIWindowMgr::LockLayout(BOOL bLock)
{
	m_bLockLayout = bLock;
}


VOID CDUIWindowMgr::SetFrameBkgnd(CRefPtr<CDUIFrameBK> pFrameBK)
{
	m_pFrameBkgnd = pFrameBK;
}

CRefPtr<CDUIFrameBK> CDUIWindowMgr::GetFrameBkgnd() const
{
	return m_pFrameBkgnd;
}

VOID CDUIWindowMgr::SetUseSkinBK(BOOL bUseSkinBK)
{
	m_bUseSkinBK = bUseSkinBK;
}

BOOL CDUIWindowMgr::GetUseSkinBK() const 
{
	return m_bUseSkinBK;
}

VOID CDUIWindowMgr::SetClientRectMargin(RECT rtMargin)
{
	m_rtClientMargin = rtMargin;
}

RECT CDUIWindowMgr::GetClietnRectMargin() const 
{
	return m_rtClientMargin;
}

DUI_END_NAMESPCE