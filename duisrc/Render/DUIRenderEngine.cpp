#include "DUIRenderEngine.h"
#include "DUIDdb.h"

DUI_BGN_NAMESPCE

CDUIRenderClip::CDUIRenderClip(HDC dc, RECT& rtItem)
{
   RECT rcClip = { 0 };
   ::GetClipBox(dc, &rcClip);
   m_hRgnOld = ::CreateRectRgnIndirect(&rcClip);
   m_hRgnNew = ::CreateRectRgnIndirect(&rtItem);
   ::ExtSelectClipRgn(dc, m_hRgnNew, RGN_AND);
   m_dc = dc;
}

CDUIRenderClip::~CDUIRenderClip()
{
   DUI_ASSERT(::GetObjectType(m_dc)==OBJ_DC || ::GetObjectType(m_dc)==OBJ_MEMDC);
   DUI_ASSERT(::GetObjectType(m_hRgnNew)==OBJ_REGION);
   DUI_ASSERT(::GetObjectType(m_hRgnOld)==OBJ_REGION);

   ::SelectClipRgn(m_dc, m_hRgnOld);
   ::DeleteObject(m_hRgnOld);
   ::DeleteObject(m_hRgnNew);		
}


//this function need improvement, do not use alpha channel here
VOID CDUIRenderEngine::DrawRoundRectBorder(HDC dc, const RECT& rt, INT nCornerWidth, 
										   INT nCornerHeight, INT nBorderWidth, const CARGB& clr)
{
	if(nBorderWidth <= 0
		|| IsRectEmpty(&rt))
	{
		DUI_ASSERT(FALSE);
		return;
	}

	COLORREF clrBorder = clr.GetColor();
	HPEN hNewPen = CreatePen(PS_SOLID|PS_INSIDEFRAME, nBorderWidth, clrBorder);
	HPEN hOldPen = (HPEN)SelectObject(dc, hNewPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
	RoundRect(dc, rt.left, rt.top, rt.right-1, rt.bottom-1, nCornerWidth, nCornerHeight);
	SelectObject(dc, hOldBrush);
	SelectObject(dc, hOldPen);
	DeleteObject(hNewPen);
}

VOID CDUIRenderEngine::DrawRectangleBorder(HDC dc, const RECT& rt, const RECT& rtBorder, const CARGB& clr)
{
	DUI_ASSERT(rtBorder.left >= 0 && rtBorder.top >= 0
		&& rtBorder.right >= 0 && rtBorder.bottom >= 0);


	INT nWidth = rt.right - rt.left;
	INT nHeight = rt.bottom - rt.top;
	if(rtBorder.left + rtBorder.right >= nWidth
		|| rtBorder.top + rtBorder.bottom >= nHeight)
	{
		DrawColor(dc, rt, clr);
		return;
	}

	RECT rtTop = rt;
	rtTop.bottom = rtTop.top + rtBorder.top;
	RECT rtBottom = rt;
	rtBottom.top = rtBottom.bottom - rtBorder.bottom;
	RECT rtLeft = rt;
	rtLeft.top = rtTop.bottom;
	rtLeft.bottom = rtBottom.top;
	rtLeft.right = rtLeft.left + rtBorder.left;
	RECT rtRight = rtLeft;
	rtRight.right = rt.right;
	rtRight.left = rtRight.right - rtBorder.right;
	
	DrawColor(dc, rtLeft, clr);
	DrawColor(dc, rtRight, clr);
	DrawColor(dc, rtTop, clr);
	DrawColor(dc, rtBottom, clr);
}

VOID CDUIRenderEngine::DrawColor(HDC dc, const RECT& rt, const CARGB& clr)
{
	if(IsRectEmpty(&rt)) return;

	BYTE alpha = clr.GetA();
	if(alpha == 0 || alpha == 0xFF) // not use alpha chanel
	{
		::SetBkColor(dc, clr.GetColor());
		::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rt, NULL, 0, NULL);	
	}
	else
	{
		CDibSection newDIB;
		if(newDIB.Create(1, 1, 32))
		{
			newDIB.SetPixel(0, 0, clr);
		}
		CBitmapCommonFunction::AlphaBlend(dc, newDIB.GetBitmap(), rt, alpha);
	}
}

VOID CDUIRenderEngine::DrawGradien(HDC dc, const RECT& rt, const CARGB& clrStart, const CARGB& clrEnd, BOOL bVertical)
{
	if(IsRectEmpty(&rt)) return;

	BYTE alpha1 = clrStart.GetA();
	BYTE alpha2 = clrEnd.GetA();

	BYTE alpha = (alpha1 + alpha2) / 2;
	COLORREF clr1 = clrStart.GetColor();
	COLORREF clr2 = clrEnd.GetColor();
	if(alpha == 0 || alpha == 0xFF)
	{
		GradientFill(dc, rt, clr1, clr2, bVertical);
		return;
	}

	INT nWidth = rt.right - rt.left;
	INT nHeight = rt.bottom - rt.top;
	HDC dcMem = CreateCompatibleDC(dc);
	CDibSection newDIB;
	if(!newDIB.Create(nWidth, nHeight, 32)) return;
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(dcMem, newDIB.GetBitmap());
	RECT rtDraw = {0, 0, nWidth, nHeight};
	GradientFill(dcMem, rtDraw, clr1, clr2, bVertical);
	 BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
	AlphaBlend(dc, rt.left, rt.top, nWidth, nHeight, dcMem, 0, 0, nWidth, nHeight, bf);
	::SelectObject(dcMem, hBmpOld);
	DeleteDC(dcMem);
}

VOID CDUIRenderEngine::GradientFill(HDC dc, const RECT& rtDraw, COLORREF clrStart, COLORREF clrEnd, BOOL bVertical)
{
	TRIVERTEX vert[2] ;
	GRADIENT_RECT    gRect;

	vert [0] .x      = rtDraw.left;
	vert [0] .y      = rtDraw.top;
	vert [0] .Red    = GetRValue(clrStart) << 8;
	vert [0] .Green  = GetGValue(clrStart) << 8;
	vert [0] .Blue   = GetBValue(clrStart) << 8;
	vert [0] .Alpha  = 0xFF00;

	vert [1] .x      = rtDraw.right;
	vert [1] .y      = rtDraw.bottom; 
	vert [1] .Red    = GetRValue(clrEnd) << 8;
	vert [1] .Green  = GetGValue(clrEnd) << 8;
	vert [1] .Blue   = GetBValue(clrEnd) << 8;
	vert [1] .Alpha  = 0xFF00;
	
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	
	if(!bVertical)
	{
		::GradientFill(dc,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
	}
	else
	{
		::GradientFill(dc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
	}	
}

SIZE CDUIRenderEngine::GetTextIdealSize(const CDUIString& strText, HFONT hFont, INT nWidthLimit /* = 0 */, UINT uFormat /* = DT_LEFT | DT_WORDBREAK */)
{
	DUI_ASSERT(hFont != NULL);

	RECT rtText = {0, 0, nWidthLimit, 0};
	
	uFormat |= DT_SINGLELINE;
	uFormat &= ~DT_WORDBREAK;

	HDC dc = GetDC(NULL);
	HFONT hFontOld = (HFONT)SelectObject(dc, hFont);
	DrawText(dc, strText.c_str(), strText.size(), &rtText, uFormat | DT_CALCRECT);
	SelectObject(dc, hFontOld);
	::ReleaseDC(NULL, dc);

	SIZE sizeRet = { rtText.right - rtText.left, rtText.bottom - rtText.top };
	return sizeRet;
}

INT CDUIRenderEngine::DrawText( HDC hDC, LPCTSTR lpString, INT nCount, LPRECT lpRect, UINT uFormat)
{
	return ::DrawText(hDC, lpString, nCount, lpRect, uFormat);
}

DUI_END_NAMESPCE