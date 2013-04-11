#include "DUIChildWindow.h"

DUI_BGN_NAMESPCE

CDUIChildWindow::CDUIChildWindow()
{

}

CDUIChildWindow::~CDUIChildWindow()
{

}


VOID CDUIChildWindow::OnDUINotify(const DUINotify& info, BOOL& bHandled)
{
	bHandled= FALSE;

	
	if(!bHandled)
	{
		theBase::OnDUINotify(info, bHandled);
	}
}


BOOL CDUIChildWindow::PreTranslateMessage(MSG* pMsg)
{
	BOOL bHandled(FALSE);
	if(pMsg->hwnd == NULL) return FALSE;
	if(pMsg->hwnd == m_hWnd)
	{
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
	}

	if(bHandled) return TRUE;
	
	return theBase::PreTranslateMessage(pMsg);
}


LRESULT CDUIChildWindow::OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp != NULL)
	{
		CDUIMessageLoop* pMsgLoop = pDUIApp->GetMessageLoop();
		pMsgLoop->AddMessageFilter(this);
	}

	bHandled = FALSE;
	return S_OK;
}

LRESULT CDUIChildWindow::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp != NULL)
	{
		CDUIMessageLoop* pMsgLoop = pDUIApp->GetMessageLoop();
		pMsgLoop->RemoveMessageFilter(this);
	}

	bHandled = FALSE;
	return S_OK;
}

LRESULT CDUIChildWindow::OnGetDlgCode(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	return DLGC_WANTALLKEYS;
}

LRESULT CDUIChildWindow::OnKeyDown(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	
	if(wParam == VK_TAB)
	{
		bHandled = SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
	}

	return S_OK;
}

LRESULT CDUIChildWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	if(m_pFocusControl == NULL)
	{
		SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
	}

	bHandled = FALSE;

	return S_OK;
}

BOOL CDUIChildWindow::SetNextTabControl(BOOL bForward /* = TRUE */)
{
	BOOL bRet(FALSE);
	FINDTABINFO info1 = { 0 };
	info1.pFocus = m_pFocusControl;
	info1.bForward = bForward;
	IDUIControl* pControl = m_pRootControl->FindControl(ControlFromTabCallBack
		, &info1, FIND_FLAG_VISIBLE | FIND_FLAG_ENABLED | FIND_FLAG_MEFIRST);

	if(pControl == NULL)
	{
		HWND hWndParent = GetMostTopParent();
		HWND hWndNext = NULL;
		hWndNext = ::GetNextDlgTabItem(hWndParent, m_hWnd, !bForward);
		if(hWndNext == m_hWnd)
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
			bRet = TRUE;
		}
		else if(hWndNext != NULL)
		{
			if(m_pFocusControl != NULL)
			{
				m_pFocusControl->SetFocus(FALSE);
			}

			::SetFocus(hWndNext);
			bRet = TRUE;
		}
	}

	if( pControl!= NULL ) 
	{  
		pControl->SetFocus(TRUE);
		bRet = TRUE;
	}

	return bRet;
}

DUI_END_NAMESPCE