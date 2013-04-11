#include "DUIDibFilter.h"

DUI_BGN_NAMESPCE


void CDibFilter::Filter24bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy)
{
	memcpy(pDest, pSource, m_nHalf * 3);
	pDest   += m_nHalf * 3;
	pSource += m_nHalf * 3;

	for (int i=nWidth - 2 * m_nHalf; i>0; i--)
	{
		* pDest ++ = Kernel(pSource++, 3, dy);
		* pDest ++ = Kernel(pSource++, 3, dy);
		* pDest ++ = Kernel(pSource++, 3, dy);
	}

	memcpy(pDest, pSource, m_nHalf * 3);
}


void CDibFilter::Filter32bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy)
{
	memcpy(pDest, pSource, m_nHalf * 4);
	pDest   += m_nHalf * 4;
	pSource += m_nHalf * 4;

	for (int i=nWidth - 2 * m_nHalf; i>0; i--)
	{
		* pDest ++ = Kernel(pSource++, 4, dy);
		* pDest ++ = Kernel(pSource++, 4, dy);
		* pDest ++ = Kernel(pSource++, 4, dy);
		* pDest ++ = * pSource++;				// copy alpha channel
	}

	memcpy(pDest, pSource, m_nHalf * 4);
}


TCHAR szSmooth[]        = _T("Smooth");
TCHAR szGuasianSmooth[] = _T("Guasian Smooth");
TCHAR szSharpening[]    = _T("Sharpening");
TCHAR szLaplasian[]     = _T("Laplasian");
TCHAR szEmboss135[]     = _T("Emboss 135");
TCHAR szEmboss90[]      = _T("Emboss 90_50%");

CDib33Filter< 1,  1,  1,  1,  1,  1,  1,  1,  1, 9,   0, false, szSmooth        > filter33_smooth;
CDib33Filter< 0,  1,  0,  1,  4,  1,  0,  1,  0, 8,   0, false, szGuasianSmooth > filter33_guasiansmooth;
CDib33Filter< 0, -1,  0, -1,  9, -1,  0, -1,  0, 5,   0, true,  szSharpening    > filter33_sharpening;
CDib33Filter<-1, -1, -1, -1,  8, -1, -1, -1, -1, 1, 128, true,  szLaplasian     > filter33_laplasian;
CDib33Filter< 1,  0,  0,  0,  0,  0,  0,  0, -1, 1, 128, true,  szEmboss135     > filter33_emboss135;
CDib33Filter< 0,  1,  0,  0,  0,  0,  0, -1,  0, 2, 128, true,  szEmboss90      > filter33_emboss90;

CDibFilter * CConvoluteFilter::m_sStockFilters[] = {
	NULL,
	& filter33_smooth,
	& filter33_guasiansmooth,
	& filter33_sharpening,
	& filter33_laplasian,
	& filter33_emboss135,
	& filter33_emboss90
};

BOOL CConvoluteFilter::Process(CDibSection* pDst, CDibFilter* pFilter)
{
	DUI_ASSERT(pFilter != NULL);
	DUI_ASSERT(pDst != NULL && !pDst->IsNull());
	if(pDst->IsNull()) return FALSE;

	CDibSection* pDibTemp = pDst->Clone();
	if(pDibTemp == NULL || pDibTemp->IsNull())
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	INT nWidth = pDst->GetWidth();
	INT nHeight = pDst->GetHeight();
	INT nPixelWidth = pDst->GetPixelWidth();
	INT nLineWidth = pDst->GetLineWidth();
	
	LPBYTE lpSrcBits = pDst->GetBits();
	LPBYTE pDestBits = pDibTemp->GetBits();
	BOOL bRet(FALSE);

	for (int y=0; y<nHeight; y++)
	{
		unsigned char * pBuffer = (unsigned char *) lpSrcBits   + nLineWidth * y;
		unsigned char * pDest   = (unsigned char *) pDestBits + nLineWidth * y;

		if (  (y>=pFilter->GetHalf()) && (y<(nHeight - pFilter->GetHalf())) )
		{
			switch ( nPixelWidth )
			{
				case 3:	// 24-bpp RGB
					pFilter->Filter24bpp(pDest, pBuffer, nWidth, nLineWidth);
					bRet = TRUE;
					break;

				case 4: // 32-bpp RGBA
					pFilter->Filter32bpp(pDest, pBuffer, nWidth, nLineWidth);
					bRet = TRUE;
					break;

				default:
					DUI_ASSERT(FALSE);
					break;
			}	
		}
		else
		{
			memcpy(pDest, pBuffer, nLineWidth);
		}
	}

	if(bRet)
	{
		bRet = pDst->Transfer(pDibTemp);
	}

	delete pDibTemp;

	return bRet;
}
BOOL CConvoluteFilter::Process(CDibSection* pDst, Convolute_Type eType)
{
	DUI_ASSERT(eType > FILTER_IDENTITY && eType < FILTER_TYPE_END);
	CDibFilter* pFilter = NULL;
	if(eType > FILTER_IDENTITY && eType < FILTER_TYPE_END)
	{
		pFilter = m_sStockFilters[eType];
	}
	if(pFilter == NULL) return FALSE;

	return CConvoluteFilter::Process(pDst, pFilter);
}

DUI_END_NAMESPCE