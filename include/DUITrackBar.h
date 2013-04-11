#ifndef __DUI_TRACK_BAR_H__
#define __DUI_TRACK_BAR_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

enum TRACK_BAR_IMAGE_INDEX
{
	TRACK_BAR_IMAGE_BK = 0,
	TRACK_BAR_IMAGE_FORE,

	TRACK_BAR_IMAGE_COUNT
};

class DUILIB_API CTrackBarUIData
{
public:
	CRefPtr<CImageList> m_pImageBK; // follow TRACK_BAR_IMAGE_INDEX
	CRefPtr<CImageList> m_pImageThumb;
};

class IDUIButton;
class DUILIB_API IDUITrackBar: public CDUIControlBase
{
public:
	virtual ~IDUITrackBar() { NULL; }

	virtual VOID SetRange(INT nMin, INT nMax) = 0;
	virtual VOID GetRange(INT& nMin, INT& nMax) const = 0;

	virtual VOID SetPos(INT nPos) = 0;
	virtual INT GetPos() const = 0;

	virtual VOID SetStep(INT nStep) = 0;
	virtual INT GetStep() const = 0;
	
	virtual IDUIButton* GetThumbButton() = 0;

	virtual VOID SetUIData(const CTrackBarUIData& data) = 0;
	virtual const CTrackBarUIData& GetUIData() const = 0;
};


class DUILIB_API CDUITrackBarBase: public IDUITrackBar
{
public:
	typedef IDUITrackBar theBase;
	CDUITrackBarBase();
	virtual ~CDUITrackBarBase();
	

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet);
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	
	virtual VOID SetTooltip(const CDUIString& strTooltip);
	virtual VOID SetControlRect(const RECT& rt);

	//IDUITrackBar
	virtual VOID SetRange(INT nMin, INT nMax);
	virtual VOID GetRange(INT& nMin, INT& nMax) const;

	virtual VOID SetPos(INT nPos);
	virtual INT GetPos() const;

	virtual VOID SetStep(INT nStep);
	virtual INT GetStep() const;

	virtual IDUIButton* GetThumbButton();

	virtual VOID SetUIData(const CTrackBarUIData& data);
	virtual const CTrackBarUIData& GetUIData() const;

protected:
	virtual BOOL IsVertical() const = 0;
	
	virtual VOID OnCreate();
	virtual VOID OnDestroy();

	virtual VOID PaintBkgnd(HDC dc);
	virtual VOID PaintBorder(HDC dc);

	INT CalculateThumbPos(POINT ptHit);
	VOID SetThumbPos(INT nNewPos);
	INT GetThumbPos();
	INT GetThumbLength();
	RECT GetThumbRect();

	BOOL OnLButtonDown(const DUIEvent& info);
	BOOL OnMouseMove(const DUIEvent& info);
	BOOL OnLButtonUp(const DUIEvent& info);
	BOOL OnKeyDown(const DUIEvent& info);
	
	VOID CreateThumb();

protected:
	CTrackBarUIData m_uiData;
	INT m_nMin;
	INT m_nMax;
	INT m_nPos;
	BOOL m_bTracking;
	INT m_nStep;
};

class CDUIVerticalTrackBar: public CDUITrackBarBase
{
public:
	virtual CRefPtr<IDUIControl> Clone();
	
protected:
	virtual BOOL IsVertical() const;
};

class CDUIHorizontalTrackBar: public CDUITrackBarBase
{
public:
	virtual CRefPtr<IDUIControl> Clone();
	
protected:
	virtual BOOL IsVertical() const;
};

DUI_END_NAMESPCE
#endif //__DUI_TRACK_BAR_H__