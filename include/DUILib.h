#ifndef __DUI_LIB_H__
#define __DUI_LIB_H__

#include "DUILibDef.h"
#include "DUIImage.h"
#include "DUICursor.h"
#include "DUIFont.h"
#include "DUIColor.h"
#include "DUIBkgnd.h"
#include <vector>

DUI_BGN_NAMESPCE

//send message from control to manager
class DUILIB_API IDUINotifyHandler
{
public:
	virtual VOID OnDUINotify(const DUINotify& info, BOOL& bHandled) = 0;
};

//send notify to out side world
class DUILIB_API IDUIControlMgr
{
public:	
	virtual BOOL RegisterNotifyHandler(IDUINotifyHandler* pNotifier) = 0;
	virtual BOOL UnregisterNotifyHandler(IDUINotifyHandler* pNotifier) = 0;
	virtual VOID SendNotify(const DUINotify& info) = 0;

	virtual LRESULT OnControlRequest( DUINotify& info) = 0;
};

//send event from manager to control
class DUILIB_API IDUIEventHandler
{
public:
	virtual LRESULT OnDUIEvent(const DUIEvent& info, BOOL& bHandled) = 0;
};

class DUILIB_API IDUIControl: public CDUIObject,
							  public IDUIEventHandler,
							  public IDUINotifyHandler
{
public:
	virtual ~IDUIControl() { NULL; }
	
	virtual VOID AttachWindow(IDUIControlMgr* pControlMgr) = 0;
	virtual VOID DetachWindow() = 0;
	
	virtual LPVOID GetInterface(const CDUIString& strName) = 0;
	virtual CRefPtr<IDUIControl> Clone() = 0;
	
	virtual BOOL Activate() = 0;
	
	virtual VOID SetFocus(BOOL bSet = TRUE) = 0;
	
	virtual	VOID SetText(const CDUIString& strText) = 0;
	virtual const CDUIString& GetText() = 0;

	virtual VOID SetTooltip(const CDUIString& strTooltip) = 0;
	virtual const CDUIString& GetTooltip() const = 0;
	
	virtual VOID SetShortcut(TCHAR c) = 0;
	virtual TCHAR GetShortcut() const = 0;
	
	virtual VOID SetName(const CDUIString& strName) = 0;
	virtual const CDUIString& GetName() const = 0;

	virtual VOID SetID(DWORD dwID) = 0;
	virtual DWORD GetID() const = 0;

	virtual VOID SetParent(IDUIControl* pParent) = 0;
	virtual IDUIControl* GetParent() const = 0;
	
	virtual VOID SetControlRect(const RECT& rt) = 0;
	virtual RECT GetControlRect() = 0;
	
	virtual VOID SetTextStyle(CRefPtr<CTextStyle> pTextStyle) = 0;
	virtual CRefPtr<CTextStyle> GetTextStyle() = 0;

	virtual VOID SetCursor(CRefPtr<CCursorObject> pCursor) = 0;
	virtual CRefPtr<CCursorObject> GetCursor() = 0;

	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue) = 0;
	
	virtual VOID UpdateLayout(BOOL bNeedResize = FALSE, RECT* pRect = NULL) = 0;
	
	virtual VOID ModifyStatus(DWORD dwRemove, DWORD dwAdd) = 0;
	virtual DWORD GetStatus() const = 0;

	virtual VOID ModifyStyle(DWORD dwRemove, DWORD dwAdd) = 0;
	virtual DWORD GetStyle() const = 0;

	virtual VOID ModifyExStyle(DWORD dwRemove, DWORD dwAdd) = 0;
	virtual DWORD GetExStyle() const = 0;
	
	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet) = 0;
	virtual VOID DoPaint(HDC dc, const RECT& rtDraw) = 0;
	
	virtual BOOL SetTimer(UINT nTimerID, UINT nElapse) = 0;
	virtual VOID KillTimer(UINT nTimerID) = 0;	

	virtual VOID SubclassControl(IDUIControl* pControl, BOOL bSubclass) = 0;

	virtual IDUIControl* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags) = 0;
	
	virtual VOID BroadcastEvent(const DUIEvent& info) = 0;

	virtual BOOL IsVisible(BOOL bCheckParent = FALSE) const = 0;
	virtual BOOL IsEnabled() const = 0;

public:
	virtual BOOL AddControl(CRefPtr<IDUIControl> pItem) = 0;
	virtual BOOL RemoveControl(CRefPtr<IDUIControl> pItem) = 0;
	virtual INT GetControlCount() const = 0;
	virtual IDUIControl* GetControlByIndex(INT nIndex) const = 0;
	virtual VOID RemoveAllControl() = 0;
	virtual BOOL AddControlAt(INT nIndex, CRefPtr<IDUIControl> pItem) = 0;
	virtual BOOL RemoveControlAt(INT nIndex) = 0;
	virtual INT GetControlIndex(CRefPtr<IDUIControl> pControl) = 0;
	virtual BOOL SetControlIndex(CRefPtr<IDUIControl> pControl, INT nIndex) = 0;
	
	virtual VOID SetMargin(RECT rtMargin) = 0;
	virtual RECT GetMargin() = 0;
	virtual VOID SetPaddingWidth(INT nPadding) = 0;
	virtual INT GetPaddingWidth() = 0;
	
	virtual VOID SetFixedWidth(INT nWidth) = 0;
	virtual VOID SetFixedHeight(INT nHeight) = 0;
	virtual SIZE GetFixedSize() = 0;

};

//interface for UI skin


class DUILIB_API  IDUISkinObserver
{
public:
	virtual VOID OnSkinUIDataChanged(CRefPtr<CDUIFrameBK>) { NULL; }
};

class DUILIB_API IDUILanguageObserver
{
public:
	virtual VOID OnLanguageTypeChanged(const CDUIString& strOldType, const CDUIString& strNewType) { NULL; }
};



class DUILIB_API IDUISkinMgr
{
public:
	virtual ~IDUISkinMgr() { NULL; }

	virtual BOOL InitializeSkin(const CDUIString& strConfigXML) = 0;
	virtual VOID UninitializeSkin() = 0;
	
	virtual BOOL SetSkinUIData(CRefPtr<CDUIFrameBK> pSkinUI) = 0;
	virtual const CRefPtr<CDUIFrameBK> GetSKinUIData() const = 0;
	
	virtual BOOL SetLanguageType(const CDUIString& strLanType) = 0;
	virtual CDUIString GetLanguageType() const = 0;
	
	virtual BOOL AddSkinObserver(IDUISkinObserver* pSink) = 0;
	virtual BOOL RemoveSkinObserver(IDUISkinObserver* pSink) = 0;
	
	virtual BOOL AddLanguageObserver(IDUILanguageObserver* pObserver) = 0;
	virtual BOOL RemoveLanguageObserver(IDUILanguageObserver* pObserver) = 0;
	
	
	virtual CRefPtr<CImageList> GetImage(const CDUIString& strID, BOOL bCache = TRUE) = 0;
	virtual CRefPtr<CCursorObject> GetCursor(const CDUIString& strID, BOOL bCache = TRUE) = 0;
	virtual CRefPtr<CFontObject> GetFont(const CDUIString& strID, BOOL bCache = TRUE) = 0;
	virtual CARGB GetColor(const CDUIString& strID) = 0;

	virtual CRefPtr<CDUIFrameBK> GetFrameBK(const CDUIString& strName) = 0;
	virtual CRefPtr<CTextStyle> GetTextStyle(const CDUIString& strID) = 0;
	
	virtual BOOL GetWindowTemplate(const CDUIString& strID, CDUIString& strTemplateXML) = 0;
	virtual BOOL GetMenuTemplate(const CDUIString& strID, CDUIString& strTemplateXML) = 0;
	virtual BOOL GetControlStyle(const CDUIString& strID, CDUIString& strStyleXML) = 0;
	virtual BOOL GetStringText(const CDUIString& strID, CDUIString& strContent) = 0;
	virtual BOOL GetPlugin(const CDUIString& strID, CDUIString& strPluginXML) = 0;
};

class DUILIB_API IDUIControlFactory
{
public:
	virtual ~IDUIControlFactory() { NULL; }

	virtual	BOOL InitializeFactory() = 0;
	virtual VOID UninitializeFactory() = 0;

	virtual BOOL RegisterControl(const CDUIString& strName, CRefPtr<IDUIControl> pControl) = 0;
	virtual BOOL UnregisterControl(const CDUIString& strName) = 0;
	
	virtual CRefPtr<IDUIControl> NewControl(const CDUIString& strName) = 0;
};

class DUILIB_API IDUIMessageFilter
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
};

class DUILIB_API IDUIIdleHandler
{
public:
	virtual BOOL OnIdle() = 0;
};

class DUILIB_API CDUIMessageLoop
{
public:
	BOOL AddMessageFilter(IDUIMessageFilter* pMessageFilter);
	VOID RemoveMessageFilter(IDUIMessageFilter* pMessageFilter);
	
	BOOL AddIdleHandler(IDUIIdleHandler* pIdleHandler);
	VOID RemoveIdleHandler(IDUIIdleHandler* pIdleHandler);
	
	int Run();
	
	static BOOL IsIdleMessage(MSG* pMsg);
	
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(int);
	
protected:
	std::vector<IDUIMessageFilter*> m_aMsgFilter;
	std::vector<IDUIIdleHandler*> m_aIdleHandler;
	MSG m_msg;	
};

class  DUILIB_API IDUIApp
{
public:
	virtual ~IDUIApp() { NULL; }
	
	virtual BOOL Init(const CDUIString& strEntryXML) = 0;
	virtual void Term() = 0;
	
	virtual IDUISkinMgr* GetSkinMgr() const = 0;
	virtual IDUIControlFactory* GetControlFactory() const = 0;
	virtual CDUIMessageLoop* GetMessageLoop() = 0;
};

extern "C" DUILIB_API IDUIApp* WINAPI DUIGetAppInstance();

DUI_END_NAMESPCE

#endif //__DUI_LIB_H__