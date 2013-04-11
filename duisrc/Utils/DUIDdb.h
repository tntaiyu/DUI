#ifndef __DUI_DDB_H__
#define __DUI_DDB_H__

#include "DUIBase.h"
#include "DUIColor.h"

DUI_BGN_NAMESPCE

class DUILIB_API CBitmapCommonFunction
{
public:
	static BOOL GetBitmapSize(HBITMAP hBitmap, SIZE& sizeRet);
	static BYTE GetBitmapBitCount(HBITMAP hBitmap);
	static HRGN BitmapToRgn(HBITMAP hBmp, const CARGB& cTransparentColor, COLORREF cTolerance = 0);

	static HBITMAP GetSubBitmap(HDC dc, HBITMAP hBitmapSrc, RECT& rt);
	static HBITMAP GetMiddleStretchBitmap(HDC dc, HBITMAP hBitmapSrc, INT nLen, BOOL bVert);
	static HBITMAP GetMiddleStretchBitmap(HDC dc, HBITMAP hBitmapSrc, SIZE sizeTarget);
	
	static BOOL DrawBitmap(HDC dc, HBITMAP hBitmap, const RECT& rt);
	static BOOL AlphaBlend(HDC dc, HBITMAP hBitmap, const RECT& rt, BYTE alpha = 255);
	static BOOL TransparentBlt(HDC dc, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey);
	
	static BOOL StretchBitmap(HDC dc, HBITMAP hBitmap, const RECT& rt);
	static BOOL AlphaBlendWithoutStretch(HDC dc, HBITMAP hBitmap, const RECT& rt, BYTE alpha = 255);
	static BOOL TransparentBltWithoutStretch(HDC dc, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey);


	static BOOL DrawBitmapInCenter(HDC dc, HBITMAP hBitmap, const RECT& rt);
	static BOOL AlphaBlendInCenter(HDC dc, HBITMAP hBitmap, const RECT& rt,  BYTE alpha = 255);
	static BOOL TransparentBltInCenter(HDC dc, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey);

	static BOOL FillBitmap(HDC dc, HBITMAP hBitmap, const RECT& rt);
	static BOOL MiddleStretchDraw(HDC hDC, HBITMAP hBitmap, const RECT& rt);
	static BOOL MiddleStretchAlphaBlend(HDC hDC, HBITMAP hBitmap, const RECT& rt, BYTE alpha = 255);
	static BOOL MiddleStretchTransparentBlt(HDC hDC, HBITMAP hBitmap, const RECT& rt, const CARGB& clrKey) ;

	static	BOOL UpdateLayeredWindow(HWND hwnd, HDC hdcDst, POINT *pptDst, SIZE *psize,HDC hdcSrc
		, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
};



DUI_END_NAMESPCE	


#endif //__DUI_DDB_H__