#ifndef __DUI_CONTROL_CONTAINER_H__
#define __DUI_CONTROL_CONTAINER_H__

#include "DUIControlBase.h"
#include "DUIScrollbar.h"
#include <vector>

DUI_BGN_NAMESPCE

class DUILIB_API CDUIHorizontalLayout: public CDUIControlBase
{
public:
	typedef CDUIControlBase theBase;
	CDUIHorizontalLayout();
	CDUIHorizontalLayout(const CDUIHorizontalLayout& obj);

	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual VOID SetControlRect(const RECT& rt);
	virtual CRefPtr<IDUIControl> Clone();

	VOID SetEnableScroll(BOOL bEnable);
	BOOL GetEnableScroll() const;

protected:
	virtual IDUIScrollBar* GetHorizontalSB();

	virtual VOID OnCreate();
	virtual VOID OnDestroy();

protected:
	CRefPtr<IDUIScrollBar> m_pHorSB;
	BOOL m_bEnableScroll;
};


class DUILIB_API CDUIVerticalLayout: public CDUIControlBase
{
public:
	typedef CDUIControlBase theBase;
	CDUIVerticalLayout();
	CDUIVerticalLayout(const CDUIVerticalLayout& obj);

	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual VOID SetControlRect(const RECT& rt);
	virtual CRefPtr<IDUIControl> Clone();

	VOID SetEnableScroll(BOOL bEnable);
	BOOL GetEnableScroll() const;

protected:
	virtual IDUIScrollBar* GetVerticalSB();

	virtual VOID OnCreate();
	virtual VOID OnDestroy();

protected:
	CRefPtr<IDUIScrollBar> m_pVertSB;
	BOOL m_bEnableScroll;
};

class DUILIB_API IDUITileLayout: public CDUIControlBase
{
public:
	virtual VOID SetColumnCount(INT nCount) = 0;
	virtual INT GetColumnCount() const = 0;
};

class DUILIB_API CDUITileLayout: public IDUITileLayout
{
public:
	typedef IDUITileLayout theBase;
	CDUITileLayout();
	CDUITileLayout(const CDUITileLayout& obj);

	virtual VOID SetControlRect(const RECT& rt);
	virtual CRefPtr<IDUIControl> Clone();
	
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual LPVOID GetInterface(const CDUIString& strName);

	virtual VOID SetColumnCount(INT nCount);
	virtual INT GetColumnCount() const;

protected:
	INT CalculateColumnCount(RECT& rtUsable);

protected:
	virtual IDUIScrollBar* GetVerticalSB();

	virtual VOID OnCreate();
	virtual VOID OnDestroy();

protected:
	CRefPtr<IDUIScrollBar> m_pVertSB;
	INT m_nColumns;
};

class DUILIB_API CDUIDialogLayout: public CDUIControlBase
{
public:
	CDUIDialogLayout();
	~CDUIDialogLayout();

	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet);
	virtual VOID SetControlRect(const RECT& rt);
	virtual CRefPtr<IDUIControl> Clone();
	
protected:
	VOID CalculateDlgSize();
	VOID ClearData();
	
protected:
	SIZE m_sizeDialog;
	BOOL m_bFirstLayout;
	
	struct CStretchElement
	{
		IDUIControl* m_pControl;
		DWORD m_dwMode;
		RECT m_rtPos;
	};
	
	std::vector<CStretchElement*> m_arElements;
};

class DUILIB_API IDUIDockLayout: public CDUIControlBase
{
public:
	virtual VOID SetLastFill(BOOL bSet) = 0;
	virtual BOOL GetLastFill() const = 0;
};

class DUILIB_API CDUIDockLayout: public IDUIDockLayout
{
public:
	CDUIDockLayout();
	typedef IDUIDockLayout theBase;

	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual VOID SetControlRect(const RECT& rt);
	virtual CRefPtr<IDUIControl> Clone();
	virtual LPVOID GetInterface(const CDUIString& strName);

	//IDUIDockLayout
	virtual VOID SetLastFill(BOOL bSet);
	virtual BOOL GetLastFill() const;

protected:
	VOID DockControl(CRefPtr<IDUIControl> pControl, RECT& rtAvailable, BOOL bLastControl);

protected:
	BOOL m_bLastFill;
};

class DUILIB_API IDUITabLayout: public CDUIControlBase
{
public:
	virtual ~IDUITabLayout() { NULL; }

	virtual BOOL SetSelectIndex(INT nIndex) = 0;
	virtual INT GetSelectIndex() const = 0;
};

class DUILIB_API CDUITabLayout: public IDUITabLayout
{
public:
	CDUITabLayout();
	typedef IDUITabLayout theBase;
	
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual VOID SetControlRect(const RECT& rt);
	virtual CRefPtr<IDUIControl> Clone();
	virtual LPVOID GetInterface(const CDUIString& strName);

	//IDUITabLayout
	virtual BOOL SetSelectIndex(INT nIndex);
	virtual INT GetSelectIndex() const;

protected:
	virtual VOID OnCreate();

protected:
	VOID UpdateControls();
	VOID CheckAndResetFocus(IDUIControl* pControl);

protected:
	INT m_nSelectIndex;
};

DUI_END_NAMESPCE


#endif //__DUI_CONTROL_CONTAINER_H__