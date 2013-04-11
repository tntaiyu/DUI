#ifndef __DUI_DIB_FILTER__
#define __DUI_DIB_FILTER__

#include "DUIDib.h"

DUI_BGN_NAMESPCE


class DUILIB_API CDibFilter
{
	int	m_nHalf;

	virtual BYTE Kernel(BYTE * pPixel, int dx, int dy) = 0;

public:

	int GetHalf(void) const
	{
		return m_nHalf;
	}

	CDibFilter(void)
	{
		m_nHalf = 1;
	}

	virtual ~ CDibFilter()
	{
	}

	virtual void Filter24bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy);
	virtual void Filter32bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy);
};

template <int k00, int k01, int k02, int k10, int k11, int k12, int k20, int k21, int k22, 
		  int weight, int add, bool checkbound, TCHAR * name>
class DUILIB_API CDib33Filter : public CDibFilter
{
	virtual BYTE Kernel(BYTE * pPixel, int dx, int dy)
	{
		int rslt = ( pPixel[-dy-dx] * k00 + pPixel[-dy] * k01 + pPixel[-dy+dx] * k02 +
			         pPixel[   -dx] * k10 + pPixel[0]   * k11 + pPixel[   +dx] * k12 +
			         pPixel[ dy-dx] * k20 + pPixel[dy]  * k21 + pPixel[ dy+dx] * k22 ) / weight + add;

		if ( checkbound )
			if ( rslt < 0 )
				return 0;
			else if ( rslt > 255 )
				return 255;

		return rslt;
	}
};


enum Convolute_Type
{
	FILTER_IDENTITY,

	FILTER_SMOOTH,
	FILTER_GUASSIANSMOOTH,
	FILTER_SHARPENING,
	FILTER_LAPLASION,
	FILTER_EMBOSS135,
	FILTER_EMBOSS90,

	FILTER_TYPE_END
};

class DUILIB_API CConvoluteFilter
{
public:
	static BOOL Process(CDibSection* pDst, Convolute_Type eType);
	static BOOL Process(CDibSection* pDst, CDibFilter* pFilter);
protected:
	static CDibFilter * m_sStockFilters[];
};

DUI_END_NAMESPCE


#endif //__DUI_DIB_FILTER__