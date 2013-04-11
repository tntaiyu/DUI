#ifndef __DUI_SCROLLBAR_H__
#define __DUI_SCROLLBAR_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUIScrollBar: public CDUIControlBase
{
public:
	virtual ~IDUIScrollBar() { NULL; }

	virtual BOOL SetScrollBarInfo(LPSCROLLINFO pSi) = 0;
	virtual VOID GetScrollBarInfo(LPSCROLLINFO pSi) = 0;
	
	virtual VOID SetScrollOwner(const CDUIString& strOwnerName) = 0;
	virtual VOID SetScrollOwner(IDUIControl* pOwner) = 0;
	virtual IDUIControl* GetScrollOwner() = 0;
};

class DUILIB_API IDUIScrollSink
{
public:
	virtual LRESULT OnVScrollEvent(WPARAM wParam, LPARAM lParam) { return S_OK; }
	virtual LRESULT OnHScrollEvent(WPARAM wParam, LPARAM lParam) { return S_OK; }

	virtual BOOL IsVerticalSB() = 0;
};


class IDUIButton;

class DUILIB_API CDUIScrollBarBase: public IDUIScrollBar,
									public IDUIScrollSink
{
public:
	typedef IDUIScrollBar theBase;
	CDUIScrollBarBase();
	virtual ~CDUIScrollBarBase();
	
	virtual LPVOID GetInterface(const CDUIString& strName);

	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual VOID SetControlRect(const RECT& rt);
	virtual VOID ModifyStatus(DWORD dwRemove, DWORD dwAdd);

	virtual VOID SetScrollOwner(const CDUIString& strOwnerName);
	virtual VOID SetScrollOwner(IDUIControl* pOwner);
	virtual IDUIControl* GetScrollOwner();
	
	virtual BOOL SetScrollBarInfo(LPSCROLLINFO pSi);
	virtual VOID GetScrollBarInfo(LPSCROLLINFO pSi);
	
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);

	virtual LRESULT OnVScrollEvent(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnHScrollEvent(WPARAM wParam, LPARAM lParam);
	
protected:
	BOOL CreateCtrls();
	IDUIButton* GetElement(INT nType);
	BOOL CheckCtrlsSink();

	VOID UpdateCtrlsAttributes();
	VOID UpdateThumUI();

	VOID StartMessageLoop();
	BOOL SetMsgSink(BOOL bSet);

	VOID EnableScrollBar(BOOL bEnable);

protected:
	virtual	VOID OnCreate();
	virtual VOID OnDestroy();

	virtual VOID PaintBkgnd(HDC dc);

protected:
	INT m_nMin;
	INT m_nMax;
	INT m_nPos;
	INT m_nPage;

	SIZE m_sizeArrow1;
	SIZE m_sizeArrow2;

	RECT m_rtScrollPage1;
	RECT m_rtScrollPage2;
	
	IDUIControl* m_pNotifier;
	CDUIString m_strNotifierName;

	BOOL m_bMsgLoop;
	BOOL m_bScrollPage1;
	POINT m_ptCursor;
	double m_fRatio;

	CARGB m_clrBK;
};

class DUILIB_API CDUIVerticalSB: public CDUIScrollBarBase
{
public:
	virtual CRefPtr<IDUIControl> Clone();
	virtual BOOL IsVerticalSB()  { return TRUE; }
};

class DUILIB_API CDUIHorizontalSB: public CDUIScrollBarBase
{
public:
	virtual CRefPtr<IDUIControl> Clone();
	virtual BOOL IsVerticalSB()  { return FALSE; }
};


DUI_END_NAMESPCE



#endif //__DUI_SCROLLBAR_H__