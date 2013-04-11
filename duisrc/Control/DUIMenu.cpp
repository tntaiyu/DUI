#include "DUIMenu.h"
#include "DUIRenderEngine.h"
#include "DUIPopupWindow.h"

#if   defined (_MSC_VER)  && (_MSC_VER <1300) 
	#include <multimon.h>    // DUIPopupMenuWindow.cpp have defined the STUB
#else
	#if WINVER < 0x0500
	#define COMPILE_MULTIMON_STUBS
	#include <multimon.h>
	#endif // WINVER < 0x0500
#endif

#define  DUI_MENU_PROP_FALG  _T("dui_menu_flag")
#define  DUI_MENU_PROP_VALUE 314

#define MENU_TEXT_HOTKEY_GAP 10 //gap between "abc Ctrl+A"
#define DUI_MENU_CHILD_GAP 2

DUI_BGN_NAMESPCE

BOOL IsDUIMenuWnd(HWND hWnd)
{
	if(hWnd == NULL || !IsWindow(hWnd)) return FALSE;

	TCHAR szClassName[32] = {0};
	GetClassName(hWnd, szClassName, sizeof(szClassName) / sizeof(TCHAR) - 1);
	if(lstrcmpi(szClassName, DUI_WND_CLASS) == 0)
	{
		HANDLE hProp = GetProp(hWnd, DUI_MENU_PROP_FALG);
		if(hProp == (HANDLE)DUI_MENU_PROP_VALUE)
		{
			return TRUE;
		}
	}

	return FALSE;
}

CRefPtr<IDUIMenuItem> g_pMenuItemClicked = NULL;

class CDUIMenuWnd: public CDUIPopupWindow
{
public:
	typedef CDUIPopupWindow theBase;
	CDUIMenuWnd(CDUIMenu*);

	BEGIN_MSG_MAP(CDUIMenuWnd)
		CHAIN_MSG_MAP(theBase)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_MSG_MAP()

	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnKillFocus(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CDUIMenu* m_pMenu;
};

CDUIMenuWnd::CDUIMenuWnd(CDUIMenu* pMenu)
{
	DUI_ASSERT(pMenu != NULL);
	m_pMenu = pMenu;
}

LRESULT CDUIMenuWnd::OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;
	

	::SetProp(m_hWnd, DUI_MENU_PROP_FALG, (HANDLE)DUI_MENU_PROP_VALUE);


	return S_OK;
}


LRESULT CDUIMenuWnd::OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;

	::RemoveProp(m_hWnd, DUI_MENU_PROP_FALG);
	
	//for ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0); in ShowMenuWnd()
	HWND hWndOwner = ::GetWindow(m_hWnd, GW_OWNER);
	if(hWndOwner != NULL
		&& GetActiveWindow() != hWndOwner
		&& !IsDUIMenuWnd(hWndOwner))
	{
		::SendMessage(hWndOwner, WM_NCACTIVATE, FALSE, 0);
	}

	return S_OK;
}

LRESULT CDUIMenuWnd::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;

	if(m_pMenu != NULL)
	{
		m_pMenu->SetFocus(TRUE);
	}

	return S_OK;
}

LRESULT CDUIMenuWnd::OnKillFocus(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	HWND hWnd = (HWND)wParam;
	if(!IsDUIMenuWnd(hWnd))
	{
		if(m_pMenu != NULL)
		{
			m_pMenu->CancelMenu(TRUE);
		}
	}
	
	return S_OK;
}

////////////////////  CDUIMenuItem  /////////////////////////
CDUIMenuItem::CDUIMenuItem()
{
	SetFixedHeight(30);
	m_dwUserData = 0;
	m_bSelect = FALSE;
	m_dwMenuStyle = 0;
	SetPaddingWidth(5);
}

LPVOID CDUIMenuItem::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IMenuItem) == 0)
	{
		return (IDUIMenuItem*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIMenuItem::Clone()
{
	return new CDUIMenuItem(*this);
}

VOID CDUIMenuItem::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	
	UpdateLayout(FALSE);
}	

IDUIControl* CDUIMenuItem::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
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

	return Proc(this, pData);
}

LRESULT CDUIMenuItem::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);
	switch(info.dwEventType)
	{
	case DUI_EVENT_MOUSEENTER:
		ModifyStatus(0, CONTROL_STATUS_HOVER);
		if(m_pParentControl != NULL)
		{
			DUIEvent event = info;
			event.pSender = this;
			event.dwEventType = DUI_EVENT_ELE_ITEM_MOUSEENTER;
			m_pParentControl->OnDUIEvent(event, bHandled);
		}

		break;
		
	case DUI_EVENT_MOUSELEAVE:
		ModifyStatus(CONTROL_STATUS_HOVER, 0);
		if(m_pParentControl != NULL)
		{
			DUIEvent event = info;
			event.pSender = this;
			event.dwEventType = DUI_EVENT_ELE_ITEM_MOUSELEAVE;
			m_pParentControl->OnDUIEvent(event, bHandled);
		}
		break;
	
		
	case DUI_EVENT_LBUTTONUP:
		ModifyStatus(CONTROL_STATUS_PRESSED, 0);
		if(PtInRect(&m_rtControl, info.ptMouse))
		{
			if(m_pParentControl != NULL)
			{
				DUIEvent event = info;
				event.pSender = this;
				event.dwEventType = DUI_EVENT_ELE_ITEM_CLICK;
				m_pParentControl->OnDUIEvent(event, bHandled);
			}
		}
		break;

	
	default:
		break;
	}

	if(bHandled) return lRet;

	return theBase::ProcessEvent(info, bHandled);
}

DWORD CDUIMenuItem::ParseMenuItemStyle(const CDUIString& strStyle)
{
	DWORD dwStyle(MF_STRING);
	if(strStyle.find(_T("MF_STRING")) != -1)
	{
		dwStyle |= MF_STRING;
	}

	if(strStyle.find(_T("MF_SEPARATOR")) != -1)
	{
		dwStyle &= ~MF_STRING;
		dwStyle |= MF_SEPARATOR;
	}

	if(strStyle.find(_T("MF_POPUP")) != -1)
	{
		dwStyle |= MF_POPUP;
	}

	if(strStyle.find(_T("MF_CHECKED")) != -1)
	{
		dwStyle |= MF_CHECKED;
	}

	if(strStyle.find(_T("MF_GRAYED")) != -1)
	{
		dwStyle |= MF_GRAYED;
	}

	if(strStyle.find(_T("MF_DISABLED")) != -1)
	{
		dwStyle |= MF_DISABLED;
	}

	return dwStyle;
}

VOID CDUIMenuItem::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	
	if(strName.compare(_T("select_frame")) == 0)
	{
		SetSelectFrameBK(GetSkinMgr()->GetFrameBK(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("menu_item_style")) == 0)
	{
		SetMenuItemStyle(ParseMenuItemStyle(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("icon")) == 0)
	{
		SetIcon(GetSkinMgr()->GetImage(strValue));
		bHandled = TRUE;
	}


	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUIMenuItem::DoPaint(HDC dc, const RECT& rtDraw)
{
	PaintBkgnd(dc);
	PaintForgnd(dc);
	PaintText(dc);
    PaintBorder(dc);

	//do not need to draw the child control, they are pop up in a single menu window
	//...
}

VOID CDUIMenuItem::SetMenuItemStyle(DWORD dwStyle) 
{
	m_dwMenuStyle = dwStyle;
}

DWORD CDUIMenuItem::GetMenuItemStyle() const 
{
	return m_dwMenuStyle;
}

VOID CDUIMenuItem::SetIcon(CRefPtr<CImageList> pIcon) 
{
	m_pIcon = pIcon;
}

CRefPtr<CImageList> CDUIMenuItem::GetIcon() const
{
	return m_pIcon;
}

VOID CDUIMenuItem::SetSelect(BOOL bSelect) 
{
	m_bSelect = bSelect;
}

BOOL CDUIMenuItem::GetSelect() const 
{
	return m_bSelect;
}

VOID CDUIMenuItem::SetSelectFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) 
{
	m_pSelectFrameBK = pFrameBK;	
}

CRefPtr<CDUIFrameBK> CDUIMenuItem::GetSelectFrameBK() const 
{
	return m_pSelectFrameBK;
}

VOID CDUIMenuItem::SetItemData(DWORD dwData) 
{
	m_dwUserData = dwData;
}

DWORD CDUIMenuItem::GetItemData() const
{
	return m_dwUserData;
}

IDUIMenu* CDUIMenuItem::GetChildMenu() const
{
	if(GetMenuItemStyle()&MF_POPUP)
	{
		if(GetControlCount() > 0)
		{
			DUI_ASSERT(GetControlCount() <= 1);
			IDUIMenu* pMenu = (IDUIMenu*)GetControlByIndex(0)->GetInterface(IMenu);
			DUI_ASSERT(pMenu != NULL);
			return pMenu;
		}
	}

	return NULL;
}

VOID CDUIMenuItem::SetText(const CDUIString& strText)
{
	theBase::SetText(stringReplace(strText, _T("\\t"), _T("\t")));
}

INT CDUIMenuItem::GetIdealWidth()
{
	INT nRet(0);
	HFONT hFont = NULL;
	IDUIMenu* pMenu = GetMenuOwner();
	if(pMenu != NULL)
	{
		nRet += pMenu->GetIconRectWidth();
		nRet += pMenu->GetArrowRectWidth();
		nRet += GetPaddingWidth() * 2;

		CRefPtr<CTextStyle> pTextStyle = pMenu->GetTextStyle();
		if(!pTextStyle.IsNull())
		{
			CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
			if(!pFont.IsNull())
			{
				hFont = pFont->GetFont();
			}
		}
	}

	if(m_strText.size() > 0 && hFont != NULL)
	{
		BOOL bHotKey = (m_strText.find(_T('\t'), 0) != -1);
		if(bHotKey) nRet += MENU_TEXT_HOTKEY_GAP;
		SIZE sizeText = CDUIRenderEngine::GetTextIdealSize(m_strText, hFont, 0, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);
		nRet += sizeText.cx;
	}

	return nRet;
}

RECT CDUIMenuItem::GetBKRect()
{
	return m_rtControl;
}

RECT CDUIMenuItem::GetIconRect()
{
	RECT rtRet = {0, 0, 0, 0};
	IDUIMenu* pMenu = GetMenuOwner();
	if(pMenu)
	{
		RECT rtBK = GetBKRect();
		INT nIconWidth = pMenu->GetIconRectWidth();
		rtBK.right = rtBK.left + nIconWidth;
		rtRet = rtBK;
	}
	return rtRet;
}

RECT CDUIMenuItem::GetTextRect()
{
	RECT rtIcon = GetIconRect();
	RECT rtArrow = GetArrowRect();
	RECT rtBK = GetBKRect();
	rtBK.left = rtIcon.right;
	rtBK.right = rtArrow.left;
	rtBK.left += GetPaddingWidth();
	rtBK.right -= GetPaddingWidth();
	if(IsRectEmpty(&rtBK)) 
	{
		rtBK.right = rtBK.left;
	}
	return rtBK;
}

RECT CDUIMenuItem::GetArrowRect()
{
	RECT rtRet = {0, 0, 0, 0};
	IDUIMenu* pMenu = GetMenuOwner();
	if(pMenu)
	{
		RECT rtBK = GetBKRect();
		INT nArrowWidth = pMenu->GetArrowRectWidth();
		rtBK.left = rtBK.right - nArrowWidth;	
		rtRet = rtBK;
	}
	return rtRet;
}


BOOL CDUIMenuItem::IsMenuItemEnable()
{
	if(!IsEnabled()) return FALSE;
	if((m_dwMenuStyle & MF_GRAYED)
		|| (m_dwMenuStyle & MF_DISABLED)
		|| (m_dwMenuStyle & MF_SEPARATOR))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDUIMenuItem::BeforeAddControl(CRefPtr<IDUIControl> pItem)
{
	if(!theBase::BeforeAddControl(pItem)) return FALSE;
	IDUIMenuItem* pMenuItem = (IDUIMenuItem*)pItem->GetInterface(IMenu);
	DUI_ASSERT(pMenuItem != NULL);
	if(pMenuItem == NULL) return FALSE;

	return TRUE;	
}

VOID CDUIMenuItem::PaintBkgnd(HDC dc)
{
	if(m_bSelect)
	{
		if(!m_pSelectFrameBK.IsNull()
			&& IsEnabled())
		{
			RECT rtBK = GetBKRect();
			m_pSelectFrameBK->DrawBK(dc, rtBK);
		}
	}
}

VOID CDUIMenuItem::PaintForgnd(HDC dc)
{
	BOOL bEnabled = IsMenuItemEnable();
	BOOL bChecked = (m_dwMenuStyle&MF_CHECKED);
	INT nIndex = MENU_ICON_NORMAL;
	//draw icon
	if(!m_pIcon.IsNull()
		&& !m_pIcon->IsNull())
	{
		RECT rtIcon = GetIconRect();

		if(!bEnabled) nIndex = MENU_ICON_DISABLE;
		else if(bChecked) nIndex = MENU_ICON_CHECK;
		else if(m_bSelect) nIndex = MENU_ICON_SELECT;

		if(nIndex >= m_pIcon->GetItemCount())
		{
			nIndex = 0;
		}

		m_pIcon->DrawInCenter(dc, rtIcon, nIndex);
	}


	//draw arrow image for pop up menu
	CRefPtr<CImageList> pArrowImage = NULL;
	if(m_dwMenuStyle&MF_POPUP)
	{
		IDUIMenu* pMenu = GetMenuOwner();
		if(pMenu != NULL)
		{
			pArrowImage = pMenu->GetArrowImage();
		}
	}
	if(!pArrowImage.IsNull()
		&& !pArrowImage->IsNull())
	{
		nIndex = MENU_ICON_NORMAL;
		if(!bEnabled) nIndex = MENU_ICON_DISABLE;
		if(m_bSelect) nIndex = MENU_ICON_SELECT;


		RECT rtArrow = GetArrowRect();
		if(nIndex >= pArrowImage->GetItemCount())
		{
			nIndex = 0;
		}

		pArrowImage->DrawInCenter(dc, rtArrow, nIndex);
	}
}

VOID CDUIMenuItem::PaintText(HDC dc)
{
	CRefPtr<CTextStyle> pTextStyle = NULL;
	CRefPtr<CFontObject> pFont = NULL;
	IDUIMenu* pMenu = GetMenuOwner();
	if(pMenu != NULL)
	{
		pTextStyle = pMenu->GetTextStyle();
		if(!pTextStyle.IsNull())
		{
			pFont = pTextStyle->GetFont();
		}
	}

	if(pFont.IsNull()
		|| pFont->IsNull())
	{
		DUI_ASSERT(FALSE);
		return;
	}

	RECT rtText = GetTextRect();
	if(IsRectEmpty(&rtText)) return;
	BOOL bSeperator = (m_dwMenuStyle & MF_SEPARATOR);
	if(!bSeperator)
	{
		if(m_strText.size() > 0)
		{
			TCHAR szText[256] = {0};
			lstrcpyn(szText, m_strText.c_str(), sizeof(szText) / sizeof(TCHAR) - 1);
			LPCTSTR lpszText = szText;
			int nTabPos = -1;
			int nLen = lstrlen(lpszText);
			for(int i=0; i<nLen; ++i)
			{
				if(lpszText[i] == _T('\t'))
				{
					nTabPos = i;
					break;
				}
			}

			BOOL bEnabled = IsMenuItemEnable();
			CARGB clr = pTextStyle->GetNormalColor();
			if(!bEnabled)
			{	
				clr = pTextStyle->GetDisableColor();	
			}
			if(m_bSelect)
			{
				clr = pTextStyle->GetSelectColor();
			}

			::SetTextColor(dc, clr.GetColor());
			::SetBkMode(dc, TRANSPARENT);
			::SelectObject(dc, pFont->GetFont());
			
			if(nTabPos == -1)
			{
				CDUIRenderEngine::DrawText(dc, lpszText, nLen, &rtText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				CDUIRenderEngine::DrawText(dc, lpszText, nTabPos + 1, &rtText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				CDUIRenderEngine::DrawText(dc, lpszText + nTabPos + 1, (nLen - nTabPos - 1), &rtText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
			}		
		}
	}
	else
	{
		RECT rtSep = rtText;
		RECT rtArrow = GetArrowRect();
		rtSep.right = rtArrow.right - 5;
		CRefPtr<CImageList> pSepLine = pMenu->GetSeparateLine();
		if(!pSepLine.IsNull()
			&& !pSepLine->IsNull())
		{
			pSepLine->HorizontalStretch(dc, rtSep, 0);
		}	
	}
}

IDUIMenu* CDUIMenuItem::GetMenuOwner()
{
	if(m_pParentControl != NULL)
	{
		return (IDUIMenu*)m_pParentControl->GetInterface(IMenu);
	}

	return NULL;
}


////////////////////  CDUIMenuItem  /////////////////////////


////////////////////  CDUIMenu  /////////////////////////
CDUIMenu::CDUIMenu()
{
	m_nIconWidth = 30;
	m_nArrowWidth = 30;

	m_pMenuWnd = new CDUIMenuWnd(this);

	m_dwStyle |= CONTROL_STYLE_TABSTOP;

	m_bExitLoop = TRUE;
	m_nSelectIndex = -1;
}

CDUIMenu::CDUIMenu(const CDUIMenu& menu)
:IDUIMenu(menu)
{
	m_nArrowWidth = menu.m_nArrowWidth;
	m_nIconWidth = menu.m_nIconWidth;
	m_pMenuWnd = new CDUIMenuWnd(this);

	m_pArrowImage = menu.m_pArrowImage;
	m_pSeparateLine = menu.m_pSeparateLine;
	m_pIconFrameBK = menu.m_pIconFrameBK;
	m_pMenuWndFrameBK = menu.m_pMenuWndFrameBK;

	m_bExitLoop = TRUE;
	m_nSelectIndex = -1;
}

CDUIMenu::~CDUIMenu()
{
	if(m_pMenuWnd != NULL)
	{
		delete m_pMenuWnd;
		m_pMenuWnd = NULL;
	}
}

LPVOID CDUIMenu::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IMenu) == 0)
	{
		return (IDUIMenu*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIMenu::Clone()
{
	return new CDUIMenu(*this);
}
	
LRESULT CDUIMenu::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);

	switch(info.dwEventType)
	{
	case DUI_EVENT_MENU_CANCEL:
		if(m_pMenuWnd->IsWindow())
		{
			m_pMenuWnd->SendMessage(WM_CLOSE);
			bHandled  = TRUE;

			m_bExitLoop = TRUE;
			SetSelectIndex(-1);
		}
		break;

	case DUI_EVENT_ELE_ITEM_MOUSEENTER:
	case DUI_EVENT_ELE_ITEM_MOUSELEAVE:
		OnMouseHover(info);
		bHandled = TRUE;
		break;

	case DUI_EVENT_ELE_ITEM_CLICK:
		OnMouseClick(info);
		bHandled = TRUE;
		break;
	
	case DUI_EVENT_KEYDOWN:
		OnKeyDown(info);	
		break;

	case DUI_EVENT_CHAR:
		OnChar(info);
		break;

	default:
		break;
	}
	
	if(bHandled) return lRet;

	return theBase::ProcessEvent(info, bHandled);
}

VOID CDUIMenu::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);

	if(strName.compare(_T("separate_line")) == 0)
	{
		SetSeparateLine(GetSkinMgr()->GetImage(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("icon_frame")) == 0)
	{
		SetIconFrameBK(GetSkinMgr()->GetFrameBK(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("menu_frame")) == 0)
	{
		SetMenuWndFrameBK(GetSkinMgr()->GetFrameBK(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("arrow_image")) == 0)
	{
		SetArrowImage(GetSkinMgr()->GetImage(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("icon_width")) == 0)
	{
		SetIconRectWidth(_ttol(strValue.c_str()));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("arrow_width")) == 0)
	{
		SetArrowRectWidth(_ttol(strValue.c_str()));
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

SIZE CDUIMenu::GetMenuWndSize()
{
	SIZE sizeMenuWnd = GetMenuControlSize();
	if(m_pMenuWnd == NULL) return sizeMenuWnd;

	RECT rtFrame = {0};
	CRefPtr<CDUIFrameBK> pFrameBK = m_pMenuWnd->GetFrameBkgnd();
	if(!pFrameBK.IsNull())
	{
		rtFrame = pFrameBK->GetFrameBorder();
	}
	RECT rtClientMargin = m_pMenuWnd->GetClietnRectMargin();	
	INT nExtrWidth = rtFrame.left + rtFrame.right + rtClientMargin.left + rtClientMargin.right;
	INT nExtrHeight = rtFrame.top + rtFrame.bottom + rtClientMargin.top + rtClientMargin.bottom;
	INT nMenuWidth = sizeMenuWnd.cx + nExtrWidth;
	INT nMenuHeight = sizeMenuWnd.cy + nExtrHeight;

	sizeMenuWnd.cx += nExtrWidth;
	sizeMenuWnd.cy += nExtrHeight;

	return sizeMenuWnd;
}

RECT CDUIMenu::GetIdealMenuWndRect(HWND hWndParent, SIZE sizeMenuWnd, POINT ptRight, POINT ptLeft, BOOL bUseFirst)
{
	MONITORINFO oMonitor = {0};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWndParent, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	RECT rtWork = oMonitor.rcWork;	
	
	BOOL bRight = TRUE;
	BOOL bDown = TRUE;

	INT nX = ptRight.x;
	INT nY = ptRight.y;
	RECT rtRect = {nX, nY, nX + sizeMenuWnd.cx, nY + sizeMenuWnd.cy};
	
	if(nX + sizeMenuWnd.cx > rtWork.right)
	{
		bRight = FALSE;
		if(!bUseFirst)
		{
			nX = ptLeft.x;
			nY = ptLeft.y;
		}
	}
	if(nY + sizeMenuWnd.cy > rtWork.bottom)
	{
		bDown = FALSE;
	}

	if(!bRight && bUseFirst)
	{
		if(nX - sizeMenuWnd.cx < rtWork.left)
		{
			bRight = TRUE;
			nX = rtWork.right - sizeMenuWnd.cx;
		}
	}

	if(!bDown)
	{
		if(nY - sizeMenuWnd.cy < rtWork.top)
		{
			bDown = TRUE;
			nY = rtWork.bottom - sizeMenuWnd.cy;
		}
	}

	if(bRight)
	{
		rtRect.left = nX;
		rtRect.right = nX + sizeMenuWnd.cx;
	}
	else
	{
		rtRect.right = nX;
		rtRect.left = nX - sizeMenuWnd.cx;
	}

	if(bDown)
	{
		rtRect.top = nY;
		rtRect.bottom = nY + sizeMenuWnd.cy;
	}
	else
	{
		rtRect.bottom = nY;
		rtRect.top = nY - sizeMenuWnd.cy;
	}

	return rtRect;
}


//bUseFirt (true ==> for first pop up menu) (false ==> for child pop up menu)
//ptRight ==> at right side of menu item
//ptLeft ==> left side
BOOL CDUIMenu::ShowMenuWnd(IDUIControl* pOwner, POINT ptRight, POINT ptLeft, BOOL bUseFirt/* = TRUE*/)
{
	if(IsMemuWndPopup())
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	CRefPtr<CDUIFrameBK> pMenuWndFrameBK = GetDefaultMenuWndFrameBK();
	if(pMenuWndFrameBK.IsNull()) return 0;

	m_pMenuWnd->SetFrameBkgnd(pMenuWndFrameBK);
	RECT rtMargin = {1, 1, 1, 1};
	m_pMenuWnd->SetClientRectMargin(rtMargin);

	//set Region
	INT nRoundX(0),nRoundY(0);
	pMenuWndFrameBK->GetFrameRound(nRoundX, nRoundY);
	CRefPtr<CImageList> pBorderImage = pMenuWndFrameBK->GetFrameImage();
	if((!pBorderImage.IsNull() && !pBorderImage->IsNull())
		||(nRoundX > 0 && nRoundY > 0))
	{
		m_pMenuWnd->SetDUIStyle(0, DUI_WS_CUSTOM_NC | DUI_WS_NC_RGN);
	}
	//

	HWND hWndParent = NULL;
	CDUIControlBase* pControlOwner = (CDUIControlBase*)pOwner->GetInterface(IControlBase);
	DUI_ASSERT(pControlOwner != NULL);
	if(pControlOwner != NULL) 
	{
		hWndParent = pControlOwner->GetManagerWnd(FALSE);
	}
	if(hWndParent == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	SIZE sizeMenuWnd = GetMenuWndSize();
	RECT rtMenuWnd = GetIdealMenuWndRect(hWndParent, sizeMenuWnd, ptRight, ptLeft, bUseFirt);

	if(GetCapture() != NULL) ::ReleaseCapture();

	m_pMenuWnd->CreateFromRootControl((IDUIControl*)GetInterface(IControl),
		hWndParent /*NULL*/, rtMenuWnd, NULL, WS_POPUP | WS_VISIBLE,  WS_EX_TOPMOST/* | WS_EX_TOOLWINDOW  | WS_EX_NOACTIVATE*/);

	if(!m_pMenuWnd->IsWindow())
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	//make parent window look like active
	if(!IsDUIMenuWnd(hWndParent))
	{
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0);
	}	
	//

	m_pMenuWnd->ShowWindow(SW_SHOWNOACTIVATE);
	m_pMenuWnd->UpdateWindow();

	return TRUE;
}


VOID CDUIMenu::RunMenuMsgLoop()
{
	MSG msg = {0};

	m_bExitLoop = FALSE;

	while(!m_bExitLoop)
	{
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				PostQuitMessage(0);
				break;
			}
		}

		Sleep(1);
	}
}

CRefPtr<IDUIMenuItem> CDUIMenu::TrackPopupMenu(IDUIControl* pOwner, INT nX, INT nY)
{
	if(m_pMenuWnd == NULL 
		|| m_pMenuWnd->IsWindow()
		|| pOwner == NULL)
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}

	POINT pt = {nX, nY};
	if(!ShowMenuWnd(pOwner, pt, pt, TRUE))
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}
	
	g_pMenuItemClicked = NULL;

	RunMenuMsgLoop();

	CRefPtr<IDUIMenuItem> pRet = g_pMenuItemClicked;
	g_pMenuItemClicked = NULL;

	return pRet;
}

VOID CDUIMenu::SetIconRectWidth(INT nWidth)
{
	m_nIconWidth = nWidth;
}

INT CDUIMenu::GetIconRectWidth()
{
	if(m_nIconWidth <= 0)
	{
		m_nIconWidth = 30;
	}

	return m_nIconWidth;
}

VOID CDUIMenu::SetArrowRectWidth(INT nWidth)
{
	m_nArrowWidth = nWidth;
}

INT CDUIMenu::GetArrowRectWidth()
{
	if(m_nArrowWidth <= 0)
	{
		if(!m_pArrowImage.IsNull()
			&& !m_pArrowImage->IsNull())
		{
			m_nArrowWidth = m_pArrowImage->GetIconWidth();
		}
		else
		{
			m_nArrowWidth = 30;
		}
	}

	return m_nArrowWidth;
}

VOID CDUIMenu::SetArrowImage(CRefPtr<CImageList> pArrowImage)
{
	m_pArrowImage  =pArrowImage;
}

CRefPtr<CImageList> CDUIMenu::GetArrowImage() const
{
	return m_pArrowImage;
}

VOID CDUIMenu::SetIconFrameBK(CRefPtr<CDUIFrameBK> pFrameBK)
{
	m_pIconFrameBK = pFrameBK;
}

CRefPtr<CDUIFrameBK> CDUIMenu::GetIconFrameBK() const
{
	return m_pIconFrameBK;
}

VOID CDUIMenu::SetMenuWndFrameBK(CRefPtr<CDUIFrameBK> pFrameBK)
{
	m_pMenuWndFrameBK = pFrameBK;
}

CRefPtr<CDUIFrameBK> CDUIMenu::GetMenuWndFrameBK() const
{
	return m_pMenuWndFrameBK;
}

VOID CDUIMenu::SetSeparateLine(CRefPtr<CImageList> pSeperatorImage)
{
	m_pSeparateLine = pSeperatorImage;
}

CRefPtr<CImageList> CDUIMenu::GetSeparateLine() const
{
	return m_pSeparateLine;
}

VOID CDUIMenu::PaintBkgnd(HDC dc)
{
	RECT rtIconBK = GetContentRect();
	rtIconBK.right = rtIconBK.left + m_nIconWidth;
	if(!m_pIconFrameBK.IsNull()
		&& !m_pIconFrameBK->IsEmpty())
	{
		m_pIconFrameBK->DrawBK(dc, rtIconBK);
	}
}

VOID CDUIMenu::OnCreate()
{

}

VOID CDUIMenu::OnDestroy()
{

}

BOOL CDUIMenu::BeforeAddControl(CRefPtr<IDUIControl> pItem)
{
	if(!theBase::BeforeAddControl(pItem)) return FALSE;
	IDUIMenuItem* pMenuItem = (IDUIMenuItem*)pItem->GetInterface(IMenuItem);
	DUI_ASSERT(pMenuItem != NULL);
	if(pMenuItem == NULL) return FALSE;

	return TRUE;	
}

SIZE CDUIMenu::GetMenuControlSize()
{
	SIZE sizeRet = {0};
	INT nCount = GetControlCount();

	for(INT i=0; i<nCount; ++i)
	{
		IDUIControl* pControl = GetControlByIndex(i);
		DUI_ASSERT(pControl != NULL);
		IDUIMenuItem* pMenuItem = (IDUIMenuItem*)pControl->GetInterface(IMenuItem);
		DUI_ASSERT(pMenuItem != NULL);
		if(pMenuItem != NULL && pMenuItem->IsVisible())
		{
			INT nItemWidth = pMenuItem->GetIdealWidth();
			if(nItemWidth > sizeRet.cx)
			{
				sizeRet.cx = nItemWidth;
			}
			sizeRet.cy += pMenuItem->GetFixedSize().cy;
		}
	}

	return sizeRet;
}

VOID CDUIMenu::CancelMenu(BOOL bCancelAll /* = TRUE */)
{
	DUIEvent info  = {0};
	info.dwEventType = DUI_EVENT_MENU_CANCEL;
	info.pSender = this;

	if(bCancelAll)
	{
		IDUIControl* pParent = this;
		while(pParent->GetParent() != NULL)
		{
			pParent = pParent->GetParent();
		}

		pParent->BroadcastEvent(info);
	}
	else
	{
		BroadcastEvent(info);
	}
}

VOID CDUIMenu::OnMouseHover(const DUIEvent& info)
{
	INT nNewIndex = -1;
	if(info.dwEventType == DUI_EVENT_ELE_ITEM_MOUSEENTER)
	{
		nNewIndex = GetControlIndex(info.pSender);
	}
	else if(info.dwEventType == DUI_EVENT_ELE_ITEM_MOUSELEAVE)
	{
		if(!IsMenuItemPoupChild(info.pSender))
		{
			nNewIndex = -1;
		}
		else
		{
			nNewIndex = m_nSelectIndex;
		}
	}

	if(m_nSelectIndex != nNewIndex)
	{
		SetSelectIndex(nNewIndex, TRUE);
	}
}

BOOL CDUIMenu::IsMenuItemPoupChild(IDUIControl* pControl)
{
	DUI_ASSERT(pControl != NULL);
	IDUIMenuItem* pItem = (IDUIMenuItem*)pControl->GetInterface(IMenuItem);
	if(pItem == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	BOOL bPopupChild = TRUE;
	if(pItem->IsEnabled())
	{
		DWORD dwItemStyle = pItem->GetMenuItemStyle();
		if((dwItemStyle & MF_DISABLED)
			|| (dwItemStyle & MF_GRAYED)
			|| (dwItemStyle & MF_SEPARATOR)
			|| !(dwItemStyle & MF_POPUP))
		{
			bPopupChild = FALSE;
		}
	}

	return bPopupChild;
}


BOOL CDUIMenu::IsMenuItemClickable(IDUIControl* pControl)
{
	DUI_ASSERT(pControl != NULL);
	IDUIMenuItem* pItem = (IDUIMenuItem*)pControl->GetInterface(IMenuItem);
	if(pItem == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	BOOL bEnableClick = pItem->IsEnabled();
	if(bEnableClick)
	{
		DWORD dwItemStyle = pItem->GetMenuItemStyle();
		if((dwItemStyle & MF_DISABLED)
			|| (dwItemStyle & MF_GRAYED)
			|| (dwItemStyle & MF_SEPARATOR)
			|| (dwItemStyle & MF_POPUP)
			|| !(pItem->IsVisible()))
		{
			bEnableClick = FALSE;
		}
	}

	return bEnableClick;
}

VOID CDUIMenu::ClickMenuItem(IDUIControl* pMenuItem)
{
	DUI_ASSERT(pMenuItem != NULL);

	if(IsMenuItemClickable(pMenuItem))
	{
		g_pMenuItemClicked = (IDUIMenuItem*)pMenuItem->GetInterface(IMenuItem);
		CancelMenu(TRUE);
	}
}

VOID CDUIMenu::OnMouseClick(const DUIEvent& info)
{
	ClickMenuItem(info.pSender);
}

BOOL CDUIMenu::IsMenuItemSelectable(INT nIndex)
{
	BOOL bRet(FALSE);
	IDUIMenuItem* pItem = NULL;
	pItem = (IDUIMenuItem*)GetControlByIndex(nIndex)->GetInterface(IMenuItem);
	DUI_ASSERT(pItem != NULL);
	if(pItem != NULL)
	{
		DWORD dwItemStyle = pItem->GetMenuItemStyle();
		if(!(dwItemStyle&MF_SEPARATOR)
			&& pItem->IsVisible())
		{
			bRet = TRUE;			
		}
	}

	return bRet;
}

INT CDUIMenu::GetNextSelectIndex(BOOL bNext)
{
	INT nIndex = m_nSelectIndex;
	INT nCount = GetControlCount();
	if(nCount <= 0) return -1;

	if(nIndex < 0 || nIndex >= nCount)
	{
		if(bNext) //before the first element
		{
			nIndex = -1;
		}
		else
		{
			nIndex = nCount;
		}
	}

	
	INT nIndexRet = -1;

	if(bNext)
	{
		for(INT i=nIndex+1; i<nCount; ++i)
		{
			if(IsMenuItemSelectable(i))
			{
				nIndexRet = i;
				break;
			}
		}

		if(nIndexRet == -1)
		{
			for(INT i=0; i<=nIndex; ++i)
			{
				if(IsMenuItemSelectable(i))
				{
					nIndexRet = i;
					break;
				}
			}
		}
	}
	else
	{
		for(INT i=nIndex-1; i>=0; --i)
		{
			if(IsMenuItemSelectable(i))
			{
				nIndexRet = i;
				break;
			}			
		}

		if(nIndexRet == -1)
		{	
			for(INT i=nCount-1; i>=nIndex; --i)
			{
				if(IsMenuItemSelectable(i))
				{
					nIndexRet = i;
					break;
				}
			}
		}
	}


	return nIndexRet;
}


VOID CDUIMenu::OnChar(const DUIEvent& info)
{
	TCHAR ch = info.wParam;
	if((ch >= _T('a') && ch <= _T('z'))
		|| (ch >= _T('A') && ch <= _T('Z')))
	{
		CDUIString strKey1(_T("&"));
		strKey1 += tolower(info.chKey);

		CDUIString strKey2(_T("&"));
		strKey2 += toupper(info.chKey);

		INT nCount = GetControlCount();
		for(INT i=0; i<nCount; ++i)
		{
			IDUIMenuItem* pItem = (IDUIMenuItem*)GetControlByIndex(i)->GetInterface(IMenuItem);
			DUI_ASSERT(pItem != NULL);
			if(pItem != NULL
				&& IsMenuItemClickable(pItem))
			{
				CDUIString strText = pItem->GetText();
				INT nKey1 = strText.find(strKey1);
				if(nKey1 != -1)
				{
					if((nKey1 == 0)
						|| (nKey1 >= 1 && (strText[nKey1-1] != _T('&'))))
					{
						ClickMenuItem(pItem);
						break;
					}
				}
				else
				{
					INT nKey2 = strText.find(strKey2);
					if((nKey2 == 0)
						|| (nKey2 >= 1 && (strText[nKey2-1] != _T('&'))))
					{
						ClickMenuItem(pItem);
						break;
					}
				}

			}
		}
	}
}

VOID CDUIMenu::OnKeyDown(const DUIEvent& info)
{
	switch(info.wParam)
	{
	case VK_UP:
		{
			SetSelectIndex(GetNextSelectIndex(FALSE), FALSE);
		}
		break;


	case VK_DOWN:
		{
			SetSelectIndex(GetNextSelectIndex(TRUE), FALSE);
		}
		break;

//	case VK_F10:
	case VK_ESCAPE:
		CancelMenu(TRUE);
		break;

	case VK_RETURN:
		{
			if(m_nSelectIndex >= 0 
				&& m_nSelectIndex < GetControlCount())
			{
				ClickMenuItem(GetControlByIndex(m_nSelectIndex));
			}
		}

	case VK_RIGHT:
		{
			if(m_nSelectIndex >=0 && m_nSelectIndex < GetControlCount())
			{
				IDUIControl* pControl = GetControlByIndex(m_nSelectIndex);
				if(pControl != NULL)
				{
					IDUIMenuItem* pMenuItem = (IDUIMenuItem*)pControl->GetInterface(IMenuItem);
					DUI_ASSERT(pMenuItem != NULL);
					if(pMenuItem != NULL)
					{
						ShowChildMenu(pMenuItem, TRUE);
					}
				}
			}			
		}
		break;

	case VK_LEFT:	
		{
			IDUIControl* pParent = GetParent();
			if((pParent != NULL) 
				&& 
				(pParent->GetInterface(IMenu) != NULL
				|| pParent->GetInterface(IMenuItem) != NULL))
			{
				CancelMenu(FALSE);
			}
		}
		break;

	default:	
		break;
	}
}

VOID CDUIMenu::SetSelectIndex(INT nIndex, BOOL bFromMouse /* = TRUE */)
{
	if(m_nSelectIndex == nIndex) return;
	
	if(m_nSelectIndex != -1)
	{
		IDUIControl* pControl = GetControlByIndex(m_nSelectIndex);
		if(pControl != NULL)
		{
			IDUIMenuItem* pMenuItem = (IDUIMenuItem*)pControl->GetInterface(IMenuItem);
			DUI_ASSERT(pMenuItem != NULL);
			if(pMenuItem != NULL)
			{
				pMenuItem->SetSelect(FALSE);
				HideChildMenu(pMenuItem);
			}
		}
	}

	if(nIndex != -1)
	{
		IDUIControl* pControl = GetControlByIndex(nIndex);
		if(pControl != NULL)
		{
			IDUIMenuItem* pMenuItem = (IDUIMenuItem*)pControl->GetInterface(IMenuItem);
			DUI_ASSERT(pMenuItem != NULL);
			if(pMenuItem != NULL)
			{
				pMenuItem->SetSelect(TRUE);
				if(bFromMouse)
				{
					ShowChildMenu(pMenuItem, FALSE);
				}
			}
		}
	}

	m_nSelectIndex = nIndex;
	SendNotify(WM_NOTIFY_MENU_NOTIFY, MENU_WPARAM_SELCHANGE, m_nSelectIndex);

	UpdateLayout(FALSE);
}

BOOL CDUIMenu::IsMemuWndPopup()
{
	if(m_pMenuWnd != NULL
		&& m_pMenuWnd->IsWindow())
	{
		return TRUE;
	}

	return FALSE;
}

VOID CDUIMenu::ShowChildMenu(IDUIMenuItem* pMenuItem, BOOL bSelectFirst /* = FALSE */)
{
	DUI_ASSERT(pMenuItem != NULL);
	DWORD dwItemStyle = pMenuItem->GetMenuItemStyle();
	if(!IsMenuItemPoupChild(pMenuItem)) return;
	RECT rtItem = pMenuItem->GetControlRect();

	IDUIMenu* pChildMenu = pMenuItem->GetChildMenu();
	if(pChildMenu == NULL) return;

	DUI_ASSERT(m_pMenuWnd != NULL && m_pMenuWnd->IsWindow());
	RECT rtMenuWnd = {0};
	m_pMenuWnd->GetWindowRect(&rtMenuWnd);

	CDUIMenu* pDUIMenu = (CDUIMenu*)pChildMenu;
	if(!pDUIMenu->IsMemuWndPopup())
	{
		POINT ptRight = {0};
		ptRight.x = rtMenuWnd.right + DUI_MENU_CHILD_GAP;
		ptRight.y = rtMenuWnd.top + rtItem.top;
		
		POINT ptLeft = {0};
		ptLeft.x = rtMenuWnd.left - DUI_MENU_CHILD_GAP;
		ptLeft.y = ptRight.y;
		pDUIMenu->ShowMenuWnd(this, ptRight, ptLeft, FALSE);
	}

	if(bSelectFirst)
	{
		pDUIMenu->SetSelectIndex(pDUIMenu->GetNextSelectIndex(TRUE), FALSE);
	}
}

VOID CDUIMenu::HideChildMenu(IDUIMenuItem* pMenuItem)
{
	DUI_ASSERT(pMenuItem != NULL);
	DWORD dwItemStyle = pMenuItem->GetMenuItemStyle();
	if(!IsMenuItemPoupChild(pMenuItem)) return;

	IDUIMenu* pChildMenu = pMenuItem->GetChildMenu();
	if(pChildMenu == NULL) return;

	if(m_pMenuWnd != NULL
		&& m_pMenuWnd->IsWindow())
	{
		((CDUIMenu*)pChildMenu)->CancelMenu(FALSE);
	}
}

CRefPtr<CDUIFrameBK> CDUIMenu::GetDefaultMenuWndFrameBK()
{
	if(m_pMenuWndFrameBK.IsNull())
	{
		m_pMenuWndFrameBK = new CDUIFrameBK;
		
		if(!m_pMenuWndFrameBK.IsNull())
		{
			CARGB clrBK;
			clrBK.SetColor(RGB(255,255,255));
			m_pMenuWndFrameBK->SetBKColor(clrBK);

			CARGB clrFrame;
			clrFrame.SetColor(RGB(0,0,0));
			m_pMenuWndFrameBK->SetFrameColor(clrFrame);

			RECT rtBorder = {1,1,1,1};
			m_pMenuWndFrameBK->SetFrameBorder(rtBorder);
		}
	}

	return m_pMenuWndFrameBK;
}

CRefPtr<IDUIMenu>  CDUIMenu::CreateFromXML(const CDUIString& strXML)
{
	CDUIMenuBuilder menuBuilder;
	return menuBuilder.CreateFromXML(strXML);
}

CRefPtr<IDUIMenu> CDUIMenu::CreateFromResource(const CDUIString& strMenuId)
{
	CDUIMenuBuilder menuBuilder;
	return menuBuilder.CreateFromResource(strMenuId);
}

////////////////////  CDUIMenu  /////////////////////////
DUI_END_NAMESPCE