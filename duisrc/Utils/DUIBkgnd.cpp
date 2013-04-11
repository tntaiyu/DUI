#include "DUIBkgnd.h"
#include "DUIRenderEngine.h"
#include "DUIDdb.h"

DUI_BGN_NAMESPCE

////////////////////  CDUIColorBK  /////////////////////////////
CDUIColorBK::CDUIColorBK()
{
	m_bVertical = TRUE;
	m_dbPos = 0.5;
}

VOID CDUIColorBK::SetBKColor(const CARGB& clr)
{
	m_clrBK = clr;
}

VOID CDUIColorBK::SetBKColor(const CARGB& clr1, const CARGB& clr2, BOOL bVertical/* = TRUE*/)
{
	m_clrBK1 = clr1;
	m_clrBK2 = clr2;
	m_bVertical = bVertical;
}

VOID CDUIColorBK::SetBKColor(const CARGB& clr1, const CARGB& clr2, const CARGB& clrMid, double dbPos/* = 0.5*/, BOOL bVertical /*= TRUE*/)
{
	m_clrBK1 = clr1;
	m_clrBK2 = clr2;
	m_clrMid = clrMid;
	m_bVertical = bVertical;
	m_dbPos = dbPos;
}

VOID CDUIColorBK::SetBKVertical(BOOL bVertical)
{
	m_bVertical = bVertical;
}

VOID CDUIColorBK::SetBKSepPos(double dbPos)
{
	m_dbPos = dbPos	;
}

CARGB CDUIColorBK::GetBKColor() const
{
	return m_clrBK;
}

VOID CDUIColorBK::GetBKColor(CARGB& clr1, CARGB& clr2, BOOL& bVertical)
{
	 clr1 = m_clrBK1;
	 clr2 = m_clrBK2;
	 bVertical = m_bVertical;
}

VOID CDUIColorBK::GetBKCOlor(CARGB& clr1, CARGB& clr2, CARGB& clrMid, double& dbPos, BOOL& bVertical)
{
	 clr1 = m_clrBK1;
	 clr2 = m_clrBK2;
	 clrMid = m_clrMid;
	 bVertical= m_bVertical;
	 dbPos = m_dbPos;
}

BOOL CDUIColorBK::IsOneColorEmpty() const
{
	return m_clrBK.IsEmpty();
}

BOOL CDUIColorBK::IsTwoColorEmpty() const
{
	return m_clrBK1.IsEmpty() 
		|| m_clrBK2.IsEmpty();
}

BOOL CDUIColorBK::IsTreeColorEmpty() const
{
	return m_clrBK1.IsEmpty() 
		|| m_clrBK2.IsEmpty() 
		|| m_clrMid.IsEmpty();
}

BOOL CDUIColorBK::IsEmpty() const
{
	return IsOneColorEmpty()
		&& IsTwoColorEmpty()
		&& IsTreeColorEmpty();
}

VOID CDUIColorBK::DrawBK(HDC dc, const RECT& rt)
{
	if(!IsTreeColorEmpty())
	{
		INT nWidth = rt.right - rt.left;
		INT nHeight = rt.bottom - rt.top;
		RECT rt1 = rt, rt2 = rt;
		if(m_dbPos > 1.0)
		{
			if(m_bVertical)
			{
				rt1.bottom  = (LONG)(rt1.top + m_dbPos);
				rt2.top = rt1.bottom;
				if(rt2.top > rt2.bottom) rt2.bottom = rt2.top;
			}
			else
			{
				rt1.right = (LONG)(rt1.left + m_dbPos);
				rt2.left = rt1.right;
				if(rt2.left > rt2.right) rt2.right = rt2.left;
			}
		}
		else if(m_dbPos >= 0.0 && m_dbPos <= 1.0 )
		{
			
			if(m_bVertical)
			{
				INT nPadding = (INT)(nHeight * m_dbPos);
				rt1.bottom  = rt1.top + nPadding;
				rt2.top = rt1.bottom;
				if(rt2.top > rt2.bottom) rt2.bottom = rt2.top;
			}
			else
			{
				INT nPadding = (INT)(nWidth * m_dbPos);
				rt1.right = rt1.left + nPadding;
				rt2.left = rt1.right;
				if(rt2.left > rt2.right) rt2.right = rt2.left;
			}
		}

		CDUIRenderEngine::DrawGradien(dc, rt1, m_clrBK1, m_clrMid, m_bVertical);
		CDUIRenderEngine::DrawGradien(dc, rt2, m_clrMid, m_clrBK2,  m_bVertical);
	}
	else if(!IsTwoColorEmpty())
	{
		CDUIRenderEngine::DrawGradien(dc, rt, m_clrBK1, m_clrBK2, m_bVertical);
	}
	else if(!IsOneColorEmpty())
	{
		CDUIRenderEngine::DrawColor(dc, rt, m_clrBK);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}
////////////////////  CDUIColorBK  /////////////////////////////



////////////////////  CDUIColorImageBK  /////////////////////////////
VOID CDUIColorImageBK::SetSkinImage(CRefPtr<CImageList> pImage)
{
	m_pImageSkin = pImage;
}

CRefPtr<CImageList> CDUIColorImageBK::GetSkinImage() const
{
	return m_pImageSkin;
}


BOOL CDUIColorImageBK::IsEmpty() const
{
	return theBase::IsEmpty()
		&& (m_pImageSkin.IsNull() || m_pImageSkin->IsNull());
}

VOID CDUIColorImageBK::SetSkinImagePos(const CFrameBKImagePos& pos)
{
	m_skinImagePos = pos;
}

CFrameBKImagePos CDUIColorImageBK::GetSkinImagePos() const
{
	return m_skinImagePos;
}

RECT CDUIColorImageBK::GetSkinImageRect(RECT rtBK)
{
	RECT rtWaterMark = rtBK;
	CRefPtr<CImageList> pImage = m_pImageSkin;
	if(pImage.IsNull() || pImage->IsNull()) return rtWaterMark;

	INT nOffsetX = m_skinImagePos.m_nOffsetX;
	INT nOffsetY = m_skinImagePos.m_nOffsetY;
	FRAME_SKIN_IMAGE_POS ePosType = m_skinImagePos.m_ePosType;

	SIZE sizeIcon = {pImage->GetIconWidth(), pImage->GetIconHeight()};
	if(ePosType == SKIN_POS_LEFTTOP)
	{	
		rtWaterMark.left += nOffsetX;
		rtWaterMark.top += nOffsetY;
		rtWaterMark.right = rtWaterMark.left + sizeIcon.cx;
		rtWaterMark.bottom = rtWaterMark.top + sizeIcon.cy;
	}
	else if(ePosType == SKIN_POS_LEFTBOTTOM)
	{
		rtWaterMark.left += nOffsetX;
		rtWaterMark.bottom -= nOffsetY;
		rtWaterMark.right = rtWaterMark.left + sizeIcon.cx;
		rtWaterMark.top = rtWaterMark.bottom - sizeIcon.cy;
	}
	else if(ePosType == SKIN_POS_RIGHTTOP)
	{
		rtWaterMark.right -= nOffsetX;
		rtWaterMark.top += nOffsetY;
		rtWaterMark.left = rtWaterMark.right - sizeIcon.cx;
		rtWaterMark.bottom = rtWaterMark.top + sizeIcon.cy;
	}
	else if(ePosType == SKIN_POS_RIGHTBOTTOM)
	{
		rtWaterMark.right -= nOffsetX;
		rtWaterMark.bottom -= nOffsetY;
		rtWaterMark.left = rtWaterMark.right - sizeIcon.cx;
		rtWaterMark.top = rtWaterMark.bottom - sizeIcon.cy;
	}
	else if(ePosType == SKIN_POS_CENTER)
	{
		if(rtWaterMark.right - rtWaterMark.left > sizeIcon.cx)
		{
			rtWaterMark.left += (rtWaterMark.right - rtWaterMark.left - sizeIcon.cx) / 2;
		}
		if(rtWaterMark.bottom - rtWaterMark.top > sizeIcon.cy)
		{
			rtWaterMark.top += (rtWaterMark.bottom - rtWaterMark.top - sizeIcon.cy) / 2;
		}
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	RECT rtInterset = {0};
	::IntersectRect(&rtInterset, &rtWaterMark, & rtBK);
	if(IsRectEmpty(&rtInterset))
	{
		rtWaterMark.right = rtWaterMark.left;
		rtWaterMark.bottom = rtWaterMark.top;
	}

	return rtWaterMark;
}

VOID CDUIColorImageBK::DrawBK(HDC dc, const RECT& rt)
{
	theBase::DrawBK(dc, rt);
	if(!m_pImageSkin.IsNull()
		&& !m_pImageSkin->IsNull())
	{
		RECT rtDraw = GetSkinImageRect(rt);
		m_pImageSkin->Draw(dc, rtDraw, 0);
	}
}

////////////////////  CDUIColorImageBK  /////////////////////////////


////////////////////  CDUIFrameBK  /////////////////////////////
CDUIFrameBK::CDUIFrameBK()
{
	m_nCornerHeight = 0;
	m_nCornerWidth = 0;
	ZeroMemory(&m_rtFrameBorder, sizeof(m_rtFrameBorder));
}

VOID CDUIFrameBK::SetFrameColor(const CARGB& clrFrame)
{
	m_clrFrame = clrFrame;
}


VOID CDUIFrameBK::SetFrameRound(INT nCornerWidth, INT nCornerHeight)
{
	m_nCornerHeight = nCornerHeight;
	m_nCornerWidth = nCornerWidth;
}

VOID CDUIFrameBK::SetFrameImage(CRefPtr<CImageList> pImageFrame)
{
	m_pImageBorder = pImageFrame;
}

CRefPtr<CImageList> CDUIFrameBK::GetFrameImage() const
{
	return m_pImageBorder;
}

VOID CDUIFrameBK::SetFrameBorder(const RECT& rtFrame)
{
	m_rtFrameBorder = rtFrame;
}
	
CARGB CDUIFrameBK::GetFrameColor() const
{
	return m_clrFrame;
}

RECT CDUIFrameBK::GetFrameBorder() const
{
	return m_rtFrameBorder;
}

VOID CDUIFrameBK::GetFrameRound(INT& nCornerWidth, INT& nCornerHeight)
{
	nCornerWidth = m_nCornerWidth;
	nCornerHeight = m_nCornerHeight;
}

BOOL CDUIFrameBK::IsFrameEmpty() const
{
	return m_rtFrameBorder.left == 0
		&& m_rtFrameBorder.right == 0
		&& m_rtFrameBorder.top == 0
		&& m_rtFrameBorder.bottom == 0;
}

BOOL CDUIFrameBK::IsColorFrameAvailable() const 
{
	return !IsFrameEmpty()
		&& !m_clrFrame.IsEmpty();
}

BOOL CDUIFrameBK::IsImageFrameAvailable() const 
{
	return (!IsFrameEmpty()
		&& !m_pImageBorder.IsNull() 
		&& !m_pImageBorder->IsNull());
}

BOOL CDUIFrameBK::IsEmpty() const
{
	return (theBase::IsEmpty()
		&& !IsColorFrameAvailable()
		&& !IsImageFrameAvailable());
}

VOID CDUIFrameBK::DrawBK(HDC dc, const RECT& rt)
{
	HRGN hRgnOld = NULL;
	HRGN hRgnNew = NULL;
		
	if(IsImageFrameAvailable())
	{
		theBase::DrawBK(dc, rt);

		RECT rtClient = rt;
		rtClient.left += m_rtFrameBorder.left;
		rtClient.right -= m_rtFrameBorder.right;
		rtClient.top += m_rtFrameBorder.top;
		rtClient.bottom -= m_rtFrameBorder.bottom;
		
		if(::IsRectEmpty(&rtClient))
		{
			m_pImageBorder->MiddleStretch(dc, rt, 0);
		}
		else
		{
			hRgnNew = CreateRectRgn(rt.left, rt.top, rt.right, rt.bottom);
			HRGN hRngClient = CreateRectRgn(rtClient.left, rtClient.top, rtClient.right, rtClient.bottom);
			if(hRgnNew != NULL 
				&& hRngClient != NULL) 
			{
				//do not draw on the client area
				INT nRet = ::CombineRgn(hRgnNew, hRgnNew, hRngClient, RGN_DIFF);
				if(nRet != ERROR)
				{
					hRgnOld = CreateRectRgn(0, 0, 0, 0);
					if(GetClipRgn(dc, hRgnOld) == 1)
					{
						::CombineRgn(hRgnNew, hRgnNew, hRgnOld, RGN_AND);
						SelectClipRgn(dc, hRgnNew);
					}
					else
					{
						DUI_ASSERT(FALSE);
						DeleteObject(hRgnOld);
						hRgnOld = NULL;
					}
				}
			}

			m_pImageBorder->MiddleStretch(dc, rt, 0);

			if(hRngClient != NULL)
			{
				DeleteObject(hRngClient);
				hRngClient = NULL;
			}
		}
	}
	else if(IsColorFrameAvailable())
	{
		if(m_nCornerWidth > 0 && m_nCornerHeight > 0)
		{
			hRgnNew = CreateRoundRectRgn(rt.left, rt.top, rt.right, 
				rt.bottom, m_nCornerWidth, m_nCornerHeight);
			
			if(hRgnNew != NULL) //do not draw out of the round rect
			{
				hRgnOld = CreateRectRgn(0, 0, 0, 0);
				if(GetClipRgn(dc, hRgnOld) == 1)
				{
					::CombineRgn(hRgnNew, hRgnNew, hRgnOld, RGN_AND);
					SelectClipRgn(dc, hRgnNew);
				}
				else
				{
					DUI_ASSERT(FALSE);
					DeleteObject(hRgnOld);
					hRgnOld = NULL;
				}
			}
		}

		theBase::DrawBK(dc, rt);

		if(m_nCornerWidth > 0 && m_nCornerHeight > 0)
		{
			INT nFrameWidth = m_rtFrameBorder.top;
			CDUIRenderEngine::DrawRoundRectBorder(dc, rt, m_nCornerWidth
				, m_nCornerHeight, nFrameWidth, m_clrFrame);
		}
		else
		{
			CDUIRenderEngine::DrawRectangleBorder(dc, rt, m_rtFrameBorder, m_clrFrame);
		}
	}
	else
	{
		theBase::DrawBK(dc, rt);
	}

	if(hRgnOld != NULL)
	{
		SelectClipRgn(dc, hRgnOld);
		DeleteObject(hRgnOld);
		hRgnOld = NULL;
	}

	if(hRgnNew != NULL)
	{
		DeleteObject(hRgnNew);
		hRgnNew = NULL;
	}
}


BOOL CDUIFrameBK::SetWinFrameRgn(HWND hWnd)
{
	BOOL bRet(FALSE);
	if(hWnd == NULL || !IsWindow(hWnd))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	RECT rtClient = {0};
	GetClientRect(hWnd, &rtClient);

	if(IsImageFrameAvailable())
	{
		HDC dc = GetDC(hWnd);
		HBITMAP hBmpBK = m_pImageBorder->GetIconBitmap(dc, 0);
		if(hBmpBK != NULL)
		{
			SIZE sizeClient = { rtClient.right - rtClient.left, rtClient.bottom - rtClient.top};
			HBITMAP hBmpNew = CBitmapCommonFunction::GetMiddleStretchBitmap(dc, hBmpBK, sizeClient);
			if(hBmpNew != NULL)
			{
				HRGN hRgn = CBitmapCommonFunction::BitmapToRgn(hBmpNew, m_pImageBorder->GetColorKey());
				if(hRgn != NULL)
				{
					::SetWindowRgn(hWnd, hRgn, TRUE);
					bRet = TRUE;
				}
				DeleteObject(hBmpNew);
			}
		}	

		DeleteObject(hBmpBK);	
		ReleaseDC(hWnd, dc);
	}
	else if(IsColorFrameAvailable())
	{
		if(m_nCornerHeight > 0 && m_nCornerWidth > 0)
		{
			HRGN	hRgnNew = CreateRoundRectRgn(rtClient.left, rtClient.top, rtClient.right, 
			rtClient.bottom, m_nCornerWidth, m_nCornerHeight);
			if(hRgnNew != NULL)
			{
				::SetWindowRgn(hWnd,  hRgnNew, TRUE);
				bRet = TRUE;
			}
		}
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	return bRet;
}

DUI_END_NAMESPCE