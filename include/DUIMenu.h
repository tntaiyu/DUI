#ifndef __DUI_MENU_H__
#define __DUI_MENU_H__

#include "DUIControlContainer.h"


DUI_BGN_NAMESPCE

enum MENU_ICON_STATUS_INDEX
{	
	MENU_ICON_NORMAL,
	MENU_ICON_SELECT,
	MENU_ICON_DISABLE,
	MENU_ICON_CHECK,

	MENU_ICON_COUNT
};

class IDUIMenu;
class CDUIMenuWnd;

class DUILIB_API IDUIMenuItem: public CDUIControlBase
{
public:
	virtual ~IDUIMenuItem() { NULL; }

	virtual VOID SetMenuItemStyle(DWORD dwStyle) = 0;
	virtual DWORD GetMenuItemStyle() const = 0;
	
	virtual	VOID SetIcon(CRefPtr<CImageList> pIcon) = 0;
	virtual CRefPtr<CImageList> GetIcon() const = 0;

	virtual VOID SetSelect(BOOL bSelect) = 0;
	virtual BOOL GetSelect() const = 0;

	virtual VOID SetSelectFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetSelectFrameBK() const = 0;

	virtual VOID SetItemData(DWORD dwData) = 0;
	virtual DWORD GetItemData() const = 0;

	virtual IDUIMenu* GetChildMenu() const = 0;

	virtual INT GetIdealWidth() = 0;

};	


class DUILIB_API CDUIMenuItem: public IDUIMenuItem
{
public:
	typedef IDUIMenuItem theBase;
	CDUIMenuItem();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);	
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	virtual VOID SetText(const CDUIString& strText);

	virtual VOID SetControlRect(const RECT& rt);
	virtual VOID DoPaint(HDC dc, const RECT& rtDraw);

	virtual IDUIControl* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	//IDUIMenuItem
	virtual VOID SetMenuItemStyle(DWORD dwStyle) ;
	virtual DWORD GetMenuItemStyle() const ;
	
	virtual	VOID SetIcon(CRefPtr<CImageList> pIcon) ;
	virtual CRefPtr<CImageList> GetIcon() const;

	virtual VOID SetSelect(BOOL bSelect) ;
	virtual BOOL GetSelect() const ;

	virtual VOID SetSelectFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) ;
	virtual CRefPtr<CDUIFrameBK> GetSelectFrameBK() const ;

	virtual VOID SetItemData(DWORD dwData) ;
	virtual DWORD GetItemData() const;

	virtual IDUIMenu* GetChildMenu() const;

	virtual INT GetIdealWidth();

protected:
	virtual VOID PaintBkgnd(HDC dc);
	virtual VOID PaintForgnd(HDC dc);
	virtual VOID PaintText(HDC dc);

	virtual BOOL BeforeAddControl(CRefPtr<IDUIControl> pItem);

protected:
	RECT GetBKRect();
	RECT GetIconRect();
	RECT GetTextRect();
	RECT GetArrowRect();

	IDUIMenu* GetMenuOwner();
	
	DWORD ParseMenuItemStyle(const CDUIString& strStyle);

	BOOL IsMenuItemEnable();

protected:
	DWORD m_dwMenuStyle;
	CRefPtr<CImageList> m_pIcon;
	CRefPtr<CDUIFrameBK> m_pSelectFrameBK;
	BOOL m_bSelect;
	DWORD m_dwUserData;
};	


class DUILIB_API IDUIMenu: public CDUIVerticalLayout
{
public:
	virtual CRefPtr<IDUIMenuItem> TrackPopupMenu(IDUIControl* pOwner, INT nX, INT nY) = 0;

	virtual VOID SetIconRectWidth(INT nWidth) = 0;
	virtual INT GetIconRectWidth() = 0;

	virtual VOID SetArrowRectWidth(INT nWidth) = 0;
	virtual INT GetArrowRectWidth() = 0;

	virtual VOID SetArrowImage(CRefPtr<CImageList> pArrowImage) = 0;
	virtual CRefPtr<CImageList> GetArrowImage() const = 0;

	virtual VOID SetIconFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetIconFrameBK() const = 0;

	virtual VOID SetMenuWndFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetMenuWndFrameBK() const = 0;

	virtual VOID SetSeparateLine(CRefPtr<CImageList> pSeperatorImage) = 0;
	virtual CRefPtr<CImageList> GetSeparateLine() const = 0;
};

class DUILIB_API CDUIMenu: public IDUIMenu
{
public:
	typedef IDUIMenu theBase;
	CDUIMenu();
	CDUIMenu(const CDUIMenu& menu);
	~CDUIMenu();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);	
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	// IDUIMenu
	virtual CRefPtr<IDUIMenuItem> TrackPopupMenu(IDUIControl* pOwner, INT nX, INT nY);

	virtual VOID SetIconRectWidth(INT nWidth);
	virtual INT GetIconRectWidth();

	virtual VOID SetArrowRectWidth(INT nWidth);
	virtual INT GetArrowRectWidth();

	virtual VOID SetArrowImage(CRefPtr<CImageList> pArrowImage);
	virtual CRefPtr<CImageList> GetArrowImage() const;

	virtual VOID SetIconFrameBK(CRefPtr<CDUIFrameBK> pFrameBK);
	virtual CRefPtr<CDUIFrameBK> GetIconFrameBK() const;

	virtual VOID SetMenuWndFrameBK(CRefPtr<CDUIFrameBK> pFrameBK);
	virtual CRefPtr<CDUIFrameBK> GetMenuWndFrameBK() const;

	virtual VOID SetSeparateLine(CRefPtr<CImageList> pSeperatorImage);
	virtual CRefPtr<CImageList> GetSeparateLine() const;

	static CRefPtr<IDUIMenu>  CreateFromXML(const CDUIString& strXML);
	static CRefPtr<IDUIMenu> CreateFromResource(const CDUIString& strMenuId);

protected:
	virtual VOID PaintBkgnd(HDC dc);

	virtual VOID OnCreate();
	virtual VOID OnDestroy();

	virtual BOOL BeforeAddControl(CRefPtr<IDUIControl> pItem);

protected:
	CRefPtr<CDUIFrameBK> GetDefaultMenuWndFrameBK();
	SIZE GetMenuControlSize();
	SIZE GetMenuWndSize();
	RECT GetIdealMenuWndRect(HWND hWndParent, SIZE sizeMenuWnd, POINT ptRight, POINT ptLeft, BOOL bUseFirst);
	
	BOOL IsMemuWndPopup();
	BOOL ShowMenuWnd(IDUIControl* pOwner, POINT ptRight, POINT ptLeft, BOOL bUseFirt = TRUE);
	VOID RunMenuMsgLoop();

	VOID CancelMenu(BOOL bCancelAll = TRUE);

	VOID OnMouseHover(const DUIEvent& info);
	VOID OnMouseClick(const DUIEvent& info);
	VOID OnKeyDown(const DUIEvent& info);
	VOID OnChar(const DUIEvent& info);

	INT GetNextSelectIndex(BOOL bNext);
	VOID SetSelectIndex(INT nIndex, BOOL bFromMouse = TRUE);

	
	VOID ClickMenuItem(IDUIControl* pMenuItem);

	VOID ShowChildMenu(IDUIMenuItem* pMenuItem, BOOL bSelectFirst = FALSE);
	VOID HideChildMenu(IDUIMenuItem* pMenuItem);

	BOOL IsMenuItemClickable(IDUIControl* pControl);
	BOOL IsMenuItemSelectable(INT nIndex);
	BOOL IsMenuItemPoupChild(IDUIControl* pControl);

protected:
	INT m_nArrowWidth;
	INT m_nIconWidth;
	BOOL m_bExitLoop;
	
	friend class CDUIMenuWnd;
	CDUIMenuWnd* m_pMenuWnd;

	INT m_nSelectIndex;

	CRefPtr<CImageList> m_pArrowImage;
	CRefPtr<CImageList> m_pSeparateLine;
 	CRefPtr<CDUIFrameBK> m_pIconFrameBK;
	CRefPtr<CDUIFrameBK> m_pMenuWndFrameBK;
};



DUI_END_NAMESPCE


#endif //__DUI_MENU_H__