#ifndef __DUI_FONT_H__
#define __DUI_FONT_H__

#include "DUIBase.h"
#include "DUIColor.h"

DUI_BGN_NAMESPCE

class DUILIB_API CFontObject: public CDUIObject
{
public:
	CFontObject();
	~CFontObject();
	
	BOOL CreateFont();
	BOOL IsNull() const;
	HFONT GetFont() const;
	
	VOID SetHeight(INT nHeight);
	VOID SetWeight(INT nHeight);
	VOID SetItalic(BOOL bItalic);
	VOID SetUnderline(BOOL bUnderLine);
	VOID SetStrikeout(BOOL bStrikeout);
	VOID SetFaceName(LPCTSTR lspzFaceName);

	INT GetHeight() const;
	INT GetWeight() const;
	BOOL GetItalic() const;
	BOOL GetUnderline() const;
	BOOL GetStrikeout() const;
	LPCTSTR GetFaceName() const;


protected:
	VOID DeleteObject();

protected:
	HFONT m_hFont;
	LOGFONT m_logFont;
};

class DUILIB_API CTextStyle: public CDUIObject
{
public:
	CTextStyle();
	
	VOID SetTextFormat(DWORD dwFormat);
	DWORD GetTextFormat() const;

	VOID SetFont(CRefPtr<CFontObject> pFont);
	CRefPtr<CFontObject> GetFont() const;

	VOID SetNormalColor(const CARGB& clr);
	CARGB GetNormalColor() const;

	VOID SetDisableColor(const CARGB& clr);
	CARGB GetDisableColor() const;

	VOID SetHoverColor(const CARGB& clr);
	CARGB GetHoverColor() const;

	VOID SetSelectColor(const CARGB& clr);
	CARGB GetSelectColor() const;

protected:
	DWORD m_dwFormat;
	CRefPtr<CFontObject> m_pFont;
	CARGB m_clrNormal;
	CARGB m_clrDisable;
	CARGB m_clrHover;
	CARGB m_clrSelect;
};


DUI_END_NAMESPCE

#endif //__DUI_FONT_H__