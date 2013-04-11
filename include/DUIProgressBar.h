#ifndef __DUI_PROGRESS_BAR_H__
#define __DUI_PROGRESS_BAR_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

enum PROCRSS_BAR_IMAGE_INDEX
{
	PROCRSS_BAR_IMAGE_BK = 0,
	PROCRSS_BAR_IMAGE_FORE,

	PROCRSS_BAR_IMAGE_COUNT
};

class DUILIB_API CProgressBarUIData
{
public:
	CRefPtr<CImageList> m_imageBK;
};

class DUILIB_API IDUIProgressBar: public CDUIControlBase
{
public:
	virtual ~IDUIProgressBar() { NULL; }

	virtual VOID SetRange(INT nMin, INT nMax) = 0;
	virtual VOID GetRange(INT& nMin, INT& nMax) const = 0;

	virtual VOID SetPos(INT nPos) = 0;
	virtual INT GetPos() const = 0;
	virtual BOOL OffsetPos(INT nOffset) = 0;

	virtual VOID SetUIData(const CProgressBarUIData& data) = 0;
	virtual const CProgressBarUIData& GetUIData() const = 0;
};


class DUILIB_API CDUIProgressBarBase: public IDUIProgressBar
{
public:
	typedef IDUIProgressBar theBase;
	CDUIProgressBarBase();
	virtual ~CDUIProgressBarBase();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet);
	

	//IDUIProgressBar
	virtual VOID SetRange(INT nMin, INT nMax);
	virtual VOID GetRange(INT& nMin, INT& nMax) const;

	virtual VOID SetPos(INT nPos);
	virtual INT GetPos() const;
	virtual BOOL OffsetPos(INT nOffset);

	virtual VOID SetUIData(const CProgressBarUIData& data);
	virtual const CProgressBarUIData& GetUIData() const;
	
protected:
	VOID GetProgressRect(const RECT& rtTotal, RECT& rt);
	virtual BOOL IsVertical() const = 0;
	
	virtual VOID PaintBkgnd(HDC dc);
	virtual VOID PaintForgnd(HDC dc);

protected:
	INT m_nMin;
	INT m_nMax;
	INT m_nPos;
	CProgressBarUIData m_uiData;
};

class DUILIB_API CDUIVerticalProgressBar: public CDUIProgressBarBase
{
public:
	CRefPtr<IDUIControl> Clone();

protected:
	virtual BOOL IsVertical() const;
};

class DUILIB_API CDUIHorizontalProgressBar: public CDUIProgressBarBase
{
public:
	CRefPtr<IDUIControl> Clone();

protected:
	virtual BOOL IsVertical() const;
};

DUI_END_NAMESPCE


#endif