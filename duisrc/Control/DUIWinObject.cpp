#include "DUIWinObject.h"

DUI_BGN_NAMESPCE

CDUIWinObject::CDUIWinObject()
{
	m_hWnd = NULL;
}

LPVOID CDUIWinObject::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IWinObject) == 0)
	{
		return (IDUIWinObject*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIWinObject::Clone()
{
	return new CDUIWinObject(*this);
}

LRESULT CDUIWinObject::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
// 	LRESULT lRet(S_OK);
// 	
// 	switch(info.dwEventType)
// 	{
// 	case DUI_EVENT_SETFOCUS:
// 		if(m_hWnd != NULL && ::IsWindow(m_hWnd))
// 		{
// 			::SetFocus(m_hWnd);
// 			bHandled = TRUE;
// 		}
// 		break;
// 
// 	default:
// 		break;
// 	}
// 
// 	if(bHandled) return lRet;

	return theBase::ProcessEvent(info, bHandled);
}

VOID CDUIWinObject::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	if(m_hWnd != NULL && ::IsWindow(m_hWnd))
	{
		RECT rtContent = GetContentRect();
		::SetWindowPos(m_hWnd, NULL, rtContent.left, rtContent.top, 
			rtContent.right - rtContent.left, rtContent.bottom - rtContent.top, SWP_NOACTIVATE);
	}
}

VOID CDUIWinObject::ModifyStyle(DWORD dwRemove, DWORD dwAdd)
{
	theBase::ModifyStyle(dwRemove, dwAdd);

	SyncProperty();
}

VOID CDUIWinObject::ModifyStatus(DWORD dwRemove, DWORD dwAdd)
{
	theBase::ModifyStatus(dwRemove, dwAdd);
	
//	SyncProperty();
}


VOID CDUIWinObject::SetText(const CDUIString& strText)
{
	theBase::SetText(strText);
	
	if(m_strText.size() > 0)
	{
		SetWindowText(m_hWnd, m_strText.c_str());
	}	
}

VOID CDUIWinObject::SetHWnd(HWND hWnd)
{
	m_hWnd = hWnd;

	SyncProperty();
}

HWND CDUIWinObject::GetHWnd() const
{
	return m_hWnd;
}


VOID CDUIWinObject::OnCreate()
{
	SyncProperty();
}

VOID CDUIWinObject::SyncProperty()
{
	if(m_hWnd == NULL
		|| !::IsWindow(m_hWnd))
	{
		return;
	}

	HWND hWndManager = GetManagerWnd(FALSE);
	if(hWndManager == NULL) return;

	::SetParent(m_hWnd, hWndManager);

	::ShowWindow(m_hWnd, IsVisible()?SW_SHOW:SW_HIDE);
	::EnableWindow(m_hWnd, IsEnabled());

// 	if(GetFocus())
// 	{
// 		::SetFocus(m_hWnd);
// 	}
	
	RECT rtContent = GetContentRect();
	::SetWindowPos(m_hWnd, NULL, rtContent.left, rtContent.top, 
			rtContent.right - rtContent.left, rtContent.bottom - rtContent.top, SWP_NOACTIVATE);

	if(m_strText.size() > 0)
	{
		SetWindowText(m_hWnd, m_strText.c_str());
	}
}

DUI_END_NAMESPCE