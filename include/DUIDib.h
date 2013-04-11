#ifndef __DUI_DIB_H__
#define __DUI_DIB_H__

#include "DUIBase.h"
#include "DUIColor.h"
#include "DUIFile.h"

DUI_BGN_NAMESPCE

class DUILIB_API CDibSection:  public CDUIObject
{
public:
	CDibSection();
	~CDibSection();
	
	CDibSection(const CDibSection& dib);
	CDibSection& operator = (const CDibSection& dib);

	BOOL Create(DWORD dwWidth, DWORD dwHeight, WORD wBits = 24);
	BOOL Create(HBITMAP hBitmap, WORD wBits = 24);
	BOOL Create(LPBYTE pData, DWORD dwSize);
	
	BOOL SaveAsBmpFile(LPCTSTR lpszFileName) const;
	
	CDibSection* NewObject() const;
	CDibSection* Clone() const;
	BOOL Transfer(CDibSection* from);

	BOOL IsNull() const;
	VOID DeleteObject();
	HBITMAP GetBitmap() const { return m_hBitmap; }
	VOID Reset();
	HBITMAP Detach();
	
	DWORD GetImageSize() const;
	LPBYTE GetBits() const;
	DWORD GetWidth() const;
	DWORD GetHeight()const;
	DWORD GetLineWidth() const;
	DWORD GetPixelWidth() const;
	WORD GetBitCount() const;
	LPBITMAPINFOHEADER GetDibHeader();	
	
	CARGB GetPixel(INT nX, INT nY, BOOL bTopToBottom = TRUE) const;
	BOOL SetPixel(INT nX, INT nY, const CARGB& clr, BOOL bTopToBottom = TRUE);
	
	BOOL Draw(HDC hDC, INT nOffsetX, INT nOffsetY) const;
	BOOL Stretch(HDC hDC, const RECT& rt) const;
	BOOL AlphaBlend(HDC hDC, const RECT& rt, BOOL bSrcAlpha = TRUE, BYTE alphaConstant = 255);
	BOOL TransparentBlt(HDC hDC, const RECT& rt, COLORREF clrMask = RGB(255, 0, 255));

protected:
	BOOL CopyTo(CDibSection* pDst) const;

protected:
	BITMAPINFO m_bi;
	LPBYTE m_lpData;
	DWORD m_dwLineWidth;
	HBITMAP m_hBitmap;
};

DUI_END_NAMESPCE

#endif //endif __DUI_DIB_H__