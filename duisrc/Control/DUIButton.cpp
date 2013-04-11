#include "DUIButton.h"
#include "DUIRenderEngine.h"

DUI_BGN_NAMESPCE

//////////////////  CDUIButton  ////////////////////////
CDUIButton::CDUIButton()
{
	m_dwButtonStyle = 0;
	m_dwStyle |= (CONTROL_STYLE_TABSTOP | CONTROL_STYLE_TOOLTIP);
}

CDUIButton::~CDUIButton()
{

}

LPVOID CDUIButton::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IButton) == 0)
	{
		return (IDUIButton*)this;
	}
	
	return theBase::GetInterface(strName);	
}

CRefPtr<IDUIControl> CDUIButton::Clone()
{
	return new CDUIButton(*this);
}

BOOL CDUIButton::Activate()
{
	if(!theBase::Activate()) return FALSE;

	SendNotify(WM_NOTIFY_BUTTON_CLICKED);

	return TRUE;
}

VOID CDUIButton::SetCheck(BOOL bSetChecked)
{
	if(bSetChecked)
	{
		ModifyStatus(0, CONTROL_STATUS_CHECKED);
	}
	else
	{
		ModifyStatus(CONTROL_STATUS_CHECKED, 0);
	}
}

BOOL CDUIButton::GetCheck() const
{
	return GetStatus() & CONTROL_STATUS_CHECKED;	
}

VOID CDUIButton::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("back_image")) == 0)
	{
		m_ButtonData.m_imageBK = GetSkinMgr()->GetImage(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("icon")) == 0)
	{
		m_ButtonData.m_imageFore = GetSkinMgr()->GetImage(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("button_style")) == 0)
	{
		m_dwButtonStyle = ParseButtonStyle(strValue);
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

DWORD CDUIButton::ParseButtonStyle(const CDUIString& strValue)
{
	DWORD dwStyle(0);
	if(strValue.find(_T("center_icon")) != -1)
	{
		dwStyle &= ~BTN_FORE_MASK;
		dwStyle |= BTN_STYLE_CENTER_ICON;
	}
	else if(strValue.find(_T("left_icon")) != -1)
	{
		dwStyle &= ~BTN_FORE_MASK;
		dwStyle |= BTN_STYLE_LEFT_ICON;			
	}
	else if(strValue.find(_T("right_icon")) != -1)
	{
		dwStyle &= ~BTN_FORE_MASK;
		dwStyle |= BTN_STYLE_RIGHT_ICON;			
	}
	else if(strValue.find(_T("top_icon")) != -1)
	{
		dwStyle &= ~BTN_FORE_MASK;
		dwStyle |= BTN_STYLE_TOP_ICON;			
	}
	else if(strValue.find(_T("bottom_icon")) != -1)
	{
		dwStyle &= ~BTN_FORE_MASK;
		dwStyle |= BTN_STYLE_BOTTOM_ICON;			
	}

	if(strValue.find(_T("no_text")) != -1)
	{
		dwStyle |= BTN_STYLE_NOTEXT;
	}
	
	if(strValue.find(_T("auto_fit")) != -1)
	{
		dwStyle |= BTN_STYLE_AUTO_FIT_BK;
	}

	return dwStyle;
}

BOOL CDUIButton::EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet)
{
	if((GetButtonStyle()&BTN_STYLE_AUTO_FIT_BK)
		&& !m_ButtonData.m_imageBK.IsNull()
		&& !m_ButtonData.m_imageBK->IsNull())
	{
		sizeRet.cx = m_ButtonData.m_imageBK->GetIconWidth();
		sizeRet.cy = m_ButtonData.m_imageBK->GetIconHeight();
		return TRUE;
	}

	SIZE sizeImage = {0};
	if((GetButtonStyle()&BTN_FORE_MASK) 
		&& !m_ButtonData.m_imageFore.IsNull())
	{
		sizeImage.cx = m_ButtonData.m_imageFore->GetIconWidth();
		sizeImage.cy = m_ButtonData.m_imageFore->GetIconHeight();
	}
	
	SIZE sizeText = {0};
	if(!(GetButtonStyle() & BTN_STYLE_NOTEXT) 
		&& m_strText.size() > 0)
	{
		CRefPtr<CTextStyle> pTextStyle = GetTextStyle();
		if(!pTextStyle.IsNull())
		{
			CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
			if(!pFont.IsNull() && !pFont->IsNull())
			{
				sizeText = CDUIRenderEngine::GetTextIdealSize(m_strText, pFont->GetFont(), 0, DT_SINGLELINE);
			}
		}
	}

	sizeRet = sizeText;
	RECT rtMargin = m_rtMargin;
	INT nPadding = GetPaddingWidth();//conbine text and image
	if(sizeImage.cx > 0 && sizeImage.cy > 0)
	{
		if(m_dwButtonStyle & BTN_STYLE_CENTER_ICON)
		{
			if(sizeImage.cx > sizeRet.cx) sizeRet.cx = sizeImage.cx;
			if(sizeImage.cy > sizeRet.cy) sizeRet.cy = sizeImage.cy;
		}
		else if(m_dwButtonStyle & (BTN_STYLE_LEFT_ICON | BTN_STYLE_RIGHT_ICON))
		{
			sizeRet.cx += (nPadding + sizeImage.cx);
			if(sizeImage.cy > sizeRet.cy) sizeRet.cy = sizeImage.cy;
		}
		else if(m_dwButtonStyle & (BTN_STYLE_BOTTOM_ICON | BTN_STYLE_TOP_ICON))
		{
			sizeRet.cy += (nPadding + sizeImage.cy);
			if(sizeImage.cx > sizeRet.cx)  sizeRet.cx = sizeImage.cx;
		}
	}

	sizeRet.cx += (rtMargin.left + rtMargin.right);
	sizeRet.cy += (rtMargin.top + rtMargin.bottom);

	//use fixed width and fixed height
	if(m_sizeFixed.cx != 0)
	{
		sizeRet.cx = m_sizeFixed.cx;
	}
	if(m_sizeFixed.cy != 0)
	{
		sizeRet.cy = m_sizeFixed.cy;
	}

	return TRUE;
}

BTN_IMAGE_STATUS CDUIButton::GetImageStatusIndex()
{
	DWORD dwStatus = GetStatus();

	BOOL bEnable = !(dwStatus & CONTROL_STATUS_DISABLE);
	BOOL bFocus = (dwStatus & CONTROL_STATUS_FOCUS);
	BOOL bPressed = (dwStatus & CONTROL_STATUS_PRESSED);
	BOOL bHover = (dwStatus & CONTROL_STATUS_HOVER);
	BOOL bChecked = (dwStatus & CONTROL_STATUS_CHECKED);

	BTN_IMAGE_STATUS eRet = BTN_INDEX_NORMAL;
	if(!bEnable)
	{
		eRet = BTN_INDEX_DISABLE;
	}
	else if(bPressed && bHover)
	{
		eRet = BTN_INDEX_PRESS;
	}
	else if(bHover)
	{
		eRet = BTN_INDEX_HOVER;
	}
	else if(bPressed)
	{
		eRet = BTN_INDEX_HOVER;
	}
	else if(bChecked)
	{
		eRet = BTN_INDEX_CHECK;
	}
	else if(bFocus)
	{
		eRet = BTN_INDEX_FOCUS;
	}

	return eRet;
}

VOID CDUIButton::PaintBkgnd(HDC dc)
{
	RECT rtButton = m_rtControl;
	BTN_IMAGE_STATUS nImageIndex = GetImageStatusIndex();
	CRefPtr<CImageList> pImageBK = m_ButtonData.m_imageBK;
	if(pImageBK.IsNull() || pImageBK->IsNull()) return;
	
	INT nIndex(nImageIndex);
	if(nImageIndex >= pImageBK->GetItemCount())
	{
		DUI_ASSERT(FALSE);
		nIndex = 0;
	}
	
	pImageBK->MiddleStretch(dc, rtButton, nIndex);
}

VOID CDUIButton::PaintForgnd(HDC dc)
{
	::SetRectEmpty(&m_rtIcon);

	CRefPtr<CImageList> pImageIcon = m_ButtonData.m_imageFore;
	if(pImageIcon.IsNull() || pImageIcon->IsNull()) return;
	if(!(m_dwButtonStyle&BTN_FORE_MASK)) return;

	BTN_IMAGE_STATUS nImageIndex = GetImageStatusIndex();
	INT nIndex(nImageIndex);
	if(nImageIndex >= pImageIcon->GetItemCount())
	{
		nIndex = 0;
	}

	RECT rtButton = m_rtControl;
	RECT rtMargin = GetMargin();
	INT nPadding = GetPaddingWidth();
	SIZE sizeButton = { rtButton.right - rtButton.left, rtButton.bottom - rtButton.top };
	RECT rtIcon = {0};
	SIZE sizeIcon = { pImageIcon->GetIconWidth(), pImageIcon->GetIconHeight() };

	if(m_dwButtonStyle & BTN_STYLE_CENTER_ICON)
	{
		rtIcon = rtButton;
		if(sizeButton.cy > sizeIcon.cy)
		{
			rtIcon.top += (sizeButton.cy - sizeIcon.cy) / 2;
		}
		if(sizeButton.cx > sizeIcon.cx)
		{
			rtIcon.left += (sizeButton.cx - sizeIcon.cx) / 2;
		}

		rtIcon.right = rtIcon.left + sizeIcon.cx;
		rtIcon.bottom = rtIcon.top + sizeIcon.cy;
	}
	else if(m_dwButtonStyle & BTN_STYLE_LEFT_ICON)
	{
		rtIcon = rtButton;
		if(sizeButton.cy > sizeIcon.cy)
		{
			rtIcon.top += (sizeButton.cy - sizeIcon.cy) / 2;
		}
		rtIcon.left += rtMargin.left;

		rtIcon.right = rtIcon.left + sizeIcon.cx;
		rtIcon.bottom = rtIcon.top + sizeIcon.cy;
	}
	else if(m_dwButtonStyle & BTN_STYLE_RIGHT_ICON)
	{
		rtIcon = rtButton;
		if(sizeButton.cy > sizeIcon.cy)
		{
			rtIcon.top += (sizeButton.cy - sizeIcon.cy) / 2;
		}
		rtIcon.right -= rtMargin.right;
		rtIcon.left = rtIcon.right - sizeIcon.cx;
		rtIcon.bottom = rtIcon.top + sizeIcon.cy;
	}
	else if(m_dwButtonStyle & BTN_STYLE_TOP_ICON)
	{
		rtIcon = rtButton;
		if(sizeButton.cx > sizeIcon.cx)
		{
			rtIcon.left += (sizeButton.cx - sizeIcon.cx) / 2;
		}
		rtIcon.top += rtMargin.top;
		rtIcon.bottom = rtIcon.top + sizeIcon.cy;
		rtIcon.right = rtIcon.left + sizeIcon.cx;

	}
	else if(m_dwButtonStyle & BTN_STYLE_BOTTOM_ICON)
	{
		rtIcon = rtButton;
		if(sizeButton.cx > sizeIcon.cx)
		{
			rtIcon.left += (sizeButton.cx - sizeIcon.cx) / 2;
		}
		rtIcon.bottom -= rtMargin.bottom;
		rtIcon.top = rtIcon.bottom - sizeIcon.cy;
		rtIcon.right = rtIcon.left + sizeIcon.cx;
	}
		
	::IntersectRect(&rtIcon, &rtIcon, &rtButton);
	pImageIcon->Draw(dc, rtIcon, nIndex);

	m_rtIcon = rtIcon;
}

VOID CDUIButton::PaintText(HDC dc)
{
	::SetRectEmpty(&m_rtText);
	RECT rtIcon = m_rtIcon;
	if(m_strText.size() <= 0 || (m_dwButtonStyle&BTN_STYLE_NOTEXT)) return;
	
	CRefPtr<CTextStyle> pTextStyle = GetTextStyle();
	if(pTextStyle.IsNull()) return;

	CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
	if(pFont.IsNull() || pFont->IsNull()) return;


	RECT rtButton = m_rtControl;
	RECT rtMargin = GetMargin();
	INT nPadding = GetPaddingWidth();
	SIZE sizeButton = { rtButton.right - rtButton.left, rtButton.bottom - rtButton.top };
	RECT rtText = rtButton;
	
	if((m_dwButtonStyle & BTN_STYLE_CENTER_ICON)
		|| ::IsRectEmpty(&rtIcon))
	{
		rtText = rtButton;
	}

	else if(m_dwButtonStyle & BTN_STYLE_LEFT_ICON)
	{
		rtText.left = rtIcon.right + nPadding;
		rtText.right = rtButton.right - rtMargin.right;
		rtText.top = rtButton.top + rtMargin.top;
		rtText.bottom = rtButton.bottom - rtMargin.bottom;			
	}
	else if(m_dwButtonStyle & BTN_STYLE_RIGHT_ICON)
	{
		rtText.left = rtButton.left + rtMargin.left;
		rtText.right = rtIcon.left - nPadding;
		rtText.top = rtButton.top + rtMargin.top;
		rtText.bottom = rtButton.bottom - rtMargin.bottom;
	}
	else if(m_dwButtonStyle & BTN_STYLE_TOP_ICON)
	{
		rtText.left = rtButton.left + rtMargin.left;
		rtText.right = rtButton.right - rtMargin.right;
		rtText.top = rtIcon.bottom + nPadding;
		rtText.bottom = rtButton.bottom - rtMargin.bottom;
	}
	else if(m_dwButtonStyle & BTN_STYLE_BOTTOM_ICON)
	{
		rtText.left = rtButton.left + rtMargin.left;
		rtText.right = rtButton.right - rtMargin.right;
		rtText.top = rtButton.top + rtMargin.top;
		rtText.bottom = rtIcon.top - nPadding;
	}	

	::IntersectRect(&rtText, &rtText, &rtButton);
	if(::IsRectEmpty(&rtText)) return;

	BTN_IMAGE_STATUS nImageIndex = GetImageStatusIndex();
	COLORREF clrText = GetColorByStatus(nImageIndex);	
	::SetBkMode(dc, TRANSPARENT);
	::SetTextColor(dc, clrText);
	::SelectObject(dc, pFont->GetFont());

	CDUIRenderEngine::DrawText(dc, m_strText.c_str(), m_strText.size(), &rtText
		, pTextStyle->GetTextFormat());

	m_rtText = rtText;	
}

VOID CDUIButton::PaintBorder(HDC dc)
{
	if(GetFocus())
	{
		RECT rtFocus = m_rtControl;
		::InflateRect(&rtFocus, -2, -2);
		::DrawFocusRect(dc, &rtFocus);
	}
}

COLORREF CDUIButton::GetColorByStatus(BTN_IMAGE_STATUS eStatus)
{
 	COLORREF clrRet(RGB(0, 0, 0));
	CRefPtr<CTextStyle> pTextStyle = GetTextStyle();
	if(pTextStyle.IsNull()) return clrRet;

	switch(eStatus)
	{
	case BTN_INDEX_NORMAL:
	case BTN_INDEX_FOCUS:
	case BTN_INDEX_CHECK:
	case BTN_INDEX_PRESS:
		clrRet = pTextStyle->GetNormalColor().GetColor();
		break;

	case BTN_INDEX_DISABLE:
		clrRet = pTextStyle->GetDisableColor().GetColor();
		break;

	case BTN_INDEX_HOVER:
		clrRet = pTextStyle->GetHoverColor().GetColor();
		break;

	default:
		DUI_ASSERT(FALSE);
	}

	return clrRet;
}

LRESULT CDUIButton::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	switch(info.dwEventType)
	{
	case DUI_EVENT_LBUTTONDOWN:
	case DUI_EVENT_LBUTTONDBCLK:
		if(IsEnabled())
			ModifyStatus(0, CONTROL_STATUS_PRESSED);
		break;

	case DUI_EVENT_LBUTTONUP:
		if(IsEnabled())
		{
			ModifyStatus(CONTROL_STATUS_PRESSED, 0);
			if(PtInRect(&GetControlRect(), info.ptMouse))
			{
				Activate();
			}
		}
		break;

	case DUI_EVENT_MOUSEENTER:
		if(IsEnabled())
			ModifyStatus(0, CONTROL_STATUS_HOVER);
		break;
	
	case DUI_EVENT_MOUSELEAVE:
		if(IsEnabled())
			ModifyStatus(CONTROL_STATUS_HOVER, 0);
		break;

	case DUI_EVENT_KEYDOWN:
		if(IsEnabled())
		{
			if(info.wParam == VK_SPACE)
			{
				ModifyStatus(0, CONTROL_STATUS_PRESSED);
			}
		}
		break;

	case DUI_EVENT_KEYUP:
		if(IsEnabled())
		{
			if(info.wParam == VK_SPACE)
			{
				ModifyStatus(CONTROL_STATUS_PRESSED, 0);
				Activate();
			}			
		}
		break;

	default:
		break;
									
	}
	
	return 	theBase::ProcessEvent(info, bHandled);
}

VOID CDUIButton::SetButtonUIData(const CButtonUIData& data)
{
	m_ButtonData = data;
}

const CButtonUIData& CDUIButton::GetButtonUIData() const
{
	return m_ButtonData;
}

VOID CDUIButton::SetButtonStyle(DWORD dwStyle)
{
	if(m_dwButtonStyle != dwStyle)
	{
		m_dwButtonStyle = dwStyle;
		UpdateLayout(FALSE);
	}
}

DWORD CDUIButton::GetButtonStyle() const
{
	return m_dwButtonStyle;
}
//////////////////  CDUIButton  ////////////////////////


///////////////////  CDUICheckButton  ///////////////////////
CDUICheckButton::CDUICheckButton()
{
	m_dwButtonStyle &= ~BTN_FORE_MASK;
	m_dwButtonStyle |= BTN_STYLE_LEFT_ICON;	

	SetPaddingWidth(5);
}
VOID CDUICheckButton::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("checked_image")) == 0)
	{
		m_ButtonData.m_imageStatus1 = GetSkinMgr()->GetImage(strValue);
		if(GetCheck())
		{
			m_ButtonData.m_imageFore = m_ButtonData.m_imageStatus1;
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("unchecked_image")) == 0)
	{
		m_ButtonData.m_imageStatus2 = GetSkinMgr()->GetImage(strValue);
		if(!GetCheck())
		{
			m_ButtonData.m_imageFore = m_ButtonData.m_imageStatus2;
		}
		bHandled = TRUE;
	}
	
	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUICheckButton::PaintForgnd(HDC dc)
{
	if(GetCheck())
	{
		if(!m_ButtonData.m_imageStatus1.IsNull())
		{
			m_ButtonData.m_imageFore = m_ButtonData.m_imageStatus1;
		}
	}
	else
	{
		if(!m_ButtonData.m_imageStatus2.IsNull())
		{
			m_ButtonData.m_imageFore = m_ButtonData.m_imageStatus2;
		}
	}
	
	theBase::PaintForgnd(dc);
}

VOID CDUICheckButton::PaintBorder(HDC dc)
{
	if(GetFocus())
	{
		if(!::IsRectEmpty(&m_rtText))
		{
			RECT rtFocus = m_rtText;
			DrawFocusRect(dc, &rtFocus);
		}
	}
}
///////////////////  CDUICheckButton  ///////////////////////


///////////////////  CDUIRadioButton  ///////////////////////
CRefPtr<IDUIControl> CDUIRadioButton::Clone()
{
	return new CDUIRadioButton(*this);
}

BOOL CDUIRadioButton::Activate()
{
	if(!theBase::Activate()) return FALSE;
	
	SetCheck(TRUE);
	
	SendNotify(WM_NOTIFY_BUTTON_CLICKED);
	
	return TRUE;
}
///////////////////  CDUIRadioButton  ///////////////////////


///////////////////  CDUICheckBox  ///////////////////////
CRefPtr<IDUIControl> CDUICheckBox::Clone()
{
	return new CDUICheckBox(*this);
}

BOOL CDUICheckBox::Activate()
{
	if(!theBase::Activate()) return FALSE;
	
	SetCheck(!GetCheck());
	
	SendNotify(WM_NOTIFY_BUTTON_CLICKED);
	
	return TRUE;
}

LRESULT CDUICheckBox::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	switch(info.dwEventType)
	{
	case DUI_EVENT_CHAR:
		if(info.chKey == _T('+'))
		{
			if(!(GetStatus() & CONTROL_STATUS_CHECKED))
			{
				Activate();
			}
		}
		else if(info.chKey == _T('-'))
		{
			if(GetStatus() & CONTROL_STATUS_CHECKED)
			{
				Activate();
			}
		}
		break;

	default:
		break;
									
	}
	
	return 	theBase::ProcessEvent(info, bHandled);
}
///////////////////  CDUICheckBox  ///////////////////////

DUI_END_NAMESPCE