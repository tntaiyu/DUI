#ifndef __DUI_POPUP_WINDOW_H__
#define __DUI_POPUP_WINDOW_H__

#include "DUIWindowMgr.h"

DUI_BGN_NAMESPCE


class DUILIB_API CMinMaxInfo
{
public:
	CMinMaxInfo();
	BOOL IsEmpty() const;
	
	POINT m_ptMinTrackSize;
	POINT m_ptMaxTrackSize;
};

class DUILIB_API CDUIPopupWindow: public CDUIWindowMgr
{
public:
	typedef CDUIWindowMgr theBase;
	BEGIN_MSG_MAP(CDUIPopupWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		CHAIN_MSG_MAP(theBase)
	END_MSG_MAP()
		
	CDUIPopupWindow();
	virtual ~CDUIPopupWindow();

	//IDUINotifyHandler
	virtual VOID OnDUINotify(const DUINotify& info, BOOL& bHandled);
	
	//IDUIMessageFilter
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual	VOID OnBuilderWindowAttribute(const CDUIString& strAttriName, const CDUIString& strAttriValue);
	
	DWORD GetDUIStyle() const;
	DWORD SetDUIStyle(DWORD dwStyleRemove, DWORD dwStyleAdd);

	VOID SetDUIMinMaxInfo(const CMinMaxInfo& info);

	//caption rect
	VOID SetCaptionHeight(INT nHeight);

protected:
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnNcPaint(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnNcActivate(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnKillFocus(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnSysCommand(UINT, WPARAM, LPARAM, BOOL& bHandled);

	LRESULT OnPreKeyDown(UINT, WPARAM, LPARAM, BOOL& bHandled);
	
protected:
	VOID CheckSystemNotification(const DUINotify& info, BOOL& bHandled);
	BOOL SetNextTabControl(BOOL bForward = TRUE);
	BOOL SetNextTabPage(BOOL bForward = TRUE);

protected:
	virtual BOOL IsNcActivate();

protected:
	BOOL IsDUICustomNC();
	BOOL IsDUILayeredWindow();
	BOOL IsDUISetNCRgn();
	BOOL IsDUINCSizable();

protected:
	INT NcHitTest(const POINT& ptHit);
	VOID CheckSysButtonStatus();
	VOID CheckDUIStyle();

protected:
	DWORD m_dwDUIStyle;
	BOOL m_bNCActivate;

	CMinMaxInfo m_MinMaxInfo;
	INT m_nCaptionHeight;
	
	IDUIControl* m_pLastFocusControl;
};


DUI_END_NAMESPCE


#endif //__DUI_POPUP_WINDOW_H__