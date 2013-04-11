#ifndef __DUI_WIN_OBJECT_H__
#define __DUI_WIN_OBJECT_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUIWinObject: public CDUIControlBase
{
public:
	virtual ~IDUIWinObject() { NULL; }

	virtual VOID SetHWnd(HWND hWnd) = 0;
	virtual HWND GetHWnd() const = 0;
};


class DUILIB_API CDUIWinObject: public IDUIWinObject
{
public:
	typedef IDUIWinObject theBase;
	CDUIWinObject();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual VOID SetControlRect(const RECT& rt);

	virtual VOID ModifyStyle(DWORD dwRemove, DWORD dwAdd);
	virtual VOID ModifyStatus(DWORD dwRemove, DWORD dwAdd);
	virtual VOID SetText(const CDUIString& strText);

	//IDUIWinObject
	virtual VOID SetHWnd(HWND hWnd);
	virtual HWND GetHWnd() const;

protected:
	virtual VOID OnCreate();

protected:
	VOID SyncProperty();

protected:
	HWND m_hWnd;
};

DUI_END_NAMESPCE


#endif //__DUI_WIN_OBJECT_H__