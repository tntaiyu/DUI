#include "DUIFont.h"


DUI_BGN_NAMESPCE


CFontObject::CFontObject()
{
	CTrace::TraceInfo(_T("CFontObject::CFontObject()"));

	HFONT hFontSys = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if(hFontSys != NULL)
	{
		::GetObject(hFontSys, sizeof(m_logFont), &m_logFont);
	}

	LANGID gId=GetSystemDefaultLangID();
	if(gId==0x0411) //Japanese
	{
		m_logFont.lfCharSet = SHIFTJIS_CHARSET;
	}
	else if(gId==0x0404 || gId==0x0c04 || gId==0x1004) // big 5
	{
		m_logFont.lfCharSet = CHINESEBIG5_CHARSET;
	}
	else if(gId==0x0804 )
	{
		m_logFont.lfCharSet = GB2312_CHARSET;
	}
	else if(gId==0x0412 || gId==0x0812)
	{
		m_logFont.lfCharSet = JOHAB_CHARSET;
	}
	else
	{
		m_logFont.lfCharSet =  ANSI_CHARSET;
	}

	m_hFont = NULL;
}

VOID CFontObject::DeleteObject()
{
	if(m_hFont != NULL)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

CFontObject::~CFontObject()
{
	CTrace::TraceInfo(_T("CFontObject::~CFontObject(): m_hFont(%u)"), m_hFont);

	DeleteObject();
}

BOOL CFontObject::IsNull() const
{
	return m_hFont == NULL;
}

HFONT CFontObject::GetFont() const 
{
	return m_hFont;
}

BOOL CFontObject::CreateFont()
{
	DeleteObject();
	
	m_hFont = CreateFontIndirect(&m_logFont);
	DUI_ASSERT(m_hFont != NULL);
	
	return !IsNull();
}

VOID CFontObject::SetHeight(INT nHeight)
{
	m_logFont.lfHeight = nHeight;
}

VOID CFontObject::SetWeight(INT nWeight)
{
	m_logFont.lfWeight = nWeight;
}

VOID CFontObject::SetItalic(BOOL bItalic)
{
	m_logFont.lfItalic = bItalic;
}

VOID CFontObject::SetUnderline(BOOL bUnderLine)
{
	m_logFont.lfUnderline = bUnderLine;
}

VOID CFontObject::SetStrikeout(BOOL bStrikeout)
{
	m_logFont.lfStrikeOut = bStrikeout;
}

VOID CFontObject::SetFaceName(LPCTSTR lpszFaceName)
{
	if(lpszFaceName != NULL)
	{
		lstrcpyn(m_logFont.lfFaceName, lpszFaceName, LF_FACESIZE - 1);
	}
}

INT CFontObject::GetHeight() const
{
	return m_logFont.lfHeight;
}

INT CFontObject::GetWeight() const
{
	return m_logFont.lfWeight;
}

BOOL CFontObject::GetItalic() const
{
	return m_logFont.lfItalic;
}

BOOL CFontObject::GetUnderline() const
{
	return m_logFont.lfUnderline;
}

BOOL CFontObject::GetStrikeout() const
{
	return m_logFont.lfStrikeOut;
}

LPCTSTR CFontObject::GetFaceName() const
{
	return m_logFont.lfFaceName;
}

CTextStyle::CTextStyle()
:m_dwFormat(0)
{ NULL; }

VOID CTextStyle::SetTextFormat(DWORD dwFormat)
{
	m_dwFormat = dwFormat;
}

DWORD CTextStyle::GetTextFormat() const
{
	return m_dwFormat;
}

VOID CTextStyle::SetFont(CRefPtr<CFontObject> pFont)
{
	m_pFont = pFont;
}

CRefPtr<CFontObject> CTextStyle::GetFont() const 
{
	return m_pFont;
}

VOID CTextStyle::SetNormalColor(const CARGB& clr)
{
	m_clrNormal = clr;
}

CARGB CTextStyle::GetNormalColor() const 
{
	return m_clrNormal;
}

VOID CTextStyle::SetDisableColor(const CARGB& clr)
{
	m_clrDisable = clr;
}

CARGB CTextStyle::GetDisableColor() const 
{
	return m_clrDisable;
}

VOID CTextStyle::SetHoverColor(const CARGB& clr)
{
	m_clrHover = clr;
}

CARGB CTextStyle::GetHoverColor() const 
{
	return m_clrHover;
}

VOID CTextStyle::SetSelectColor(const CARGB& clr)
{
	m_clrSelect = clr;
}

CARGB CTextStyle::GetSelectColor() const 
{
	return m_clrSelect;
}

DUI_END_NAMESPCE