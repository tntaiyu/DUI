#include "DUIWindowBase.h"
#include "DUIDdb.h"

DUI_BGN_NAMESPCE


///////////////////  CDUIWindowBase  //////////////////////////
CDUIWindowBase::CDUIWindowBase()
{

}

CDUIWindowBase::~CDUIWindowBase()
{

}


LRESULT CDUIWindowBase::OnPaint(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	CTimeCostDebug debug(_T("CDUIWindowBase::OnPaint"));

	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint ( m_hWnd, &ps );

	DoPaint ( hDC );

    ::EndPaint ( m_hWnd, &ps );

	return S_OK;
}

VOID CDUIWindowBase::DoPaint(HDC hDC)
{
	RECT rtClip;
	GetClipBox(hDC, &rtClip);
	m_backDib.InvalidateRect(rtClip);

	m_backDib.DoPaint(m_hWnd, hDC);
}

VOID CDUIWindowBase::OnBeforeFlipDIB(CDibSection* pDIB)
{

}

LRESULT CDUIWindowBase::OnDUIFlip(UINT, WPARAM, LPARAM lParam, BOOL& bHandled)
{
	DBLBUFFER* pBuf = (DBLBUFFER*)lParam;
	DUI_ASSERT(pBuf != NULL);
	CDibSection* pDib = pBuf->pBack;
	DUI_ASSERT(pDib != NULL);
	INT nWidth = pDib->GetWidth();
	INT nHeight = pDib->GetHeight();

	OnBeforeFlipDIB(pDib);

	if(GetExStyle() & WS_EX_LAYERED)
	{
        BLENDFUNCTION bf = { 0 };
        bf.AlphaFormat = AC_SRC_ALPHA; 
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = 255;

		POINT ptSrc = { 0, 0 };
		SIZE sz = { nWidth, nHeight };
	
		CBitmapCommonFunction::UpdateLayeredWindow(m_hWnd, pBuf->hFront, 
			NULL, &sz, pBuf->hBack, &ptSrc, 0, &bf, ULW_ALPHA);
	}
	else
	{
		BitBlt(pBuf->hFront, 0, 0, nWidth, 
			nHeight, pBuf->hBack, 0, 0, SRCCOPY);
	}

	return S_OK;
}

LRESULT CDUIWindowBase::OnPrintClient(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	HDC dc = (HDC)wParam;

	PaintFrameBkgnd(dc);
	PaintClientArea(dc);

	return S_OK;
}

LRESULT CDUIWindowBase::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	return TRUE;
}


VOID CDUIWindowBase::PaintFrameBkgnd(HDC dc)
{

}

VOID CDUIWindowBase::PaintClientArea(HDC dc)
{

}


BOOL CDUIWindowBase::Invalidate(BOOL bErase/* = TRUE*/)
{
	DUI_ASSERT(IsWindow());

	RECT rtClient;
	GetClientRect(&rtClient);

	if(rtClient.right - rtClient.left > 0
		&& rtClient.bottom - rtClient.top > 0)
	{
		InvalidateRect(&rtClient, bErase);
	}

	return TRUE;	
}

BOOL CDUIWindowBase::InvalidateRect(LPCRECT lpRect, BOOL bErase/* = TRUE*/)
{
	DUI_ASSERT(IsWindow());

	//m_backDib.InvalidateRect(*lpRect); //move to onpain
	::InvalidateRect(m_hWnd, lpRect, FALSE);

	if(GetExStyle() & WS_EX_LAYERED)
	{
		PostMessage(WM_PAINT, 0, 0);
	}

	return TRUE;
}

void CDUIWindowBase::InvalidateRgn(HRGN hRgn, BOOL bErase/* = TRUE*/)
{
	DUI_ASSERT(IsWindow());
	
	//m_backDib.InvalidateRgn(hRgn); //move to onpain
	::InvalidateRgn(m_hWnd, hRgn, FALSE);
	
	if(GetExStyle() & WS_EX_LAYERED)
	{
		PostMessage(WM_PAINT, 0, 0);
	}
}


BOOL CDUIWindowBase::GetDUIClientRect(LPRECT lpRect)
{
	DUI_ASSERT(IsWindow() && lpRect != NULL);
	if(lpRect == NULL) return FALSE;
	ZeroMemory(lpRect, sizeof(RECT));
	if(!IsWindow()) return FALSE;
	
	GetClientRect(lpRect);
	
	return TRUE;
}	

///////////////////  CDUIWindowBase  //////////////////////////

DUI_END_NAMESPCE