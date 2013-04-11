#include "DUIDdb.h"
#include "DUIDib.h"


DUI_BGN_NAMESPCE

////////////////  CBitmapCommonFunction  ///////////////////////////
BOOL CBitmapCommonFunction::GetBitmapSize(HBITMAP hBitmap, SIZE& sizeRet)
{
	DUI_ASSERT(hBitmap != NULL && GetObjectType(hBitmap) == OBJ_BITMAP);
	if(hBitmap == NULL) return FALSE;

	BITMAP bmp = {0};
	GetObject(hBitmap, sizeof(bmp), &bmp);
	sizeRet.cx = bmp.bmWidth;
	sizeRet.cy = bmp.bmHeight;

	return TRUE;
}

BYTE CBitmapCommonFunction::GetBitmapBitCount(HBITMAP hBitmap)
{
	DUI_ASSERT(hBitmap != NULL && GetObjectType(hBitmap) == OBJ_BITMAP);
	
	BITMAP bmp = {0};
	GetObject(hBitmap, sizeof(bmp), &bmp);
	
	return (BYTE)bmp.bmBitsPixel;
}

HBITMAP CBitmapCommonFunction::GetSubBitmap(HDC dc, HBITMAP hBitmapSrc, RECT& rt)
{
	BOOL bRet(FALSE);
	if(::IsRectEmpty(&rt)) return NULL;
	
	DUI_ASSERT(hBitmapSrc != NULL && GetObjectType(hBitmapSrc) == OBJ_BITMAP);
	if(hBitmapSrc == NULL) return NULL;
	
	BITMAP bmpInfo = {0};
	GetObject(hBitmapSrc, sizeof(bmpInfo), &bmpInfo);
	
	if(rt.right > bmpInfo.bmWidth) 
	{
		DUI_ASSERT(FALSE);
		rt.right = bmpInfo.bmWidth;
	}
	if(rt.bottom > bmpInfo.bmHeight)
	{
		DUI_ASSERT(FALSE);
		rt.bottom = bmpInfo.bmHeight;
	}
	
	
	HDC dcMemSrc = CreateCompatibleDC(dc);
	HDC dcMemDst = CreateCompatibleDC(dc);
	HBITMAP hBmpOldSrc = (HBITMAP)::SelectObject(dcMemSrc, hBitmapSrc);
	CDibSection newDIB;
 	newDIB.Create(rt.right - rt.left, rt.bottom - rt.top, 32);
	if(!newDIB.IsNull())
	{
 		HBITMAP hBmpOldDst = (HBITMAP)::SelectObject(dcMemDst, newDIB.GetBitmap());
		BitBlt(dcMemDst, 0, 0, rt.right - rt.left, rt.bottom - rt.top, dcMemSrc, rt.left, rt.top, SRCCOPY);
		::SelectObject(dcMemDst, hBmpOldDst);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	::SelectObject(dcMemSrc, hBmpOldSrc);
	DeleteDC(dcMemDst);
	DeleteDC(dcMemSrc);
	
	return newDIB.Detach();
}

HBITMAP CBitmapCommonFunction::GetMiddleStretchBitmap(HDC dc, HBITMAP hBitmapSrc, INT nLen, BOOL bVert)
{
	DUI_ASSERT(hBitmapSrc != NULL && GetObjectType(hBitmapSrc) == OBJ_BITMAP);
	if(hBitmapSrc == NULL) return NULL;
	
	SIZE sizeBmp = { 0 };
	GetBitmapSize(hBitmapSrc, sizeBmp);

	if(!bVert)
	{
		DUI_ASSERT(nLen > sizeBmp.cx);
		RECT rt1 = {0, 0, sizeBmp.cx / 2 - 1, sizeBmp.cy};
		RECT rt2 = {sizeBmp.cx / 2 - 1, 0, sizeBmp.cx / 2 + 1, sizeBmp.cy};
		RECT rt3 = { sizeBmp.cx / 2 + 1, 0, sizeBmp.cx, sizeBmp.cy };
		HBITMAP bmpLeft = GetSubBitmap(dc, hBitmapSrc, rt1);
		HBITMAP bmpMiddle = GetSubBitmap(dc, hBitmapSrc, rt2);
		HBITMAP bmpRight = GetSubBitmap(dc, hBitmapSrc, rt3);
		
		HDC dcMem = CreateCompatibleDC(dc);
		CDibSection newDIB;
 		newDIB.Create(nLen, sizeBmp.cy, 32);
		if(!newDIB.IsNull())
		{
 			HBITMAP bmpOld = (HBITMAP)::SelectObject(dcMem, newDIB.GetBitmap());
			RECT rtLeft = {0, 0, sizeBmp.cx / 2 - 1, sizeBmp.cy};
			RECT rtMiddle = { sizeBmp.cx / 2 - 1, 0, nLen - sizeBmp.cx / 2 + 1, sizeBmp.cy };
			RECT rtRight = {nLen - sizeBmp.cx / 2 + 1, 0, nLen, sizeBmp.cy};
			DrawBitmap(dcMem, bmpLeft, rtLeft);
			FillBitmap(dcMem, bmpMiddle, rtMiddle);
			DrawBitmap(dcMem, bmpRight, rtRight);
			SelectObject(dcMem, bmpOld);
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
		
		DeleteObject(bmpLeft);
		DeleteObject(bmpMiddle);
		DeleteObject(bmpRight);
		
		DeleteDC(dcMem);
		
		return newDIB.Detach();
	}
	else
	{
		DUI_ASSERT(nLen > sizeBmp.cy);
		RECT rt1 = { 0, 0, sizeBmp.cx, sizeBmp.cy / 2 - 1 };
		RECT rt2 = { 0, sizeBmp.cy / 2 - 1, sizeBmp.cx, sizeBmp.cy / 2 + 1 };
		RECT rt3 = { 0, sizeBmp.cy / 2 + 1, sizeBmp.cx, sizeBmp.cy };
		HBITMAP bmpTop = GetSubBitmap(dc, hBitmapSrc, rt1);
		HBITMAP bmpMiddle = GetSubBitmap(dc, hBitmapSrc, rt2);
		HBITMAP bmpBottom = GetSubBitmap(dc, hBitmapSrc, rt3);
		
		HDC dcMem = CreateCompatibleDC(dc);
		CDibSection newDIB;
 		newDIB.Create(sizeBmp.cx, nLen, 32);
		if(!newDIB.IsNull())
		{
 			HBITMAP bmpOld = (HBITMAP)SelectObject(dcMem,newDIB.GetBitmap());
			RECT rtTop = { 0, 0, sizeBmp.cx, sizeBmp.cy / 2 - 1};
			RECT rtMiddle = { 0, sizeBmp.cy / 2 - 1, sizeBmp.cx, nLen - sizeBmp.cy / 2 + 1 };
			RECT rtBottom = { 0, nLen - sizeBmp.cy / 2 + 1, sizeBmp.cx, nLen };
			DrawBitmap((HDC)dcMem, bmpTop, rtTop);
			FillBitmap((HDC)dcMem, bmpMiddle, rtMiddle);
			DrawBitmap((HDC)dcMem, bmpBottom, rtBottom);
			SelectObject(dcMem, bmpOld);
		}
		else
		{
			DUI_ASSERT(FALSE);
		}

		DeleteObject(bmpTop);
		DeleteObject(bmpMiddle);
		DeleteObject(bmpBottom);
		
		DeleteDC(dcMem);
		
		return newDIB.Detach();
	}
}

HBITMAP CBitmapCommonFunction::GetMiddleStretchBitmap(HDC dc, HBITMAP hBitmapSrc, SIZE sizeTarget)
{
	if(sizeTarget.cx == 0 || sizeTarget.cy == 0) return NULL;

	DUI_ASSERT(hBitmapSrc != NULL && GetObjectType(hBitmapSrc) == OBJ_BITMAP);
	if(hBitmapSrc == NULL) return NULL;
	
	SIZE sizeBmp = { 0 };
	GetBitmapSize(hBitmapSrc, sizeBmp);
	
	if(sizeBmp.cx >= sizeTarget.cx 
		&& sizeBmp.cy >= sizeTarget.cy)
	{
		RECT rtTemp = { 0, 0, sizeTarget.cx, sizeTarget.cy };
		return GetSubBitmap(dc, hBitmapSrc, rtTemp);
	}
	
	HBITMAP bmpHorStretch = NULL;
	if(sizeTarget.cx > sizeBmp.cx)
	{
		bmpHorStretch = GetMiddleStretchBitmap(dc, hBitmapSrc, sizeTarget.cx, FALSE);	
	}
	
	HBITMAP bmpStretch;
	if(sizeTarget.cy > sizeBmp.cy)
	{
		if(bmpHorStretch != NULL)	
		{
			bmpStretch = GetMiddleStretchBitmap(dc, bmpHorStretch, sizeTarget.cy, TRUE);
			DeleteObject(bmpHorStretch);
			bmpHorStretch = NULL;
		}
		else
		{
			bmpStretch = GetMiddleStretchBitmap(dc, hBitmapSrc, sizeTarget.cy, TRUE);
		}
	}
	else
	{
		bmpStretch = bmpHorStretch;
	}
	
	HBITMAP hBmpRet = bmpStretch;
	GetBitmapSize(bmpStretch, sizeBmp);
	if(sizeBmp.cx >= sizeTarget.cx 
		&& sizeBmp.cy >= sizeTarget.cy)
	{
		RECT rtTemp = { 0, 0, sizeTarget.cx, sizeTarget.cy };
		hBmpRet =  GetSubBitmap(dc, bmpStretch, rtTemp);
		DeleteObject(bmpStretch);
	}
	
	return hBmpRet;
}

BOOL CBitmapCommonFunction::DrawBitmap(HDC dc, HBITMAP hBitmap, const RECT& rt)
{
	if(hBitmap == NULL || dc == NULL) return FALSE;

	BITMAP bmp = {0};
	GetObject(hBitmap, sizeof(bmp), &bmp);
	HDC dcMem = CreateCompatibleDC(dc);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(dcMem, hBitmap);
	INT nWidth = bmp.bmWidth;
	INT nHeight = bmp.bmHeight;
	if(nWidth > (rt.right - rt.left)) nWidth = rt.right - rt.left;
	if(nHeight > (rt.bottom - rt.top)) nHeight = rt.bottom - rt.top;
	BitBlt(dc, rt.left, rt.top, nWidth, nHeight, dcMem, 0 ,0, SRCCOPY);
	SelectObject(dcMem, hBmpOld);
	DeleteDC(dcMem);
	
	return TRUE;
}

BOOL CBitmapCommonFunction::StretchBitmap(HDC dc, HBITMAP hBitmap, const RECT& rt)
{
	if(hBitmap == NULL || dc == NULL) return FALSE;

	BITMAP bmp = {0};
	GetObject(hBitmap, sizeof(bmp), &bmp);
	HDC dcMem = CreateCompatibleDC(dc);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(dcMem, hBitmap);
	StretchBlt(dc, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, dcMem, 
		0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	SelectObject(dcMem, hBmpOld);
	DeleteDC(dcMem);

	return TRUE;
}

BOOL CBitmapCommonFunction::AlphaBlend(HDC dc, HBITMAP hBitmapNew, const RECT& rt, BYTE alpha /* = 255 */)
{
	if(hBitmapNew == NULL) return FALSE;

	BITMAP bmpInfo = {0};
	::GetObject(hBitmapNew, sizeof(bmpInfo), &bmpInfo);
	DWORD wBitCount = bmpInfo.bmBitsPixel;
	
	BLENDFUNCTION bf = {0};
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = alpha;
	bf.AlphaFormat = AC_SRC_ALPHA;

	HDC dcMem = CreateCompatibleDC(dc);
	HBITMAP bmpOld = (HBITMAP)::SelectObject(dcMem, hBitmapNew);	
	BOOL bRet = ::AlphaBlend(dc, rt.left, rt.top, rt.right - rt.left,
		rt.bottom - rt.top, dcMem, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf);
	
	::SelectObject(dcMem, bmpOld);
	::DeleteDC(dcMem);
	
	return TRUE;
}

BOOL CBitmapCommonFunction::AlphaBlendWithoutStretch(HDC dc, HBITMAP hBitmapNew, const RECT& rt, BYTE alpha /* = 255 */)
{
	if(hBitmapNew == NULL) return FALSE;
	
	BITMAP bmpInfo = {0};
	::GetObject(hBitmapNew, sizeof(bmpInfo), &bmpInfo);

	INT nWidth = bmpInfo.bmWidth;
	INT nHeight = bmpInfo.bmHeight;
	if(nWidth > rt.right - rt.left)
	{
		nWidth = rt.right - rt.left;
	}

	if(nHeight > rt.bottom - rt.top)
	{
		nHeight = rt.bottom - rt.top;
	}

	BLENDFUNCTION bf = {0};
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = alpha;
	bf.AlphaFormat = AC_SRC_ALPHA;
	
	HDC dcMem = CreateCompatibleDC(dc);
	HBITMAP bmpOld = (HBITMAP)::SelectObject(dcMem, hBitmapNew);	
	BOOL bRet = ::AlphaBlend(dc, rt.left, rt.top, nWidth,
		nHeight, dcMem, 0, 0, nWidth, nHeight, bf);
	
	::SelectObject(dcMem, bmpOld);
	::DeleteDC(dcMem);
	
	return TRUE;
}



BOOL CBitmapCommonFunction::TransparentBltWithoutStretch(HDC dc, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey)
{
	SIZE sizBitmap = {0};
	GetBitmapSize(hBitmap, sizBitmap);

	INT nWidth = sizBitmap.cx;
	INT nHeight = sizBitmap.cy;
	if(nWidth > rt.right - rt.left)
	{
		nWidth = rt.right - rt.left;
	}
	
	if(nHeight > rt.bottom - rt.top)
	{
		nHeight = rt.bottom - rt.top;
	}

	HDC dcMem = CreateCompatibleDC(dc);
	HBITMAP bmpOld = (HBITMAP)::SelectObject(dcMem, hBitmap);
	
	BOOL bRet = ::TransparentBlt(dc, rt.left, rt.top, nWidth,
		nHeight, dcMem, 0, 0, nWidth, nHeight, clrKey.GetColor());
	
	::SelectObject(dcMem, bmpOld);
	::DeleteDC(dcMem);

	return TRUE;
}

BOOL CBitmapCommonFunction::TransparentBlt(HDC dc, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey)
{
	SIZE sizBitmap = {0};
	GetBitmapSize(hBitmap, sizBitmap);
	
	HDC dcMem = CreateCompatibleDC(dc);
	HBITMAP bmpOld = (HBITMAP)::SelectObject(dcMem, hBitmap);
	
	BOOL bRet = ::TransparentBlt(dc, rt.left, rt.top, rt.right - rt.left,
		rt.bottom - rt.top, dcMem, 0, 0, sizBitmap.cx, sizBitmap.cy, clrKey.GetColor());
	
	::SelectObject(dcMem, bmpOld);
	::DeleteDC(dcMem);
	
	return TRUE;
}

RECT GetBitampCenterRect(HBITMAP hBitmap, const RECT& rt)
{
	SIZE sizeBitmap = {0};
	CBitmapCommonFunction::GetBitmapSize(hBitmap, sizeBitmap);
	RECT rtDraw = rt;
	if(rt.right - rt.left > sizeBitmap.cx)
	{
		rtDraw.left += (rt.right - rt.left - sizeBitmap.cx) / 2;
		rtDraw.right = rtDraw.left + sizeBitmap.cx;
	}

	if(rt.bottom - rt.top > sizeBitmap.cy)
	{
		rtDraw.top += (rt.bottom - rt.top - sizeBitmap.cy) / 2;
		rtDraw.bottom = rtDraw.top + sizeBitmap.cy;	
	}
	
	return rtDraw;
}

BOOL CBitmapCommonFunction::DrawBitmapInCenter(HDC dc, HBITMAP hBitmap, const RECT& rt)
{
	RECT rtDraw = GetBitampCenterRect(hBitmap, rt);
	return CBitmapCommonFunction::DrawBitmap(dc, hBitmap, rtDraw);
}

BOOL CBitmapCommonFunction::AlphaBlendInCenter(HDC dc, HBITMAP hBitmap, const RECT& rt, BYTE alpha)
{
	RECT rtDraw = GetBitampCenterRect(hBitmap, rt);
	return CBitmapCommonFunction::AlphaBlendWithoutStretch(dc, hBitmap, rtDraw, alpha);
}

BOOL CBitmapCommonFunction::TransparentBltInCenter(HDC dc, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey)
{
	RECT rtDraw = GetBitampCenterRect(hBitmap, rt);
	return CBitmapCommonFunction::TransparentBltWithoutStretch(dc, hBitmap, rtDraw, clrKey);
}

BOOL CBitmapCommonFunction::FillBitmap(HDC dc, HBITMAP hBitmap, const RECT& rt)
{
	if(hBitmap == NULL || dc == NULL) return FALSE;
	HBRUSH hBrush = CreatePatternBrush(hBitmap);
	::FillRect(dc, &rt, hBrush);
	DeleteObject(hBrush);
	
	return TRUE;
}

BOOL CBitmapCommonFunction::MiddleStretchDraw(HDC hDC, HBITMAP hBitmap, const RECT& rt)
{
	SIZE sizBitmap = {0};
	if(!GetBitmapSize(hBitmap, sizBitmap)) return FALSE;

	if(rt.right - rt.left <= sizBitmap.cx
		&& rt.bottom - rt.top <= sizBitmap.cy)
	{
		return DrawBitmap(hDC, hBitmap, rt);
	}

 	if(hBitmap == NULL && hDC == NULL) return FALSE;
	SIZE size = { rt.right - rt.left, rt.bottom - rt.top };
	HBITMAP hBitmapNew = GetMiddleStretchBitmap(hDC, hBitmap, size);
	if(hBitmapNew == NULL) return FALSE;
	
	DrawBitmap(hDC, hBitmapNew, rt);
	DeleteObject(hBitmapNew);
	
	return TRUE;
}

BOOL CBitmapCommonFunction::MiddleStretchAlphaBlend(HDC hDC, HBITMAP hBitmap, const RECT& rt, BYTE alpha)
{
	SIZE sizBitmap = {0};
	if(!GetBitmapSize(hBitmap, sizBitmap)) return FALSE;
	
	if(rt.right - rt.left <= sizBitmap.cx
		&& rt.bottom - rt.top <= sizBitmap.cy)
	{
		return AlphaBlendWithoutStretch(hDC, hBitmap, rt, alpha);
	}

	SIZE size = { rt.right - rt.left, rt.bottom - rt.top };
	HBITMAP hBitmapNew = GetMiddleStretchBitmap(hDC, hBitmap, size);
	if(hBitmapNew == NULL) return FALSE;
	
	CBitmapCommonFunction::AlphaBlendWithoutStretch(hDC, hBitmapNew, rt, alpha);
	DeleteObject(hBitmapNew);

	return TRUE;
}

BOOL CBitmapCommonFunction::MiddleStretchTransparentBlt(HDC hDC, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey)
{
	SIZE sizBitmap = {0};
	if(!GetBitmapSize(hBitmap, sizBitmap)) return FALSE;
	
	if(rt.right - rt.left <= sizBitmap.cx
		&& rt.bottom - rt.top <= sizBitmap.cy)
	{
		return TransparentBltWithoutStretch(hDC, hBitmap, rt, clrKey);
	}

	SIZE size = { rt.right - rt.left, rt.bottom - rt.top };
	HBITMAP hBitmapNew = GetMiddleStretchBitmap(hDC, hBitmap, size);
	if(hBitmapNew == NULL) return FALSE;
	
	CBitmapCommonFunction::TransparentBltWithoutStretch(hDC, hBitmapNew, rt, clrKey);
	DeleteObject(hBitmapNew);

	return TRUE;
}

BOOL CBitmapCommonFunction::UpdateLayeredWindow(HWND hwnd, HDC hdcDst, POINT *pptDst, SIZE *psize
												, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags)
{
	BOOL bRet(FALSE);
	
	typedef BOOL (WINAPI * UpdateLayeredWindowProc)(HWND hwnd, HDC hdcDst, POINT *pptDst, SIZE *psize,HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
	UpdateLayeredWindowProc pFun = NULL;
	
	HMODULE hModUser32 = GetModuleHandle(_T("User32.dll"));
	if(hModUser32 != NULL)
	{
		pFun = (UpdateLayeredWindowProc)GetProcAddress(hModUser32, "UpdateLayeredWindow");
		if(pFun != NULL)
		{
			bRet = pFun( hwnd,  hdcDst,  pptDst,  psize, hdcSrc,  pptSrc,  crKey,  pblend,  dwFlags);
		}
	}
	
	return bRet;
}

HRGN CBitmapCommonFunction::BitmapToRgn(HBITMAP hBmp, const CARGB& cTransparentColor1, COLORREF cTolerance)
{
	COLORREF  cTransparentColor = cTransparentColor1.GetColor();
	HRGN hRgn = NULL;
	// return hRgn;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = ::CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			::GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
					sizeof(BITMAPINFOHEADER),	// biSize 
					bm.bmWidth,					// biWidth; 
					bm.bmHeight,				// biHeight; 
					1,							// biPlanes; 
					32,							// biBitCount 
					BI_RGB,						// biCompression; 
					0,							// biSizeImage; 
					0,							// biXPelsPerMeter; 
					0,							// biYPelsPerMeter; 
					0,							// biClrUsed; 
					0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = ::CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)::SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = ::CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					::GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)::SelectObject(hDC, hBmp);
					::BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
					#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)::GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					::SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									::GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = ::GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)::GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								::SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										::CombineRgn(hRgn, hRgn, h, RGN_OR);
										::DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									::SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						::CombineRgn(hRgn, hRgn, h, RGN_OR);
						::DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					::GlobalUnlock(hData);
					::GlobalFree(hData);
					::SelectObject(hDC, holdBmp);
					::DeleteDC(hDC);
				}

				::DeleteObject(::SelectObject(hMemDC, holdBmp));
			}

			::DeleteDC(hMemDC);
		}	
	}

	return hRgn;	
}
////////////////  CBitmapCommonFunction  ///////////////////////////




DUI_END_NAMESPCE