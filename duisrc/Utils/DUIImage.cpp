#include "DUIImage.h"
#include "DUIDdb.h"

#include <olectl.h>

#pragma  warning(disable:4018)


DUI_BGN_NAMESPCE


////////////////  CImage  ///////////////////////////
CImage::CImage()
:m_pDib(NULL), m_alpha(0)
{
}

CImage::~CImage()
{
	m_pDib = NULL;
}

CImage::CImage(const CImage& image)
{
	if(!image.m_pDib.IsNull())
	{
		m_pDib = image.m_pDib->Clone();
	}
	else
	{
		m_pDib = NULL;
	}
	m_clrKey = image.m_clrKey;
	m_alpha = image.m_alpha;
}

CImage& CImage::operator = (const CImage& image)
{
	if(this == &image) return *this;

	if(!image.m_pDib.IsNull())
	{
		m_pDib = image.m_pDib->Clone();
	}
	else
	{
		m_pDib = NULL;
	}

	m_clrKey = image.m_clrKey;
	m_alpha = image.m_alpha;

	return *this;
}

CRefPtr<CDibSection> CImage::NewDibInstance()
{
	CRefPtr<CDibSection>  pRet = new CDibSection;
	DUI_ASSERT(!pRet.IsNull());
	return pRet;
}

HBITMAP CImage::GetBitmap() const
{
	if(!m_pDib.IsNull() && !m_pDib->IsNull())
	{
		return m_pDib->GetBitmap();
	}
	else
	{
		return NULL;
	}
}


BOOL CImage::IsNull() const
{
	if(m_pDib.IsNull() || m_pDib->IsNull())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CRefPtr<CDibSection> CImage::GetDIB() const
{
	return m_pDib;
}

BOOL CImage::CreateIndirect(DWORD dwWidth, DWORD dwHeight, BYTE nBitCount)
{
	if(dwWidth == 0 || dwHeight == 0) return FALSE;
	DUI_ASSERT(nBitCount == 24 || nBitCount == 32);

	m_pDib = NewDibInstance();
	return m_pDib->Create(dwWidth, dwHeight, nBitCount);
}

BOOL CImage::CreateFromStream(LPBYTE pStreamData, DWORD dwSize)
{
	DUI_ASSERT(pStreamData != NULL && dwSize > 0);

	m_pDib = NewDibInstance();
	DUI_ASSERT(!m_pDib.IsNull());

	return m_pDib->Create(pStreamData, dwSize);
}

BOOL CImage::CreateFromFile(LPCTSTR lpszFileName)
{
	DUI_ASSERT(lpszFileName != 0);
	
	 CIOFile file;
 	if(!file.Open(lpszFileName, _T("rb"))) return FALSE;

	DWORD dwSize = file.Size();
	if(dwSize == 0) return FALSE;

	LPBYTE pData = new BYTE[dwSize];
	if(pData == NULL) return FALSE;

	DWORD dwReadLen = file.Read(pData, 1, dwSize);
	DUI_ASSERT(dwReadLen == dwSize);
	if(dwReadLen != 0)
	{
		 CreateFromStream(pData, dwReadLen);
	}
	
	delete []pData;
	return TRUE;
}


BOOL CImage::CreateFromResource(HINSTANCE hRes, LPCTSTR lpszName, LPCTSTR lpszType)
{
    HRSRC hRsrc = FindResource (hRes, lpszName, lpszType);
    if (hRsrc)
    {
        BYTE* ptr = (BYTE*)LoadResource(hRes, hRsrc);
        DWORD nSize = SizeofResource(hRes, hRsrc);
		return CreateFromStream(ptr, nSize);
    }
	else
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
}

VOID CImage::SetColorKey(const CARGB& clrKey)
{
	m_clrKey = clrKey;
}


const CARGB& CImage::GetColorKey() const
{
	return m_clrKey;
}
	
VOID CImage::SetAlpha(BYTE byte)
{
	m_alpha = byte;
}

BYTE CImage::GetAlpha() const
{
	return m_alpha;
}

DWORD CImage::GetWidth() const
{
	DWORD dwWidth(0);
	if(GetBitmap() != NULL)
	{
		dwWidth = m_pDib->GetWidth();
	}
	return dwWidth;
}

DWORD CImage::GetHeight() const
{
	DWORD dwHeight(0);
	if(GetBitmap() != NULL)
	{
		dwHeight = m_pDib->GetHeight();
	}
	return dwHeight;
}

DWORD CImage::GetBitCount() const
{
	DWORD dwBitCount(0);
	if(GetBitmap() != NULL)
	{
		dwBitCount = m_pDib->GetBitCount();
	}
	return dwBitCount;
}

BOOL CImage::Draw(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	BOOL bRet(FALSE);
	HBITMAP hBitmap = GetBitmap();
	if(hBitmap == NULL) return bRet;

	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::DrawBitmap(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::TransparentBltWithoutStretch(dc, hBitmap, rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		bRet = CBitmapCommonFunction::AlphaBlendWithoutStretch(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	return bRet;
}

BOOL CImage::DrawInCenter(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	BOOL bRet(FALSE);
	HBITMAP hBitmap = GetBitmap();
	if(hBitmap == NULL) return bRet;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::DrawBitmapInCenter(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::TransparentBltInCenter(dc, hBitmap,rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{

		bRet = CBitmapCommonFunction::AlphaBlendInCenter(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	
	return bRet;
}

BOOL CImage::Stretch(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode ) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	BOOL bRet(FALSE);
	HBITMAP hBitmap = GetBitmap();
	if(hBitmap == NULL) return bRet;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::StretchBitmap(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::TransparentBlt(dc, hBitmap, rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		
		bRet = CBitmapCommonFunction::AlphaBlend(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	return bRet;
}

BOOL CImage::VerticalStretch(HDC dc, const RECT& rt,  IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	BOOL bRet(FALSE);
	HBITMAP hBitmap = GetBitmap();
	if(hBitmap == NULL) return bRet;
	
	SIZE sizeBitmap = {0};
	RECT rtTemp = rt;
	CBitmapCommonFunction::GetBitmapSize(hBitmap, sizeBitmap);
	rtTemp.right = rtTemp.left + sizeBitmap.cx;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::MiddleStretchDraw(dc, hBitmap, rtTemp);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::MiddleStretchTransparentBlt(dc, hBitmap, rtTemp, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		
		bRet = CBitmapCommonFunction::MiddleStretchAlphaBlend(dc, hBitmap, rtTemp, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	return bRet;
}

BOOL CImage::HorizontalStretch(HDC dc, const RECT& rt,  IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	BOOL bRet(FALSE);
	HBITMAP hBitmap = GetBitmap();
	if(hBitmap == NULL) return bRet;
	
	SIZE sizeBitmap = {0};
	RECT rtTemp = rt;
	CBitmapCommonFunction::GetBitmapSize(hBitmap, sizeBitmap);
	rtTemp.bottom = rtTemp.top + sizeBitmap.cy;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::MiddleStretchDraw(dc, hBitmap, rtTemp);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::MiddleStretchTransparentBlt(dc, hBitmap, rtTemp, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		
		bRet = CBitmapCommonFunction::MiddleStretchAlphaBlend(dc, hBitmap, rtTemp, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	

	return bRet;
}

BOOL CImage::MiddleStretch(HDC dc, const RECT& rt, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	BOOL bRet(FALSE);
	HBITMAP hBitmap = GetBitmap();
	if(hBitmap == NULL) return bRet;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::MiddleStretchDraw(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::MiddleStretchTransparentBlt(dc, hBitmap, rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		
		bRet = CBitmapCommonFunction::MiddleStretchAlphaBlend(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	return bRet;
}



////////////////  CImage  ///////////////////////////



////////////////  CImageList  ///////////////////////////
CImageList::CImageList()
:CImage()
{
	m_nItemCount = 1;
	
	CTrace::TraceInfo(_T("CImageList::CImageList()"));
}

CImageList::~CImageList()
{
	CTrace::TraceInfo(_T("CImageList::~CImageList(), m_pDIB:%u"), m_pDib);	
}

CImageList::CImageList(const CImageList& image)
:CImage(image), m_nItemCount(image.m_nItemCount)
{
		
}

CImageList& CImageList::operator =(const CImageList& imagelist)
{
	CImage::operator = (imagelist);
	m_nItemCount = imagelist.m_nItemCount;
	return *this;
}

VOID CImageList::SetItemCount(DWORD dwCount)
{
	DUI_ASSERT(dwCount >0 );
	if(dwCount > 0)
	{
		m_nItemCount = dwCount;
	}
}

INT CImageList::GetItemCount() const 
{
	return m_nItemCount;
}

INT CImageList::GetIconWidth() const 
{
	return GetWidth() / m_nItemCount;
}

INT CImageList::GetIconHeight() const
{
	return GetHeight();
}

BOOL CImageList::GetIconRect(INT nIndex, RECT& rtRet) const
{
	INT nIconWidth = GetIconWidth();
	INT nIconHeight = GetIconHeight();
	if(nIconWidth == 0 || nIconHeight == 0) return FALSE;

	DUI_ASSERT(nIndex >= 0 && nIndex < m_nItemCount);
	rtRet.left = nIndex * nIconWidth;
	rtRet.right = rtRet.left + nIconWidth;
	rtRet.top = 0;
	rtRet.bottom = nIconHeight;

	return TRUE;
}

HBITMAP CImageList::GetIconBitmap(HDC dc, INT nIndex) const
{
	DUI_ASSERT(dc != NULL);
	DUI_ASSERT(nIndex >= 0 && nIndex < m_nItemCount);
	if(nIndex < 0 && nIndex >= m_nItemCount) return NULL;

	RECT rtIcon = {0};
	if(!GetIconRect(nIndex, rtIcon)) return NULL;

	HBITMAP hBitmap = GetBitmap();
	DUI_ASSERT(hBitmap != NULL);
	if(hBitmap == NULL) return NULL;

	return CBitmapCommonFunction::GetSubBitmap(dc, hBitmap, rtIcon);
}


BOOL CImageList::Draw(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	HBITMAP hBitmap = NULL;
	BOOL bNeedDelete(FALSE);
	BOOL bRet(FALSE);
	if(m_nItemCount == 1)
	{
		hBitmap = GetBitmap();
		bNeedDelete = FALSE;
	}
	else
	{
		hBitmap = GetIconBitmap(dc, nIconIndex);
		bNeedDelete = TRUE;
	}

	if(hBitmap == NULL) return bRet;

	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::DrawBitmap(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::TransparentBltWithoutStretch(dc, hBitmap, rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		bRet = CBitmapCommonFunction::AlphaBlendWithoutStretch(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	if(bNeedDelete && hBitmap != NULL)
	{
		DeleteObject(hBitmap);
	}

	return bRet;
}

BOOL CImageList::DrawInCenter(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	HBITMAP hBitmap = NULL;
	BOOL bNeedDelete(FALSE);
	BOOL bRet(FALSE);
	if(m_nItemCount == 1)
	{
		hBitmap = GetBitmap();
		bNeedDelete = FALSE;
	}
	else
	{
		hBitmap = GetIconBitmap(dc, nIconIndex);
		bNeedDelete = TRUE;
	}

	if(hBitmap == NULL) return bRet;

	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::DrawBitmapInCenter(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::TransparentBltInCenter(dc, hBitmap,rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{

		bRet = CBitmapCommonFunction::AlphaBlendInCenter(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	if(bNeedDelete && hBitmap != NULL)
	{
		DeleteObject(hBitmap);
	}
	
	return bRet;
}

BOOL CImageList::Stretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	HBITMAP hBitmap = NULL;
	BOOL bNeedDelete(FALSE);
	BOOL bRet(FALSE);
	if(m_nItemCount == 1)
	{
		hBitmap = GetBitmap();
		bNeedDelete = FALSE;
	}
	else
	{
		hBitmap = GetIconBitmap(dc, nIconIndex);
		bNeedDelete = TRUE;
	}
	
	if(hBitmap == NULL) return bRet;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::StretchBitmap(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::TransparentBlt(dc, hBitmap, rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		
		bRet = CBitmapCommonFunction::AlphaBlend(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	if(bNeedDelete && hBitmap != NULL)
	{
		DeleteObject(hBitmap);
	}
	
	return bRet;
}

BOOL CImageList::VerticalStretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	HBITMAP hBitmap = NULL;
	BOOL bNeedDelete(FALSE);
	BOOL bRet(FALSE);
	if(m_nItemCount == 1)
	{
		hBitmap = GetBitmap();
		bNeedDelete = FALSE;
	}
	else
	{
		hBitmap = GetIconBitmap(dc, nIconIndex);
		bNeedDelete = TRUE;
	}
	
	if(hBitmap == NULL) return bRet;
	
	SIZE sizeBitmap = {0};
	RECT rtTemp = rt;
	CBitmapCommonFunction::GetBitmapSize(hBitmap, sizeBitmap);
	rtTemp.right = rtTemp.left + sizeBitmap.cx;

	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::MiddleStretchDraw(dc, hBitmap, rtTemp);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::MiddleStretchTransparentBlt(dc, hBitmap, rtTemp, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		bRet = CBitmapCommonFunction::MiddleStretchAlphaBlend(dc, hBitmap, rtTemp, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	if(bNeedDelete && hBitmap != NULL)
	{
		DeleteObject(hBitmap);
	}
	
	return bRet;
}

BOOL CImageList::HorizontalStretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	HBITMAP hBitmap = NULL;
	BOOL bNeedDelete(FALSE);
	BOOL bRet(FALSE);
	if(m_nItemCount == 1)
	{
		hBitmap = GetBitmap();
		bNeedDelete = FALSE;
	}
	else
	{
		hBitmap = GetIconBitmap(dc, nIconIndex);
		bNeedDelete = TRUE;
	}
	
	if(hBitmap == NULL) return bRet;
	
	SIZE sizeBitmap = {0};
	RECT rtTemp = rt;
	CBitmapCommonFunction::GetBitmapSize(hBitmap, sizeBitmap);
	rtTemp.bottom = rtTemp.top + sizeBitmap.cy;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::MiddleStretchDraw(dc, hBitmap, rtTemp);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::MiddleStretchTransparentBlt(dc, hBitmap, rtTemp, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		
		bRet = CBitmapCommonFunction::MiddleStretchAlphaBlend(dc, hBitmap, rtTemp, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	if(bNeedDelete && hBitmap != NULL)
	{
		DeleteObject(hBitmap);
	}
	
	return bRet;
}

BOOL CImageList::MiddleStretch(HDC dc, const RECT& rt, INT nIconIndex, IMAGE_DRAW_MODE mode) const
{
	if(IsRectEmpty(&rt)) return FALSE;

	HBITMAP hBitmap = NULL;
	BOOL bNeedDelete(FALSE);
	BOOL bRet(FALSE);
	if(m_nItemCount == 1)
	{
		hBitmap = GetBitmap();
		bNeedDelete = FALSE;
	}
	else
	{
		hBitmap = GetIconBitmap(dc, nIconIndex);
		bNeedDelete = TRUE;
	}
	
	if(hBitmap == NULL) return bRet;
	
	if(mode == DRAW_NORMAL)
	{
		bRet = CBitmapCommonFunction::MiddleStretchDraw(dc, hBitmap, rt);
	}
	else if(mode == DRAW_TRANSPARENT)
	{
		bRet = CBitmapCommonFunction::MiddleStretchTransparentBlt(dc, hBitmap, rt, m_clrKey);
	}
	else if(mode == DRAW_ALPHA)
	{
		bRet = CBitmapCommonFunction::MiddleStretchAlphaBlend(dc, hBitmap, rt, m_alpha);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	if(bNeedDelete && hBitmap != NULL)
	{
		DeleteObject(hBitmap);
	}
	
	return bRet;
}



BOOL CImageList::Draw(HDC dc, const RECT& rt, INT nIconIndex) const
{
	return Draw(dc, rt, nIconIndex, GetBestDrawMode());
}

BOOL CImageList::DrawInCenter(HDC dc, const RECT& rt, INT nIconIndex) const
{
	return DrawInCenter(dc, rt, nIconIndex, GetBestDrawMode());
}

BOOL CImageList::Stretch(HDC dc, const RECT& rt, INT nIconIndex) const
{
	return Stretch(dc, rt, nIconIndex, GetBestDrawMode());
}

BOOL CImageList::VerticalStretch(HDC dc, const RECT& rt, INT nIconIndex) const
{
	return VerticalStretch(dc, rt, nIconIndex, GetBestDrawMode());
}

BOOL CImageList::HorizontalStretch(HDC dc, const RECT& rt, INT nIconIndex) const
{
	return HorizontalStretch(dc, rt, nIconIndex, GetBestDrawMode());
}

BOOL CImageList::MiddleStretch(HDC dc, const RECT& rt, INT nIconIndex) const
{
	return MiddleStretch(dc, rt, nIconIndex, GetBestDrawMode());
}

IMAGE_DRAW_MODE CImageList::GetBestDrawMode() const
{
	IMAGE_DRAW_MODE eMode;
	if(!m_clrKey.IsEmpty())
	{
		eMode = DRAW_TRANSPARENT;
	}
	else if(m_alpha != 0)
	{
		eMode = DRAW_ALPHA;
	}
	else
	{
		eMode = DRAW_NORMAL;
	}

	return eMode;
}

HRGN CImageList::GetIconRgn()
{
	if(m_clrKey.IsEmpty()
		|| IsNull())
	{
		return NULL;
	}

	HRGN hRgn = NULL;
	HWND hWnd = GetDesktopWindow();
	HDC hDC = GetWindowDC(hWnd);
	
	HBITMAP hIcon = GetIconBitmap(hDC, 0);
	if(hIcon != NULL)
	{
		hRgn = CBitmapCommonFunction::BitmapToRgn(hIcon, m_clrKey);
		::DeleteObject(hIcon);
	}

	ReleaseDC(hWnd, hDC);
	
	return hRgn;
}

////////////////  CImageList  ///////////////////////////

DUI_END_NAMESPCE