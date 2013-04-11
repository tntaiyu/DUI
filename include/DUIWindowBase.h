#ifndef __DUI_WINDOW_H__
#define __DUI_WINDOW_H__

#include "DUILib.h"
#include "DUIAtl.h"
#include "DUIBitBuffer.h"
#include "DUIImage.h"

DUI_BGN_NAMESPCE

class DUILIB_API CDUIWindowBase: public CWindowImpl<CDUIWindowBase>
{
public:
	DECLARE_WND_CLASS(DUI_WND_CLASS)

	BEGIN_MSG_MAP(CDUIWindowBase)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DUI_FLIP, OnDUIFlip)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()
	
	CDUIWindowBase();
	virtual ~CDUIWindowBase();
	
	BOOL Invalidate(BOOL bErase = TRUE);
	BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);
	void InvalidateRgn(HRGN hRgn, BOOL bErase = TRUE);

protected:
	LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnDUIFlip(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnPrintClient(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL& bHandled);

protected:
	VOID DoPaint(HDC hDC);

protected:
	virtual BOOL GetDUIClientRect(LPRECT lpRect);

	virtual VOID PaintFrameBkgnd(HDC dc);
	virtual VOID PaintClientArea(HDC dc);

	virtual VOID OnBeforeFlipDIB(CDibSection* pDIB);

protected:
	CBitBuffer m_backDib;
};


DUI_END_NAMESPCE

#endif// __DUI_WINDOW_H__