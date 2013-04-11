#ifndef __DUI_BUTTON_H__
#define __DUI_BUTTON_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

enum BTN_IMAGE_STATUS
{
	BTN_INDEX_NORMAL = 0,
	BTN_INDEX_HOVER,
	BTN_INDEX_PRESS,
	BTN_INDEX_DISABLE,
	BTN_INDEX_FOCUS,
	BTN_INDEX_CHECK,

	BTN_INDEX_COUNT
};

class DUILIB_API CButtonUIData
{
public:
	CRefPtr<CImageList> m_imageBK;
	CRefPtr<CImageList> m_imageFore;

	CRefPtr<CImageList> m_imageStatus1;
	CRefPtr<CImageList> m_imageStatus2;
};

class DUILIB_API IDUIButton: public CDUIControlBase
{
public:
	virtual VOID SetCheck(BOOL bChecked) = 0;
	virtual BOOL GetCheck() const = 0;

	virtual VOID SetButtonUIData(const CButtonUIData& data) = 0;
	virtual const CButtonUIData& GetButtonUIData() const = 0;

	virtual VOID SetButtonStyle(DWORD dwStyle) = 0;
	virtual DWORD GetButtonStyle() const = 0;

	virtual ~IDUIButton() { NULL; }
};


class DUILIB_API CDUIButton: public IDUIButton
{
public:
	typedef IDUIButton theBase;

	CDUIButton();
	virtual ~CDUIButton();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();

	virtual BOOL Activate();

	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet);

	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);

	//IDUIButton
	virtual VOID SetCheck(BOOL bSetChecked);
	virtual BOOL GetCheck() const;
	
	virtual VOID SetButtonUIData(const CButtonUIData& data);
	virtual const CButtonUIData& GetButtonUIData() const;

	virtual VOID SetButtonStyle(DWORD dwStyle);
	virtual DWORD GetButtonStyle() const;

protected:
	DWORD ParseButtonStyle(const CDUIString& strValue);
	BTN_IMAGE_STATUS GetImageStatusIndex();
	COLORREF GetColorByStatus(BTN_IMAGE_STATUS eStatus);

	virtual VOID PaintBkgnd(HDC dc);
	virtual VOID PaintForgnd(HDC dc);
	virtual VOID PaintText(HDC dc);
	virtual VOID PaintBorder(HDC dc);

protected:
	CButtonUIData m_ButtonData;
	DWORD m_dwButtonStyle;
	RECT m_rtIcon;
	RECT m_rtText;
};

class DUILIB_API CDUICheckButton: public CDUIButton
{
public:
	typedef CDUIButton theBase;
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);	
	
protected:
	CDUICheckButton();
	virtual VOID PaintForgnd(HDC dc);
	virtual VOID PaintBorder(HDC dc);	
};

class DUILIB_API CDUIRadioButton: public CDUICheckButton
{
public:
	typedef CDUICheckButton theBase;
	virtual CRefPtr<IDUIControl> Clone();
	virtual BOOL Activate();
};

class DUILIB_API CDUICheckBox: public CDUICheckButton
{
public:
	typedef CDUICheckButton theBase;
	
	virtual CRefPtr<IDUIControl> Clone();
	virtual BOOL Activate();
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
};

DUI_END_NAMESPCE


#endif //__DUI_BUTTON_H__