#ifndef __DUI_BIT_BUFFER_H__
#define __DUI_BIT_BUFFER_H__

#include "DUIDib.h"

DUI_BGN_NAMESPCE

class CDibSection;
struct DBLBUFFER
{
    HDC hFront;
    HDC hBack;
    HRGN hClip;
    CDibSection* pBack;
};

class CBitBuffer
{
public:
	CBitBuffer();
	~CBitBuffer();

	VOID Reset();
	VOID InvalidateRect(const RECT& rt);
	VOID InvalidateRgn(HRGN hRgn);
	VOID DoPaint(HWND hWnd, HDC hDC);

	CDibSection* GetDIB();

protected:
	CDibSection m_dib;
	HRGN m_hRgn;
};


DUI_END_NAMESPCE


#endif //__DUI_BIT_BUFFER_H__