#include "DUILabel.h"
#include "DUIRenderEngine.h"

DUI_BGN_NAMESPCE

///////////////   CDUILabel  ///////////////////////
CDUILabel::CDUILabel()
{

}
	
CRefPtr<IDUIControl> CDUILabel::Clone()
{
	return new CDUILabel(*this);
}

LRESULT CDUILabel::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
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
			ModifyStatus(CONTROL_STATUS_PRESSED, 0);
		break;
		
	case DUI_EVENT_MOUSEENTER:
		if(IsEnabled())
			ModifyStatus(0, CONTROL_STATUS_HOVER);
		break;
		
	case DUI_EVENT_MOUSELEAVE:
		if(IsEnabled())
			ModifyStatus(CONTROL_STATUS_HOVER, 0);
		break;	
		
	default:
		break;
	}

	return theBase::ProcessEvent(info, bHandled);
}

LPVOID CDUILabel::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ILabel) == 0)
	{
		return (IDUILabel*)this;
	}

	return theBase::GetInterface(strName);
}

BOOL CDUILabel::EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet)
{
	if(m_strText.size() <= 0) return FALSE;

	CRefPtr<CTextStyle> pTextStyle = GetTextStyle();
	if(pTextStyle.IsNull()) return FALSE;

	CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
	if(pFont.IsNull() || pFont->IsNull()) return FALSE;
	
	SIZE sizeText = CDUIRenderEngine::GetTextIdealSize(m_strText, pFont->GetFont(), 0, pTextStyle->GetTextFormat());
	if(sizeRet.cx > 0) sizeRet.cx;

	if(m_sizeFixed.cx != 0) sizeText.cx = m_sizeFixed.cx;
	if(m_sizeFixed.cy != 0) sizeText.cy = m_sizeFixed.cy;
	
	sizeRet = sizeText;

	return TRUE;
}

COLORREF CDUILabel::GetLabelTextColor()
{
	COLORREF clrRet(RGB(0, 0, 0));
	CRefPtr<CTextStyle> pTextStyle = GetTextStyle();
	if(pTextStyle.IsNull()) return clrRet;

	if(m_dwStatus & CONTROL_STATUS_DISABLE)
	{
		clrRet = pTextStyle->GetDisableColor().GetColor();
	}
	else if(m_dwStatus & CONTROL_STATUS_HOVER)
	{
		clrRet = pTextStyle->GetHoverColor().GetColor();
	}
	else
	{
		clrRet = pTextStyle->GetNormalColor().GetColor();
	}
	
	return clrRet;
}

VOID CDUILabel::PaintText(HDC dc)
{
	if(m_strText.size() <= 0) return;

	CRefPtr<CTextStyle> pTextStyle = GetTextStyle();
	if(pTextStyle.IsNull()) return;

	CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
	if(pFont.IsNull() || pFont->IsNull()) return;
	
	RECT rtText = GetContentRect();
	DWORD dwTextFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	if(!m_pTextStyle.IsNull()) dwTextFormat =  m_pTextStyle->GetTextFormat();

	::SetBkMode(dc, TRANSPARENT);
	::SetTextColor(dc, GetLabelTextColor());
	::SelectObject(dc, pFont->GetFont());
	CDUIRenderEngine::DrawText(dc, m_strText.c_str(), m_strText.size(), &rtText, dwTextFormat);
}
///////////////   CDUILabel  ///////////////////////


///////////////   CDUIHyperLink  ///////////////////////
CDUIHyperLink::CDUIHyperLink()
{
	m_dwStyle |= CONTROL_STYLE_SETCURSOR;
	m_dwStyle |= CONTROL_STYLE_TOOLTIP;
}

CRefPtr<IDUIControl> CDUIHyperLink::Clone()
{
	return new CDUIHyperLink(*this);
}

LRESULT CDUIHyperLink::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	switch(info.dwEventType)
	{
	case DUI_EVENT_LBUTTONUP:
		if(PtInRect(&GetControlRect(), info.ptMouse))
		{
			Activate();
		}
		break;

	case DUI_EVENT_SETCURSOR:
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649))); //IDC_HAND
			return S_OK;
		}
		break;
		
	default:
		break;
		
	}

	return theBase::ProcessEvent(info, bHandled);
}

BOOL CDUIHyperLink::Activate()
{
	if(!theBase::Activate()) return FALSE;
	
	SendNotify(WM_NOTIFY_LINK_CLICKED);

	return TRUE;
}

LPVOID CDUIHyperLink::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IHyperLink) == 0)
	{
		return (IDUIHyperLink*)this;
	}

	return theBase::GetInterface(strName);
}

VOID CDUIHyperLink::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	if(strName.compare(_T("href")) == 0)
	{
		SetHyperLink(strValue);
		return;
	}

	theBase::SetAttribute(strName, strValue);
}

VOID CDUIHyperLink::SetHyperLink(const CDUIString& strLink)
{
	m_strHyperLink = strLink;
	if(m_strTooltip.size() <= 0)
	{
		m_strTooltip = strLink;
	}
}

const CDUIString& CDUIHyperLink::GetHyperLink() const
{
	return m_strHyperLink;
}


///////////////   CDUIHyperLink  ///////////////////////

DUI_END_NAMESPCE