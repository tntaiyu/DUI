#ifndef __DUI_CHILD_WINDOW__
#define __DUI_CHILD_WINDOW__

#include "DUIWindowMgr.h"

DUI_BGN_NAMESPCE


class DUILIB_API CDUIChildWindow: public CDUIWindowMgr
{
public:
	typedef CDUIWindowMgr theBase;
	BEGIN_MSG_MAP(CDUIChildWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		CHAIN_MSG_MAP(theBase)
	END_MSG_MAP()
		
	CDUIChildWindow();
	virtual ~CDUIChildWindow();
	
	//IDUINotifyHandler
	virtual VOID OnDUINotify(const DUINotify& info, BOOL& bHandled);
	
	//IDUIMessageFilter
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnGetDlgCode(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnKeyDown(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled);

protected:
	BOOL SetNextTabControl(BOOL bForward  = TRUE );	
};

DUI_END_NAMESPCE

#endif //__DUI_CHILD_WINDOW__