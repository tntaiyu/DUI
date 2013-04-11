#include "DUIBitBuffer.h"
#include "DUILib.h"

DUI_BGN_NAMESPCE

CBitBuffer::CBitBuffer()
{
	m_hRgn = NULL;
}

CBitBuffer::~CBitBuffer()
{
	Reset();
}

CDibSection* CBitBuffer::GetDIB()
{
	return &m_dib;
}

VOID CBitBuffer::Reset()
{
	if(m_hRgn != NULL)
	{
		DeleteObject(m_hRgn);
		m_hRgn = NULL;
	}
	
	if(!m_dib.IsNull())
	{
		m_dib.DeleteObject();
	}
}

VOID CBitBuffer::InvalidateRect(const RECT& rt)
{
	if(m_dib.IsNull()) return;

	if(m_hRgn == NULL)
	{
		m_hRgn = CreateRectRgn(rt.left, rt.top, rt.right, rt.bottom);
	}
	else
	{
		HRGN hRgnNew = CreateRectRgn(rt.left, rt.top, rt.right, rt.bottom);
		if(hRgnNew != NULL)
		{
			::CombineRgn(m_hRgn, m_hRgn, hRgnNew, RGN_OR);
			DeleteObject(hRgnNew);
		}
	}
}

VOID CBitBuffer::InvalidateRgn(HRGN hRgn)
{
	if(m_dib.IsNull()) return;

	if(m_hRgn == NULL)
	{
		m_hRgn = CreateRectRgn(0, 0, 0, 0);
	}
	
	if(m_hRgn != NULL && hRgn != NULL)
	{
		::CombineRgn(m_hRgn, m_hRgn, hRgn, RGN_OR);
	}
}

VOID CBitBuffer::DoPaint(HWND hWnd, HDC hDC)
{
	RECT rtClient  = {0};
	GetClientRect(hWnd, &rtClient);

	INT nWidth = rtClient.right - rtClient.left;
	INT nHeight = rtClient.bottom - rtClient.top;

	if(nWidth <=0 || nHeight <=0) return;

	const INT nBits = 32;
	BOOL bFill(FALSE);
	if(m_dib.IsNull())
	{
		m_dib.Create(nWidth, nHeight, nBits);
		bFill = TRUE;
	}
	if(m_dib.IsNull()) return;
	
	HDC dcMem = CreateCompatibleDC(hDC);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(dcMem, m_dib.GetBitmap());
	HRGN hRgnDraw = m_hRgn;
	m_hRgn = NULL;

	if(bFill || hRgnDraw != NULL)
	{
		INT nDC = SaveDC(dcMem);
		
		if(hRgnDraw != NULL)
		{
			::SelectClipRgn(dcMem, hRgnDraw);
		}
		else
		{
			HRGN hRgnNew = CreateRectRgn(rtClient.left, rtClient.top, rtClient.right, rtClient.bottom);
			if(hRgnNew != NULL)
			{
				::SelectClipRgn(dcMem, hRgnNew);
				DeleteObject(hRgnNew);
			}
		}

		::SendMessage(hWnd, WM_PRINTCLIENT, (WPARAM)dcMem
			, PRF_NONCLIENT | PRF_ERASEBKGND | PRF_CLIENT | PRF_CHECKVISIBLE | PRF_CHILDREN);

		::RestoreDC(dcMem, nDC);
	}
	
	if(hRgnDraw != NULL)
	{
		DeleteObject(hRgnDraw);
		hRgnDraw = NULL;
	}

	//draw it next time if invalidate rect when send WM_PRINT message
	if(m_hRgn != NULL)
	{
		ExtSelectClipRgn(hDC, m_hRgn, RGN_DIFF);
	}
	
	DBLBUFFER buf = { hDC, dcMem, m_hRgn, &m_dib };
	SendMessage(hWnd, WM_DUI_FLIP, 0, (LPARAM)&buf);

	if(hBmpOld != NULL)
	{
		::SelectObject(dcMem, hBmpOld);
	}
	DeleteDC(dcMem);
}


DUI_END_NAMESPCE