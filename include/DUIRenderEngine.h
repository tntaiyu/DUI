#ifndef __DUI_RENDER_ENGINE_H__
#define __DUI_RENDER_ENGINE_H__

#include "DUILib.h"

DUI_BGN_NAMESPCE

class DUILIB_API CDUIRenderClip
{
public:
	CDUIRenderClip(HDC dc, RECT& rtItem);
	~CDUIRenderClip();
	
private:
	CDUIRenderClip(const CDUIRenderClip&);
	
	HDC m_dc;
	HRGN m_hRgnNew;
	HRGN m_hRgnOld;
};

class DUILIB_API CDUIRenderEngine
{
public:
	static VOID DrawRoundRectBorder(HDC dc, const RECT& rt, INT nCornerWidth, INT nCornerHeight, INT nBorderWidth, const CARGB& clr);
	static VOID DrawRectangleBorder(HDC dc, const RECT& rt, const RECT& rtBorder, const CARGB& clr);
	static VOID DrawColor(HDC dc, const RECT& rt, const CARGB& clr);
	static VOID DrawGradien(HDC dc, const RECT& rt, const CARGB& clrStart, const CARGB& clrEnd, BOOL bVertical);
	
	static SIZE GetTextIdealSize(const CDUIString& strText, HFONT hFont, INT nWidthLimit  = 0 , UINT uFormat  = DT_LEFT);
	static INT DrawText( HDC hDC, LPCTSTR lpString, INT nCount, LPRECT lpRect, UINT uFormat);

private:
	static VOID GradientFill(HDC dc, const RECT& rtDraw, COLORREF clrStart, COLORREF clrEnd, BOOL bVertical);	

};


DUI_END_NAMESPCE

#endif //__DUI_RENDER_ENGINE_H__