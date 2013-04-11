#include "DUITrackBar.h"
#include "DUIButton.h"

DUI_BGN_NAMESPCE

CDUITrackBarBase::CDUITrackBarBase()
{
	m_nPos = m_nMin = m_nMax = 0;
	m_dwStyle |= CONTROL_STYLE_TABSTOP;
	m_bTracking = FALSE;
	m_nStep = 1;

	CreateThumb();
}	

CDUITrackBarBase::~CDUITrackBarBase()
{

}

VOID CDUITrackBarBase::OnCreate()
{
	IDUIButton* pThumb = GetThumbButton();
	if(pThumb != NULL)
	{
		SubclassControl(pThumb, TRUE);
	}
}	

VOID CDUITrackBarBase::OnDestroy()
{
	IDUIButton* pThumb = GetThumbButton();
	if(pThumb != NULL)
	{
		SubclassControl(pThumb, FALSE);
	}
}

VOID CDUITrackBarBase::CreateThumb()
{
	IDUIButton* pButton = new CDUIButton;
	if(pButton != NULL)
	{
		AddControl(pButton);
		pButton->ModifyStyle(CONTROL_STYLE_TABSTOP, 0);
		pButton->SetButtonStyle(BTN_STYLE_CENTER_ICON | BTN_STYLE_NOTEXT);
	}
}

IDUIButton* CDUITrackBarBase::GetThumbButton()
{
	DUI_ASSERT(GetControlCount() > 0);
	if(GetControlCount() > 0)
	{
		return (IDUIButton*)m_arChildControl[0].GetPtr()->GetInterface(IButton);
	}

	return NULL;
}

VOID CDUITrackBarBase::SetTooltip(const CDUIString& strTooltip)
{
	theBase::SetTooltip(strTooltip);

	IDUIButton* pThumb = GetThumbButton();
	if(pThumb != NULL 
		&& pThumb->GetTooltip().size() == 0)
	{
		pThumb->SetTooltip(strTooltip);
	}
}

LPVOID CDUITrackBarBase::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ITrackBar) == 0)
	{
		return (IDUITrackBar*)this;
	}

	return theBase::GetInterface(strName);
}

VOID CDUITrackBarBase::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("back_image")) == 0)
	{
		CRefPtr<CImageList> pImage = GetSkinMgr()->GetImage(strValue);
		DUI_ASSERT(!pImage.IsNull()
			&& !pImage->IsNull()
			&& pImage->GetItemCount() >= TRACK_BAR_IMAGE_COUNT); 

		m_uiData.m_pImageBK = pImage;

		bHandled = TRUE;
	}
	else if(strName.compare(_T("thumb_image")) == 0)
	{
		CRefPtr<CImageList> pImage = GetSkinMgr()->GetImage(strValue);
		DUI_ASSERT(!pImage.IsNull()
			&& !pImage->IsNull());
		
		CTrackBarUIData uiData = m_uiData;
		uiData.m_pImageThumb = pImage;
		SetUIData(uiData);
		
		bHandled = TRUE;
	}
	else if(strName.compare(_T("range")) == 0)
	{
		TCHAR szValue[128] = {0};
		lstrcpyn(szValue, strValue.c_str(), sizeof(szValue) / sizeof(TCHAR) - 1);
		LPTSTR pstr = szValue;
		INT nItem1 = _tcstol(pstr, &pstr, 10);    
		INT nItem2 = _tcstol(pstr + 1, &pstr, 10); 	

		SetRange(nItem1, nItem2);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("pos")) == 0)
	{
		INT nPos = _ttol(strValue.c_str());
		SetPos(nPos);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("step")) == 0)
	{
		INT nStep = _ttol(strValue.c_str());
		SetStep(nStep);
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUITrackBarBase::PaintBkgnd(HDC dc)
{
	RECT rtContent = GetContentRect();
	CRefPtr<CImageList> pImageBK = m_uiData.m_pImageBK;
	if(pImageBK.IsNull() 
		|| pImageBK->IsNull() 
		|| pImageBK->GetItemCount() < TRACK_BAR_IMAGE_COUNT)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	if(IsVertical())
	{
		pImageBK->VerticalStretch(dc, rtContent, TRACK_BAR_IMAGE_BK);	
	}
	else
	{
		pImageBK->HorizontalStretch(dc, rtContent, TRACK_BAR_IMAGE_BK);
	}

	INT nThumLen = GetThumbLength();
	INT nThumbPos = GetThumbPos();
	RECT rtDraw = rtContent;
	if(IsVertical())
	{
		//rtDraw.bottom = nThumbPos + rtDraw.top + nThumLen / 2;
		rtDraw.top = rtDraw.bottom - nThumbPos - nThumLen/2;
		pImageBK->VerticalStretch(dc, rtDraw, TRACK_BAR_IMAGE_FORE);	
	}
	else
	{
		rtDraw.right = nThumbPos + rtDraw.left + nThumLen / 2;
		pImageBK->HorizontalStretch(dc, rtDraw, TRACK_BAR_IMAGE_FORE);
	}
}

VOID CDUITrackBarBase::PaintBorder(HDC dc)
{
	if(GetFocus())
	{
		RECT rtFocus = GetContentRect();
		DrawFocusRect(dc, &rtFocus);
	}
}

VOID CDUITrackBarBase::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	RECT rtThumb = GetThumbRect();
	IDUIButton* pThumb = GetThumbButton();
	if(pThumb != NULL)
	{
		pThumb->SetControlRect(rtThumb);
	}
}

INT CDUITrackBarBase::GetThumbLength()
{
	INT nThumbLen = 0;
	CRefPtr<CImageList> pImageThumb = m_uiData.m_pImageThumb;
	if(!pImageThumb.IsNull() && !pImageThumb->IsNull())
	{
		if(IsVertical())
		{
			nThumbLen = pImageThumb->GetIconHeight();
		}
		else
		{
			nThumbLen = pImageThumb->GetIconWidth();
		}
	}

	return nThumbLen;
}

RECT CDUITrackBarBase::GetThumbRect()
{
	RECT rtContent = GetContentRect();
	RECT rtThumb = rtContent;
	CRefPtr<CImageList> pImageThumb = m_uiData.m_pImageThumb;
	if(pImageThumb.IsNull() || pImageThumb->IsNull())
	{
		::SetRectEmpty(&rtThumb);
		return rtThumb;
	}
	
	INT nThumbPos = GetThumbPos();
	if(IsVertical())
	{
		//rtThumb.top = rtContent.top + nThumbPos;
		//rtThumb.bottom = rtThumb.top + GetThumbLength();
		rtThumb.bottom = rtContent.bottom - nThumbPos;
		rtThumb.top = rtThumb.bottom - GetThumbLength();
	}
	else
	{
		rtThumb.left = rtContent.left + nThumbPos;
		rtThumb.right = rtThumb.left + GetThumbLength();	
	}

	::IntersectRect(&rtThumb, &rtContent, &rtThumb);

	return rtThumb;
}

INT CDUITrackBarBase::GetThumbPos()
{
	RECT rtContent = GetContentRect();

	INT nPos = m_nPos;
	DUI_ASSERT(nPos >= m_nMin && nPos <= m_nMax);
	DUI_ASSERT(m_nMin <= m_nMax);
	if(m_nMin > m_nMax) m_nMax = m_nMin;
	if(nPos < m_nMin) nPos = m_nMin;
	if(nPos > m_nMax) nPos = m_nMax;
	if(m_nMin == m_nMax)
	{
		if(IsVertical()) 
			//return rtContent.top;
			return rtContent.bottom;
		else 
			return rtContent.left;
	}

	INT nThumbLen = GetThumbLength();
	INT nAvailable(0);
	if(IsVertical())
	{
		nAvailable	 = rtContent.bottom - rtContent.top - nThumbLen;
	}
	else
	{
		nAvailable = rtContent.right - rtContent.left - nThumbLen;
	}

	if(nAvailable < 0 ) nAvailable = 0;
	double dbRadio = (m_nPos - (double)m_nMin) / (m_nMax - m_nMin);
	return  (INT)(dbRadio * nAvailable);
}

VOID CDUITrackBarBase::SetThumbPos(INT nNewPos)
{
	RECT rtContent = GetContentRect();
	
	INT nThumbLen = GetThumbLength();
	INT nAvailable(0);
	if(IsVertical())
	{
		nAvailable	 = rtContent.bottom - rtContent.top - nThumbLen;
	}
	else
	{
		nAvailable = rtContent.right - rtContent.left - nThumbLen;
	}

	INT nPos(0);
	if(nAvailable > 0)
	{
		double dbRadio = nNewPos / (double)nAvailable;
		nPos = (INT)(dbRadio * (m_nMax - m_nMin)) + m_nMin;		
	}
	
	SetPos(nPos);
}

INT CDUITrackBarBase::CalculateThumbPos(POINT ptHit)
{
	RECT rtContent = GetContentRect();
	INT nThumbLen = GetThumbLength();
	INT nNewPos;
	if(IsVertical())
	{
//		nNewPos = ptHit.y - nThumbLen / 2;
// 		if(nNewPos < rtContent.top) nNewPos = rtContent.top;
// 		if(nNewPos > rtContent.bottom - nThumbLen) nNewPos = rtContent.bottom - nThumbLen;
//		nNewPos -= rtContent.top;
		nNewPos = ptHit.y + nThumbLen / 2;
		if(nNewPos > rtContent.bottom) nNewPos = rtContent.bottom;
		if(nNewPos < rtContent.top + nThumbLen) nNewPos = rtContent.top + nThumbLen;
		nNewPos = rtContent.bottom - nNewPos;
	}
	else
	{
		nNewPos = ptHit.x - nThumbLen / 2;
		if(nNewPos < rtContent.left) nNewPos = rtContent.left;
		if(nNewPos > rtContent.right - nThumbLen) nNewPos = rtContent.right - nThumbLen;
		nNewPos -= rtContent.left;		
	}
	
	if(nNewPos < 0) nNewPos = 0;
	return nNewPos;
}

BOOL CDUITrackBarBase::OnLButtonDown(const DUIEvent& info)
{
	if(info.pSender == this)
	{
		INT nThumbPos = CalculateThumbPos(info.ptMouse);
		SetThumbPos(nThumbPos);
	}
	else 
	{
		//click on thumb
		if(!GetFocus())
		{
			SetFocus(TRUE);
		}

		m_bTracking = TRUE;
	}
	
	return FALSE;
}

BOOL CDUITrackBarBase::OnLButtonUp(const DUIEvent& info)
{
	if(m_bTracking)
	{
		m_bTracking = FALSE;
	}

	return FALSE;
}

BOOL CDUITrackBarBase::OnMouseMove(const DUIEvent& info)
{
	if(m_bTracking)
	{
		INT nThumbPos = CalculateThumbPos(info.ptMouse);
		SetThumbPos(nThumbPos);
	}

	return FALSE;
}

BOOL CDUITrackBarBase::OnKeyDown(const DUIEvent& info)
{
	BOOL bRet(FALSE);
	switch(info.wParam)
	{
	case VK_LEFT:
		{
			if(!IsVertical())
			{
				SetPos(GetPos() - GetStep());
				bRet = TRUE;
			}
		}
		break;

	case VK_DOWN:
		if(IsVertical())
		{
			SetPos(GetPos() - GetStep());
			bRet = TRUE;
		}
		break;

	case VK_RIGHT:
		if(!IsVertical())
		{
			SetPos(GetPos() + GetStep());
			bRet = TRUE;
		}
		break;

	case VK_UP:
		if(IsVertical())
		{
			SetPos(GetPos() + GetStep());
			bRet = TRUE;
		}
		break;
	}

	return bRet;
}

LRESULT CDUITrackBarBase::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);

	switch(info.dwEventType)
	{
	case DUI_EVENT_LBUTTONDOWN:
		if(IsEnabled())
		{
			bHandled = OnLButtonDown(info);
		}
		break;

	case DUI_EVENT_LBUTTONUP:
		if(IsEnabled())
		{
			bHandled = OnLButtonUp(info);
		}
		break;

	case DUI_EVENT_MOUSEMOVE:
		if(IsEnabled())
		{
			bHandled = OnMouseMove(info);
		}
		break;
		
	case DUI_EVENT_KEYDOWN:
		if(IsEnabled())
		{
			bHandled = OnKeyDown(info);			
		}
		break;

	default:
		break;
	}



	if(!bHandled)
	{
		return	theBase::ProcessEvent(info, bHandled);
	}
	else
	{
		return lRet;
	}
}

BOOL CDUITrackBarBase::EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet)
{
	CRefPtr<CImageList> image = m_uiData.m_pImageBK;
	if(!image.IsNull() && !image->IsNull())
	{
		sizeRet.cx = image->GetIconWidth();
		sizeRet.cy = image->GetIconHeight();
	}

	image = m_uiData.m_pImageThumb;
	if(!image.IsNull() && !image->IsNull())
	{
		sizeRet.cx = max(image->GetIconWidth(), sizeRet.cx);
		sizeRet.cy = max(image->GetIconHeight(), sizeRet.cy);
	}
	
	sizeRet.cx += (m_rtMargin.left + m_rtMargin.right);
	sizeRet.cy += (m_rtMargin.top + m_rtMargin.bottom);

	SIZE sizeFixed = GetFixedSize();
	if(sizeFixed.cx != 0)
	{
		sizeRet.cx = sizeFixed.cx;
	}
	if(sizeFixed.cy != 0)
	{
		sizeRet.cy = sizeFixed.cy;
	}

	return TRUE;
}


VOID CDUITrackBarBase::SetRange(INT nMin, INT nMax)
{
	DUI_ASSERT(nMin <= nMax);
	if(m_nMin > m_nMax)
	{
		INT nTemp  = m_nMin;
		m_nMin = m_nMax;
		m_nMax = nTemp;
	}

	m_nMin = nMin;
	m_nMax = nMax;

	if(m_nPos < m_nMin) m_nPos = m_nMin;
	if(m_nPos > m_nMax) m_nPos = m_nMax;	
}

VOID CDUITrackBarBase::GetRange(INT& nMin, INT& nMax) const
{
	nMax = m_nMax;
	nMin = m_nMin;
}

VOID CDUITrackBarBase::SetPos(INT nPos)
{
	//DUI_ASSERT(nPos >= m_nMin && nPos <= m_nMax);
	if(nPos < m_nMin) nPos = m_nMin;
	if(nPos > m_nMax) nPos = m_nMax;

	if(nPos != m_nPos)
	{
		m_nPos = nPos;
		SendNotify(WM_NOTIFY_TRACK_POS_CHANGED, nPos);
		UpdateLayout(TRUE);
	}
}

INT CDUITrackBarBase::GetPos() const
{
	return m_nPos;
}

VOID CDUITrackBarBase::SetStep(INT nStep)
{
	if(nStep >= 1)
	{
		m_nStep = nStep;
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

INT CDUITrackBarBase::GetStep() const
{
	return m_nStep;
}


VOID CDUITrackBarBase::SetUIData(const CTrackBarUIData& data)
{
	m_uiData = data;

	IDUIButton* pThumb = GetThumbButton();
	if(pThumb != NULL)
	{
		CButtonUIData uiData = pThumb->GetButtonUIData();
		uiData.m_imageFore = m_uiData.m_pImageThumb;
		pThumb->SetButtonUIData(uiData);
	}

	UpdateLayout(FALSE);
}

const CTrackBarUIData& CDUITrackBarBase::GetUIData() const
{
	return m_uiData;
}



CRefPtr<IDUIControl> CDUIVerticalTrackBar::Clone()
{
	return new CDUIVerticalTrackBar(*this);
}

BOOL CDUIVerticalTrackBar::IsVertical() const
{
	return TRUE;
}


CRefPtr<IDUIControl> CDUIHorizontalTrackBar::Clone()
{
	return new CDUIHorizontalTrackBar(*this);
}
	
BOOL CDUIHorizontalTrackBar::IsVertical() const
{
	return FALSE;
}

DUI_END_NAMESPCE