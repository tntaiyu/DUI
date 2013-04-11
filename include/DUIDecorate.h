#ifndef __DUI_DECORATE_H__
#define __DUI_DECORATE_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API CDUIPaddingPanel: public CDUIControlBase
{
public:
	typedef CDUIControlBase theBase;
	virtual CRefPtr<IDUIControl> Clone();
};

class DUILIB_API CFrameUIData
{
public:
	CRefPtr<CImageList> m_pImageLeft;
	CRefPtr<CImageList> m_pImageRight;
	CRefPtr<CImageList> m_pImageTop;
	CRefPtr<CImageList> m_pImageBottom;

	CARGB m_clrFrame;
};

class DUILIB_API IDUIFramePanel: public CDUIControlBase
{
public:
	virtual ~IDUIFramePanel() { NULL; }

	virtual VOID SetFrameRect(const RECT& rtFrame) = 0;
	virtual RECT GetFrameRect() const = 0;

	virtual VOID SetFrameUIData(const CFrameUIData& data) = 0;
	virtual const CFrameUIData& GetFrameUIData() const = 0;

	virtual VOID SetDrawMode(DWORD dwDrawMode) = 0;
	virtual DWORD GetDrawMode() const = 0;

	virtual VOID SetImageIndex(INT nIndex) = 0;
	virtual INT GetImageIndex() const = 0;
};

class DUILIB_API CDUIFramePanel: public IDUIFramePanel
{
public:
	typedef IDUIFramePanel theBase;
	CDUIFramePanel();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual VOID SetControlRect(const RECT& rt);

	//IDUIFramePanel
	virtual VOID SetFrameRect(const RECT& rtFrame);
	virtual RECT GetFrameRect() const;
	
	virtual VOID SetFrameUIData(const CFrameUIData& data);
	virtual const CFrameUIData& GetFrameUIData() const;
	
	virtual VOID SetDrawMode(DWORD dwDrawMode);
	virtual DWORD GetDrawMode() const;

	virtual VOID SetImageIndex(INT nIndex);
	virtual INT GetImageIndex() const;

protected:
	virtual VOID PaintBkgnd(HDC dc);

	VOID CalculateImageFrame();
	VOID GetDrawFrame(RECT& rtLeft, RECT& rtRight, RECT& rtTop, RECT& rtBottom);

	VOID DrawFrameWithImage(HDC dc);
	VOID DrawFrameWithColor(HDC dc);

protected:
	CFrameUIData m_uiData;
	DWORD m_dwDrawMode;
	RECT m_rtFrame;
	INT m_nImageIndex;
};


DUI_END_NAMESPCE

#endif //__DUI_DECORATE_H__