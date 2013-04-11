#ifndef __DUI_DIB_PROCESS_H__
#define __DUI_DIB_PROCESS_H__

#include "DUIDib.h"
#include "DUIDibFilter.h"

DUI_BGN_NAMESPCE

template<typename TFilter>
BOOL DUILIB_API FilterImage(CDibSection* pImage)
{
	return	TFilter::Process(pImage);
}

//fill a color in  rect
class DUILIB_API CFillIColorFilter
{
public:
	static BOOL Process(CDibSection* pDst, const CARGB& clr, LPRECT prc = NULL);
};


//draw a color on every interval point
class DUILIB_API CStrippleImageFilter
{
public:
	static BOOL Process(CDibSection* pDst, const CARGB& clr, DWORD divisor, LPRECT prc = NULL);
};

//gray a image and make it like disabled
class DUILIB_API CGrayScaleImageFilter
{
public:
	static BOOL Process(CDibSection* pDst, LPRECT prc = NULL);
};

class DUILIB_API CWhiteBlackImageFilter
{
public:
	static BOOL Process(CDibSection* pDst, LPRECT prc = NULL, BYTE threshold = 128);
};

//alpha blend two DIB
class DUILIB_API CFadeImageFilter
{
public:
	static BOOL Process(CDibSection* pDst, CDibSection* pSrc, short percent);
};


//negative color for each pixel
class DUILIB_API CNegativeImageFilter
{
public:
	static BOOL Process(CDibSection* pDst);
};

//replace a color with another color
class DUILIB_API CReplaceColourFilter
{
public:
   static BOOL Process(CDibSection* pDst, const CARGB& clrSearch, const CARGB& clrReplacement);
};


//select a sub image
class DUILIB_API CSelectImageFilter
{
public:
	static BOOL Process(CDibSection* pDst, CDibSection* pNew, LPRECT pSelectRect = NULL);
};

//left and right swap for the image
class DUILIB_API CMirrorImageFilter
{
public:
	static BOOL Process(CDibSection* pDst);
};

//top and bottom swap for the image
class DUILIB_API CFlipImageFilter
{
public:
	static BOOL Process(CDibSection* pDst);
};

//rotate left
class DUILIB_API CRotateLeftFilter
{
public: 
	static BOOL Process(CDibSection* pDst);
};

//rotate right
class DUILIB_API CRotateRightFilter
{
public: 
	static BOOL Process(CDibSection* pDst);
};

//contrast
class DUILIB_API CContrastFilter
{
public:
	//v (-100 to 100)
	static BOOL Process(CDibSection* pDst, INT v);

protected:
	static VOID IncreaseContrast(BYTE& byte, const INT low, const INT high, const float grad);
	static VOID DecreaseContrast(BYTE& byte, const int level, const float grad);
	static VOID ChangeContrast(INT nDelta, CARGB& clr);
};

//combine every v pixels to one pixel
class DUILIB_API CPixelizeImageFilter
{
public:
	static BOOL Process(CDibSection* pDst, INT v);

protected:
	static VOID FillAverageColor(LPBYTE lpBlock, INT nLineWidth, INT nPixelWidth, INT nWidth, INT nHeight);
};

//sharpen the image
class DUILIB_API CSharpenImageFilter: public CConvoluteFilter
{
public:
	static BOOL Process(CDibSection* pDst);
};

//make image not clear
class DUILIB_API CGaussImageFilter: public CConvoluteFilter
{
public:
	static BOOL Process(CDibSection* pDst);
};

class DUILIB_API CEmbossImageFilter: public CConvoluteFilter
{
public:
	static BOOL Process(CDibSection* pDst);
};

class DUILIB_API CEdgeImageFilter: public CConvoluteFilter
{
public:
	static BOOL Process(CDibSection* pDst);
};


//brighten a image, (100 + percent) / 100
class DUILIB_API CBrightenImageFilter
{
public:
	static BOOL Process(CDibSection* pDst, short percent, LPRECT prc = NULL);
};

//Hue change for a image, (100 + percent) / 100
class DUILIB_API CHueChangeFilter
{
public:
	static BOOL Process(CDibSection* pDst, short percent);
};

//saturation change for a image, (100 + percent) / 100
class DUILIB_API CSaturationChangeFilter
{
public:
	static BOOL Process(CDibSection* pDst, short percent);
};

DUI_END_NAMESPCE

#endif //__DUI_DIB_PROCESS_H__