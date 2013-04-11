#ifndef __DUI_IMAGE_H__
#define __DUI_IMAGE_H__

#include "DUIDib.h"



DUI_BGN_NAMESPCE

enum IMAGE_DRAW_MODE
{
	DRAW_NORMAL,
	DRAW_TRANSPARENT,
	DRAW_ALPHA
};

class DUILIB_API CImage: public CDUIObject
{
public:
	CImage();
	virtual ~CImage();

	CImage(const CImage& image);
	CImage& operator = (const CImage& image);

	CRefPtr<CDibSection> GetDIB() const;
	HBITMAP GetBitmap() const;
	BOOL IsNull() const;

	BOOL CreateIndirect(DWORD dwWidth, DWORD dwHeight, BYTE nBitCount);
	BOOL CreateFromStream(LPBYTE pStreamData, DWORD dwSize);
	BOOL CreateFromFile(LPCTSTR lpszFileName);
	BOOL CreateFromResource(HINSTANCE hRes, LPCTSTR lpszName, LPCTSTR lpszType);
	
	VOID SetColorKey(const CARGB& clrKey);
	const CARGB& GetColorKey() const;
	
	VOID SetAlpha(BYTE byte);
	BYTE GetAlpha() const;
	
	DWORD GetWidth() const;
	DWORD GetHeight() const;
	DWORD GetBitCount() const;

	BOOL Draw(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const;
	BOOL DrawInCenter(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const;
	BOOL Stretch(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const;
	BOOL VerticalStretch(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const;
	BOOL HorizontalStretch(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const;
	BOOL MiddleStretch(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const;

protected:
	CRefPtr<CDibSection> NewDibInstance();
	
protected:
	CRefPtr<CDibSection> m_pDib;
	CARGB m_clrKey;	
	BYTE m_alpha;		//m_alpha == 0 means this image is not a alpha image
};


class DUILIB_API CImageList: public CImage
{
public:
	CImageList();
	~CImageList();

	CImageList(const CImageList& image);
	CImageList& operator = (const CImageList& image);

	VOID SetItemCount(DWORD dwCount);
	INT GetItemCount() const;

	INT GetIconWidth() const;
	INT GetIconHeight() const;

	HBITMAP GetIconBitmap(HDC dc, INT nIndex) const;
	HRGN GetIconRgn();

	//need assign a  draw mode
	BOOL Draw(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const;
	BOOL DrawInCenter(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const;
	BOOL Stretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const;
	BOOL VerticalStretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const;
	BOOL HorizontalStretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const;
	BOOL MiddleStretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const;

	//auto select draw mode by image property
	BOOL Draw(HDC dc, const RECT& rt, INT nIconIndex) const;
	BOOL DrawInCenter(HDC dc, const RECT& rt, INT nIconIndex) const;
	BOOL Stretch(HDC dc, const RECT& rt, INT nIconIndex) const;
	BOOL VerticalStretch(HDC dc, const RECT& rt, INT nIconIndex) const;
	BOOL HorizontalStretch(HDC dc, const RECT& rt, INT nIconIndex) const;
	BOOL MiddleStretch(HDC dc, const RECT& rt, INT nIconIndex) const;

protected:
	BOOL GetIconRect(INT nIndex, RECT& rtRet) const;
	IMAGE_DRAW_MODE GetBestDrawMode() const;

protected:
	INT m_nItemCount;
};

DUI_END_NAMESPCE

#endif //__DUI_IMAGE_H__