#include "DUIPopupWindow.h"
#include "DUIDdb.h"
#include "DUIToolBar.h"

#if WINVER < 0x0500
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif // WINVER < 0x0500
DUI_BGN_NAMESPCE


////////////////  CMinMaxInfo  ////////////////////////
CMinMaxInfo::CMinMaxInfo()
{
	m_ptMaxTrackSize.x = m_ptMaxTrackSize.y = 0;
	m_ptMinTrackSize.x = m_ptMinTrackSize.y = 0;
}

BOOL CMinMaxInfo::IsEmpty() const
{
	return m_ptMaxTrackSize.x == 0 
		&& m_ptMaxTrackSize.y == 0
		&& m_ptMinTrackSize.x == 0
		&& m_ptMinTrackSize.y == 0;
}

////////////////  CMinMaxInfo  ////////////////////////



/////////////////////  CDUIPopupWindow  ////////////////
CDUIPopupWindow::CDUIPopupWindow()
{
	m_nCaptionHeight = 0;
	m_dwDUIStyle = 0;
	m_pLastFocusControl = NULL;
}

CDUIPopupWindow::~CDUIPopupWindow()
{

}


VOID CDUIPopupWindow::OnDUINotify(const DUINotify& info, BOOL& bHandled)
{
	CheckSystemNotification(info, bHandled);
	
	if(!bHandled)
	{
		theBase::OnDUINotify(info, bHandled);
	}
}

VOID CDUIPopupWindow::OnBuilderWindowAttribute(const CDUIString& strAttriName, const CDUIString& strAttriValue)
{
	if(strAttriName.compare(_T("nc_style")) == 0)
	{
		DWORD dwStyle(0);
		if(strAttriValue.find(_T("layered")) != -1)
		{
			dwStyle |= DUI_WS_LAYERED;
		}
		if(strAttriValue.find(_T("custom_nc")) != -1)
		{
			dwStyle |= DUI_WS_CUSTOM_NC;
		}
		if(strAttriValue.find(_T("region_nc")) != -1)
		{
			dwStyle |= DUI_WS_NC_RGN;
		}
		if(strAttriValue.find(_T("size_nc")) != -1)
		{
			dwStyle |= DUI_WS_NC_SIZABLE;
		}

		SetDUIStyle(0, dwStyle);
	}
	else if(strAttriName.compare(_T("min_size")) == 0)
	{
		TCHAR szValue[128] = {0};
		lstrcpyn(szValue, strAttriValue.c_str(), sizeof(szValue) / sizeof(TCHAR) - 1);
		LPTSTR pstr = szValue;
		INT nWidth = _tcstol(pstr, &pstr, 10);    
		INT nHeight = _tcstol(pstr + 1, &pstr, 10);

		m_MinMaxInfo.m_ptMinTrackSize.x = nWidth;
		m_MinMaxInfo.m_ptMinTrackSize.y = nHeight;
	}
	else if(strAttriName.compare(_T("max_size")) == 0)
	{
		TCHAR szValue[128] = {0};
		lstrcpyn(szValue, strAttriValue.c_str(), sizeof(szValue) / sizeof(TCHAR) - 1);
		LPTSTR pstr = szValue;
		INT nWidth = _tcstol(pstr, &pstr, 10);    
		INT nHeight = _tcstol(pstr + 1, &pstr, 10);

		m_MinMaxInfo.m_ptMaxTrackSize.x = nWidth;
		m_MinMaxInfo.m_ptMaxTrackSize.y = nHeight;
	}
	else if(strAttriName.compare(_T("caption_height")) == 0)
	{
		SetCaptionHeight(_ttol(strAttriValue.c_str()));
	}
	else
	{
		theBase::OnBuilderWindowAttribute(strAttriName, strAttriValue);
	}
}



VOID CDUIPopupWindow::SetCaptionHeight(INT nHeight)
{
	m_nCaptionHeight = nHeight;
}

BOOL CDUIPopupWindow::PreTranslateMessage(MSG* pMsg)
{
	//	DUI_DEBUG_MSG(pMsg)
	BOOL bHandled(FALSE);
	if(pMsg->hwnd == NULL) return FALSE;
	if(pMsg->hwnd == m_hWnd)
	{
		if(!IsNcActivate()) return FALSE;
		
		if(pMsg->message == WM_SYSCHAR)
		{
			TCHAR cShortcut = (TCHAR)(toupper(pMsg->wParam));
			IDUIControl* pControl = m_pRootControl->FindControl(ContorlFromShortcutCallBack
				, &cShortcut, FIND_FLAG_VISIBLE | FIND_FLAG_ENABLED | FIND_FLAG_MEFIRST);
			
			if(pControl != NULL)
			{
				pControl->SetFocus();
				pControl->Activate();

				bHandled = TRUE;
			}
		}
		else if(pMsg->message == WM_KEYDOWN)
		{
			OnPreKeyDown(pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);
		}
	}

	if(bHandled) return TRUE;

	return theBase::PreTranslateMessage(pMsg);
}

LRESULT CDUIPopupWindow::OnPreKeyDown(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if(wParam == VK_TAB)
	{
		if(::GetKeyState(VK_LCONTROL) < 0 
			|| ::GetKeyState(VK_RCONTROL) < 0) //ctrl+tab
		
		{
			bHandled = SetNextTabPage(::GetKeyState(VK_SHIFT) >= 0);
		}
		else //only tab
		{
			bHandled = SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
		}

	}
	else if(wParam == VK_RETURN)
	{
		IDUIControl* pControl = GetControlByName(DUI_STR_OK, FALSE);
		if(pControl != NULL && m_pFocusControl != pControl)
		{
			if(m_pFocusControl == NULL 
				|| !(m_pFocusControl->GetStyle() & CONTROL_STYLE_WANTRETURN))
			{
				pControl->Activate();
				bHandled = TRUE;
			}
		}
	}
	else if(wParam == VK_ESCAPE)
	{
		IDUIControl* pControl = GetControlByName(DUI_STR_CANCEL, FALSE);
		if(pControl != NULL)
		{
			pControl->Activate();
			bHandled = TRUE;
		}
	}

	return S_OK;
}

BOOL CDUIPopupWindow::SetNextTabPage(BOOL bForward /* = TRUE */)
{
	IDUIControl* pControl = GetControlByInterface(ITabBar);
	if(pControl == NULL) return FALSE;

	IDUITabBar* pTabBar = (IDUITabBar*)pControl->GetInterface(ITabBar);
	DUI_ASSERT(pTabBar != NULL);
	return pTabBar->SelectNextItem(bForward, TRUE);
}

BOOL CDUIPopupWindow::SetNextTabControl(BOOL bForward /* = TRUE */)
{
	FINDTABINFO info1 = { 0 };
	info1.pFocus = m_pFocusControl;
	info1.bForward = bForward;
	IDUIControl* pControl = m_pRootControl->FindControl(ControlFromTabCallBack
		, &info1, FIND_FLAG_VISIBLE | FIND_FLAG_ENABLED | FIND_FLAG_MEFIRST);
	if( pControl == NULL ) 
	{  
		if( bForward )
		{
			// Wrap around
			FINDTABINFO info2 = { 0 };
			info2.pFocus = bForward ? NULL : info1.pLast;
			info2.bForward = bForward;
			pControl = m_pRootControl->FindControl(ControlFromTabCallBack
				, &info2, FIND_FLAG_VISIBLE | FIND_FLAG_ENABLED | FIND_FLAG_MEFIRST);
		}
		else
		{
			pControl = info1.pLast;
		}
	}

	if( pControl != NULL ) 
	{
		pControl->SetFocus(TRUE);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

LRESULT CDUIPopupWindow::OnCreate(UINT nMsgType, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CheckDUIStyle();

	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp != NULL)
	{
		CDUIMessageLoop* pMsgLoop = pDUIApp->GetMessageLoop();
		pMsgLoop->AddMessageFilter(this);
	}
	
	m_pLastFocusControl = NULL;

	bHandled = FALSE;
	return S_OK;
}

LRESULT CDUIPopupWindow::OnDestroy(UINT nMsgType, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp != NULL)
	{
		CDUIMessageLoop* pMsgLoop = pDUIApp->GetMessageLoop();
		pMsgLoop->RemoveMessageFilter(this);
	}
	
	m_pLastFocusControl = NULL;

	bHandled = FALSE;
	return S_OK;
}

LRESULT CDUIPopupWindow::OnSize(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	
	if(!IsIconic() && IsDUISetNCRgn())
	{
		CRefPtr<CDUIFrameBK> pFrameBK = GetCurrentFrameBK();
		if(!pFrameBK.IsNull()
			&& !pFrameBK->IsEmpty())
		{
			pFrameBK->SetWinFrameRgn(m_hWnd);
		}
	}
	
	CheckSysButtonStatus();

	return S_OK;
}

LRESULT CDUIPopupWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	if(m_pLastFocusControl != NULL)
	{
		m_pLastFocusControl->SetFocus(TRUE);
	}

	bHandled = FALSE;
	return S_OK;
}

LRESULT CDUIPopupWindow::OnKillFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	if(m_pFocusControl != NULL)
	{
		m_pLastFocusControl = m_pFocusControl;
	}

	bHandled = FALSE;
	return S_OK;
}

LRESULT CDUIPopupWindow::OnSysCommand(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;

	return S_OK;
}

VOID CDUIPopupWindow::CheckSysButtonStatus()
{
	IDUIControl* pMaxBtn = GetControlByName(DUI_SYS_MAX_BTN);
	IDUIControl* pRestoreBtn = GetControlByName(DUI_SYS_RESTORE_BTN);

	if(IsZoomed())
	{
		if(pMaxBtn != NULL)
		{
			pMaxBtn->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		}

		if(pRestoreBtn != NULL)
		{
			pRestoreBtn->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}
	}
	else
	{
		if(pMaxBtn != NULL)
		{
			pMaxBtn->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}

		if(pRestoreBtn != NULL)
		{
			pRestoreBtn->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		}
	}
}

VOID CDUIPopupWindow::CheckSystemNotification(const DUINotify& info, BOOL& bHandled)
{
	IDUIControl* pControl = info.pSender;
	
	switch(info.dwMsgType)
	{
	case WM_NOTIFY_BUTTON_CLICKED:
		if(pControl != NULL)
		{
			const CDUIString& strName = pControl->GetName();
			if(strName.compare(DUI_SYS_MIN_BTN) == 0)
			{
				if(GetStyle()&WS_SYSMENU)
				{
					PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				}
				else
				{
					ShowWindow(SW_MINIMIZE);
				}
				bHandled = TRUE;
			}
			else if( strName.compare(DUI_SYS_MAX_BTN) == 0)
			{
				if(GetStyle()&WS_SYSMENU)
					PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				else	
					ShowWindow(SW_MAXIMIZE);
				bHandled = TRUE;
			}
			else if( strName.compare(DUI_SYS_RESTORE_BTN) == 0)
			{
				if(GetStyle()&WS_SYSMENU)
					PostMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
				else
					ShowWindow(SW_RESTORE);
				bHandled = TRUE;
			}
			else if(strName.compare(DUI_SYS_CLOSE_BTN) == 0)
			{
				if(GetStyle()&WS_SYSMENU)
					PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
				else
					PostMessage(WM_CLOSE, 0 , 0);
				bHandled = TRUE;			
			}
		}
		break;
		
	default:
		break;
	}
}

DWORD CDUIPopupWindow::GetDUIStyle() const
{
	return m_dwDUIStyle;
}

DWORD CDUIPopupWindow::SetDUIStyle(DWORD dwStyleRemove, DWORD dwStyleAdd)
{
	DWORD dwNewStyle = (m_dwDUIStyle & ~dwStyleRemove) | dwStyleAdd;
	if(m_dwDUIStyle != dwNewStyle)
	{
		m_dwDUIStyle = dwNewStyle;
		
		CheckDUIStyle();
	}
	
	return m_dwDUIStyle;
}


VOID CDUIPopupWindow::SetDUIMinMaxInfo(const CMinMaxInfo& info)
{
	m_MinMaxInfo = info;
}

BOOL CDUIPopupWindow::IsNcActivate() 
{
	return m_bNCActivate;
}

BOOL CDUIPopupWindow::IsDUILayeredWindow()
{
	if(IsWindow())
	{
		return GetExStyle()&WS_EX_LAYERED;
	}
	else
	{
		return GetDUIStyle()&DUI_WS_LAYERED;
	}
}

BOOL CDUIPopupWindow::IsDUICustomNC()
{
	return GetDUIStyle()&DUI_WS_CUSTOM_NC;
}

BOOL CDUIPopupWindow::IsDUISetNCRgn()
{
	return (IsDUICustomNC() && (GetDUIStyle()&DUI_WS_NC_RGN));
}

BOOL CDUIPopupWindow::IsDUINCSizable()
{
	return (IsDUICustomNC() && (GetDUIStyle()&DUI_WS_NC_SIZABLE));
}

INT CDUIPopupWindow::NcHitTest(const POINT& ptHit)
{
	RECT rtClient;
	GetClientRect(&rtClient);

	RECT rtFrame = {0};
	CRefPtr<CDUIFrameBK> pFrameBK = GetCurrentFrameBK();
	if(!pFrameBK.IsNull())
	{
		rtFrame = pFrameBK->GetFrameBorder();
	}

	RECT rtTop = rtClient;
	rtTop.bottom = rtTop.top + rtFrame.top;
	
	RECT rtBottom = rtClient;
	rtBottom.top = rtBottom.bottom - rtFrame.bottom;
	
	RECT rtLeft = rtClient;
	rtLeft.right = rtLeft.left + rtFrame.left;
	rtLeft.top = rtTop.bottom;
	rtLeft.bottom = rtBottom.top;
	
	RECT rtRight = rtLeft;
	rtRight.right = rtClient.right;
	rtRight.left = rtRight.right - rtFrame.right;
	
	INT nHitRet(HTNOWHERE);
	BOOL bNCSizable = IsDUINCSizable();
	if(IsZoomed()) bNCSizable = FALSE;
	if(PtInRect(&rtBottom, ptHit))
	{
		nHitRet = (bNCSizable? HTBOTTOM: HTBORDER);
	}
	else if(PtInRect(&rtLeft, ptHit))
	{
		nHitRet = (bNCSizable? HTLEFT: HTBORDER);
	}
	else if(PtInRect(&rtTop, ptHit))
	{
		nHitRet = (bNCSizable? HTTOP: HTBORDER);
	}
	else if(PtInRect(&rtRight, ptHit))
	{
		nHitRet = (bNCSizable? HTRIGHT: HTBORDER);
	}
	
	const INT nDiffer = 3;
	INT nTopBorder = rtTop.bottom - rtTop.top + nDiffer;
	INT nBottomBorder = rtBottom.bottom - rtBottom.top + nDiffer;
	RECT rtLeftTop = {rtTop.left, rtTop.top, rtTop.left + nTopBorder, rtTop.top + nTopBorder};
	RECT rtRightTop = {rtTop.right - nTopBorder, rtTop.top, rtTop.right, rtTop.top + nTopBorder};
	RECT rtLeftBottom = { rtBottom.left, rtBottom.bottom - nBottomBorder, rtBottom.left + nBottomBorder, rtBottom.bottom};
	RECT rtRightBottom = { rtBottom.right - nBottomBorder, rtBottom.bottom - nBottomBorder, rtBottom.right, rtBottom.bottom};
	
	if(PtInRect(&rtLeftTop, ptHit))
	{
		nHitRet = (bNCSizable? HTTOPLEFT: HTBORDER);
	}
	else if(PtInRect(&rtRightTop, ptHit))
	{
		nHitRet = (bNCSizable? HTTOPRIGHT: HTBORDER);
	}
	else if(PtInRect(&rtLeftBottom, ptHit))
	{
		nHitRet = (bNCSizable? HTBOTTOMLEFT: HTBORDER);
	}
	else if(PtInRect(&rtRightBottom, ptHit))
	{
		nHitRet = (bNCSizable? HTBOTTOMRIGHT: HTBORDER);
	}
	
	if(nHitRet == HTNOWHERE)
	{
		RECT rtCaption = {0};
		if(m_nCaptionHeight > 0)
		{
			RECT rtClient = {0};
			GetDUIClientRect(&rtClient);
			rtCaption = rtClient;
			rtCaption.bottom = rtCaption.top + m_nCaptionHeight;
		}

		if(!IsRectEmpty(&rtCaption))
		{
			if(::PtInRect(&rtCaption, ptHit))
			{
				POINT ptControl = ptHit;
				IDUIControl* pControl =  FindControl(&ptControl);
				if(pControl == NULL 
					|| (pControl->GetInterface(IButton) == NULL))
				{
					nHitRet = HTCAPTION;
				}
			}
		}
	}

	return nHitRet;
}

VOID CDUIPopupWindow::CheckDUIStyle()
{
	if(!IsWindow()) return;
	
	DWORD dwWinStyle = GetStyle();
	DWORD dwWinExStyle = GetExStyle();
	BOOL bDUILayered = (m_dwDUIStyle&DUI_WS_LAYERED);
	BOOL bWinLayered = (dwWinExStyle&WS_EX_LAYERED);
	
	BOOL bUpdate(FALSE);
	if(bDUILayered != bWinLayered)
	{
		if(bDUILayered)
		{
			ModifyStyleEx(0, WS_EX_LAYERED, 0);
		}
		else
		{
			ModifyStyleEx(WS_EX_LAYERED,0 , 0);
		}
		
		bUpdate = TRUE;
	}
	
	BOOL bCustomNC = IsDUICustomNC();
	DWORD dwBorderStyle = WS_BORDER | WS_CAPTION | WS_DLGFRAME;
	BOOL bWinBorder = (dwWinStyle&dwBorderStyle);
	if(bCustomNC && bWinBorder)
	{
		ModifyStyle(dwBorderStyle, 0);
		bUpdate = TRUE;
	}
	
	if(bUpdate)
	{
		InvalidateRect(NULL, TRUE);
	}
}

LRESULT CDUIPopupWindow::OnNcHitTest(UINT, WPARAM, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	if(!IsDUICustomNC()) return S_OK;
	
	POINT ptHit = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	::ScreenToClient(m_hWnd, &ptHit);
	INT nHitPos = NcHitTest(ptHit);
	if(nHitPos != HTNOWHERE) bHandled = TRUE;
	
	return nHitPos;
}

LRESULT CDUIPopupWindow::OnNcActivate(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	
	m_bNCActivate = (BOOL)wParam;
	
	Invalidate(TRUE);
	
	return S_OK;
}

LRESULT CDUIPopupWindow::OnNcCalcSize(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	if(IsDUICustomNC())
	{
		bHandled = TRUE;
	}
	else
	{
		bHandled = FALSE;
	}

	return 0;
}

LRESULT CDUIPopupWindow::OnNcPaint(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	if(IsDUICustomNC())
	{
		bHandled = TRUE;
	}
	else
	{
		bHandled = FALSE;
	}

	return 0;
}

LRESULT CDUIPopupWindow::OnGetMinMaxInfo(UINT, WPARAM, LPARAM lParam, BOOL& bHandled)
{
	MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
	if(!m_MinMaxInfo.IsEmpty())
	{
		if(m_MinMaxInfo.m_ptMinTrackSize.x > 0)
		{
			pInfo->ptMinTrackSize.x = m_MinMaxInfo.m_ptMinTrackSize.x;
		}
		
		if(m_MinMaxInfo.m_ptMinTrackSize.y > 0)
		{
			pInfo->ptMinTrackSize.y = m_MinMaxInfo.m_ptMinTrackSize.y;
		}
		
		if(m_MinMaxInfo.m_ptMaxTrackSize.x > 0)
		{
			pInfo->ptMaxTrackSize.x = pInfo->ptMaxSize.x = m_MinMaxInfo.m_ptMaxTrackSize.x;
		}
		
		if(m_MinMaxInfo.m_ptMaxTrackSize.y > 0)
		{
			pInfo->ptMaxTrackSize.y = pInfo->ptMaxSize.y = m_MinMaxInfo.m_ptMaxTrackSize.y;
		}
	}
	else
	{
		MONITORINFO oMonitor = {0};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		RECT rcWork = oMonitor.rcWork;
		::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
		pInfo->ptMaxPosition.x	= rcWork.left;
		pInfo->ptMaxPosition.y = rcWork.top;
		pInfo->ptMaxSize.x = rcWork.right;
		pInfo->ptMaxSize.y = rcWork.bottom;
	}
	
	return S_OK;
}
/////////////////////  CDUIPopupWindow  ////////////////

DUI_END_NAMESPCE