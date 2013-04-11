#ifndef __DUI_CONTROL_BASE_H__
#define __DUI_CONTROL_BASE_H__

#include "DUILib.h"
#include "DUIAnimation.h"
#include <vector>

DUI_BGN_NAMESPCE

IDUISkinMgr*    GetSkinMgr();
IDUIControlFactory*    GetControlFactory();

class IDUIScrollBar;
class DUILIB_API CDUIControlBase: public IDUIControl
{
public:
	CDUIControlBase();
	virtual ~CDUIControlBase();
	
	CDUIControlBase(const CDUIControlBase& object);
	virtual CRefPtr<IDUIControl> Clone();

	virtual LRESULT OnDUIEvent(const DUIEvent& info, BOOL& bHandled);
	virtual VOID OnDUINotify(const DUINotify& info, BOOL& bHandled);
	
	
	virtual VOID AttachWindow(IDUIControlMgr* pControlMgr);
	virtual VOID DetachWindow();
	
	virtual LPVOID GetInterface(const CDUIString& strName);
	
	virtual BOOL Activate();
	
	virtual VOID SetFocus(BOOL bSet = TRUE);
	
	virtual	VOID SetText(const CDUIString& strText);
	virtual const CDUIString& GetText();
	
	virtual VOID SetTooltip(const CDUIString& strTooltip);
	virtual const CDUIString& GetTooltip() const;
	
	virtual VOID SetShortcut(TCHAR c);
	virtual TCHAR GetShortcut() const;
	
	virtual VOID SetName(const CDUIString& strName);
	virtual const CDUIString& GetName() const;

	virtual VOID SetID(DWORD dwID);
	virtual DWORD GetID() const;

	virtual VOID SetParent(IDUIControl* pParent);
	virtual IDUIControl* GetParent() const;
	
	virtual VOID SetControlRect(const RECT& rt);
	virtual RECT GetControlRect();
	
	virtual VOID SetTextStyle(CRefPtr<CTextStyle> pTextStyle);
	virtual CRefPtr<CTextStyle> GetTextStyle();

	virtual VOID SetCursor(CRefPtr<CCursorObject> pCursor);
	virtual CRefPtr<CCursorObject> GetCursor();
	
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	
	virtual VOID UpdateLayout(BOOL bNeedResize = FALSE, RECT* pRect = NULL);
	
	virtual VOID ModifyStatus(DWORD dwRemove, DWORD dwAdd);
	virtual DWORD GetStatus() const;

	virtual VOID ModifyExStyle(DWORD dwRemove, DWORD dwAdd);
	virtual DWORD GetExStyle() const;	

	virtual VOID ModifyStyle(DWORD dwRemove, DWORD dwAdd);
	virtual DWORD GetStyle() const;
	
	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet);
	virtual VOID DoPaint(HDC dc, const RECT& rtDraw);
	
	virtual BOOL SetTimer(UINT nTimerID, UINT nElapse);
	virtual VOID KillTimer(UINT nTimerID);	

	virtual VOID SubclassControl(IDUIControl* pControl, BOOL bSubclass);

	virtual IDUIControl* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	virtual VOID BroadcastEvent(const DUIEvent& info);	

	BOOL IsVisible(BOOL bCheckParent = FALSE) const;
	BOOL IsEnabled() const;

public:
	virtual BOOL AddControl(CRefPtr<IDUIControl> pItem);
	virtual BOOL RemoveControl(CRefPtr<IDUIControl> pItem);
	virtual INT GetControlCount() const;
	virtual IDUIControl* GetControlByIndex(INT nIndex) const;
	virtual VOID RemoveAllControl();
	virtual BOOL AddControlAt(INT nIndex, CRefPtr<IDUIControl> pItem);
	virtual BOOL RemoveControlAt(INT nIndex);
	virtual INT GetControlIndex(CRefPtr<IDUIControl> pControl);
	virtual BOOL SetControlIndex(CRefPtr<IDUIControl> pControl, INT nIndex);

	virtual VOID SetMargin(RECT rtMargin);
	virtual RECT GetMargin();
	virtual VOID SetPaddingWidth(INT nPadding);
	virtual INT GetPaddingWidth();
	
	virtual VOID SetFixedWidth(INT nWidth);
	virtual VOID SetFixedHeight(INT nHeight);
	virtual SIZE GetFixedSize();
	
public:
	RECT GetContentRect();
	HWND GetManagerWnd(BOOL bTopParent = FALSE);
	IDUIControl* GetSiblingControl(const CDUIString& strName);
	IDUIControl* GetSiblingControl(POINT ptHit);
	IDUIControl* GetFocusControl();
	IDUIControl*	GetNextSiblingControl(CRefPtr<IDUIControl> pItem);
	IDUIControl*	GetPrevSiblingControl(CRefPtr<IDUIControl> pItem);

	DWORD GetManagerWndStatus();

	BOOL ApplyControlStyle(const CDUIString& strStyleName);

	VOID LockLayout(BOOL bLock);

	BOOL GetFocus() const;

protected:
	BOOL ProcessVerticalSB(RECT rt, INT nNeededHeight, INT nExtraLenth = 0);
	BOOL ProcessHorizontalSB(RECT rt, INT nNeededWidth, INT nExtraLenth = 0);
	
	VOID OnVertialSBEvent(const DUIEvent& info);
	VOID OnHorizontalSBEvent(const DUIEvent& info);
	VOID OnMouseWheel(const DUIEvent& info);

	BOOL SendNotify(const DUINotify& info);
	BOOL SendNotify(UINT nMsgType, WPARAM wParam = 0, LPARAM lParam = 0, IDUIControl* pSender = NULL);

protected:
	virtual IDUIScrollBar* GetVerticalSB();
	virtual IDUIScrollBar* GetHorizontalSB();
	virtual INT GetScrollLineStep();

	virtual VOID OnCreate();
	virtual VOID OnDestroy();
	virtual VOID OnTimer(INT nTimerID);

	virtual LRESULT PreTranslateEvent(const DUIEvent& event, BOOL& bHandled);
	virtual LRESULT ProcessEvent(const DUIEvent& event, BOOL& bHandled);

	virtual VOID PaintBkgnd(HDC dc);
	virtual VOID PaintForgnd(HDC dc);
	virtual VOID PaintText(HDC dc);
	virtual VOID PaintBorder(HDC dc);

	virtual BOOL BeforeAddControl(CRefPtr<IDUIControl> pItem);
	virtual VOID AfterAddControl(CRefPtr<IDUIControl> pItem);
	virtual VOID BeforeRemoveControl(CRefPtr<IDUIControl> pItem);
	virtual VOID AfterRemoveControl(CRefPtr<IDUIControl> pItem);

protected:
	CDUIString m_strName;
	CDUIString m_strText;
	CDUIString m_strTooltip;

	DWORD m_dwStyle;
	DWORD m_dwExStyle;
	DWORD m_dwStatus;
	TCHAR m_chShorcut;
	RECT m_rtControl;
	RECT m_rtMargin;
	SIZE m_sizeFixed;
	INT m_nPaddingWidth;
	DWORD m_dwID;

	typedef std::vector<CRefPtr<IDUIControl> > CControlVector;
	CControlVector m_arChildControl;

	CRefPtr<CTextStyle> m_pTextStyle;
	CRefPtr<CCursorObject> m_pCursor;

	IDUIControl* m_pSubclassControl;
	IDUIControl* m_pParentControl;
	IDUIControlMgr* m_pControlMgr;

private:
	CDUIControlBase& operator = (const CDUIControlBase& object);
}; 


class DUILIB_API CDUIAnimationControl: public CDUIControlBase,
							public CDUIAnimation
{
public:
	CDUIAnimationControl();
	CDUIAnimationControl(const CDUIAnimationControl& object);

	virtual CRefPtr<IDUIControl> Clone();

protected:
	virtual VOID OnTimer(INT nTimerID);
	virtual VOID OnDestroy();
};

DUI_END_NAMESPCE

#endif// __DUI_CONTROL_BASE_H__