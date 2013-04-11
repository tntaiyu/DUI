#ifndef __DUI_WINDOW_MGR_H__
#define __DUI_WINDOW_MGR_H__

#include "DUIWindowBase.h"
#include "DUIControlBuilder.h"
#include "DUIControlBase.h"
#include <vector>
#include <map>


DUI_BGN_NAMESPCE

class DUILIB_API CDUIWindowMgr: public CDUIWindowBase,
					 public IDUIControlMgr,
					 public IDUINotifyHandler,
					 public IDUIControlBuilderCallback,
					 public IDUIMessageFilter,
					 public IDUIIdleHandler,
					 public IDUISkinObserver
{
public:
	typedef CDUIWindowBase theBase;
	BEGIN_MSG_MAP(CDUIWindowMgr)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOVE, OnMove)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)

		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDBClick)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)

		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_CHAR, OnChar)	
		
		NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnGetDispInfo)

		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)

		MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)

		CHAIN_MSG_MAP(CDUIWindowBase)
	END_MSG_MAP()

	CDUIWindowMgr();
	virtual ~CDUIWindowMgr();

	//IDUINotifyDispatch
	virtual BOOL RegisterNotifyHandler(IDUINotifyHandler* pNotifier);
	virtual BOOL UnregisterNotifyHandler(IDUINotifyHandler* pNotifier);
	virtual VOID SendNotify(const DUINotify& info);
	virtual LRESULT OnControlRequest( DUINotify& info);

	//IDUINotifyHandler
	virtual VOID OnDUINotify(const DUINotify& info, BOOL& bHandled);


	//IDUIMessageFilter
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//IDUIIdleHandler
	virtual BOOL OnIdle();
	
	//IDUIControlBuilderCallback
	virtual CRefPtr<IDUIControl> OnBuilderNewControl(const CDUIString& strClassName);
	virtual VOID OnBuilderControlCreated(CRefPtr<IDUIControl> pControl);
	virtual VOID OnBuilderWindowAttribute(const CDUIString& strAttriName, const CDUIString& strAttriValue);
	
	//IDUISkinObserver
	virtual VOID OnSkinUIDataChanged(CRefPtr<CDUIFrameBK> pSkinUIData);

	HWND CreateFromXML(const CDUIString& strXML, HWND hWndParent,
		RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		UINT nID = 0, LPVOID lpCreateParam = NULL);

	HWND CreateFromRootControl(CRefPtr<IDUIControl> pRootControl, 
		HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		UINT nID = 0, LPVOID lpCreateParam = NULL);

	HWND CreateFromResource(const CDUIString& strResourceID, 
		HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		UINT nID = 0, LPVOID lpCreateParam = NULL);

	CRefPtr<IDUIControl> DetachRootControl();
	
	IDUIControl* GetControlByName(const CDUIString& strName, BOOL bRefresh = FALSE);
	IDUIControl* GetControlByID(DWORD dwControlID, BOOL bRefresh = FALSE);
	IDUIControl* GetControlByInterface(LPCTSTR lpszInterfaceName);

	VOID LockLayout(BOOL bLock);
	VOID LayoutRootControl();
	
	VOID SetUseSkinBK(BOOL bUseSkinBK);
	BOOL GetUseSkinBK() const;

	VOID SetFrameBkgnd(CRefPtr<CDUIFrameBK> pFrameBK);
	CRefPtr<CDUIFrameBK> GetFrameBkgnd() const;

	VOID SetClientRectMargin(RECT rtMargin);
	RECT GetClietnRectMargin() const;

protected:
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnMove(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnSetCursor(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnNcActivate(UINT, WPARAM wParam, LPARAM, BOOL& bHandled);

	LRESULT OnLButtonDown(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDBClick(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetDispInfo(INT, LPNMHDR, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextMenu(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnKeyDown(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnKeyUp(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnTimer(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnKillFocus(UINT, WPARAM, LPARAM, BOOL& bHandled);
	
protected:
	virtual BOOL GetDUIClientRect(LPRECT lpRect);

	virtual VOID PaintFrameBkgnd(HDC dc);
	virtual VOID PaintClientArea(HDC dc);

	virtual BOOL IsNcActivate();

protected:
	LRESULT HandleFocusRequest(const DUINotify& info);
	LRESULT HandleTimerRequest(const DUINotify& info);

	HWND GetMostTopParent();
	DWORD GetMostTopParentStatus();

	IDUIControl* GetControlByShortcut(TCHAR chShortcut);
	
	VOID ClearTimer();
	UINT GetIdleTimerIndex();

	CRefPtr<CDUIFrameBK> GetCurrentFrameBK();

	IDUIControl* FindControl(LPPOINT pt);

	static	IDUIControl* CALLBACK ControlFromPointCallBack(IDUIControl* pControl, LPVOID pData);
	static  IDUIControl* CALLBACK ControlFromNameCallBack(IDUIControl* pControl, LPVOID pData);
	static  IDUIControl* CALLBACK ControlFromIDCallBack(IDUIControl* pControl, LPVOID pData);
	static  IDUIControl* CALLBACK ContorlFromShortcutCallBack(IDUIControl* pControl, LPVOID pData);
	static  IDUIControl* CALLBACK ControlFromTabCallBack(IDUIControl* pThis, LPVOID pData);
	static  IDUIControl* CALLBACK ControlFromInterfaceCallBack(IDUIControl* pControl, LPVOID pData);

protected:
	typedef std::vector<IDUINotifyHandler*> CNotifyVector;
	CNotifyVector m_arNotifier;

	typedef std::vector<LPDUITimerInfo> CTimerVector;
	CTimerVector m_arTimer;

	typedef std::map<CDUIString, IDUIControl*, CStrLessCmp> CNameControlMap;
	CNameControlMap m_mapNameControl;

	typedef std::map<DWORD, IDUIControl*> CIDControlMap;
	CIDControlMap m_mapIDControl;

	BOOL m_bControlLayouting;
	BOOL m_bMouseTracking;
	POINT m_ptLastCursor;
	
	IDUIControl* m_pClickControl;
	IDUIControl* m_pHoverControl;
	IDUIControl* m_pFocusControl;

	CRefPtr<IDUIControl> m_pRootControl;
	BOOL m_bLayoutAgain;
	BOOL m_bLockLayout;
	BOOL m_bUseSkinBK;

	CRefPtr<CDUIFrameBK> m_pFrameBkgnd;
	RECT m_rtClientMargin;
};



DUI_END_NAMESPCE


#endif //__DUI_WINDOW_MGR_H__