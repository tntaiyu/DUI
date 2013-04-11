#include "DUIDecorate.h"
#include "DUIRenderEngine.h"

DUI_BGN_NAMESPCE

CRefPtr<IDUIControl> CDUIPaddingPanel::Clone()
{
	return new CDUIPaddingPanel(*this);
}

CDUIFramePanel::CDUIFramePanel()
{
	m_dwDrawMode = 0;
	ZeroMemory(&m_rtFrame, sizeof(m_rtFrame));
	m_nImageIndex = 0;
}
	
LPVOID CDUIFramePanel::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IFramePanel) == 0)
	{
		return (IDUIFramePanel*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIFramePanel::Clone()
{
	return new CDUIFramePanel(*this);
}

VOID CDUIFramePanel::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("frame_type")) == 0)
	{
		DWORD dwMode(0);
		if(strValue.find(_T("use_color")) != -1)
		{
			dwMode = FRAME_PANEL_SINGLE_COLOR;
		}
		else if(strValue.find(_T("use_image")) != -1)
		{
			dwMode = FRAME_PANEL_USE_IMAGE;
		}

		DUI_ASSERT(dwMode != 0);
		SetDrawMode(dwMode);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("border_color")) == 0)
	{
		m_uiData.m_clrFrame = GetSkinMgr()->GetColor(strValue);
		bHandled = TRUE;
	}
	if(strName.compare(_T("left_border_image")) == 0)
	{
		m_uiData.m_pImageLeft = GetSkinMgr()->GetImage(strValue);	
		bHandled = TRUE;
	}
	else if(strName.compare(_T("right_border_image")) == 0)
	{
		m_uiData.m_pImageRight = GetSkinMgr()->GetImage(strValue);	
		bHandled = TRUE;
	}
	else if(strName.compare(_T("top_border_image")) == 0)
	{
		m_uiData.m_pImageTop = GetSkinMgr()->GetImage(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("bottom_border_image")) == 0)
	{
		m_uiData.m_pImageBottom = GetSkinMgr()->GetImage(strValue);	
		bHandled = TRUE;
	}	
	else if(strName.compare(_T("border_rect")) == 0)
	{
		TCHAR szTextBuffer[128] = {0};
		lstrcpyn(szTextBuffer, strValue.c_str(), sizeof(szTextBuffer) / sizeof(TCHAR) - 1);
		
		RECT rtTemp = {0};
		LPTSTR pstr = NULL;
		rtTemp.left = _tcstol(szTextBuffer, &pstr, 10);
		rtTemp.top = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.right = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.bottom = _tcstol(pstr + 1, &pstr, 10);

		SetFrameRect(rtTemp);

		bHandled = TRUE;
	}


	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}



VOID CDUIFramePanel::GetDrawFrame(RECT& rtLeft, RECT& rtRight, RECT& rtTop, RECT& rtBottom)
{
	RECT rtClient = GetContentRect();
	
	rtTop = rtClient;
	rtTop.bottom = rtTop.top + m_rtFrame.top;
	
	rtBottom = rtClient;
	rtBottom.top = rtBottom.bottom - m_rtFrame.bottom;
	
	rtLeft = rtClient;
	rtLeft.right = rtLeft.left + m_rtFrame.left;
	rtLeft.top = rtTop.bottom;
	rtLeft.bottom = rtBottom.top;
	
	rtRight = rtLeft;
	rtRight.right = rtClient.right;
	rtRight.left = rtRight.right - m_rtFrame.right;
}

VOID CDUIFramePanel::DrawFrameWithColor(HDC dc)
{
	RECT rtLeft, rtRight, rtTop, rtBottom;
	GetDrawFrame(rtLeft, rtRight, rtTop, rtBottom);

 	CDUIRenderEngine::DrawColor(dc, rtLeft, m_uiData.m_clrFrame);
 	CDUIRenderEngine::DrawColor(dc, rtRight, m_uiData.m_clrFrame);
 	CDUIRenderEngine::DrawColor(dc, rtTop, m_uiData.m_clrFrame);
 	CDUIRenderEngine::DrawColor(dc, rtBottom, m_uiData.m_clrFrame);
}

VOID CDUIFramePanel::CalculateImageFrame()
{
	CRefPtr<CImageList>& pLeftImage = m_uiData.m_pImageLeft;
	CRefPtr<CImageList>& pRightImage = m_uiData.m_pImageRight;
	CRefPtr<CImageList>& pTopImage = m_uiData.m_pImageTop;
	CRefPtr<CImageList>& pBottomImage = m_uiData.m_pImageBottom;

	if(!pLeftImage.IsNull() && !pLeftImage->IsNull())
	{
		m_rtFrame.left = pLeftImage->GetIconWidth();
	}
	else
	{
		m_rtFrame.left = 0;
	}

	if(!pRightImage.IsNull() && !pRightImage->IsNull())
	{
		m_rtFrame.right = pRightImage->GetIconWidth();
	}
	else
	{
		m_rtFrame.right = 0;
	}

	if(!pTopImage.IsNull() && !pTopImage->IsNull())
	{
		m_rtFrame.top = pTopImage->GetIconHeight();
	}
	else
	{
		m_rtFrame.top = 0;
	}

	if(!pBottomImage.IsNull() && !pBottomImage->IsNull())
	{
		m_rtFrame.bottom = pBottomImage->GetIconHeight();
	}
	else
	{
		m_rtFrame.bottom = 0;
	}
}

VOID CDUIFramePanel::DrawFrameWithImage(HDC dc)
{
	CalculateImageFrame();
	
	RECT rtLeft, rtRight, rtTop, rtBottom;
	GetDrawFrame(rtLeft, rtRight, rtTop, rtBottom);
	
	CRefPtr<CImageList>& pLeftImage = m_uiData.m_pImageLeft;
	CRefPtr<CImageList>& pRightImage = m_uiData.m_pImageRight;
	CRefPtr<CImageList>& pTopImage = m_uiData.m_pImageTop;
	CRefPtr<CImageList>& pBottomImage = m_uiData.m_pImageBottom;
	if(!pLeftImage.IsNull() && !pLeftImage->IsNull())
	{
		INT nImageIndex = m_nImageIndex;
		if(nImageIndex >= pLeftImage->GetItemCount()) nImageIndex = 0;
		pLeftImage->VerticalStretch(dc, rtLeft, nImageIndex);
	}
	
	if(!pRightImage.IsNull() && !pRightImage->IsNull())
	{
		INT nImageIndex = m_nImageIndex;
		if(nImageIndex >= pRightImage->GetItemCount()) nImageIndex = 0;
		pRightImage->VerticalStretch(dc, rtRight, nImageIndex);
	}
	
	if(!pBottomImage.IsNull() && !pBottomImage->IsNull())
	{
		INT nImageIndex = m_nImageIndex;
		if(nImageIndex >= pBottomImage->GetItemCount()) nImageIndex = 0;
		pBottomImage->HorizontalStretch(dc, rtBottom, nImageIndex);
	}
	
	if(!pTopImage.IsNull() && !pTopImage->IsNull())
	{
		INT nImageIndex = m_nImageIndex;
		if(nImageIndex >= pTopImage->GetItemCount()) nImageIndex = 0;
		pTopImage->HorizontalStretch(dc, rtTop, nImageIndex);
	}
}

VOID CDUIFramePanel::PaintBkgnd(HDC dc)
{
	if(GetDrawMode() & FRAME_PANEL_SINGLE_COLOR)
	{	
		DrawFrameWithColor(dc);
	}
	else 
	{
		DrawFrameWithImage(dc);
	}
}


VOID CDUIFramePanel::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	if(GetDrawMode() != FRAME_PANEL_SINGLE_COLOR)
	{
		CalculateImageFrame();
	}

	RECT rtAvailable = GetContentRect();
	rtAvailable.left += m_rtFrame.left;
	rtAvailable.right -= m_rtFrame.right;
	rtAvailable.top += m_rtFrame.top;
	rtAvailable.bottom -= m_rtFrame.bottom;

	if(IsRectEmpty(&rtAvailable))
	{
		rtAvailable.right = rtAvailable.left;
		rtAvailable.bottom = rtAvailable.top;
	}

	//DUI_ASSERT(GetControlCount() == 1);
	if(GetControlCount() >= 1)
	{
		GetControlByIndex(0)->SetControlRect(rtAvailable);
	}
}
	
VOID CDUIFramePanel::SetFrameRect(const RECT& rtFrame)
{
	m_rtFrame = rtFrame;
}

RECT CDUIFramePanel::GetFrameRect() const
{
	return m_rtFrame;
}

VOID CDUIFramePanel::SetFrameUIData(const CFrameUIData& data)
{
	m_uiData = data;
}

const CFrameUIData& CDUIFramePanel::GetFrameUIData() const
{
	return m_uiData;
}

VOID CDUIFramePanel::SetDrawMode(DWORD dwDrawMode)
{
	m_dwDrawMode = dwDrawMode;
}

DWORD CDUIFramePanel::GetDrawMode() const
{
	return m_dwDrawMode;
}

VOID CDUIFramePanel::SetImageIndex(INT nIndex)
{
	m_nImageIndex = nIndex;
}

INT CDUIFramePanel::GetImageIndex() const
{
	return m_nImageIndex;
}

DUI_END_NAMESPCE