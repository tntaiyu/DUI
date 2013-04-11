#include "DUIProgressBar.h"
#include "DUIRenderEngine.h"

DUI_BGN_NAMESPCE

CDUIProgressBarBase::CDUIProgressBarBase()
{
	m_nMax = 0;
	m_nMin = 0;
	m_nPos = 0;
}

CDUIProgressBarBase::~CDUIProgressBarBase()
{

}

LPVOID CDUIProgressBarBase::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IProgressBar) == 0)
	{
		return (IDUIProgressBar*)this;
	}

	return theBase::GetInterface(strName);
}

VOID CDUIProgressBarBase::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("back_image")) == 0)
	{
		CRefPtr<CImageList> pImage = GetSkinMgr()->GetImage(strValue);
		DUI_ASSERT(!pImage.IsNull() && (pImage->GetItemCount() >= PROCRSS_BAR_IMAGE_COUNT));
		
		CProgressBarUIData data;
		data.m_imageBK = pImage;
		SetUIData(data);

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
	

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

BOOL CDUIProgressBarBase::EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet)
{
	CRefPtr<CImageList> image = m_uiData.m_imageBK;
	if(!image.IsNull() && !image->IsNull())
	{
		sizeRet.cx = image->GetIconWidth();
		sizeRet.cy = image->GetIconHeight();
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

VOID CDUIProgressBarBase::GetProgressRect(const RECT& rtTotal, RECT& rt)
{
	INT nPos = m_nPos;
	DUI_ASSERT(nPos >= m_nMin && nPos <= m_nMax);
	DUI_ASSERT(m_nMin <= m_nMax);

	if(m_nMin > m_nMax) m_nMax = m_nMin;
	if(nPos < m_nMin) nPos = m_nMin;
	if(nPos > m_nMax) nPos = m_nMax;
	if(m_nMin == m_nMax)
	{
		DUI_ASSERT(FALSE);
		rt.left = rt.right = rtTotal.left;
		rt.bottom = rt.top = rtTotal.top;
		return;
	}


	INT nLen(0);
	rt = rtTotal;
	if(!IsVertical())
	{
		double dbRadio = (m_nPos - (double)m_nMin) / (m_nMax - m_nMin);
		nLen = (INT)(dbRadio * (rtTotal.right - rtTotal.left));
		rt.right = rt.left + nLen;
	}
	else
	{
		double dbRadio = (m_nPos - (double)m_nMin) / (m_nMax - m_nMin);
		nLen = (INT)(dbRadio * (rtTotal.bottom - rtTotal.top));
		//rt.bottom = rt.top + nLen;
		rt.top = rt.bottom - nLen;
	}
}

VOID CDUIProgressBarBase::PaintBkgnd(HDC dc)
{
	RECT rtContent = GetContentRect();
	
	CRefPtr<CImageList> pImage = m_uiData.m_imageBK;
	if(!pImage.IsNull() 
		&& !pImage->IsNull() 
		&& pImage->GetItemCount() >= PROCRSS_BAR_IMAGE_COUNT)
	{
		if(IsVertical())
		{
			pImage->VerticalStretch(dc, rtContent, PROCRSS_BAR_IMAGE_BK);
		}
		else
		{
			pImage->HorizontalStretch(dc, rtContent, PROCRSS_BAR_IMAGE_BK);
		}
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

VOID CDUIProgressBarBase::PaintForgnd(HDC dc)
{
	RECT rtContent = GetContentRect();
	
	CRefPtr<CImageList> pImage = m_uiData.m_imageBK;
	if(!pImage.IsNull() 
		&& !pImage->IsNull() 
		&& pImage->GetItemCount() >= PROCRSS_BAR_IMAGE_COUNT)
	{
		RECT rtTemp = {0};
		GetProgressRect(rtContent, rtTemp);
		if(!IsRectEmpty(&rtTemp))
		{
			CDUIRenderClip clip2(dc, rtTemp);
			if(IsVertical())
			{
				pImage->VerticalStretch(dc, rtContent, PROCRSS_BAR_IMAGE_FORE);
			}
			else
			{
				pImage->HorizontalStretch(dc, rtContent, PROCRSS_BAR_IMAGE_FORE);
			}
		}
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

VOID CDUIProgressBarBase::SetRange(INT nMin, INT nMax)
{
	DUI_ASSERT(nMin <= nMax);
	if(m_nMin > m_nMax) m_nMax = m_nMin;

	m_nMin = nMin;
	m_nMax = nMax;

	if(m_nPos < m_nMin) m_nPos = m_nMin;
	if(m_nPos > m_nMax) m_nPos = m_nMax;
	
}

VOID CDUIProgressBarBase::GetRange(INT& nMin, INT& nMax) const
{
	nMax = m_nMax;
	nMin = m_nMin;
}

VOID CDUIProgressBarBase::SetPos(INT nPos)
{
	DUI_ASSERT(nPos >= m_nMin && nPos <= m_nMax);
	if(nPos < m_nMin) nPos = m_nMin;
	if(nPos > m_nMax) nPos = m_nMax;
	
	if(m_nPos != nPos)
	{
		m_nPos = nPos;
		SendNotify(WM_NOTIFY_PROGRESS_POS_CHANGED, nPos);
		UpdateLayout(FALSE);
	}
}

INT CDUIProgressBarBase::GetPos() const
{
	return m_nPos;
}

VOID CDUIProgressBarBase::SetUIData(const CProgressBarUIData& data)
{
	m_uiData = data;
	UpdateLayout(FALSE);
}

BOOL CDUIProgressBarBase::OffsetPos(INT nOffset)
{
	BOOL bRet(TRUE);
	INT nPos = m_nPos + nOffset;
	if(nPos > m_nMax) 
	{
		bRet = FALSE;
		nPos = m_nMax;
	}
	if(nPos < m_nMin)
	{
		bRet = FALSE;
		nPos = m_nMin;
	}

	SetPos(nPos);

	return bRet;
}

const CProgressBarUIData& CDUIProgressBarBase::GetUIData() const
{
	return m_uiData;
}


CRefPtr<IDUIControl> CDUIVerticalProgressBar::Clone()
{
	return new CDUIVerticalProgressBar(*this);
}

BOOL CDUIVerticalProgressBar::IsVertical() const
{
	return TRUE;
}

CRefPtr<IDUIControl> CDUIHorizontalProgressBar::Clone()
{
	return new CDUIHorizontalProgressBar(*this);
}

BOOL CDUIHorizontalProgressBar::IsVertical() const
{
	return FALSE;
}

DUI_END_NAMESPCE