#include "DUIControlBase.h"
#include "DUIScrollbar.h"
#include "DUIRenderEngine.h"
#include "DUIXml.h"
#include <algorithm>

#pragma warning(disable:4355) //'this' : used in base member initializer list  

DUI_BGN_NAMESPCE

IDUISkinMgr* GetSkinMgr()
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return NULL;

	return pDUIApp->GetSkinMgr();
}

IDUIControlFactory* GetControlFactory()
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return NULL;
	
	return pDUIApp->GetControlFactory();
}


///////////////////  CDUIControlBase  //////////////////////////
CDUIControlBase::CDUIControlBase()
{
	m_pParentControl = NULL;
	m_pControlMgr = NULL;
	m_pSubclassControl = NULL;

	m_dwStyle = CONTROL_STYLE_VISIBLE;
	m_dwExStyle = 0;
	m_dwStatus = 0;
	m_chShorcut = 0;
	m_dwID = 0;
	ZeroMemory(&m_rtControl, sizeof(m_rtControl));
	ZeroMemory(&m_rtMargin, sizeof(m_rtMargin));
	m_sizeFixed.cx = m_sizeFixed.cy = 0;
	m_nPaddingWidth = 0;
}

CDUIControlBase::~CDUIControlBase()
{
	RemoveAllControl();	
}

CDUIControlBase::CDUIControlBase(const CDUIControlBase& object)
{
	m_strName = object.m_strName;
	m_strText = object.m_strText;
	m_strTooltip = object.m_strTooltip;

	m_dwStatus = object.m_dwStatus;
	m_dwStyle = object.m_dwStyle;
	m_dwExStyle = object.m_dwExStyle;
	m_chShorcut = object.m_chShorcut;
	m_rtControl = object.m_rtControl;
	m_rtMargin = object.m_rtMargin;
	m_sizeFixed = object.m_sizeFixed;
	m_nPaddingWidth = object.m_nPaddingWidth;
	m_dwID = object.m_dwID;
	
// 	m_pControlMgr = object.m_pControlMgr;
// 	m_pParentControl = object.m_pParentControl;
// 	m_pSubclassControl = object.m_pSubclassControl;
	m_pControlMgr = NULL;
	m_pParentControl = NULL;
	m_pSubclassControl = NULL;
	
	//m_pFont = object.m_pFont;
	m_pTextStyle = object.m_pTextStyle;
	m_pCursor = object.m_pCursor;

	//copy child controls
	INT nCount = object.GetControlCount();
	for(int i=0; i<nCount; ++i)
	{
		IDUIControl* pControl = object.GetControlByIndex(i);
		if(pControl != NULL)
		{
			CRefPtr<IDUIControl> pNewControl = pControl->Clone();
			if(!pNewControl.IsNull())
			{
				AddControl(pNewControl);
			}
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
	}
}

LRESULT CDUIControlBase::PreTranslateEvent(const DUIEvent& event, BOOL& bHandled)
{
	LRESULT lRet(S_OK);
	DUI_ASSERT(bHandled == FALSE);
	if(m_pSubclassControl != NULL)
	{
		lRet = m_pSubclassControl->OnDUIEvent(event, bHandled);
	}

	return lRet;
}

LRESULT CDUIControlBase::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	switch(info.dwEventType)
	{
	case DUI_EVENT_SETFOCUS:
		ModifyStatus(0, CONTROL_STATUS_FOCUS);
		break;
		
	case DUI_EVENT_KILLFOCUS:
		ModifyStatus(CONTROL_STATUS_FOCUS, 0);
		break;
		
	case DUI_EVENT_VSCROLL:
		OnVertialSBEvent(info);
		break;
		
	case DUI_EVENT_HSCROLL:
		OnHorizontalSBEvent(info);
		break;

	case DUI_EVENT_MOUSEWHEEL:
		OnMouseWheel(info);
		break;
		
	case DUI_EVENT_TIMER:
		OnTimer(info.wParam);
		break;
		
	default:
		break;
	}
	
	return S_OK;	
}

LRESULT CDUIControlBase::OnDUIEvent(const DUIEvent& info, BOOL& bHandled)
{
	bHandled = FALSE;
	LRESULT lRet =	PreTranslateEvent(info, bHandled);
	if(bHandled) return lRet;
	
	return ProcessEvent(info, bHandled);
}

VOID CDUIControlBase::OnDUINotify(const DUINotify& info, BOOL& bHandled)
{
	switch(info.dwMsgType)
	{
	case WM_NOTIFY_CONTROL_ATTACHED:
		{
			OnCreate();
			break;
		}
		
	case WM_NOTIFY_CONTROL_DETACHED:
		{
			OnDestroy();
			break;
		}
		
	default:
		break;
	}
}

VOID CDUIControlBase::AttachWindow(IDUIControlMgr* pControlMgr)
{
	CControlVector::iterator itr;
	for(itr = m_arChildControl.begin(); itr != m_arChildControl.end(); ++itr)
	{
		DUI_ASSERT(!(*itr).IsNull());
		(*itr)->AttachWindow(pControlMgr);
	}

	m_pControlMgr = pControlMgr;

	SendNotify(WM_NOTIFY_CONTROL_ATTACHED);
}

VOID CDUIControlBase::DetachWindow()
{
	CControlVector::iterator itr;
	for(itr = m_arChildControl.begin(); itr != m_arChildControl.end(); ++itr)
	{
		DUI_ASSERT(!(*itr).IsNull());
		(*itr)->DetachWindow();
	}

	SendNotify(WM_NOTIFY_CONTROL_DETACHED);

	m_pControlMgr = NULL;
}

VOID CDUIControlBase::SetParent(IDUIControl* pParent)
{
	m_pParentControl = pParent;
}

IDUIControl* CDUIControlBase::GetParent() const
{
	return m_pParentControl;
}

LPVOID CDUIControlBase::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IControlBase) == 0)
	{
		return (CDUIControlBase*)this;
	}
	else if(strName.compare(IControl) == 0)
	{
		return (IDUIControl*)this;
	}

	return NULL;
}

CRefPtr<IDUIControl> CDUIControlBase::Clone()
{
	DUI_ASSERT(!_T("Please realize it in your child class"));
	CRefPtr<IDUIControl> pNewControl = new CDUIControlBase(*this);
	DUI_ASSERT(!pNewControl.IsNull());
	return pNewControl;
}

BOOL CDUIControlBase::Activate()
{
	if(!(IsVisible())
		|| !IsEnabled()) 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

VOID CDUIControlBase::SetFocus(BOOL bSet /*= TRUE*/)
{
	if(m_pControlMgr == NULL) return;
	
	DUINotify info = {0};
	info.dwMsgType = WM_CONTROL_REQUEST_FOCUS;
	info.pSender = this;
	if(bSet)
		info.wParam  = REQUEST_WPARAM_SET_FOCUS;
	else
		info.wParam  = REQUEST_WPARAM_KILL_FOCUS;

	m_pControlMgr->OnControlRequest(info);

}

IDUIControl* CDUIControlBase::GetFocusControl()
{
	if(m_pControlMgr == NULL) return NULL;

	DUINotify info = {0};
	info.dwMsgType = WM_CONTROL_REQUEST_FOCUS;
	info.pSender = this;
	info.wParam  = REQUEST_WPARAM_GET_FOCUS_CONTROL;
	return (IDUIControl*)m_pControlMgr->OnControlRequest(info);
}

IDUIControl*	CDUIControlBase::GetNextSiblingControl(CRefPtr<IDUIControl> pItem)
{
	INT nCount = m_arChildControl.size();
	INT nIndex = GetControlIndex(pItem);
	if ( nIndex+1>=0 && nIndex+1 <nCount )
		return GetControlByIndex(nIndex+1);

	return NULL;
}

IDUIControl*	CDUIControlBase::GetPrevSiblingControl(CRefPtr<IDUIControl> pItem)
{
	INT nCount = m_arChildControl.size();
	INT nIndex = GetControlIndex(pItem);
	return GetControlByIndex(nIndex-1);
}

BOOL CDUIControlBase::GetFocus() const
{
	return (m_dwStatus & CONTROL_STATUS_FOCUS);
}

BOOL CDUIControlBase::IsVisible(BOOL bCheckParent) const
{
	if (!(m_dwStyle & CONTROL_STYLE_VISIBLE))
	{
		return FALSE;
	}
	else
	{
		if(m_pParentControl != NULL && bCheckParent)
		{
			return m_pParentControl->IsVisible(bCheckParent);
		}
		return TRUE;
	}
}

BOOL CDUIControlBase::IsEnabled() const 
{
	return !(m_dwStatus & CONTROL_STATUS_DISABLE);
}

VOID CDUIControlBase::SetText(const CDUIString& strText)
{
	if(m_strText.compare(strText) != 0)
	{
		m_strText = strText;
		UpdateLayout(FALSE);
	}
}

const CDUIString& CDUIControlBase::GetText()
{
	return m_strText;
}

VOID CDUIControlBase::SetTooltip(const CDUIString& strTooltip)
{
	m_strTooltip = strTooltip;
}

const CDUIString& CDUIControlBase::GetTooltip() const
{
	return m_strTooltip;
}

VOID CDUIControlBase::SetShortcut(TCHAR c)
{
	m_chShorcut = c;
}

TCHAR CDUIControlBase::GetShortcut() const
{
	return m_chShorcut;
}

VOID CDUIControlBase::SetName(const CDUIString& strName)
{
	m_strName = strName;
}

const CDUIString& CDUIControlBase::GetName() const
{
	return m_strName;
}

VOID CDUIControlBase::SetID(DWORD dwID)
{
	m_dwID = dwID;
}

DWORD CDUIControlBase::GetID() const
{
	return m_dwID;
}

VOID CDUIControlBase::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	
	INT nCount = m_arChildControl.size();
	if(nCount > 0)
	{
		RECT rtContent = GetContentRect();
		m_arChildControl[0]->SetControlRect(rtContent);
	}
	
	UpdateLayout(FALSE);
}

RECT CDUIControlBase::GetControlRect()
{
	return m_rtControl;
}

VOID CDUIControlBase::SetTextStyle(CRefPtr<CTextStyle> pTextStyle)
{
	m_pTextStyle = pTextStyle;
}

CRefPtr<CTextStyle> CDUIControlBase::GetTextStyle() 
{
	if(m_pTextStyle.IsNull())
	{
		m_pTextStyle = GetSkinMgr()->GetTextStyle(_T(""));
	}

	return m_pTextStyle;
}

VOID CDUIControlBase::SetCursor(CRefPtr<CCursorObject> pCursor)
{
	m_pCursor = pCursor;
}

CRefPtr<CCursorObject> CDUIControlBase::GetCursor()
{
	return m_pCursor;
}

VOID CDUIControlBase::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	IDUISkinMgr* pSkinMgr = GetSkinMgr();
	if(pSkinMgr == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	BOOL bHandled(FALSE);

	if(strName.compare(_T("position")) == 0)
	{
		TCHAR szTextBuffer[128] = {0};
		lstrcpyn(szTextBuffer, strValue.c_str(), sizeof(szTextBuffer) / sizeof(TCHAR) - 1);

		RECT rtTemp = {0};
		LPTSTR pstr = NULL;
		rtTemp.left = _tcstol(szTextBuffer, &pstr, 10);
		rtTemp.top = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.right = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.bottom = _tcstol(pstr + 1, &pstr, 10);
		
		SetControlRect(rtTemp);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("text")) == 0)
	{
		CDUIString strTemp;
		pSkinMgr->GetStringText(strValue, strTemp);
		SetText(strTemp);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("tooltip")) == 0)
	{
		CDUIString strTemp;
		pSkinMgr->GetStringText(strValue, strTemp);
		SetTooltip(strTemp);
		ModifyStyle(0, CONTROL_STYLE_TOOLTIP);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("shortcut")) == 0)
	{
		DUI_ASSERT(strValue.size() > 0);
		if(strValue.size() > 0)
		{
			m_chShorcut = strValue[0];
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("name")) == 0)
	{
		m_strName = strValue;
		bHandled = TRUE;
	}
	else if(strName.compare(_T("id")) == 0)
	{
		DWORD dwID = _ttol(strValue.c_str());
		SetID(dwID);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("visible")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}
		else
		{
			ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("transparent")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			ModifyStyle(0, CONTROL_STYLE_TRANSPARENT);
		}
		else
		{
			ModifyStyle(CONTROL_STYLE_TRANSPARENT, 0);
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("enable")) == 0)
	{
		if(strValue.compare(DUI_STR_FALSE) == 0)
		{
			ModifyStatus(0, CONTROL_STATUS_DISABLE);
		}
		else
		{
			ModifyStatus(CONTROL_STATUS_DISABLE, 0);
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("tab_stop")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			ModifyStyle(0, CONTROL_STYLE_TABSTOP);
		}
		else
		{
			ModifyStyle(CONTROL_STYLE_TABSTOP, 0);
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("margin")) == 0)
	{
		TCHAR szTextBuffer[128] = {0};
		lstrcpyn(szTextBuffer, strValue.c_str(), sizeof(szTextBuffer) / sizeof(TCHAR) - 1);
		
		RECT rtTemp = {0};
		LPTSTR pstr = NULL;
		rtTemp.left = _tcstol(szTextBuffer, &pstr, 10);
		rtTemp.top = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.right = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.bottom = _tcstol(pstr + 1, &pstr, 10);
		
		SetMargin(rtTemp);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("padding")) == 0)
	{
		INT nTemp = _ttol(strValue.c_str());
		SetPaddingWidth(nTemp);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("width")) == 0)
	{
		INT nTemp = _ttol(strValue.c_str());
		SetFixedWidth(nTemp);
		bHandled = TRUE;
	}	
	else if(strName.compare(_T("height")) == 0)
	{
		INT nTemp = _ttol(strValue.c_str());
		SetFixedHeight(nTemp);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("text_style")) == 0)
	{
		SetTextStyle(pSkinMgr->GetTextStyle(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("cursor")) == 0)
	{
		ModifyStyle(0, CONTROL_STYLE_SETCURSOR);
		SetCursor(pSkinMgr->GetCursor(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("stretch")) == 0)
	{
		bHandled = TRUE;

		if(strValue.find(_T("move_x")) != -1)
		{
			m_dwExStyle |= CONTROL_EX_STYLE_MOVE_X;
		}

		if(strValue.find(_T("move_y")) != -1)
		{
			m_dwExStyle |= CONTROL_EX_STYLE_MOVE_Y;
		}

		if(strValue.find(_T("size_x")) != -1)
		{
			m_dwExStyle |= CONTROL_EX_STYLE_SIZE_X;
		}

		if(strValue.find(_T("size_y")) != -1)
		{
			m_dwExStyle |= CONTROL_EX_STYLE_SIZE_Y;
		}	
		
		if(strValue.find(_T("group")) != -1)
		{
			m_dwExStyle |= CONTROL_EX_STYLE_GROUP;
		}
	}
	else if(strName.compare(_T("dock_style")) == 0)
	{
		bHandled = TRUE;
		if(strValue.find(_T("dock_left")) != -1)
		{
			m_dwExStyle &= ~CONTROL_EX_STYLE_DOCK_MASK;
			m_dwExStyle |= CONTROL_EX_STYLE_DOCK_LEFT;
		}
		else if(strValue.find(_T("dock_right")) != -1)
		{
			m_dwExStyle &= ~CONTROL_EX_STYLE_DOCK_MASK;
			m_dwExStyle |= CONTROL_EX_STYLE_DOCK_RIGHT;
		}
		else if(strValue.find(_T("dock_top")) != -1)
		{
			m_dwExStyle &= ~CONTROL_EX_STYLE_DOCK_MASK;
			m_dwExStyle |= CONTROL_EX_STYLE_DOCK_TOP;
		}
		else if(strValue.find(_T("dock_bottom")) != -1)
		{
			m_dwExStyle &= ~CONTROL_EX_STYLE_DOCK_MASK;
			m_dwExStyle |= CONTROL_EX_STYLE_DOCK_BOTTOM;
		}
	}
	else if(strName.compare(_T("control_style")) == 0)
	{
		bHandled = TRUE;
		ApplyControlStyle(strValue);
	}

	DUI_ASSERT(bHandled);
}

VOID CDUIControlBase::UpdateLayout(BOOL bNeedResize /*= FALSE*/, RECT* pRect/* = NULL*/)
{
	if(m_pControlMgr != NULL)
	{
		DUINotify info = {0};
		info.pSender = this;
		info.wParam = (bNeedResize ? 1 : 0);
		info.lParam = (LPARAM)pRect;
		info.dwMsgType = WM_CONTROL_REQUEST_REDRAW;

		m_pControlMgr->OnControlRequest(info);
	}
}

VOID CDUIControlBase::ModifyStatus(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwNewStatus = (m_dwStatus & ~dwRemove) | dwAdd;
	if(m_dwStatus != dwNewStatus)
	{
		m_dwStatus = dwNewStatus;
		
		UpdateLayout(FALSE);
	}
}

DWORD CDUIControlBase::GetStatus() const
{
	return m_dwStatus;
}

VOID CDUIControlBase::ModifyStyle(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwNewStyle = (m_dwStyle & ~dwRemove) | dwAdd;
	if(m_dwStyle != dwNewStyle)
	{
		BOOL bLayout = ((m_dwStyle^dwNewStyle)&CONTROL_STYLE_VISIBLE);

		m_dwStyle = dwNewStyle;
		
		UpdateLayout(bLayout);
	}
}

DWORD CDUIControlBase::GetStyle() const
{
	return m_dwStyle;
}

VOID CDUIControlBase::ModifyExStyle(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwNewStyle = (m_dwStyle & ~dwRemove) | dwAdd;
	if(m_dwExStyle != dwNewStyle)
	{
		m_dwExStyle = dwNewStyle;
		
		UpdateLayout(FALSE);
	}
}

DWORD CDUIControlBase::GetExStyle() const
{
	return m_dwExStyle;
}

BOOL CDUIControlBase::EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet)
{
	sizeRet = m_sizeFixed;
	return TRUE;
}

VOID CDUIControlBase::PaintBkgnd(HDC dc)
{

}


VOID CDUIControlBase::PaintForgnd(HDC dc)
{

}

VOID CDUIControlBase::PaintText(HDC dc)
{

}

VOID CDUIControlBase::PaintBorder(HDC dc)
{

}

VOID CDUIControlBase::DoPaint(HDC dc, const RECT& rtDraw)
{
	PaintBkgnd(dc);
	PaintForgnd(dc);
	PaintText(dc);
    PaintBorder(dc);

	RECT rtTemp = {0};
	//draw child control
	if(m_arChildControl.size() > 0)
	{
		CDUIRenderClip clip(dc, GetContentRect());

		CControlVector::iterator itr=m_arChildControl.begin();
		for(itr;itr != m_arChildControl.end(); ++itr)
		{
			BOOL bVisible = ((*itr)->IsVisible());
			RECT rtControl = (*itr)->GetControlRect();

			if(bVisible && IntersectRect(&rtTemp, &rtDraw, &rtControl))
			{
				(*itr)->DoPaint(dc, rtDraw);
			}
		}
	}
	
	IDUIScrollBar* pScrollBar = GetHorizontalSB();
	if(pScrollBar != NULL 
		&& (pScrollBar->IsVisible()))
	{
		CDUIRenderClip clip(dc, GetContentRect());
		pScrollBar->DoPaint(dc, rtDraw);
	}

	pScrollBar = GetVerticalSB();
	if(pScrollBar != NULL 
		&& (pScrollBar->IsVisible()))
	{
		CDUIRenderClip clip(dc, GetContentRect());
		pScrollBar->DoPaint(dc, rtDraw);
	}
}

BOOL CDUIControlBase::SetTimer(UINT nTimerID, UINT nElapse)
{
	if(m_pControlMgr == NULL) return FALSE;

	DUITimerInfo timer = {0};
	timer.pControl = this;
	timer.nTimerID = nTimerID;
	timer.nElapse = nElapse;
	
	DUINotify info = {0};
	info.pSender = this;
	info.wParam = WPARAM_SET_TIMER;
	info.lParam = (LPARAM)&timer;
	info.dwMsgType = WM_CONTROL_REQUEST_TIMER;

	LRESULT lRet = m_pControlMgr->OnControlRequest(info);

	return (lRet == S_OK);
}

VOID CDUIControlBase::KillTimer(UINT nTimerID)
{
	if(m_pControlMgr == NULL) return;
	
	DUITimerInfo timer = {0};
	timer.pControl = this;
	timer.nTimerID = nTimerID;
	
	DUINotify info = {0};
	info.pSender = this;
	info.wParam = WPARAM_KILL_TIMER;
	info.lParam = (LPARAM)&timer;
	info.dwMsgType = WM_CONTROL_REQUEST_TIMER;
	
	m_pControlMgr->OnControlRequest(info);
}


BOOL CDUIControlBase::AddControl(CRefPtr<IDUIControl> pItem)
{
	if(!BeforeAddControl(pItem)) return FALSE;
	m_arChildControl.push_back(pItem);
	AfterAddControl(pItem);

	return TRUE;
}

BOOL CDUIControlBase::RemoveControl(CRefPtr<IDUIControl> pItem)
{
	BeforeRemoveControl(pItem);
	m_arChildControl.erase(std::remove(m_arChildControl.begin(), m_arChildControl.end(), pItem), m_arChildControl.end());
	AfterRemoveControl(pItem);
	
	return TRUE;
}	

INT CDUIControlBase::GetControlCount() const
{
	return m_arChildControl.size();
}

IDUIControl* CDUIControlBase::GetControlByIndex(INT nIndex) const
{
	DUI_ASSERT(nIndex >= 0 && nIndex < GetControlCount());
	if(nIndex >= 0 && nIndex < GetControlCount())
		return m_arChildControl[nIndex].GetPtr();
	else
		return NULL;
}

VOID CDUIControlBase::RemoveAllControl()
{
	if(m_pControlMgr != NULL)
	{
		CControlVector::iterator itr = m_arChildControl.begin();
		for(itr; itr != m_arChildControl.end(); ++itr)
		{
			BeforeRemoveControl(*itr);
			AfterRemoveControl(*itr);
		}
	}
	m_arChildControl.clear();
}

BOOL CDUIControlBase::AddControlAt(INT nIndex, CRefPtr<IDUIControl> pItem)
{
	if(!BeforeAddControl(pItem)) return FALSE;
	
	DUI_ASSERT(nIndex >= 0 && nIndex < GetControlCount());
	if(nIndex < 0 || nIndex >= GetControlCount())
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	m_arChildControl.insert(m_arChildControl.begin()+nIndex, pItem);
	AfterAddControl(pItem);

	return TRUE;
}

BOOL CDUIControlBase::RemoveControlAt(INT nIndex)
{
	DUI_ASSERT(nIndex >= 0 && nIndex < GetControlCount());
	if(nIndex < 0 || nIndex >= GetControlCount())
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	CRefPtr<IDUIControl> pItem = m_arChildControl[nIndex];
	BeforeRemoveControl(pItem);
	m_arChildControl.erase(m_arChildControl.begin()+nIndex);
	AfterRemoveControl(pItem);

	return TRUE;
}

INT CDUIControlBase::GetControlIndex(CRefPtr<IDUIControl> pControl)
{
	INT nCount = GetControlCount();
	for(INT i=0; i<nCount; ++i)
	{
		if(m_arChildControl[i] == pControl)
		{
			return i;
		}
	}

	return -1;
}

BOOL CDUIControlBase::SetControlIndex(CRefPtr<IDUIControl> pControl, INT nIndex)
{
	RemoveControl(pControl);
	return AddControlAt(nIndex, pControl);
}

VOID CDUIControlBase::SetMargin(RECT rtMargin)
{
	m_rtMargin = rtMargin;

	UpdateLayout(TRUE);

}

RECT CDUIControlBase::GetMargin()
{
	return m_rtMargin;
}

VOID CDUIControlBase::SubclassControl(IDUIControl* pControl, BOOL bSubclass)
{
	DUI_ASSERT(pControl != NULL);

	if(pControl != NULL)
	{
		CDUIControlBase* pControlBase = (CDUIControlBase*)pControl->GetInterface(IControlBase);
		if(pControlBase != NULL)
		{
			if(bSubclass)
			{
				pControlBase->m_pSubclassControl = this;
			}
			else
			{
				pControlBase->m_pSubclassControl = NULL;
			}
		}
	}
}


VOID CDUIControlBase::SetPaddingWidth(INT nPadding)
{
	if(nPadding != m_nPaddingWidth)
	{
		m_nPaddingWidth = nPadding;
		UpdateLayout(TRUE);
	}
}

INT CDUIControlBase::GetPaddingWidth()
{
	return m_nPaddingWidth;
}

VOID CDUIControlBase::SetFixedWidth(INT nWidth)
{
	if(m_sizeFixed.cx != nWidth)
	{
		m_sizeFixed.cx = nWidth;
		UpdateLayout(TRUE);
	}
}

VOID CDUIControlBase::SetFixedHeight(INT nHeight)
{
	if(m_sizeFixed.cy != nHeight)
	{
		m_sizeFixed.cy = nHeight;
		UpdateLayout(TRUE);
	}
}

SIZE CDUIControlBase::GetFixedSize()
{
	return m_sizeFixed;
}

RECT CDUIControlBase::GetContentRect()
{
	RECT rtTemp = m_rtControl;
	
	rtTemp.left += m_rtMargin.left;
	rtTemp.right -= m_rtMargin.right;
	rtTemp.top += m_rtMargin.top;
	rtTemp.bottom -= m_rtMargin.bottom;
	
	if(	IsRectEmpty(&rtTemp) )
	{
		rtTemp.right = rtTemp.left = m_rtControl.left;
		rtTemp.bottom = rtTemp.top = m_rtControl.top;
	}
	
	return rtTemp;
}

HWND CDUIControlBase::GetManagerWnd(BOOL bTopParent /* = FALSE */)
{
	if(m_pControlMgr == NULL) return NULL;
	
	DUINotify info = {0};
	info.pSender = this;
	info.dwMsgType = WM_CONTROL_REQUEST_GET_HWND;
	info.lParam = 0;
	if(bTopParent)
	{
		info.wParam = REQUEST_WPARAM_HWND_TOP_PARENT;
	}
	else
	{
		info.wParam = REQUEST_WPARAM_HWND_MANAGER;
	}

	
	m_pControlMgr->OnControlRequest(info);	
	
	DUI_ASSERT(info.lParam != NULL);

	return reinterpret_cast<HWND>(info.lParam);	
}

VOID CDUIControlBase::LockLayout(BOOL bLock)
{
	if(m_pControlMgr == NULL) return;
	
	DUINotify info = {0};
	info.pSender = this;
	info.dwMsgType = WM_CONTROL_REQUEST_LOCK_LAYOUT;
	info.lParam = 0;
	info.wParam = bLock;

	m_pControlMgr->OnControlRequest(info);	
}

IDUIControl* CDUIControlBase::GetSiblingControl(const CDUIString& strName)
{
	if(m_pControlMgr == NULL) return NULL;
	
	DUINotify info = {0};
	info.pSender = this;
	info.dwMsgType = WM_CONTROL_REQUEST_GET_CONTROL;
	info.lParam = (WPARAM)strName.c_str();
	info.wParam = REQUEST_WPARAM_CONTROL_BY_NAME;
	
	return (IDUIControl*)m_pControlMgr->OnControlRequest(info);
}

IDUIControl* CDUIControlBase::GetSiblingControl(POINT ptHit)
{
	if(m_pControlMgr == NULL) return NULL;
	
	DUINotify info = {0};
	info.pSender = this;
	info.dwMsgType = WM_CONTROL_REQUEST_GET_CONTROL;
	info.lParam = (LPARAM)&ptHit;
	info.wParam = REQUEST_WPARAM_CONTROL_BY_POINT;
	
	return (IDUIControl*)m_pControlMgr->OnControlRequest(info);
}

DWORD CDUIControlBase::GetManagerWndStatus()
{
	if(m_pControlMgr == NULL) return NULL;
	
	DUINotify info = {0};
	info.pSender = this;
	info.dwMsgType = WM_CONTROL_REQUEST_WIN_STATUS;
	info.lParam = NULL;
	info.wParam = NULL;
	
	m_pControlMgr->OnControlRequest(info);
		
	return info.wParam;

}

VOID CDUIControlBase::OnCreate()
{

}

VOID CDUIControlBase::OnDestroy()
{
	ModifyStatus(CONTROL_STATUS_HOVER, 0);
}

VOID CDUIControlBase::OnTimer(INT nTimerID)
{

}

BOOL CDUIControlBase::SendNotify(UINT nMsgType, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */, IDUIControl* pSender /* = NULL */)
{
	if(pSender == NULL)
	{
		pSender = this;
	}

	DUINotify info = {0};
	info.dwMsgType = nMsgType;
	info.lParam = lParam;
	info.wParam = wParam;
	info.pSender = pSender;
	info.hWndManager = GetManagerWnd(FALSE);

	return SendNotify(info);
}

BOOL CDUIControlBase::SendNotify(const DUINotify& info)
{
	if(m_pControlMgr != NULL)
	{
		m_pControlMgr->SendNotify(info);
		return TRUE;
	}

	return FALSE;
}

IDUIControl* CDUIControlBase::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if((uFlags & FIND_FLAG_VISIBLE)
		&& !(GetStyle() & CONTROL_STYLE_VISIBLE)) return NULL;

	if((uFlags & FIND_FLAG_ENABLED)
		&& (GetStatus() & CONTROL_STATUS_DISABLE)) return NULL;

	if((uFlags & FIND_FLAG_HITTEST)
		&& 
		(!(PtInRect(&m_rtControl, *static_cast<LPPOINT>(pData)))
		|| (GetStyle() & CONTROL_STYLE_TRANSPARENT))) return NULL;

	IDUIControl* pRet = NULL;
	if((uFlags & FIND_FLAG_MEFIRST))
	{
		pRet = Proc(this, pData);
		if(pRet != NULL) return pRet;
	}

	INT nCount = m_arChildControl.size();
	for(int i=0; i<nCount; ++i)
	{
		pRet = m_arChildControl[i]->FindControl(Proc, pData, uFlags);
		if(pRet != NULL) return pRet;
	}

	IDUIScrollBar* pScrollBar = GetHorizontalSB();
	if(pScrollBar != NULL) 
	{
		pRet = pScrollBar->FindControl(Proc, pData, uFlags);
		if(pRet != NULL) return pRet;
	}

	pScrollBar = GetVerticalSB();
	if(pScrollBar != NULL)
	{
		pRet = pScrollBar->FindControl(Proc, pData, uFlags);
		if(pRet != NULL) return pRet;
	}

	return Proc(this, pData);
}

VOID CDUIControlBase::BroadcastEvent(const DUIEvent& info)
{
	BOOL bHandled(FALSE);
	INT nCount = m_arChildControl.size();
	for(INT i=0; i<nCount; ++i)
	{
		m_arChildControl[i]->BroadcastEvent(info);
	}
	
	OnDUIEvent(info, bHandled);
}

IDUIScrollBar* CDUIControlBase::GetVerticalSB()
{
	return NULL;
}

IDUIScrollBar* CDUIControlBase::GetHorizontalSB()
{
	return NULL;
}

INT CDUIControlBase::GetScrollLineStep()
{
	return 10;
}

BOOL CDUIControlBase::ProcessVerticalSB(RECT rt, INT nNeededHeight, INT nExtraLenth /* = 0 */)
{
	BOOL bRet(FALSE);
	IDUIScrollBar* pSB = GetVerticalSB();
	if(pSB == NULL) return bRet;

	INT nRange = nNeededHeight - (rt.bottom - rt.top);
	if(nRange > 0)
	{
		nRange += nExtraLenth;

		SCROLLINFO si = {0};
		if(!(pSB->IsVisible()))
		{
			pSB->ModifyStyle(0, CONTROL_STYLE_VISIBLE);

			si.fMask = SIF_ALL;
			si.nPos = 0;
			si.nMin = 0;
			si.nMax = nRange;
			si.nPage = rt.bottom - rt.top;
			pSB->SetScrollBarInfo(&si);

			UpdateLayout(TRUE);
			bRet = TRUE;
		}
		else
		{
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nMin = 0;
			si.nMax = nRange;
			si.nPage = rt.bottom - rt.top;
			pSB->SetScrollBarInfo(&si);
		}
	}
	else
	{
		if(pSB->IsVisible())
		{
			pSB->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
			
			UpdateLayout(TRUE);
			bRet = TRUE;
		}
	}

	return bRet;
}


BOOL CDUIControlBase::ProcessHorizontalSB(RECT rt, INT nNeededWidth, INT nExtraLenth /* = 0 */)
{
	BOOL bRet(FALSE);
	IDUIScrollBar* pSB = GetHorizontalSB();
	if(pSB == NULL) return bRet;
	
	INT nRange = nNeededWidth - (rt.right - rt.left);
	if(nRange > 0)
	{
		nRange += nExtraLenth;

		SCROLLINFO si = {0};
		if(!(pSB->IsVisible()))
		{
			pSB->ModifyStyle(0, CONTROL_STYLE_VISIBLE);

			si.fMask = SIF_ALL;
			si.nPos = 0;
			si.nMin = 0;
			si.nMax = nRange;
			si.nPage = rt.right - rt.left;
			pSB->SetScrollBarInfo(&si);

			UpdateLayout(TRUE);
			bRet = TRUE;
		}
		else
		{
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nMin = 0;
			si.nMax = nRange;
			si.nPage = rt.right - rt.left;
			pSB->SetScrollBarInfo(&si);
		}
	}
	else
	{
		if(pSB->IsVisible())
		{
			pSB->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
			
			UpdateLayout(TRUE);
			bRet = TRUE;
		}
	}

	return bRet;
}

VOID CDUIControlBase::OnVertialSBEvent(const DUIEvent& info)
{
	IDUIScrollBar* pSB = GetVerticalSB();
	if(pSB == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	SCROLLINFO si = {0};
	pSB->GetScrollBarInfo(&si);

	if(info.wParam == SB_LINEUP)
	{
		si.fMask = SIF_POS;
		si.nPos -= GetScrollLineStep();
	}
	else if(info.wParam == SB_LINEDOWN)
	{
		si.fMask = SIF_POS;
		si.nPos += GetScrollLineStep();
	}
	else if(info.wParam == SB_PAGEUP)
	{
		si.fMask = SIF_POS;
		si.nPos -= (m_rtControl.bottom - m_rtControl.top);
	}
	else if(info.wParam == SB_PAGEDOWN)
	{
		si.fMask = SIF_POS;
		si.nPos += (m_rtControl.bottom - m_rtControl.top);
	}
	else if(info.wParam == SB_THUMBTRACK)
	{
		si.fMask = SIF_POS;
		si.nPos  = info.lParam;
	}

	if(si.fMask == SIF_POS)
	{
		pSB->SetScrollBarInfo(&si);
 		UpdateLayout(TRUE);
	}
}

VOID CDUIControlBase::OnHorizontalSBEvent(const DUIEvent& info)
{
	IDUIScrollBar* pSB = GetHorizontalSB();
	if(pSB == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	SCROLLINFO si = {0};
	pSB->GetScrollBarInfo(&si);

	if(info.wParam == SB_LINELEFT)
	{
		si.fMask = SIF_POS;
		si.nPos -= GetScrollLineStep();
	}
	else if(info.wParam == SB_LINERIGHT)
	{
		si.fMask = SIF_POS;
		si.nPos += GetScrollLineStep();
	}
	else if(info.wParam == SB_PAGELEFT)
	{
		si.fMask = SIF_POS;
		si.nPos -= (m_rtControl.right - m_rtControl.left);
	}
	else if(info.wParam == SB_PAGERIGHT)
	{
		si.fMask = SIF_POS;
		si.nPos += (m_rtControl.right - m_rtControl.left);
	}
	else if(info.wParam == SB_THUMBTRACK)
	{
		si.fMask = SIF_POS;
		si.nPos  = info.lParam;
	}

	if(si.fMask == SIF_POS)
	{
		pSB->SetScrollBarInfo(&si);
 		UpdateLayout(TRUE);
	}
}

VOID CDUIControlBase::OnMouseWheel(const DUIEvent& info)
{
	BOOL bVert = TRUE;
	INT nStep = (INT)(SHORT)HIWORD(info.wParam);
	BOOL bForward = (nStep > 0);
	IDUIScrollBar* pSB = GetVerticalSB();
	if(pSB == NULL || !pSB->IsVisible()) 
	{
		bVert = FALSE;
		pSB = GetHorizontalSB();
	}
	if(pSB == NULL || !pSB->IsVisible()) return;
	
	SCROLLINFO si = {0};
	pSB->GetScrollBarInfo(&si);
	if(bForward)
	{
		si.nPos -= GetScrollLineStep();
	}
	else
	{
		si.nPos += GetScrollLineStep();
	}

	si.fMask = SIF_POS;
	pSB->SetScrollBarInfo(&si);

 	UpdateLayout(TRUE);
}

BOOL CDUIControlBase::BeforeAddControl(CRefPtr<IDUIControl> pItem)
{
	if(pItem.IsNull()) return FALSE;
	
	CControlVector::iterator itr = std::find(m_arChildControl.begin(), m_arChildControl.end(), pItem);
	if(itr != m_arChildControl.end()) return FALSE;

	pItem->SetParent(this);

	return TRUE;
}

VOID CDUIControlBase::AfterAddControl(CRefPtr<IDUIControl> pItem)
{
	if(m_pControlMgr != NULL && !pItem.IsNull())
	{
		pItem->AttachWindow(m_pControlMgr);
		UpdateLayout(TRUE);
	}
}

VOID CDUIControlBase::BeforeRemoveControl(CRefPtr<IDUIControl> pItem)
{
	
}

VOID CDUIControlBase::AfterRemoveControl(CRefPtr<IDUIControl> pItem)
{
	if(m_pControlMgr != NULL && !pItem.IsNull())
	{
		pItem->DetachWindow();
		UpdateLayout(TRUE);

		pItem->SetParent(NULL);
	}
}

BOOL CDUIControlBase::ApplyControlStyle(const CDUIString& strStyleName)
{
	IDUISkinMgr* pSkinMgr = GetSkinMgr();
	if(pSkinMgr == NULL) return FALSE;

	CDUIXml xml;
	CDUIString strXML;

	BOOL bFindStyle = pSkinMgr->GetControlStyle(strStyleName, strXML);
	if(!bFindStyle)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	
	if(!xml.SetDoc(strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	if(!xml.FindElem(_T("control_style")))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	CDUIString strAttrName, strAttrValue;
	INT nIndex(0);
	do 
	{
		strAttrName = xml.GetAttribName(nIndex++);
		if(strAttrName.size() <= 0) break;
		
		if(strAttrName.compare(_T("name")) == 0) continue;
		if(strAttrName.compare(_T("control_style")) == 0) continue;

		strAttrValue = xml.GetAttrib(strAttrName);
		SetAttribute(strAttrName, strAttrValue);
		

	} while(TRUE);


	return TRUE;
}
///////////////////  CDUIControlBase  //////////////////////////





///////////////////  CDUIAnimationControl  //////////////////////////
CDUIAnimationControl::CDUIAnimationControl()
:CDUIAnimation(this)
{

}

CDUIAnimationControl::CDUIAnimationControl(const CDUIAnimationControl& object)
:CDUIAnimation(this), CDUIControlBase(object)
{

}

CRefPtr<IDUIControl> CDUIAnimationControl::Clone()
{
	return new CDUIAnimationControl(*this);
}

VOID CDUIAnimationControl::OnTimer(INT nTimerID)
{
	OnAnimationElapse(nTimerID);
}

VOID CDUIAnimationControl::OnDestroy()
{
	StopAnimation();
}
///////////////////  CDUIAnimationControl  //////////////////////////

DUI_END_NAMESPCE