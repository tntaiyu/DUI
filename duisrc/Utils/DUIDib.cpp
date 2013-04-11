#include "DUIDib.h"

#pragma warning(disable:4018)

DUI_BGN_NAMESPCE

#define WIDTHBYTES(bits)  (((bits) + 31) / 32 * 4)

extern "C"
{
    extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, int *comp, int req_comp);
	extern void stbi_image_free(void *retval_from_stbi_load);
};

////////////////////////////  CDibSection  /////////////////////////
CDibSection::CDibSection()
:m_hBitmap(NULL), m_lpData(NULL), m_dwLineWidth(0)
{
	ZeroMemory(&m_bi, sizeof(m_bi));
}

CDibSection::~CDibSection()
{
	DeleteObject();
}

CDibSection::CDibSection(const CDibSection& dib)
{
	Reset();
	dib.CopyTo(this);
}

CDibSection& CDibSection::operator = (const CDibSection& dib)
{
	dib.CopyTo(this);
	return *this;
}

BOOL CDibSection::IsNull() const 
{
	return m_hBitmap == NULL;
}

VOID CDibSection::DeleteObject()
{
	if(m_hBitmap != NULL) 
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

BOOL CDibSection::CopyTo(CDibSection* pDst) const
{
	if(this == pDst) return TRUE;

	DUI_ASSERT(pDst != NULL);
	if(!pDst->IsNull()) pDst->DeleteObject();
	
	DUI_ASSERT(!IsNull());
	if(IsNull()) return FALSE;

	DWORD dwWidth = GetWidth();
	DWORD dwHeight = GetHeight();
	WORD wBits = GetBitCount();
	
	pDst->Create(dwWidth, dwHeight, wBits);
	if(pDst->IsNull()) return FALSE;

	DWORD dwImageSize = GetImageSize();
	memcpy(pDst->GetBits(), GetBits(), dwImageSize);

	return TRUE;
}

BOOL CDibSection::Create(DWORD dwWidth, DWORD dwHeight, WORD wBits /* = 24 */)
{
	DUI_ASSERT(wBits == 24 || wBits == 32);
	if(!IsNull()) DeleteObject();
	if(dwWidth == 0 || dwHeight == 0) return FALSE;

	m_dwLineWidth = WIDTHBYTES(dwWidth * wBits);

	m_bi.bmiHeader.biSize = sizeof(m_bi.bmiHeader);
	m_bi.bmiHeader.biPlanes = 1;
	m_bi.bmiHeader.biWidth = dwWidth;
	m_bi.bmiHeader.biHeight = dwHeight;
	m_bi.bmiHeader.biBitCount = wBits;
	m_bi.bmiHeader.biCompression = BI_RGB;
	m_bi.bmiHeader.biSizeImage = m_dwLineWidth * dwHeight;

	m_hBitmap = ::CreateDIBSection(NULL, &m_bi, DIB_RGB_COLORS, (LPVOID*)&m_lpData, NULL, 0);
	DUI_ASSERT(m_hBitmap != NULL);

	return m_hBitmap != NULL;
}

BOOL CDibSection::Create(HBITMAP hBitmap, WORD wBits)
{
	DUI_ASSERT(::GetObjectType(hBitmap) == OBJ_BITMAP);
	BITMAP bmp;
	::GetObject(hBitmap, sizeof(BITMAP), &bmp);
	if(wBits < 24) wBits = 24; else wBits = 32;

	Create(bmp.bmWidth, bmp.bmHeight, wBits);
	if(IsNull()) return NULL;

	HWND hWndDc = GetDesktopWindow();
	HDC dc = GetWindowDC(hWndDc);
	INT nLines = ::GetDIBits(dc, hBitmap, 0, bmp.bmHeight, m_lpData, (LPBITMAPINFO)&m_bi, DIB_RGB_COLORS);
	::ReleaseDC(hWndDc, dc);
	
	DUI_ASSERT(nLines == bmp.bmHeight);
	if(nLines != bmp.bmHeight) DUI_ASSERT(FALSE);

	return m_hBitmap != NULL;
}	



VOID CDibSection::Reset()
{
	m_lpData = NULL;
	m_hBitmap = 0;
	m_dwLineWidth = 0;
	ZeroMemory(&m_bi, sizeof(m_bi));
}

LPBITMAPINFOHEADER CDibSection::GetDibHeader()
{
	return &m_bi.bmiHeader;
}

CDibSection* CDibSection::NewObject() const
{
	return new CDibSection;
}

CDibSection* CDibSection::Clone() const
{
	CDibSection* pRet = new CDibSection(*this);
	DUI_ASSERT(pRet != NULL);
	return pRet;
}

HBITMAP CDibSection::Detach()
{
	HBITMAP hBmpOld = m_hBitmap;
	m_hBitmap = NULL;
	return hBmpOld;
}

BOOL CDibSection::Transfer(CDibSection* from)
{
	if(from == this) return FALSE;
	if(!IsNull()) DeleteObject();

	memcpy(&m_bi, from->GetDibHeader(), sizeof(m_bi));
	m_lpData = from->GetBits();
	m_hBitmap = from->GetBitmap();
	m_dwLineWidth = from->GetLineWidth();
	
	from->Reset();

	return TRUE;
}


BOOL CDibSection::Create(LPBYTE pData, DWORD dwSize)
{
	DUI_ASSERT(pData != NULL && dwSize > 0);
	if(pData == NULL || dwSize == 0) return FALSE;

	INT nWidth, nHeight, n;
	LPBYTE pImage = stbi_load_from_memory(pData, dwSize, &nWidth, &nHeight, &n, 4);
	if(pImage == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	if(!Create(nWidth, nHeight, 32))
	{
		stbi_image_free(pImage);
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	DUI_ASSERT(m_lpData != NULL);
	INT nSrcIndex(0), nDestIndex(0);
    for( int i=0; i<nHeight; i++ ) 
    {
		for(int j=0; j<nWidth; ++j)
		{
			nSrcIndex = (nHeight - i -1)*4*nWidth + j*4;
			nDestIndex = i*4*nWidth + j*4;
			m_lpData[nDestIndex] = pImage[nSrcIndex + 2];		//B
			m_lpData[nDestIndex + 1] = pImage[nSrcIndex + 1];	//G
			m_lpData[nDestIndex + 2] = pImage[nSrcIndex];		//R
			m_lpData[nDestIndex + 3] = pImage[nSrcIndex + 3];	//A
		}
    }

	 stbi_image_free(pImage);
	
	 return TRUE;
}

BOOL CDibSection::SaveAsBmpFile(LPCTSTR lpszFileName) const 
{
	DUI_ASSERT(!IsNull());
	if(IsNull()) return FALSE;

	DWORD dwImageSize = GetImageSize();

	BITMAPFILEHEADER bmfHdr = {0};
	bmfHdr.bfType = 0x4D42; // "BM" 
	DWORD dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)  + dwImageSize;  
	bmfHdr.bfSize = dwDIBSize; 
	bmfHdr.bfReserved1 = 0; 
	bmfHdr.bfReserved2 = 0; 
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFO); 

	HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
		 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if(hFile == INVALID_HANDLE_VALUE) return FALSE;
	
	DWORD dwWritten(0);
	BOOL bRet(FALSE);
	WriteFile(hFile, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	if(dwWritten == sizeof(BITMAPFILEHEADER))
	{
		WriteFile(hFile, (LPSTR)&m_bi, sizeof(BITMAPINFO), &dwWritten, NULL);
		if(dwWritten == sizeof(BITMAPINFO))
		{
			WriteFile(hFile, m_lpData, dwImageSize, &dwWritten, NULL);
			bRet = (dwWritten == dwImageSize);
		}
	}
	CloseHandle(hFile);

	return bRet;
}

DWORD CDibSection::GetImageSize() const
{
	return m_bi.bmiHeader.biSizeImage;
}

LPBYTE CDibSection::GetBits() const
{
	DUI_ASSERT(!IsNull());
	return m_lpData;
}

DWORD CDibSection::GetWidth() const
{
	DUI_ASSERT(!IsNull());
	return m_bi.bmiHeader.biWidth;
}

DWORD CDibSection::GetHeight() const
{
	DUI_ASSERT(!IsNull());
	return m_bi.bmiHeader.biHeight;
}

DWORD CDibSection::GetLineWidth() const
{
	DUI_ASSERT(!IsNull());
	return m_dwLineWidth;
}

DWORD CDibSection::GetPixelWidth() const
{
	DUI_ASSERT(!IsNull());
	return m_bi.bmiHeader.biBitCount / 8;
}


WORD CDibSection::GetBitCount() const
{
	DUI_ASSERT(!IsNull());
	return m_bi.bmiHeader.biBitCount;
}

CARGB CDibSection::GetPixel(INT nX, INT nY, BOOL bTopToBottom /* = TRUE */) const
{
	DUI_ASSERT(!IsNull());
	DUI_ASSERT(nX >= 0 &&  nX < GetWidth()
		&& nY >= 0 && nY < GetHeight());

	LPBYTE lp  = NULL;
	if(bTopToBottom)
	{
		lp = m_lpData + (GetHeight() - nY - 1) * m_dwLineWidth + (nX * GetPixelWidth());
	}
	else
	{
		lp = m_lpData + (nY * m_dwLineWidth) + (nX * GetPixelWidth());
	}

	CARGB clr;
	if(GetBitCount() == 32)
	{
		clr.ReadFromDIB4(lp);
	}
	else if(GetBitCount() == 24)
	{
		clr.ReadFromDIB3(lp);
	}

	return clr;
}

BOOL CDibSection::SetPixel(INT nX, INT nY, const CARGB& clr, BOOL bTopToBottom /* = TRUE */)
{
	DUI_ASSERT(!IsNull());
	DUI_ASSERT(nX >= 0 &&  nX < GetWidth()
		&& nY >= 0 && nY < GetHeight());

	LPBYTE lp = NULL;

	if(bTopToBottom)
	{
		lp = GetBits() + (GetHeight() - nY - 1) * m_dwLineWidth + (nX * GetPixelWidth());		
	}
	else
	{
		lp = GetBits() + (nY * m_dwLineWidth) + (nX * GetPixelWidth());
	}

	if(GetBitCount() == 32)
	{
		clr.WriteToDIB4(lp);
	}
	else if(GetBitCount() == 24)
	{
		clr.WriteToDIB3(lp);
	}

	return TRUE;
}

BOOL CDibSection::Draw(HDC hDC, INT nOffsetX, INT nOffsetY) const
{
	DUI_ASSERT(!IsNull());
	if(IsNull() || hDC == NULL) return FALSE;


	::SetStretchBltMode(hDC,COLORONCOLOR);   
	::SetDIBitsToDevice(hDC, nOffsetX, nOffsetY,
		GetWidth(), GetHeight(), 0, 0, 
		0, GetHeight(), GetBits(),
		(BITMAPINFO*) &m_bi, DIB_RGB_COLORS);

	return TRUE;
}

BOOL CDibSection::Stretch(HDC hDC, const RECT& rt) const
{
	DUI_ASSERT(!IsNull());
	if(IsNull() || hDC == NULL) return FALSE;

	::SetStretchBltMode(hDC,COLORONCOLOR);   
	::StretchDIBits(hDC, rt.left, rt.top,
		rt.right - rt.left, rt.bottom - rt.top, 0, 0, GetWidth(), GetHeight(),
		GetBits(), (BITMAPINFO*) &m_bi, DIB_RGB_COLORS, SRCCOPY);
	
	return TRUE;
}

BOOL CDibSection::AlphaBlend(HDC hDC, const RECT& rt, BOOL bSrcAlpha /* = TRUE */, BYTE alphaConstant /* = 255 */)
{
	DUI_ASSERT(!IsNull());
	if(IsNull() || hDC == NULL) return FALSE;

	HDC dcMem = CreateCompatibleDC(hDC);
	INT nWidth = GetWidth();
	INT nHeight = GetHeight();
	HBITMAP bmpOld = (HBITMAP)::SelectObject(dcMem, m_hBitmap);
	
	WORD wBitCount = GetBitCount();
	BLENDFUNCTION bf = {0};
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = alphaConstant;
	if(bSrcAlpha)
	{
		bf.AlphaFormat = AC_SRC_ALPHA;
		DUI_ASSERT(wBitCount == 32);
	}
	
	BOOL bRet = ::AlphaBlend(hDC, rt.left, rt.top, rt.right - rt.left,
		rt.bottom - rt.top, dcMem, 0, 0, nWidth, nHeight, bf);

	::SelectObject(dcMem, bmpOld);
	::DeleteDC(dcMem);

	return bRet;
}

BOOL CDibSection::TransparentBlt(HDC hDC, const RECT& rt, COLORREF clrMask /* = RGB */)
{
	DUI_ASSERT(!IsNull());
	if(IsNull() || hDC == NULL) return FALSE;
	
	HDC dcMem = CreateCompatibleDC(hDC);
	INT nWidth = GetWidth();
	INT nHeight = GetHeight();
	HBITMAP bmpOld = (HBITMAP)::SelectObject(dcMem, m_hBitmap);
	
	BOOL bRet = ::TransparentBlt(hDC, rt.left, rt.top, rt.right - rt.left,
		rt.bottom - rt.top, dcMem, 0, 0, nWidth, nHeight, clrMask);
	
	::SelectObject(dcMem, bmpOld);
	::DeleteDC(dcMem);
	
	return bRet;
}

////////////////////////////  CDibSection  /////////////////////////

DUI_END_NAMESPCE