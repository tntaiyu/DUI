#include "DUIDibProcess.h"

#pragma  warning(disable:4244)

DUI_BGN_NAMESPCE

VOID ValidateDIBRect(CDibSection* pDst, LPRECT pRc)
{
	DUI_ASSERT(pDst != NULL);
	DUI_ASSERT(pRc != NULL);
	
	if( (DWORD)pRc->right > pDst->GetWidth() )
	{
		DUI_ASSERT(FALSE);
		pRc->right = pDst->GetWidth();
	}
	if( (DWORD)pRc->bottom > pDst->GetHeight() )
	{
		DUI_ASSERT(FALSE);
		pRc->bottom = pDst->GetHeight();
	}
	
	DUI_ASSERT(pRc->left >= 0 && pRc->top >= 0);
	DUI_ASSERT(pRc->right > pRc->left);
	DUI_ASSERT(pRc->bottom > pRc->top);
}

BOOL CFillIColorFilter::Process(CDibSection* pDst, const CARGB& clr, LPRECT prc /* = NULL */)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	RECT rtTemp = { 0 };
	if(prc == NULL)
	{
		rtTemp.left = 0;
		rtTemp.top = 0;
		rtTemp.right = pDst->GetWidth();
		rtTemp.bottom = pDst->GetHeight();
		
		prc = &rtTemp;
	}
	
	ValidateDIBRect(pDst, prc);
	
	int nPixelWidth = pDst->GetPixelWidth();
	int nHeight = pDst->GetHeight();
	int nLineWidth = pDst->GetLineWidth();
	LPBYTE lp = NULL;
	LPBYTE lpData = pDst->GetBits();
	
	for(int i=prc->top; i<prc->bottom; ++i )
	{
		for(int k=prc->left; k<prc->right; ++k)
		{
			lp = lpData + (nHeight - i - 1) * nLineWidth + (k * nPixelWidth);
			if(nPixelWidth == 4)
			{
				clr.WriteToDIB4(lp);
			}
			else if(nPixelWidth == 3)
			{
				clr.WriteToDIB3(lp);
			}
		}
	}

	return TRUE;
}

BOOL CStrippleImageFilter::Process(CDibSection* pDst, const CARGB& clr, DWORD divisor, LPRECT prc/* = NULL*/)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	RECT rtTemp = { 0 };
	if(prc == NULL)
	{
		rtTemp.left = 0;
		rtTemp.top = 0;
		rtTemp.right = pDst->GetWidth();
		rtTemp.bottom = pDst->GetHeight();
		
		prc = &rtTemp;
	}
	
	ValidateDIBRect(pDst, prc);
	
	int nPixelWidth = pDst->GetPixelWidth();
	int nHeight = pDst->GetHeight();
	int nLineWidth = pDst->GetLineWidth();
	LPBYTE lp = NULL;
	LPBYTE lpData = pDst->GetBits();
	
	for(int i=prc->top; i<prc->bottom; ++i)
	{
		for(int k=prc->left; k<prc->right; ++k)
		{
			if(i % divisor == 0 
				&& k % divisor == 0)
			{
				lp = lpData + (nHeight - i - 1) * nLineWidth + (k * nPixelWidth);
				if(nPixelWidth == 4)
				{
					clr.WriteToDIB4(lp);
				}
				else if(nPixelWidth == 3)
				{
					clr.WriteToDIB3(lp);
				}
			}
		}
	}
	
	return TRUE;
}

BOOL CGrayScaleImageFilter::Process(CDibSection* pDst, LPRECT prc /*= NULL*/)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	RECT rtTemp = { 0 };
	if(prc == NULL)
	{
		rtTemp.left = 0;
		rtTemp.top = 0;
		rtTemp.right = pDst->GetWidth();
		rtTemp.bottom = pDst->GetHeight();
		
		prc = &rtTemp;
	}
	
	ValidateDIBRect(pDst, prc);
	
	BYTE bGray;
	int nPixelWidth = pDst->GetPixelWidth();
	int nHeight = pDst->GetHeight();
	int nLineWidth = pDst->GetLineWidth();
	LPBYTE lp = NULL;
	LPBYTE lpData = pDst->GetBits();
	
	for(int i = prc->top; i < prc->bottom; ++i)
	{
		for(int k = prc->left; k < prc->right; ++k)
		{	
			lp = lpData + (nHeight - i - 1) * nLineWidth + (k * nPixelWidth);
			bGray = (lp[0] + lp[1] + lp[2]) / 3;
			lp[0] = lp[1] = lp[2] = bGray;
		}
	}
	
	return TRUE;
}

BOOL CWhiteBlackImageFilter::Process(CDibSection* pDst, LPRECT prc /* = NULL */, BYTE threshold /*= 128*/)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	RECT rtTemp = { 0 };
	if(prc == NULL)
	{
		rtTemp.left = 0;
		rtTemp.top = 0;
		rtTemp.right = pDst->GetWidth();
		rtTemp.bottom = pDst->GetHeight();
		
		prc = &rtTemp;
	}
	
	ValidateDIBRect(pDst, prc);
	
	BYTE bGray;
	int nPixelWidth = pDst->GetPixelWidth();
	int nHeight = pDst->GetHeight();
	int nLineWidth = pDst->GetLineWidth();
	LPBYTE lp = NULL;
	LPBYTE lpData = pDst->GetBits();
	
	for(int i = prc->top; i < prc->bottom; ++i)
	{
		for(int k = prc->left; k < prc->right; ++k)
		{	
			lp = lpData + (nHeight - i - 1) * nLineWidth + (k * nPixelWidth);
			bGray = (lp[0] + lp[1] + lp[2]) / 3;
			if(bGray > threshold)
			{
				lp[0] = lp[1] = lp[2] = 255;
			}
			else
			{
				lp[0] = lp[1] = lp[2] = 0;
			}
		}
	}
	
	return TRUE;
}



//alpha blend two DIB
BOOL CFadeImageFilter::Process(CDibSection* pDst, CDibSection* pSrc, short percent)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;
	DUI_ASSERT(pSrc != NULL && !pSrc->IsNull());
	if(pSrc->IsNull()) return FALSE;

	LPBYTE pSrcBits = pSrc->GetBits();
	LPBYTE pDstBits = pDst->GetBits();
	DWORD nWidth = pDst->GetWidth();
	DWORD nHeight = pDst->GetHeight();
	DWORD nLine = pDst->GetLineWidth();

	DUI_ASSERT(pSrc->GetWidth()==pDst->GetWidth());
	DUI_ASSERT(pSrc->GetHeight()==pDst->GetHeight());

	if(pSrc->GetWidth() != pDst->GetWidth()
		|| pSrc->GetHeight() != pDst->GetHeight()
		|| pSrc->GetPixelWidth() != pDst->GetPixelWidth())
	{
		return FALSE;
	}

	DWORD dwPixelWidth = pDst->GetPixelWidth();
	for( DWORD y=0; y<nHeight; y++ ) {
		LPBYTE pS = pSrcBits;
		LPBYTE pD = pDstBits;
		for( DWORD x=0; x<nWidth; x++ ) {
			for( DWORD	 z=0; z<dwPixelWidth; z++ ) {
				if(z < 3) //z=3 is alpha value
				{
					long c = *pS++ - *pD;
					c = (c * percent / 100);
					*pD++ = (BYTE)(*pD + c);
				}
			}
		}
		pSrcBits += nLine;
		pDstBits += nLine;
	}

	return TRUE;
}


//negative color for each pixel
BOOL CNegativeImageFilter::Process(CDibSection* pDst)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	LPBYTE pStream = pDst->GetBits();
	DWORD nWidth = pDst->GetWidth();
	DWORD nHeight = pDst->GetHeight();
	DWORD nLine = pDst->GetLineWidth();
	DWORD dwPixelWidth = pDst->GetPixelWidth();

	for( DWORD y=0; y<nHeight; y++ ) {
		LPBYTE p = pStream;
		for( DWORD x=0; x<nWidth; x++ ) {
			*p++ = (BYTE) (255-*p);
			*p++ = (BYTE) (255-*p);
			*p++ = (BYTE) (255-*p);
			if(dwPixelWidth == 4) //alpha value do not change
			{
				++p;
			}
		}
		pStream += nLine;
	}
	return TRUE;
}

//replace a color with another color
BOOL CReplaceColourFilter::Process(CDibSection* pDst, const CARGB& clrSearch, const CARGB& clrReplacement)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

  LPBYTE pDstBits = pDst->GetBits();
  DWORD nWidth = pDst->GetWidth();
  DWORD nHeight = pDst->GetHeight();
  DWORD dwPixelWidth = pDst->GetPixelWidth();
  //
  BYTE rSrc = clrSearch.GetR();
  BYTE gSrc = clrSearch.GetG();
  BYTE bSrc = clrSearch.GetB();
  BYTE aSrc = clrSearch.GetA();

  BYTE rRepl = clrReplacement.GetR();
  BYTE gRepl = clrReplacement.GetG();
  BYTE bRepl = clrReplacement.GetB();
  BYTE aRepl = clrReplacement.GetA();
  //
  for( DWORD y=0; y<nHeight; y++ ) {
     LPBYTE pD = pDstBits;
     for( DWORD x=0; x<nWidth; x++ ) {
		   if(dwPixelWidth == 4)
		   {
				if( *(pD+0)==bSrc &&
					*(pD+1)==gSrc &&
					*(pD+2)==rSrc &&
					*(pD+3)==aSrc ) 
				{
					*pD++ = bRepl;
					*pD++ = gRepl;
					*pD++ = rRepl;
					*pD++ = aRepl;
				}
				else
				{
				   pD += dwPixelWidth;
				}				
		   }
		   else
		   {
				if( *(pD+0)==bSrc &&
					*(pD+1)==gSrc &&
					*(pD+2)==rSrc ) 
				{
					*pD++ = bRepl;
					*pD++ = gRepl;
					*pD++ = rRepl;
				}
				else
				{
				   pD += dwPixelWidth;
				}
		   }
     }
     pDstBits += pDst->GetLineWidth();
  }
  return TRUE;
}


BOOL CSelectImageFilter::Process(CDibSection* pDst, CDibSection* pNew, LPRECT pSelectRect)
{
	DUI_ASSERT(pDst != NULL && pNew != NULL);
	DUI_ASSERT(!pDst->IsNull());
	DUI_ASSERT(pDst != pNew);
	if(pDst == pNew || pDst->IsNull()) return FALSE;

	RECT rtTemp = {0, 0, pDst->GetWidth(), pDst->GetHeight()};
	if(pSelectRect == NULL)
	{
		pSelectRect = &rtTemp;
	}

	ValidateDIBRect(pDst, pSelectRect);

	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nBitCount = pDst->GetBitCount();
	INT nLineWidth = pDst->GetLineWidth();
	INT nPixelWidth = pDst->GetPixelWidth();

	INT nNewWidth = pSelectRect->right - pSelectRect->left;
	INT nNewHeight = pSelectRect->bottom - pSelectRect->top;
	if(nNewHeight <= 0 || nNewWidth <= 0)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	pNew->Create(nNewWidth, nNewHeight, nBitCount);
	if(pNew->IsNull()) return FALSE;

	INT nLineCopy = nNewWidth * nPixelWidth;
	INT nNewLineWidth = pNew->GetLineWidth();

	LPBYTE pDstBit = pNew->GetBits();
	LPBYTE pSrc = pDst->GetBits();
	LPBYTE lpOld, lpNew;
	for(int i=0; i<nNewHeight; ++i)
	{
		lpOld = pSrc + (nHeight - i - pSelectRect->top - 1) * nLineWidth + (pSelectRect->left * nPixelWidth);
		lpNew = pDstBit + (nNewHeight - i - 1) * nNewLineWidth;
		memcpy(lpNew, lpOld, nLineCopy);
	}

	return TRUE;
}

BOOL CMirrorImageFilter::Process(CDibSection* pDst)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	
	CDibSection* dibTemp = pDst->Clone();
	LPBYTE lpData1 = pDst->GetBits();
	LPBYTE lpData2 = dibTemp->GetBits();
	
	for(INT i=0; i<nHeight; ++i)
	{
		for(INT j=0; j<nWidth; ++j)
		{
			for(INT k=0; k<nPixelWidth; ++k)
			{
				*(lpData2 + j * nPixelWidth + k) = *(lpData1 + nPixelWidth * (nWidth - j - 1) + k);
			}
		}

		lpData1 += nLineWidth;
		lpData2 += nLineWidth;
	}
	
	BOOL bRet = pDst->Transfer(dibTemp);
	delete dibTemp;

	return bRet;
}

BOOL CFlipImageFilter::Process(CDibSection* pDst)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;
	
	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	
	CDibSection* dibTemp = pDst->Clone();
	LPBYTE lpData1 = pDst->GetBits();
	LPBYTE lpData2 = dibTemp->GetBits();
	
	for(INT i=0; i<nHeight; ++i)
	{
		memcpy(lpData2, lpData1 + (nHeight - i - 1) * nLineWidth, nLineWidth);
		
		lpData2 += nLineWidth;
	}
	
	 BOOL bRet = pDst->Transfer(dibTemp);
	 delete dibTemp;

	 return bRet;
}

BOOL CRotateLeftFilter::Process(CDibSection* pDst)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;
	
	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	WORD wBitCount = pDst->GetBitCount();
	
	CDibSection* dibTemp = pDst->NewObject();
	dibTemp->Create(nHeight, nWidth, wBitCount);
	if(dibTemp->IsNull()) return FALSE;

	LPBYTE lpDataSrc = pDst->GetBits();
	LPBYTE lpDataDst = dibTemp->GetBits();
	INT nNewLineWidth = dibTemp->GetLineWidth();

	for(INT i=0; i<nHeight; ++i)
	{
		for(INT j=0; j<nWidth; ++j)
		{
			for(INT k=0; k<nPixelWidth; ++k)
			{
				*(lpDataDst + nNewLineWidth * j + (nHeight - i - 1) * nPixelWidth + k)
					= *(lpDataSrc + (nLineWidth * i + j * nPixelWidth) + k);
			}
		}
	}
	
	BOOL bRet = pDst->Transfer(dibTemp);
	delete dibTemp;

	return bRet;
}

BOOL CRotateRightFilter::Process(CDibSection* pDst)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	//Rotate 3 times
	BOOL bRet = CRotateLeftFilter::Process(pDst);
	if(bRet)
	{
		bRet = CRotateLeftFilter::Process(pDst);
		if(bRet)
		{
			bRet = CRotateLeftFilter::Process(pDst);
		}
	}

	return bRet;
}

BOOL CContrastFilter::Process(CDibSection* pDst, INT v)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;
	
	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	WORD wBitCount = pDst->GetBitCount();
	
	LPBYTE pData = pDst->GetBits();
	CARGB clr;
	
	for (INT i=0; i<nHeight; ++i)
	{
		LPBYTE lp = pData;
		for(INT j=0; j<nWidth; ++j)
		{
			if(nPixelWidth == 3)
			{
				clr.ReadFromDIB3(lp);
				ChangeContrast(v, clr);
				clr.WriteToDIB3(lp);
			}
			else if(nPixelWidth == 4)
			{
				clr.ReadFromDIB4(lp);
				ChangeContrast(v, clr);
				clr.WriteToDIB4(lp);
			}
			lp += nPixelWidth;
		}
		pData += nLineWidth;
	}

	return TRUE;
}

VOID CContrastFilter::ChangeContrast(INT nDelta, CARGB& clr)
{
	if(nDelta >= 0)
	{
		INT nLow = nDelta;
		INT nHigh = 255 - nDelta;
		float grad = ((float)(nHigh - nLow)) / 255;
		IncreaseContrast(clr.GetR(), nLow, nHigh, grad);
		IncreaseContrast(clr.GetG(), nLow, nHigh, grad);
		IncreaseContrast(clr.GetB(), nLow, nHigh, grad);
	}
	else
	{
		float grad = 255 / (255 + (float)nDelta + (float)nDelta);
		DecreaseContrast(clr.GetR(), nDelta, grad);
		DecreaseContrast(clr.GetG(), nDelta, grad);
		DecreaseContrast(clr.GetB(), nDelta, grad);
	}
}

VOID CContrastFilter::DecreaseContrast(BYTE& byte, const int level, const float grad)
{
	DUI_ASSERT(grad != 0.0);
	byte = (BYTE)(((int)(byte / grad)) - level);
}

VOID CContrastFilter::IncreaseContrast(BYTE& byte, const INT low, const INT high, const float grad)
{
	if(byte <= low)
	{
		byte = 0;
	}
	else if(low < byte && byte < high)
	{
		byte = (BYTE)((byte - low) / grad);
	}
	else
	{
		byte = 255;
	}
}

VOID CPixelizeImageFilter::FillAverageColor(LPBYTE lpBlock, INT nLineWidth, INT nPixelWidth, INT nWidth, INT nHeight)
{
	INT nR(0), nG(0), nB(0), nA(0);
	CARGB clrTemp;
	INT k, n;
	for(k=0; k<nHeight; ++k)
	{
		for(n=0; n<nWidth; ++n)
		{
			LPBYTE lpTemp = lpBlock + n * nPixelWidth + k * nLineWidth; 

			if(nPixelWidth == 3)
			{
				clrTemp.ReadFromDIB3(lpTemp);
			}
			else if(nPixelWidth == 4)
			{
				clrTemp.ReadFromDIB4(lpTemp);
			}
			nR += clrTemp.GetR();
			nG += clrTemp.GetG();
			nB += clrTemp.GetB();
			nA += clrTemp.GetA();
		}
	}
	
	//average the pixel color
	nR /= (nWidth * nHeight);
	nG /= (nWidth * nHeight);
	nB /= (nWidth * nHeight);
	nA /= (nWidth * nHeight);

	CARGB clrWrite(nR, nG, nB, nA);

	//fill back the new color
	for(k=0; k<nHeight; ++k)
	{
		for(n=0; n<nWidth; ++n)
		{
			LPBYTE lpTemp = lpBlock + n * nPixelWidth + k * nLineWidth;
			if(nPixelWidth == 3)
			{
				clrWrite.WriteToDIB3(lpTemp);
			}
			else if(nPixelWidth == 4)
			{
				clrWrite.WriteToDIB4(lpTemp);
			}
		}
	}
}

BOOL CPixelizeImageFilter::Process(CDibSection* pDst, INT v)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;
	
	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	
	DUI_ASSERT(v <= nWidth && v >= 2);
	if(v > nWidth) v = nWidth;
	if(v < 2) v = 2;
	
	LPBYTE lpData = pDst->GetBits();
	INT nWidthTime = nWidth / v;
	INT nHeightTime = nHeight / v;
	INT nWidthLeft = nWidth % v;
	INT nHeightLeft = nHeight % v;

	for(INT i=0; i<nHeightTime; ++i)
	{
		LPBYTE lpBlock = lpData + nLineWidth * i * v;
		for(INT j=0; j<nWidthTime; ++j)
		{
			LPBYTE lp = lpBlock + j * nPixelWidth * v;
			FillAverageColor(lp, nLineWidth, nPixelWidth, v, v);
		}
		
		//fill left part on each line
		if(nWidthLeft != 0)
		{
			LPBYTE lp = lpBlock + nWidthTime * nPixelWidth * v;
			FillAverageColor(lp, nLineWidth, nPixelWidth, nWidthLeft, v);
		}
	}

	//fill last left part line
	if(nHeightLeft != 0)
	{
		LPBYTE lpBlock = lpData + nLineWidth * nHeightTime * v;
		for(int j=0; j<nWidthTime; ++j)
		{
			LPBYTE lp = lpBlock + j * nPixelWidth * v;
			FillAverageColor(lp, nLineWidth, nPixelWidth, v, nHeightLeft);
		}

		//fill last left part line corner
		if(nWidthLeft != 0)
		{
			LPBYTE lp = lpBlock + nWidthTime * nPixelWidth * v;
			FillAverageColor(lp, nLineWidth, nPixelWidth, nWidthLeft, nHeightLeft);
		}
	}

	return TRUE;
}


BOOL CSharpenImageFilter::Process(CDibSection* pDst)
{
	return CConvoluteFilter::Process(pDst, FILTER_SHARPENING);
}

BOOL CGaussImageFilter::Process(CDibSection* pDst)
{
	return CConvoluteFilter::Process(pDst, FILTER_GUASSIANSMOOTH);
}

BOOL CEmbossImageFilter::Process(CDibSection* pDst)
{
	return CConvoluteFilter::Process(pDst, FILTER_EMBOSS135);
}

BOOL CEdgeImageFilter::Process(CDibSection* pDst)
{
	return CConvoluteFilter::Process(pDst, FILTER_LAPLASION);
}


//brighten a image, (100 + percent) / 100
BOOL CBrightenImageFilter::Process(CDibSection* pDst, short percent, LPRECT prc /*= NULL*/)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	RECT rtTemp = { 0 };
	if(prc == NULL)
	{
		rtTemp.left = 0;
		rtTemp.top = 0;
		rtTemp.right = pDst->GetWidth();
		rtTemp.bottom = pDst->GetHeight();
		
		prc = &rtTemp;
	}
	
	ValidateDIBRect(pDst, prc);
	
	int nPixelWidth = pDst->GetPixelWidth();
	int nHeight = pDst->GetHeight();
	int nLineWidth = pDst->GetLineWidth();
	LPBYTE lp = NULL;
	LPBYTE lpData = pDst->GetBits();
	
	percent += 100;
	
	for(int i = prc->top; i < prc->bottom; ++i)
	{
		for(int k = prc->left; k < prc->right; ++k)
		{	
			lp = lpData + (nHeight - i - 1) * nLineWidth + (k * nPixelWidth);
			lp[0] = min(lp[0] * percent / 100, 255);
			lp[1] = min(lp[1] * percent / 100, 255);
			lp[2] = min(lp[2] * percent / 100, 255);
		}
	}
	
	return TRUE;
}


BOOL CHueChangeFilter::Process(CDibSection* pDst, short percent)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	LPBYTE lpData = pDst->GetBits();
	double dbFactor = (percent + 100.0) / (double)100.0;

	LPBYTE lp = lpData;
	CHLSColor clrTemp;
	for(int i=0; i<nHeight; ++i)
	{
		for(int j=0; j<nWidth; ++j)
		{
			lp = lpData + j * nPixelWidth + i * nLineWidth;

			clrTemp.blue = lp[0];
			clrTemp.green = lp[1];
			clrTemp.red = lp[2];
			clrTemp.ToHLS();
			clrTemp.hue *= dbFactor;
			clrTemp.ToRGB();
			lp[0] = clrTemp.blue;
			lp[1] = clrTemp.green;
			lp[2] = clrTemp.red;
		}
	}
	
	return TRUE;
}

BOOL CSaturationChangeFilter::Process(CDibSection* pDst, short percent)
{
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	LPBYTE lpData = pDst->GetBits();
	double dbFactor = 0.0;
	dbFactor = (percent + 100.0) / (double)100.0;

	LPBYTE lp = lpData;
	CHLSColor clrTemp;
	for(int i=0; i<nHeight; ++i)
	{
		for(int j=0; j<nWidth; ++j)
		{
			lp = lpData + j * nPixelWidth + i * nLineWidth;

			clrTemp.blue = lp[0];
			clrTemp.green = lp[1];
			clrTemp.red = lp[2];
			clrTemp.ToHLS();
			clrTemp.saturation *= dbFactor;
			clrTemp.ToRGB();
			lp[0] = clrTemp.blue;
			lp[1] = clrTemp.green;
			lp[2] = clrTemp.red;
		}
	}
	
	return TRUE;
}


DUI_END_NAMESPCE