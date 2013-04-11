#include "DUIRichEdit.h"
#include "textserv.h"
#include "DUIScrollbar.h"
#include "DUIRenderEngine.h"

#pragma comment(lib, "Imm32.lib")

DUI_BGN_NAMESPCE

const LONG cInitTextMax = (32 * 1024) - 1;

EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

class CTxtWinHost : public ITextHost
{
public:
    CTxtWinHost();
    BOOL Init(CDUIRichEdit *re , const CREATESTRUCT *pcs);
	VOID UnInit();
    virtual ~CTxtWinHost();

    ITextServices* GetTextServices(void) { return m_pTextService ;}
	
	VOID GetViewInset(RECT& rt) const;
	VOID SetViewInset(const RECT& rt);

    void SetClientRect(RECT *prc);
    RECT* GetClientRect() { return &m_rcClient; }
    BOOL GetWordWrap(void) { return m_fWordWrap; }
    void SetWordWrap(BOOL fWordWrap);
    BOOL GetReadOnly();
    void SetReadOnly(BOOL fReadOnly);
    void SetFont(HFONT hFont);
    void SetColor(COLORREF clrText);
    SIZEL* GetExtent();
    void SetExtent(SIZEL *psizelExtent);
    void LimitText(LONG nChars);
	LONG GetLimitText();
    BOOL IsCaptured();

    BOOL GetAllowBeep();
    void SetAllowBeep(BOOL fAllowBeep);
    WORD GetDefaultAlign();
    void SetDefaultAlign(WORD wNewAlign);
    BOOL GetRichTextFlag();
    void SetRichTextFlag(BOOL fNew);
    LONG GetDefaultLeftIndent();
    void SetDefaultLeftIndent(LONG lNewIndent);
    BOOL SetSaveSelection(BOOL fSaveSelection);
    HRESULT OnTxInPlaceDeactivate();
    HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
    BOOL GetActiveState(void) { return m_fInplaceActive; }
    BOOL DoSetCursor(RECT *prc, POINT *pt);
    void SetTransparent(BOOL fTransparent);
    void GetControlRect(LPRECT prc);
    LONG SetAccelPos(LONG laccelpos);
    WCHAR SetPasswordChar(WCHAR chPasswordChar);
    void SetDisabled(BOOL fOn);
    LONG SetSelBarWidth(LONG lSelBarWidth);
    BOOL GetTimerState();
	VOID SetEventMask(DWORD dwMask);
	DWORD GetEventMask() const;

    void SetCharFormat(CHARFORMAT2W &c);
    void SetParaFormat(PARAFORMAT2 &p);

    // -----------------------------
    //	IUnknown interface
    // -----------------------------
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

    // -----------------------------
    //	ITextHost interface
    // -----------------------------
    virtual HDC TxGetDC();
    virtual INT TxReleaseDC(HDC hdc);
    virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
    virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
    virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
    virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
    virtual void TxViewChange(BOOL fUpdate);
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL TxShowCaret(BOOL fShow);
    virtual BOOL TxSetCaretPos(INT x, INT y);
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void TxKillTimer(UINT idTimer);
    virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
    virtual void TxSetCapture(BOOL fCapture);
    virtual void TxSetFocus();
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
    virtual BOOL TxScreenToClient (LPPOINT lppt);
    virtual BOOL TxClientToScreen (LPPOINT lppt);
    virtual HRESULT TxActivate( LONG * plOldState );
    virtual HRESULT TxDeactivate( LONG lNewState );
    virtual HRESULT TxGetClientRect(LPRECT prc);
    virtual HRESULT TxGetViewInset(LPRECT prc);
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
    virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
    virtual COLORREF TxGetSysColor(int nIndex);
    virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
    virtual HRESULT TxGetMaxLength(DWORD *plength);
    virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
    virtual HRESULT TxGetPasswordChar(TCHAR *pch);
    virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
    virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
    virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
    virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
    virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);
    virtual HIMC TxImmGetContext(void);
    virtual void TxImmReleaseContext(HIMC himc);
    virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

protected:
	BOOL InitDefaultCharFormat();
	BOOL InitDefaultParaFormat();

private:
    CDUIRichEdit* m_pRichEdit;
    ITextServices* m_pTextService;		    // pointer to Text Services object
    // Properties

    DWORD		m_dwStyle;				// style bits

    BOOL	m_fEnableAutoWordSel;	// enable Word style auto word selection?
    BOOL	m_fWordWrap			;	// Whether control should word wrap
    BOOL	m_fAllowBeep		;	// Whether beep is allowed
    BOOL	m_fRich				;	// Whether control is rich text
    BOOL	m_fSaveSelection	;	// Whether to save the selection when inactive
    BOOL	m_fInplaceActive	;  // Whether control is inplace active
    BOOL	m_fTransparent		;  // Whether control is transparent
    BOOL	m_fTimer		;	   // A timer is set
    BOOL    m_fCaptured;

    LONG		m_lSelBarWidth;			// Width of the selection bar
    LONG  		m_cchTextMost;			// maximum text size
    DWORD		m_dwEventMask;			// DoEvent mask to pass on to parent window
    LONG		m_icf;
    LONG		m_ipf;
    RECT		m_rcClient;				// Client Rect for this control
    SIZEL		m_sizelExtent;			// Extent array
    CHARFORMAT2W m_cf;					// Default character format
    PARAFORMAT2	m_pf;					    // Default paragraph format
    LONG		m_laccelpos;				// Accelerator position
    WCHAR		m_chPasswordChar;		    // Password character
	RECT		m_rtInset;
	RECT		m_rtInsetHimetric;
};

// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
{
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
{
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

CTxtWinHost::CTxtWinHost()
{
	m_pTextService = NULL;
	m_pRichEdit = NULL;
    m_dwStyle = 0;

    m_fEnableAutoWordSel= 0;
    m_fWordWrap	= 0;
    m_fAllowBeep = TRUE;
    m_fRich = 0;
    m_fSaveSelection = 0;
    m_fInplaceActive = 0;
    m_fTransparent = 0;
    m_fTimer = 0;
    m_fCaptured = 0;

    m_lSelBarWidth = 0;
    m_cchTextMost = cInitTextMax;
    m_dwEventMask = 0;
    m_icf = 0;
    m_ipf = 0;
    ZeroMemory(&m_rcClient, sizeof(m_rcClient));
   	m_sizelExtent.cx = m_sizelExtent.cy = 0;
	ZeroMemory(&m_cf, sizeof(m_cf));
	ZeroMemory(&m_pf, sizeof(m_pf));
    m_laccelpos = -1;
    m_chPasswordChar = _T('*');

	ZeroMemory(&m_rtInset, sizeof(m_rtInset));
	ZeroMemory(&m_rtInsetHimetric, sizeof(m_rtInsetHimetric));
}

CTxtWinHost::~CTxtWinHost()
{
	UnInit();
}

BOOL CTxtWinHost::InitDefaultCharFormat()
{
	CHARFORMAT2W* pcf = &m_cf;
    memset(pcf, 0, sizeof(CHARFORMAT2W));

    LOGFONT lf;
	HFONT hFont(NULL);
	COLORREF  clrColor;
	if(m_pRichEdit != NULL)
	{
		CRefPtr<CTextStyle> pTextStyle = m_pRichEdit->GetTextStyle();
		if(!pTextStyle.IsNull())
		{
			CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
			if(!pFont.IsNull())
			{
				hFont = pFont->GetFont();
			}
			
			if(m_pRichEdit->IsEnabled())
			{
				clrColor = pTextStyle->GetNormalColor().GetColor();
			}
			else
			{
				clrColor = pTextStyle->GetDisableColor().GetColor();
			}
		}
	}

	if(hFont == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	::GetObject(hFont, sizeof(LOGFONT), &lf);

	HDC hDC = GetDC(NULL);
	LONG yPixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
	ReleaseDC(NULL, hDC);

    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->crTextColor = clrColor;
  
    pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE 
		| CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
    if(lf.lfWeight >= FW_BOLD)
        pcf->dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        pcf->dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        pcf->dwEffects |= CFE_UNDERLINE;
    pcf->bCharSet = lf.lfCharSet;
    pcf->bPitchAndFamily = lf.lfPitchAndFamily;
    _tcscpy(pcf->szFaceName, lf.lfFaceName);

    return TRUE;
}

BOOL CTxtWinHost::InitDefaultParaFormat()
{
    memset(&m_pf, 0, sizeof(PARAFORMAT2));
    m_pf.cbSize = sizeof(PARAFORMAT2);
    m_pf.dwMask = PFM_ALL;
    m_pf.wAlignment = PFA_LEFT;
    m_pf.cTabCount = 1;
    m_pf.rgxTabs[0] = lDefaultTab;

	if(m_pRichEdit != NULL)
	{
		CRefPtr<CTextStyle> pTextStyle = m_pRichEdit->GetTextStyle();
		if(!pTextStyle.IsNull())
		{
			DWORD dwFormat = pTextStyle->GetTextFormat();
			if(dwFormat & DT_CENTER)
			{
				 m_pf.wAlignment = PFA_CENTER;
			}
			else if(dwFormat & DT_RIGHT)
			{
				 m_pf.wAlignment = PFA_RIGHT;
			}
		}
	}

	return TRUE;
}

BOOL CTxtWinHost::Init(CDUIRichEdit *pEdit , const CREATESTRUCT *pcs)
{
    m_pRichEdit = pEdit;

    if(!InitDefaultCharFormat()) return FALSE;

    if(!InitDefaultParaFormat()) return FALSE;


    if (pcs != NULL)
    {
        m_dwStyle |= pcs->style;

        if ( !(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) )
        {
            m_fWordWrap = TRUE;
        }
    }

	IUnknown* pUnk = NULL;
    HRESULT hr;
    // Create Text Services component
	HINSTANCE hModRichEdit = ::GetModuleHandle(RICHEDIT_DLL_NAME);
	if(hModRichEdit == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	PCreateTextServices pCreateFun = (PCreateTextServices)::GetProcAddress(hModRichEdit, "CreateTextServices");
	if(pCreateFun == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
    if(FAILED(pCreateFun(NULL, this, &pUnk))) return FALSE;

	DUI_ASSERT(m_pTextService == NULL);
	DUI_ASSERT(pUnk != NULL);
	if(pUnk != NULL)
	{
		hr = pUnk->QueryInterface(IID_ITextServices,(void **)&m_pTextService);
		pUnk->Release();
	}

    if(FAILED(hr))
    {
		DUI_ASSERT(FALSE);
        return FALSE;
    }
	DUI_ASSERT(m_pTextService != NULL);

    // Set window text
    if(pcs != NULL 
		&& pcs->lpszName != NULL
		&& m_pTextService != NULL)
    {
		m_pTextService->TxSetText((TCHAR *)pcs->lpszName);
		OnTxInPlaceActivate(NULL);
		m_pTextService->TxSendMessage(EM_SETEVENTMASK, 0, m_dwEventMask, 0);
    }

    return TRUE;
}

VOID CTxtWinHost::UnInit()
{
	if(m_pTextService != NULL)
	{
		OnTxInPlaceDeactivate();
		m_pTextService->Release();
		m_pTextService = NULL;
	}

	m_pRichEdit = NULL;
}

/////////////////// IUnknow  /////////////////////////////
HRESULT CTxtWinHost::QueryInterface(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown) 
        || IsEqualIID(riid, IID_ITextHost)) 
    {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG CTxtWinHost::AddRef(void)
{
	return 1;
}

ULONG CTxtWinHost::Release(void)
{
	return 1;
}

/////////////////////////////////  Far East Support  //////////////////////////////////////

HIMC CTxtWinHost::TxImmGetContext(void)
{
	HWND hWnd = m_pRichEdit->GetManagerWnd(FALSE);
	if(hWnd != NULL)
	{
		return ImmGetContext(hWnd);
	}
	else
	{
		return NULL;
	}
}

void CTxtWinHost::TxImmReleaseContext(HIMC himc)
{
	HWND hWnd = m_pRichEdit->GetManagerWnd(FALSE);
	if(hWnd != NULL)
	{
	  ImmReleaseContext(hWnd, himc);
	}
}

//////////////////////////// ITextHost Interface  ////////////////////////////

HDC CTxtWinHost::TxGetDC()
{
	DUI_ASSERT(m_pRichEdit != NULL);
	HWND hWnd = m_pRichEdit->GetManagerWnd();
	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));

	if(hWnd != NULL && ::IsWindow(hWnd))
	{
		return GetDC(hWnd);
	}
	else
	{
		return NULL;
	}
}

int CTxtWinHost::TxReleaseDC(HDC hdc)
{
	DUI_ASSERT(m_pRichEdit != NULL);
	HWND hWnd = m_pRichEdit->GetManagerWnd();
	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));

	if(hWnd != NULL && ::IsWindow(hWnd))
	{
		return ReleaseDC(hWnd, hdc);
	}
	else
	{
		return 0;
	}
}

BOOL CTxtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
	if(m_pRichEdit == NULL) return FALSE;
	IDUIScrollBar* pHorSB = m_pRichEdit->GetHorizontalSB();
	IDUIScrollBar* pVertSB = m_pRichEdit->GetVerticalSB();
	if(pHorSB == NULL || pVertSB == NULL) return FALSE;

	if(fnBar == SB_VERT || fnBar == SB_BOTH)
	{
		if(fShow)
		{
			pVertSB->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}
		else
		{
			pVertSB->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		}
	}

	if(fnBar == SB_HORZ || fnBar == SB_BOTH)
	{
		if(fShow)
		{
			pHorSB->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}
		else
		{
			pHorSB->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		}
	}

    return TRUE;
}

BOOL CTxtWinHost::TxEnableScrollBar (INT fnBar, INT fuArrowflags)
{
	if(m_pRichEdit == NULL) return FALSE;
	IDUIScrollBar* pHorSB = m_pRichEdit->GetHorizontalSB();
	IDUIScrollBar* pVertSB = m_pRichEdit->GetVerticalSB();
	if(pHorSB == NULL || pVertSB == NULL) return FALSE;

	if(fnBar == SB_VERT || fnBar == SB_BOTH)
	{
		if(fuArrowflags != ESB_ENABLE_BOTH)
		{
			pVertSB->ModifyStatus(0, CONTROL_STATUS_DISABLE);
		}
		else
		{
			pVertSB->ModifyStatus(CONTROL_STATUS_DISABLE, 0);
		}
	}

	if(fnBar == SB_HORZ || fnBar == SB_BOTH)
	{
		if(fuArrowflags != ESB_ENABLE_BOTH)
		{
			pHorSB->ModifyStatus(0, CONTROL_STATUS_DISABLE);
		}
		else
		{
			pHorSB->ModifyStatus(CONTROL_STATUS_DISABLE, 0);
		}
	}
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	if(m_pRichEdit == NULL) return FALSE;
	IDUIScrollBar* pHorSB = m_pRichEdit->GetHorizontalSB();
	IDUIScrollBar* pVertSB = m_pRichEdit->GetVerticalSB();
	if(pHorSB == NULL || pVertSB == NULL) return FALSE;

	SCROLLINFO si = {0};
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nPos = 0;
	si.nMin = nMinPos;
	si.nMax = nMaxPos;

	if(fnBar == SB_HORZ)
	{
		si.nPage = m_rcClient.right - m_rcClient.left;
		pHorSB->SetScrollBarInfo(&si);
	}
	else
	{
		si.nPage = m_rcClient.bottom - m_rcClient.top;
		pVertSB->SetScrollBarInfo(&si);
	}
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
{
	if(m_pRichEdit == NULL) return FALSE;
	IDUIScrollBar* pHorSB = m_pRichEdit->GetHorizontalSB();
	IDUIScrollBar* pVertSB = m_pRichEdit->GetVerticalSB();
	if(pHorSB == NULL || pVertSB == NULL) return FALSE;

	SCROLLINFO si = {0};
	si.fMask = SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	si.nPos = nPos;
	si.nMin = 0;
	si.nMax = 0;

	if(fnBar == SB_HORZ)
	{
		si.nPage = m_rcClient.right - m_rcClient.left;
		pHorSB->SetScrollBarInfo(&si);
	}
	else
	{
		si.nPage = m_rcClient.bottom - m_rcClient.top;
		pVertSB->SetScrollBarInfo(&si);
	}

    return TRUE;
}

void CTxtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	RECT rtUpdate;
    if( prc == NULL )
	{
        rtUpdate = m_rcClient;
    }
	else
	{
		rtUpdate = *prc;
	}
	
	DUI_ASSERT(m_pRichEdit != NULL);
	m_pRichEdit->UpdateLayout(FALSE, &rtUpdate);
}

void CTxtWinHost::TxViewChange(BOOL fUpdate) 
{
	DUI_ASSERT(m_pRichEdit != NULL);
	
    if( m_pRichEdit->OnTxViewChanged() )
	{
		m_pRichEdit->UpdateLayout(FALSE, NULL);
		if(fUpdate)
		{
			HWND hWnd = m_pRichEdit->GetManagerWnd(FALSE);
			if(hWnd != NULL)
			{
				::UpdateWindow(hWnd);
			}
		}
	}
}

BOOL CTxtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	DUI_ASSERT(m_pRichEdit != NULL);
	HWND hWnd = m_pRichEdit->GetManagerWnd();
	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));
	
	if(hWnd != NULL && ::IsWindow(hWnd))
		return ::CreateCaret(hWnd, hbmp, xWidth, yHeight);
	else
		return FALSE;
}

BOOL CTxtWinHost::TxShowCaret(BOOL fShow)
{
	DUI_ASSERT(m_pRichEdit != NULL);
	HWND hWnd = m_pRichEdit->GetManagerWnd();
	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));
	
	if(hWnd != NULL && ::IsWindow(hWnd))
	{
		if(fShow)
			return ::ShowCaret(hWnd);
		else
			return ::HideCaret(hWnd);
	}
	else
	{
		return FALSE;
	}
}

BOOL CTxtWinHost::TxSetCaretPos(INT x, INT y)
{
    return ::SetCaretPos(x, y);
}

BOOL CTxtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    DUI_ASSERT(m_pRichEdit != NULL);
    m_fTimer = m_pRichEdit->SetTimer(idTimer, uTimeout);
	return m_fTimer;
}

void CTxtWinHost::TxKillTimer(UINT idTimer)
{
    DUI_ASSERT(m_pRichEdit != NULL);
    m_pRichEdit->KillTimer(idTimer);
    m_fTimer = FALSE;
}

void CTxtWinHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	
					 LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
{
	CTrace::TraceInfo(_T("CTxtWinHost::TxScrollWindowEx"));
    return;
}

void CTxtWinHost::TxSetCapture(BOOL fCapture)
{
	DUI_ASSERT(m_pRichEdit != NULL);
	HWND hWnd = m_pRichEdit->GetManagerWnd();
	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));

	if(hWnd != NULL && ::IsWindow(hWnd))
	{
		if (fCapture)
			SetCapture(hWnd);
		else
			ReleaseCapture();

		m_fCaptured = fCapture;
	}
}

void CTxtWinHost::TxSetFocus()
{
 	DUI_ASSERT(m_pRichEdit != NULL);

	m_pRichEdit->SetFocus(TRUE);
}


void CTxtWinHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
{
    ::SetCursor(hcur);
}

BOOL CTxtWinHost::TxScreenToClient(LPPOINT lppt)
{
	DUI_ASSERT(m_pRichEdit != NULL);
	HWND hWnd = m_pRichEdit->GetManagerWnd();
	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));

	if(hWnd != NULL && ::IsWindow(hWnd))
	{
		return ::ScreenToClient(hWnd, lppt);
	}
	else
	{
		return FALSE;
	}
}

BOOL CTxtWinHost::TxClientToScreen(LPPOINT lppt)
{
	DUI_ASSERT(m_pRichEdit != NULL);
	HWND hWnd = m_pRichEdit->GetManagerWnd();
	DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));

	if(hWnd != NULL && ::IsWindow(hWnd))
	{
		return ::ClientToScreen(hWnd, lppt);
	}
	else
	{
		return FALSE;
	}
}

HRESULT CTxtWinHost::TxActivate(LONG *plOldState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxDeactivate(LONG lNewState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetClientRect(LPRECT prc)
{
    *prc = m_rcClient;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetViewInset(LPRECT prc) 
{
    *prc = m_rtInsetHimetric;
    return NOERROR;	
}

HRESULT CTxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    *ppCF = &m_cf;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
    *ppPF = &m_pf;
    return NOERROR;
}

COLORREF CTxtWinHost::TxGetSysColor(int nIndex) 
{
    return ::GetSysColor(nIndex);
}

HRESULT CTxtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = !m_fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetMaxLength(DWORD *pLength)
{
    *pLength = m_cchTextMost;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
    *pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return NOERROR;
}


HRESULT CTxtWinHost::TxGetPasswordChar(TCHAR *pch)
{
    *pch = m_chPasswordChar;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetAcceleratorPos(LONG *pcp)
{
    *pcp = m_laccelpos;
    return S_OK;
} 										   

HRESULT CTxtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
    return S_OK;
}

HRESULT CTxtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
{
    DWORD dwProperties = 0;

    if (m_fRich)
    {
        dwProperties = TXTBIT_RICHTEXT;
    }

    if (m_dwStyle & ES_MULTILINE)
    {
        dwProperties |= TXTBIT_MULTILINE;
    }

    if (m_dwStyle & ES_READONLY)
    {
        dwProperties |= TXTBIT_READONLY;
    }

    if (m_dwStyle & ES_PASSWORD)
    {
        dwProperties |= TXTBIT_USEPASSWORD;
    }

    if (!(m_dwStyle & ES_NOHIDESEL))
    {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (m_fEnableAutoWordSel)
    {
        dwProperties |= TXTBIT_AUTOWORDSEL;
    }

    if (m_fWordWrap)
    {
        dwProperties |= TXTBIT_WORDWRAP;
    }

    if (m_fAllowBeep)
    {
        dwProperties |= TXTBIT_ALLOWBEEP;
    }

    if (m_fSaveSelection)
    {
        dwProperties |= TXTBIT_SAVESELECTION;
    }

    *pdwBits = (dwProperties & dwMask); 

    return NOERROR;
}

HRESULT CTxtWinHost::TxNotify(DWORD iNotify, void *pv)
{
//     if( iNotify == EN_REQUESTRESIZE ) 
// 	{
//         RECT rc;
//         REQRESIZE *preqsz = (REQRESIZE *)pv;
//         GetControlRect(&rc);
//         rc.bottom = rc.top + preqsz->rc.bottom;
//         rc.right  = rc.left + preqsz->rc.right;
//         SetClientRect(&rc);
//         return S_OK;
//     }

	DUI_ASSERT(m_pRichEdit != NULL);
    m_pRichEdit->OnTxNotify(iNotify, pv);
    
	return S_OK;
}

HRESULT CTxtWinHost::TxGetExtent(LPSIZEL lpExtent)
{
    *lpExtent = m_sizelExtent;
    return S_OK;
}

HRESULT	CTxtWinHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
{
    *plSelBarWidth = m_lSelBarWidth;
    return S_OK;
}

////////////////////// helper functions ///////////////////////////////
VOID CTxtWinHost::GetViewInset(RECT& rt) const
{
	rt = m_rtInset;
}

VOID CTxtWinHost::SetViewInset(const RECT& rt)
{
	m_rtInset = rt;

	HDC hDC = GetDC(NULL);
    LONG xPerInch = ::GetDeviceCaps(hDC, LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(hDC, LOGPIXELSY); 
	ReleaseDC(NULL, hDC);
	
	m_rtInsetHimetric.left = DXtoHimetricX(rt.left, xPerInch);
	m_rtInsetHimetric.right = DXtoHimetricX(rt.right, xPerInch);
	m_rtInsetHimetric.top = DYtoHimetricY(rt.top, yPerInch);
	m_rtInsetHimetric.bottom = DYtoHimetricY(rt.bottom, yPerInch);

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
	}
}

void CTxtWinHost::SetWordWrap(BOOL fWordWrap)
{
    m_fWordWrap = fWordWrap;

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
	}
}

BOOL CTxtWinHost::GetReadOnly()
{
    return (m_dwStyle & ES_READONLY) != 0;
}

void CTxtWinHost::SetReadOnly(BOOL fReadOnly)
{
    if (fReadOnly)
    {
        m_dwStyle |= ES_READONLY;
    }
    else
    {
        m_dwStyle &= ~ES_READONLY;
    }

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_READONLY, 
			fReadOnly ? TXTBIT_READONLY : 0);
	}
}

void CTxtWinHost::SetFont(HFONT hFont) 
{
    if( hFont == NULL ) return;
    LOGFONT lf;
    ::GetObject(hFont, sizeof(LOGFONT), &lf);

	HDC dc = GetDC(NULL);
    LONG yPixPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);
	ReleaseDC(NULL, dc);

    m_cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    if(lf.lfWeight >= FW_BOLD)
        m_cf.dwEffects |= CFE_BOLD;
    if(lf.lfItalic)
        m_cf.dwEffects |= CFE_ITALIC;
    if(lf.lfUnderline)
        m_cf.dwEffects |= CFE_UNDERLINE;
    m_cf.bCharSet = lf.lfCharSet;
    m_cf.bPitchAndFamily = lf.lfPitchAndFamily;
    _tcscpy(m_cf.szFaceName, lf.lfFaceName);
	
	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}
}

void CTxtWinHost::SetColor(COLORREF clrText)
{
    m_cf.crTextColor = clrText;
	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}
}

SIZEL* CTxtWinHost::GetExtent() 
{
    return &m_sizelExtent;
}

void CTxtWinHost::SetExtent(SIZEL *psizelExtent) 
{ 
    m_sizelExtent = *psizelExtent; 
	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
	}
}

void CTxtWinHost::LimitText(LONG nChars)
{
    m_cchTextMost = nChars;
    if( m_cchTextMost <= 0 ) m_cchTextMost = cInitTextMax;

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
	}
}

LONG CTxtWinHost::GetLimitText()
{
	return m_cchTextMost;
}

BOOL CTxtWinHost::IsCaptured()
{
    return m_fCaptured;
}

BOOL CTxtWinHost::GetAllowBeep()
{
    return m_fAllowBeep;
}

void CTxtWinHost::SetAllowBeep(BOOL fAllowBeep)
{
    m_fAllowBeep = fAllowBeep;

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
			fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
	}
}

WORD CTxtWinHost::GetDefaultAlign()
{
    return m_pf.wAlignment;
}

void CTxtWinHost::SetDefaultAlign(WORD wNewAlign)
{
    m_pf.wAlignment = wNewAlign;

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}
}

BOOL CTxtWinHost::GetRichTextFlag()
{
    return m_fRich;
}

void CTxtWinHost::SetRichTextFlag(BOOL fNew)
{
    m_fRich = fNew;

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
			fNew ? TXTBIT_RICHTEXT : 0);
	}
}

VOID CTxtWinHost::SetEventMask(DWORD dwMask)
{
	m_dwEventMask = dwMask;
	if(m_pTextService != NULL)
	{
		m_pTextService->TxSendMessage(EM_SETEVENTMASK, 0, m_dwEventMask, 0);
	}	
}

DWORD CTxtWinHost::GetEventMask() const 
{
	return m_dwEventMask;
}

LONG CTxtWinHost::GetDefaultLeftIndent()
{
    return m_pf.dxOffset;
}

void CTxtWinHost::SetDefaultLeftIndent(LONG lNewIndent)
{
    m_pf.dxOffset = lNewIndent;
	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}
}


void CTxtWinHost::GetControlRect(LPRECT prc)
{
    prc->top = m_rcClient.top;
    prc->bottom = m_rcClient.bottom;
    prc->left = m_rcClient.left;
    prc->right = m_rcClient.right;
}

void CTxtWinHost::SetClientRect(RECT *prc) 
{
    m_rcClient = *prc;

	HDC hDC = GetDC(NULL);
    LONG xPerInch = ::GetDeviceCaps(hDC, LOGPIXELSX); 
    LONG yPerInch =	::GetDeviceCaps(hDC, LOGPIXELSY); 
	ReleaseDC(NULL, hDC);

    m_sizelExtent.cx = DXtoHimetricX(m_rcClient.right - m_rcClient.left, xPerInch);
    m_sizelExtent.cy = DYtoHimetricY(m_rcClient.bottom - m_rcClient.top, yPerInch);

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);
	}
}

BOOL CTxtWinHost::SetSaveSelection(BOOL f_SaveSelection)
{
    BOOL fResult = f_SaveSelection;
    m_fSaveSelection = f_SaveSelection;

	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
			m_fSaveSelection ? TXTBIT_SAVESELECTION : 0);
	}

    return fResult;		
}

HRESULT	CTxtWinHost::OnTxInPlaceDeactivate()
{
	HRESULT hr(E_FAIL);
	if(m_pTextService != NULL)
	{
		hr	= m_pTextService->OnTxInPlaceDeactivate();
	}
    if (SUCCEEDED(hr))
    {
        m_fInplaceActive = FALSE;
    }

    return hr;
}

HRESULT	CTxtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
    m_fInplaceActive = TRUE;
	HRESULT hr(E_FAIL);
	if(m_pTextService != NULL)
	{
		hr = m_pTextService->OnTxInPlaceActivate(prcClient);
	}
    if (FAILED(hr))
    {
        m_fInplaceActive = FALSE;
    }

    return hr;
}

BOOL CTxtWinHost::DoSetCursor(RECT *prc, POINT *pt)
{
    RECT rc = prc ? *prc : m_rcClient;

    if (PtInRect(&rc, *pt))
    {
		DUI_ASSERT(m_pRichEdit != NULL);
		HWND hWnd = m_pRichEdit->GetManagerWnd();
		DUI_ASSERT(hWnd != NULL && ::IsWindow(hWnd));
		if(hWnd != NULL && ::IsWindow(hWnd) 
			&& m_pTextService != NULL)
		{
			HDC dc = GetDC(hWnd);
			RECT *prcClient = (!m_fInplaceActive || prc) ? &rc : NULL;
			m_pTextService->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL, dc,
				NULL, prcClient, pt->x, pt->y);
			ReleaseDC(hWnd, dc);

			return TRUE;
		}
    }

    return FALSE;
}

void CTxtWinHost::SetTransparent(BOOL f_Transparent)
{
    m_fTransparent = f_Transparent;
    if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
	}
}

LONG CTxtWinHost::SetAccelPos(LONG l_accelpos)
{
    LONG laccelposOld = l_accelpos;
    m_laccelpos = l_accelpos;
    if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);
	}

    return laccelposOld;
}

WCHAR CTxtWinHost::SetPasswordChar(WCHAR ch_PasswordChar)
{
    WCHAR chOldPasswordChar = m_chPasswordChar;
    m_chPasswordChar = ch_PasswordChar;
    if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
        (m_chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

	}

    return chOldPasswordChar;
}

void CTxtWinHost::SetDisabled(BOOL fOn)
{
    m_cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
    m_cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

    if( !fOn )
    {
        m_cf.dwEffects &= ~CFE_DISABLED;
    }
	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}
}

LONG CTxtWinHost::SetSelBarWidth(LONG l_SelBarWidth)
{
    LONG lOldSelBarWidth = m_lSelBarWidth;

    m_lSelBarWidth = l_SelBarWidth;

    if (m_lSelBarWidth)
    {
        m_dwStyle |= ES_SELECTIONBAR;
    }
    else
    {
        m_dwStyle &= (~ES_SELECTIONBAR);
    }
	
	if(m_pTextService != NULL)
	{
		m_pTextService->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);
	}

    return lOldSelBarWidth;
}

BOOL CTxtWinHost::GetTimerState()
{
    return m_fTimer;
}

void CTxtWinHost::SetCharFormat(CHARFORMAT2W &c)
{
    m_cf = c;
	if(m_pTextService != NULL)
	{
		m_pTextService->TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&m_cf, 0);
	}
}

void CTxtWinHost::SetParaFormat(PARAFORMAT2 &p)
{
    m_pf = p;
}


///////////////////  CDUIRichEdit  /////////////////////
CDUIRichEdit::CDUIRichEdit() 
{
	m_pHost = new CTxtWinHost;
	DUI_ASSERT(m_pHost != NULL);
	m_dwEditStyle = 0;
	m_dwStyle |= (CONTROL_STYLE_TABSTOP | CONTROL_STYLE_SETCURSOR);
	m_bEnableContextMenu = FALSE;
}

CDUIRichEdit::CDUIRichEdit(const CDUIRichEdit& obj)
:IDUIRichEdit(obj)
{
	if(obj.m_pHost != NULL)
	{
		m_pHost = new CTxtWinHost(*obj.m_pHost);
	}
	else
	{
		DUI_ASSERT(FALSE);
		m_pHost = new CTxtWinHost;
	}

	m_dwEditStyle = obj.m_dwEditStyle;
	m_pImageBK = obj.m_pImageBK;
	m_bEnableContextMenu = obj.m_bEnableContextMenu;
}

CDUIRichEdit::~CDUIRichEdit()
{
    if( m_pHost != NULL )
	{
        delete m_pHost;
		m_pHost  = NULL;
    }
}

CRefPtr<IDUIControl> CDUIRichEdit::Clone()
{
	return new CDUIRichEdit(*this);
}

LRESULT CDUIRichEdit::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);

	switch(info.dwEventType)
	{
	case DUI_EVENT_SETCURSOR:
		if(m_pHost != NULL)
		{
			POINT ptHit = info.ptMouse;
			m_pHost->DoSetCursor(NULL, &ptHit);
			bHandled = TRUE;
		}
		break;
	
	case DUI_EVENT_SETFOCUS:
		if(m_pHost != NULL)
		{
			TxSendMessage(WM_SETFOCUS, 0, 0, 0);
			SetSelAll();
		}
		break;
	
	case DUI_EVENT_KILLFOCUS:
		if(m_pHost != NULL)
		{
			TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
		}
		break;

	case DUI_EVENT_MOUSEWHEEL:
		OnMouseWheel(info);
		bHandled = TRUE;
		break;

	case DUI_EVENT_TIMER:
		TxSendMessage(WM_TIMER, info.wParam, info.lParam, 0);
		break;

	case DUI_EVENT_VSCROLL:
		OnVertialSBEvent(info);
		bHandled = TRUE;
		break;
		
	case DUI_EVENT_HSCROLL:
		OnHorizontalSBEvent(info);
		bHandled = TRUE;
		break;

	case DUI_EVENT_MOUSEENTER:
		if(IsEnabled())
			ModifyStatus(0, CONTROL_STATUS_HOVER);
		bHandled = TRUE;
		break;
	
	case DUI_EVENT_MOUSELEAVE:
		if(IsEnabled())
			ModifyStatus(CONTROL_STATUS_HOVER, 0);
		bHandled = TRUE;
		break;

	case DUI_EVENT_CONTEXTMENU:
		if(IsEnabled())
		{
			OnContextMenu(info);
		}
		bHandled = TRUE;
		break;

	default:
		break;
	}

	if(bHandled) return lRet;
	
	return theBase::ProcessEvent(info, bHandled);
}

LPVOID CDUIRichEdit::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IRichEdit) == 0)
	{
		return (IDUIRichEdit*)this;
	}

	return theBase::GetInterface(strName);
}

VOID CDUIRichEdit::ParseScrollBarStyle(const CDUIString& strValue)
{
	if(strValue.find(_T("WS_VSCROLL")) != -1)
	{
		m_dwEditStyle |= WS_VSCROLL;
	}
	if(strValue.find(_T("WS_HSCROLL")) != -1)
	{
		m_dwEditStyle |= WS_HSCROLL;
	}
	if(strValue.find(_T("ES_DISABLENOSCROLL")) != -1)
	{
		m_dwEditStyle |= ES_DISABLENOSCROLL;
	}
	if(strValue.find(_T("ES_AUTOVSCROLL")) != -1)
	{
		m_dwEditStyle |= ES_AUTOVSCROLL;
	}
	if(strValue.find(_T("ES_AUTOHSCROLL")) != -1)
	{
		m_dwEditStyle |= ES_AUTOHSCROLL;
	}
}

VOID CDUIRichEdit::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("scrollbar_style")) == 0)
	{
		ParseScrollBarStyle(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("multiline")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			m_dwEditStyle |= ES_MULTILINE;
		}
		else
		{
			m_dwEditStyle &= ~ES_MULTILINE;
			m_dwEditStyle |= ES_AUTOHSCROLL;
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("rich")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetRich(TRUE);
		}
		else
		{
			SetRich(FALSE);
		}	
		bHandled = TRUE;
	}
	else if(strName.compare(_T("read_only")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetReadOnly(TRUE);
		}
		else
		{
			SetReadOnly(FALSE);
		}	
		bHandled = TRUE;
	}
	else if(strName.compare(_T("password")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			m_dwEditStyle |= ES_PASSWORD;
		}
		else
		{
			m_dwEditStyle &= ~ES_PASSWORD;
		}	
		bHandled = TRUE;
	}
	else if(strName.compare(_T("back_image")) == 0)
	{
		m_pImageBK = GetSkinMgr()->GetImage(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("inset")) == 0)
	{
		TCHAR szTextBuffer[128] = {0};
		lstrcpyn(szTextBuffer, strValue.c_str(), sizeof(szTextBuffer) / sizeof(TCHAR) - 1);
		
		RECT rtTemp = {0};
		LPTSTR pstr = NULL;
		rtTemp.left = _tcstol(szTextBuffer, &pstr, 10);
		rtTemp.top = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.right = _tcstol(pstr + 1, &pstr, 10);
		rtTemp.bottom = _tcstol(pstr + 1, &pstr, 10);

		SetViewInset(rtTemp);
		
		bHandled = TRUE;
	}
	else if(strName.compare(_T("context_menu")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			m_bEnableContextMenu = TRUE;
		}
		else
		{
			m_bEnableContextMenu = FALSE;
		}	
		bHandled = TRUE;
	}
	else if(strName.compare(_T("limit_text")) == 0)
	{
		INT nCount = _ttol(strValue.c_str());
		if(nCount >= 0)
		{
			SetLimitText(nCount);
		}
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUIRichEdit::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	RECT rtContent = GetContentRect();
	if(IsRectEmpty(&rtContent)) return;
	
	BOOL bVerSB(FALSE), bHorSB(FALSE);
	INT nThumbWidth(0), nThumbHeight(0);
	IDUIScrollBar* pVertSB = GetVerticalSB();
	if(pVertSB != NULL && (pVertSB->IsVisible()))
	{
		nThumbWidth = pVertSB->GetFixedSize().cx;
		bVerSB = TRUE;
	}
	IDUIScrollBar* pHorSB = GetHorizontalSB();
	if(pHorSB != NULL && (pHorSB->IsVisible()))
	{
		bHorSB = TRUE;
		nThumbHeight = pHorSB->GetFixedSize().cy;
	}	
	
	if(bVerSB)
	{
		RECT rtTemp = rtContent;
		if(bHorSB)
		{
			rtTemp.bottom -= nThumbHeight;
		}
		rtTemp.left = rtTemp.right - nThumbWidth;
		pVertSB->SetControlRect(rtTemp);
	}

	if(bHorSB)
	{
		RECT rtTemp = rtContent;
		if(bVerSB)
		{
			rtTemp.right -= nThumbWidth;
		}
		rtTemp.top = rtTemp.bottom - nThumbHeight;
		pHorSB->SetControlRect(rtTemp);
	}

	RECT rtControl = rtContent;
	rtControl.right -= nThumbWidth;
	rtControl.bottom -= nThumbHeight;
	if(IsRectEmpty(&rtControl))
	{
		rtControl.right = rtControl.left;
		rtControl.bottom = rtControl.top;
	}
	if(m_pHost != NULL)
	{
		m_pHost->SetClientRect(&rtControl);
	}
}

VOID CDUIRichEdit::DoPaint(HDC dc, const RECT& rtDraw)
{
	theBase::DoPaint(dc, rtDraw);
	
	RECT rtControl = {0};
	if(m_pHost != NULL)
	{
		m_pHost->GetControlRect(&rtControl);
	}
	ITextServices* pTextService = NULL;
	if(m_pHost != NULL)
	{
		pTextService = m_pHost->GetTextServices();
	}
	if(!::IsRectEmpty(&rtControl) 
		&& pTextService != NULL)
	{
        pTextService->TxDraw(
            DVASPECT_CONTENT,  		// Draw Aspect
            0,						// Lindex
            NULL,					// Info for drawing optimazation
            NULL,					// target device information
            dc,						// Draw device HDC
            NULL, 				   	// Target device HDC
            (RECTL*)&rtControl,		// Bounding client rectangle
            NULL, 		            // Clipping rectangle for metafiles
            (RECT*)&rtDraw,			// Update rectangle
            NULL, 	   				// Call back function
            NULL,					// Call back parameter
            0);				        // What view of the object
	}
}

BOOL CDUIRichEdit::IsEditAvailable(MSG* pMsg)
{
	if(m_pHost == NULL
		|| m_pHost->GetTextServices() == NULL)
	{
		return FALSE;
	}

	if(!IsEnabled() || !IsVisible(TRUE)
		|| IsRectEmpty(&m_rtControl))
	{
		return FALSE;
	}
	
	HWND hWnd = GetManagerWnd(FALSE);
	if(hWnd == NULL 
		|| pMsg->hwnd != hWnd)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDUIRichEdit::PreTranslateMessage(MSG* pMsg)
{
	if(!IsEditAvailable(pMsg)) return FALSE;
	HWND hWnd = GetManagerWnd(FALSE);

	UINT nMsgType = pMsg->message;
	WPARAM wParam = pMsg->wParam;
	LPARAM lParam = pMsg->lParam;
	if(nMsgType >= WM_MOUSEFIRST && nMsgType <= WM_MOUSELAST)
	{
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		RECT rtControl = {0};
		m_pHost->GetControlRect(&rtControl);

		if(nMsgType == WM_LBUTTONDOWN
			|| nMsgType == WM_LBUTTONUP
			|| nMsgType == WM_LBUTTONDBLCLK
			|| nMsgType == WM_RBUTTONDOWN
			|| nMsgType == WM_RBUTTONUP)
		{
			if(!PtInRect(&rtControl, pt))
			{
				return FALSE;
			}

		}

		//WM_MOUSEMOVE
		if(!m_pHost->IsCaptured()) 
		{
			if(!::PtInRect(&rtControl, pt) 
				|| GetCapture() == hWnd)
			{
				return FALSE;
			}
		}
		
		if(nMsgType == WM_LBUTTONDOWN
			|| nMsgType == WM_LBUTTONDBLCLK
			|| nMsgType == WM_RBUTTONDOWN)
		{
			if(!GetFocus())
			{
				SetFocus(TRUE);
				return FALSE;
			}
		}
	}
	
	if(!GetFocus()) return FALSE;

	//filter tab key, it is for navigate
	if(nMsgType == WM_KEYDOWN && wParam == VK_TAB)
	{
		return FALSE;
	}
	
	if(CDUIMessageLoop::IsIdleMessage(pMsg))
	DUI_DEBUG_MSG(pMsg);

    LRESULT lResult = 0;
	TxSendMessage(nMsgType, wParam, lParam, &lResult);

	return FALSE;
}


VOID CDUIRichEdit::OnCreate()
{
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return;

	CreateScrollBar();
	if(m_dwEditStyle & ES_DISABLENOSCROLL)
	{
		IDUIScrollBar* pVerSB = GetVerticalSB();
		if(pVerSB != NULL)
		{
			pVerSB->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}
		IDUIScrollBar* pHorSB = GetHorizontalSB();
		if(pHorSB != NULL)
		{
			pHorSB->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}
	}


	CREATESTRUCT cs;
	ZeroMemory(&cs, sizeof(cs));
	cs.style = m_dwEditStyle;
	cs.lpszName = m_strText.c_str();

	if(m_pHost->Init(this, &cs))
	{
		m_pHost->SetTransparent(TRUE);
		TxSendMessage(EM_SETLANGOPTIONS, 0, 0, 0);
		m_pHost->SetDisabled(!IsEnabled());
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	SetupMessageFilter(TRUE);
}

VOID CDUIRichEdit::OnDestroy()
{
	SetupMessageFilter(FALSE);

	if(m_pHost != NULL)
	{
		m_pHost->UnInit();
	}

	DestroyScrollBar();
}

VOID CDUIRichEdit::CreateScrollBar()
{
	if(m_pHorSB.IsNull())
	{
		CRefPtr<IDUIControl> pNewControl = GetControlFactory()->NewControl(DUI_HORZ_SCROLLBAR);
		if(!pNewControl.IsNull())
		{
			m_pHorSB = (IDUIScrollBar*)pNewControl->GetInterface(IScrollBar);
		}
	}

	if(!m_pHorSB.IsNull())
	{
		DUI_ASSERT(m_pControlMgr != NULL);
		m_pHorSB->ModifyStyle(CONTROL_STYLE_VISIBLE, 0); 
		m_pHorSB->AttachWindow(m_pControlMgr);
		m_pHorSB->SetScrollOwner(this);
	}

	if(m_pVertSB.IsNull())
	{
		CRefPtr<IDUIControl> pNewControl = GetControlFactory()->NewControl(DUI_VERT_SCROLLBAR);
		if(!pNewControl.IsNull())
		{
			m_pVertSB = (IDUIScrollBar*)pNewControl->GetInterface(IScrollBar);
		}
	}

	if(!m_pVertSB.IsNull())
	{
		DUI_ASSERT(m_pControlMgr != NULL);
		m_pVertSB->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		m_pVertSB->AttachWindow(m_pControlMgr);
		m_pVertSB->SetScrollOwner(this);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

VOID CDUIRichEdit::DestroyScrollBar()
{
	if(!m_pHorSB.IsNull())
	{
		m_pHorSB->DetachWindow();
		m_pHorSB->SetScrollOwner(NULL);
	}

	if(!m_pVertSB.IsNull())
	{
		m_pVertSB->DetachWindow();
		m_pVertSB->SetScrollOwner(NULL);
	}

	m_pHorSB = NULL;
	m_pVertSB = NULL;
}

IDUIScrollBar* CDUIRichEdit::GetHorizontalSB()
{
	return m_pHorSB.GetPtr();
}

IDUIScrollBar* CDUIRichEdit::GetVerticalSB()
{
	return m_pVertSB.GetPtr();	
}

VOID CDUIRichEdit::PaintBorder(HDC dc)
{
	
}

VOID CDUIRichEdit::PaintBkgnd(HDC dc)
{
	if(m_pImageBK.IsNull() || m_pImageBK->IsNull()) return;
	INT nImageCount = m_pImageBK->GetItemCount();

	BOOL bEnalbe = IsEnabled();
	BOOL bFocus = GetFocus();
	BOOL bHover = (m_dwStatus & CONTROL_STATUS_HOVER);

	INT nIndex(EDIT_IMAGE_INDEX_NORMAL);
	if(!bEnalbe)
	{
		nIndex = EDIT_IMAGE_INDEX_DISABLE;
	}
	else if(bHover)
	{
		nIndex = EDIT_IMAGE_INDEX_HOVER;
	}
	else if(bFocus)
	{
		nIndex = EDIT_IMAGE_INDEX_FOCUS;
	}

	if(nIndex >=  nImageCount)
	{
		nIndex = EDIT_IMAGE_INDEX_NORMAL;
	}

	m_pImageBK->MiddleStretch(dc, m_rtControl, nIndex);
}

VOID CDUIRichEdit::SetupMessageFilter(BOOL bAdd)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return;
	
	CDUIMessageLoop* pMsgLoop = pDUIApp->GetMessageLoop();
	if(pMsgLoop == NULL) return;
		
	if(bAdd)
	{
		pMsgLoop->AddMessageFilter(this);
	}
	else
	{
		pMsgLoop->RemoveMessageFilter(this);
	}
}


BOOL CDUIRichEdit::GetViewInset(RECT& rt) const
{
	if(m_pHost != NULL)
	{
		m_pHost->GetViewInset(rt);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDUIRichEdit::SetViewInset(const RECT& rt)
{
	if(m_pHost != NULL)
	{
		m_pHost->SetViewInset(rt);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL CDUIRichEdit::IsRich() const
{
	DUI_ASSERT(m_pHost != NULL);

	if(m_pHost != NULL)
		return m_pHost->GetRichTextFlag();
	else
		return FALSE;
}

VOID CDUIRichEdit::SetRich(BOOL bRich)
{
	DUI_ASSERT(m_pHost != NULL);
    if( m_pHost ) 
		m_pHost->SetRichTextFlag(bRich);
}

BOOL CDUIRichEdit::IsReadOnly() const
{
	DUI_ASSERT(m_pHost != NULL);

	if(m_pHost != NULL)
		return m_pHost->GetReadOnly();
	else
		return FALSE;
}

VOID CDUIRichEdit::SetReadOnly(BOOL bReadOnly)
{
	DUI_ASSERT(m_pHost != NULL);
    if( m_pHost ) 
		m_pHost->SetReadOnly(bReadOnly);
}

BOOL CDUIRichEdit::GetWordWrap() const 
{
	DUI_ASSERT(m_pHost != NULL);
	
	if(m_pHost != NULL)
		return m_pHost->GetWordWrap();
	else
		return FALSE;
}

VOID CDUIRichEdit::SetWordWrap(BOOL bWordWrap)
{
	DUI_ASSERT(m_pHost != NULL);
    if( m_pHost ) 
		m_pHost->SetWordWrap(bWordWrap);
}

DWORD CDUIRichEdit::GetEditStyle() const
{
    return m_dwEditStyle;
}

VOID CDUIRichEdit::SetEditStyle(DWORD dwStyle)
{
    m_dwEditStyle = dwStyle;
}

INT CDUIRichEdit::GetLimitText() const
{
	DUI_ASSERT(m_pHost != NULL);
	
	if(m_pHost != NULL)
		return m_pHost->GetLimitText();
	else
		return cInitTextMax;
}

VOID CDUIRichEdit::SetLimitText(INT iChars)
{
	DUI_ASSERT(m_pHost != NULL);
    if( m_pHost ) 
		m_pHost->LimitText(iChars);
}

DWORD CDUIRichEdit::GetTextLength(DWORD dwFlags) const
{
    GETTEXTLENGTHEX textLenEx;
    textLenEx.flags = dwFlags;
    textLenEx.codepage = 1200;

    LRESULT lResult(0);
    TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
    return (DWORD)lResult;
}

VOID CDUIRichEdit::SetText(const CDUIString& strText)
{
	if(m_strText.compare(strText) != 0)
	{
		theBase::SetText(strText);
		TxSendMessage(WM_SETTEXT, 0, (LPARAM)m_strText.c_str(), 0);
	}
}

VOID CDUIRichEdit::SetTextStyle(CRefPtr<CTextStyle> pTextStyle)
{
	theBase::SetTextStyle(pTextStyle);

	if(!pTextStyle.IsNull())
	{
		CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
		if(!pFont.IsNull())
		{
			HFONT hFont = pFont->GetFont();
			if(hFont != NULL)
			{
				m_pHost->SetFont(hFont);
			}
		}
		
		COLORREF clr(RGB(0, 0, 0));
		if(IsEnabled())
		{
			clr = pTextStyle->GetNormalColor().GetColor();
		}
		else
		{
			clr = pTextStyle->GetDisableColor().GetColor();
		}
		m_pHost->SetColor(clr);
	}
}

VOID CDUIRichEdit::ModifyStatus(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwCurrent = GetStatus();
	theBase::ModifyStatus(dwRemove, dwAdd);
	if((dwCurrent^m_dwStatus) & CONTROL_STATUS_DISABLE)
	{
		m_pHost->SetDisabled(!IsEnabled());
	}
}

const CDUIString& CDUIRichEdit::GetText()
{
    long lLen = GetTextLength(GTL_DEFAULT);
    LPTSTR lpText = NULL;
    GETTEXTEX gt;
    gt.flags = GT_DEFAULT;
    gt.cb = sizeof(TCHAR) * (lLen + 1) ;
    gt.codepage = 1200;
    lpText = new TCHAR[lLen + 1];
    ::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));

    gt.lpDefaultChar = NULL;
    gt.lpUsedDefChar = NULL;
    TxSendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)lpText, 0);
	m_strText = (LPCTSTR)lpText;
    delete[] lpText;
    return m_strText;
}

BOOL CDUIRichEdit::GetModify() const
{ 
    LRESULT lResult(FALSE);
    TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);
    return (BOOL)lResult;
}

VOID CDUIRichEdit::SetModify(BOOL bModified) const
{ 
    TxSendMessage(EM_SETMODIFY, bModified, 0, 0);
}

VOID CDUIRichEdit::GetSel(const CHARRANGE &cr) const
{ 
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
}

VOID CDUIRichEdit::GetSel(INT& nStartChar, INT& nEndChar) const
{
    CHARRANGE cr;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0); 
    nStartChar = cr.cpMin;
    nEndChar = cr.cpMax;
}

INT CDUIRichEdit::SetSel(const CHARRANGE& cr) const
{ 
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (INT)lResult;
}

INT CDUIRichEdit::SetSel(INT nStartChar, INT nEndChar) const
{
    CHARRANGE cr;
    cr.cpMin = nStartChar;
    cr.cpMax = nEndChar;
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult); 
    return (INT)lResult;
}

VOID CDUIRichEdit::ReplaceSel(const CDUIString& strNewText, BOOL bCanUndo)
{	
    TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)strNewText.c_str(), 0); 
}

CDUIString CDUIRichEdit::GetSelText() const
{
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return _T("");
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
    LPWSTR lpText = NULL;
    lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
    ::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
    TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpText, 0);
    CDUIString sText(lpText);
    delete[] lpText;
    return sText;
}

INT CDUIRichEdit::SetSelAll()
{
    return SetSel(0, -1);
}

INT CDUIRichEdit::SetSelNone()
{
    return SetSel(-1, 0);
}

BOOL CDUIRichEdit::GetZoom(int& nNum, int& nDen) const
{
    LRESULT lResult(FALSE);
    TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
    return (BOOL)lResult;
}

BOOL CDUIRichEdit::SetZoom(int nNum, int nDen)
{
    if (nNum < 0 || nNum > 64) return false;
    if (nDen < 0 || nDen > 64) return false;
    LRESULT lResult(FALSE);
    TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
    return (BOOL)lResult;
}

BOOL CDUIRichEdit::SetZoomOff()
{
    LRESULT lResult(FALSE);
    TxSendMessage(EM_SETZOOM, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

WORD CDUIRichEdit::GetSelectionType() const
{
    LRESULT lResult(0);
    TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
    return (WORD)lResult;
}

BOOL CDUIRichEdit::GetAutoURLDetect() const
{
    LRESULT lResult(FALSE);
    TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

BOOL CDUIRichEdit::SetAutoURLDetect(BOOL bAutoDetect)
{
    LRESULT lResult(FALSE);
    TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
    return (BOOL)lResult == FALSE;
}

DWORD CDUIRichEdit::GetEventMask() const
{
	if(m_pHost != NULL)
	{
		return m_pHost->GetEventMask();
	}
	else
	{
		return 0;
	}
}

VOID CDUIRichEdit::SetEventMask(DWORD dwEventMask)
{
	if(m_pHost != NULL)
	{
		m_pHost->SetEventMask(dwEventMask);
	}
}

CDUIString CDUIRichEdit::GetTextRange(long nStartChar, long nEndChar) const
{
    TEXTRANGEW tr = { 0 };
    tr.chrg.cpMin = nStartChar;
    tr.chrg.cpMax = nEndChar;
    LPWSTR lpText = NULL;
    lpText = new WCHAR[nEndChar - nStartChar + 1];
    ::ZeroMemory(lpText, (nEndChar - nStartChar + 1) * sizeof(WCHAR));
    tr.lpstrText = lpText;
    TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
    CDUIString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

VOID CDUIRichEdit::HideSelection(BOOL bHide)
{
    TxSendMessage(EM_HIDESELECTION, bHide, 0, 0);
}

VOID CDUIRichEdit::ScrollCaret()
{
    TxSendMessage(EM_SCROLLCARET, 0, 0, 0);
}

INT CDUIRichEdit::InsertText(INT nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo)
{
	DUI_ASSERT(lpstrText != NULL);
	if(lpstrText == NULL) return -1;

    int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

INT CDUIRichEdit::AppendText(LPCTSTR lpstrText, BOOL bCanUndo)
{
	DUI_ASSERT(lpstrText != NULL);
	if(lpstrText == NULL) return -1;

    int nRet = SetSel(-1, -1);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

DWORD CDUIRichEdit::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult(0);
    TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

BOOL CDUIRichEdit::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost != NULL)
	{
		m_pHost->SetCharFormat(cf);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

DWORD CDUIRichEdit::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

BOOL CDUIRichEdit::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return FALSE;

    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
    return (BOOL)lResult;
}

BOOL CDUIRichEdit::SetWordCharFormat(CHARFORMAT2 &cf)
{
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return FALSE;

    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION|SCF_WORD, (LPARAM)&cf, &lResult); 
    return (BOOL)lResult;
}

DWORD CDUIRichEdit::GetParaFormat(PARAFORMAT2 &pf) const
{
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    return (DWORD)lResult;
}

BOOL CDUIRichEdit::SetParaFormat(PARAFORMAT2 &pf)
{
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return FALSE;
	m_pHost->SetParaFormat(pf);

    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
	return (BOOL)lResult;
}

BOOL CDUIRichEdit::Redo()
{ 
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return FALSE;

    LRESULT lResult;
    TxSendMessage(EM_REDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

BOOL CDUIRichEdit::Undo()
{ 
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return FALSE;

    LRESULT lResult;
    TxSendMessage(EM_UNDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE; 
}

BOOL CDUIRichEdit::CanUndo() 
{
	LRESULT bRet(0);
	TxSendMessage(EM_CANUNDO, 0, 0, &bRet);
	return bRet;
}

BOOL CDUIRichEdit::CanRedo()
{
	LRESULT bRet(0);
	TxSendMessage(EM_CANREDO, 0, 0, &bRet);
	return bRet;
}

BOOL CDUIRichEdit::CanPaste()
{
	LRESULT bRet(0);
	TxSendMessage(EM_CANPASTE, CF_UNICODETEXT, 0, &bRet);
	return bRet;
}

VOID CDUIRichEdit::Clear()
{ 
    TxSendMessage(WM_CLEAR, 0, 0, 0); 
}

VOID CDUIRichEdit::Copy()
{ 
    TxSendMessage(WM_COPY, 0, 0, 0); 
}

VOID CDUIRichEdit::Cut()
{ 
    TxSendMessage(WM_CUT, 0, 0, 0); 
}

VOID CDUIRichEdit::Paste()
{ 
    TxSendMessage(WM_PASTE, 0, 0, 0); 
}

VOID CDUIRichEdit::SetEnableContextMenu(BOOL bEnable)
{
	m_bEnableContextMenu = bEnable;
}

BOOL CDUIRichEdit::GetEnableContextMenu() const
{
	return m_bEnableContextMenu;
}

INT CDUIRichEdit::GetLineCount() const
{ 
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return FALSE;

    LRESULT lResult(0);
    TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
    return (INT)lResult; 
}

CDUIString CDUIRichEdit::GetLine(int nIndex, int nMaxLength) const
{
    LPWSTR lpText = new WCHAR[nMaxLength + 2];
	if(lpText == NULL) return _T("");

    ::ZeroMemory(lpText, (nMaxLength + 2) * sizeof(WCHAR));
    *(LPWORD)lpText = (WORD)nMaxLength;
    TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpText, 0);
    CDUIString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

INT CDUIRichEdit::LineIndex(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINEINDEX, nLine,
		0, &lResult);
    return (int)lResult;
}

INT CDUIRichEdit::LineLength(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
    return (int)lResult;
}

BOOL CDUIRichEdit::LineScroll(int nLines, int nChars)
{
    LRESULT lResult;
    TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
    return (BOOL)lResult;
}


long CDUIRichEdit::LineFromChar(long nIndex) const
{ 
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return 0;

    LRESULT lResult(0);
    TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
    return (long)lResult;
}

POINT CDUIRichEdit::PosFromChar(UINT nChar) const
{ 
// 	Rich Edit 1.0 and 3.0: The return value is not used. 
// 	Edit controls and Rich Edit 2.0: The return value contains the client area coordinates of the character. The low-order word contains the horizontal coordinate and the high-order word contains the vertical coordinate. 

    POINT pt = {0, 0}; 
	LRESULT lRet(0);
    TxSendMessage(EM_POSFROMCHAR, nChar, (WPARAM)&pt, &lRet);
	pt.x = GET_X_LPARAM(lRet);
	pt.y = GET_Y_LPARAM(lRet);
	return pt;
}

INT CDUIRichEdit::CharFromPos(POINT pt) const
{ 
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return 0;

    LRESULT lResult(0);
    TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&pt, &lResult);
    return (int)lResult; 
}

VOID CDUIRichEdit::EmptyUndoBuffer()
{ 
    TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0); 
}

UINT CDUIRichEdit::SetUndoLimit(UINT nLimit)
{ 
    LRESULT lResult;
    TxSendMessage(EM_SETUNDOLIMIT, (WPARAM) nLimit, 0, &lResult);
    return (UINT)lResult; 
}

long CDUIRichEdit::StreamIn(int nFormat, EDITSTREAM &es)
{ 
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return 0;

    LRESULT lResult(0);
    TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult;
}

long CDUIRichEdit::StreamOut(int nFormat, EDITSTREAM &es)
{ 
	DUI_ASSERT(m_pHost != NULL);
	if(m_pHost == NULL) return 0;

    LRESULT lResult(0);
    TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult; 
}

INT CDUIRichEdit::GetFirstVisibleLine() const
{
	LRESULT lRet(0);
	TxSendMessage(EM_GETFIRSTVISIBLELINE, 0, 0, &lRet);
	return lRet;
}

IDropTarget* CDUIRichEdit::GetTxDropTarget()
{
	return NULL;
}

BOOL CDUIRichEdit::OnTxViewChanged()
{
    return TRUE;
}

VOID CDUIRichEdit::OnTxNotify(DWORD iNotify, void *pv)
{
	SendNotify(WM_NOTIFY_EDIT_TX_NOTIFY, iNotify, (LPARAM)pv);
}

HRESULT CDUIRichEdit::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
	HRESULT hRet(S_FALSE);

	ITextServices* pTxtService = NULL;
	if(m_pHost != NULL)
	{
		pTxtService = m_pHost->GetTextServices();
	}
	if(pTxtService != NULL)
	{
		hRet = pTxtService->TxSendMessage(msg, wparam, lparam, plresult);
	}

    return hRet;
}

VOID CDUIRichEdit::OnContextMenu(const DUIEvent& info)
{
	if(!m_bEnableContextMenu) return;

	if(m_pHost == NULL) return;
	RECT* pRtEdit = m_pHost->GetClientRect();
	POINT ptHit = info.ptMouse;
	if(!PtInRect(pRtEdit, ptHit)) return;

	HMODULE hUser = GetModuleHandle ( _T("user32.dll") );
	HMENU hMenu = LoadMenu ( hUser, MAKEINTRESOURCE(1) );
	HMENU hPop = GetSubMenu ( hMenu, 0 );
	if(hMenu == NULL || hPop == NULL ) return;
	
	BOOL bUndo = CanUndo() && !IsReadOnly();
	EnableMenuItem ( hPop, WM_UNDO, bUndo?MF_ENABLED:MF_GRAYED);
	
	INT nSelStart(0), nSelEnd(0);
	GetSel(nSelStart, nSelEnd);
	BOOL bCopy = ( nSelEnd - nSelStart );
	EnableMenuItem ( hPop, WM_COPY, bCopy?MF_ENABLED:MF_GRAYED );
	
	BOOL bCut = bCopy && !IsReadOnly();
	EnableMenuItem ( hPop, WM_CUT, bCut?MF_ENABLED:MF_GRAYED );
	
	BOOL bPaste = CanPaste();
	EnableMenuItem ( hPop, WM_PASTE, bPaste?MF_ENABLED:MF_GRAYED );
	
	BOOL bDelete = ( nSelEnd - nSelStart ) && !IsReadOnly();
	EnableMenuItem ( hPop, WM_CLEAR, bDelete?MF_ENABLED:MF_GRAYED );
	
	int nTextLen =  GetTextLength();
	BOOL bSelectAll = nTextLen > 0
		&& ( nSelStart != 0 || nSelEnd != nTextLen)
		&& ( nSelStart != nTextLen || nSelEnd != 0 );
	EnableMenuItem ( hPop, EM_SETSEL, bSelectAll?MF_ENABLED:MF_GRAYED );
	
	//  There options of right-click menu of search input field are disable
	DeleteMenu(hPop, 11, MF_BYPOSITION);    // delete menu item : Insert Unicode control character
	DeleteMenu(hPop, 32769, MF_BYCOMMAND);  // delete menu item : Show Unicode control characters
	DeleteMenu(hPop, 32768, MF_BYCOMMAND);  // delete menu item : Right to left Reading order
	DeleteMenu(hPop, 8, MF_BYPOSITION);     // delete the separator below menu item "Selete All"
	
	HWND hWnd = GetManagerWnd(FALSE);
	if(hWnd != NULL)
	{
		::ClientToScreen(hWnd, &ptHit);
		int nCmd = TrackPopupMenu ( hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, ptHit.x, ptHit.y, 0,
			hWnd, NULL );

		switch(nCmd)
		{
        case WM_UNDO:
        case EM_UNDO:
            Undo();
            break;
			
        case WM_CUT:
            Cut();
            break;
			
        case WM_COPY:
            Copy();
            break;
			
        case WM_PASTE:
            Paste();
            break;
			
        case WM_CLEAR:
            Clear();
            break;
			
        case EM_SETSEL: 
            SetSelAll();
            break;
		}
	}

    DestroyMenu ( hMenu );
}


VOID CDUIRichEdit::OnMouseWheel(const DUIEvent& info)
{
	BOOL bVert = TRUE;
	INT nStep = (INT)(SHORT)HIWORD(info.wParam);
	BOOL bForward = (nStep > 0);
	IDUIScrollBar* pSB = GetVerticalSB();
	if(pSB == NULL || !pSB->IsVisible()) 
	{
		bVert = FALSE;
		pSB = GetHorizontalSB();
	}
	if(pSB == NULL || !pSB->IsVisible()) return;
	
	SCROLLINFO si = {0};
	pSB->GetScrollBarInfo(&si);
	if(bForward)
	{
		if(bVert)
		{
			TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
		}
		else
		{
			TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
		}
	}
	else
	{
		if(bVert)
		{
			TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
		}
		else
		{
			TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
		}
	}
}

VOID CDUIRichEdit::OnVertialSBEvent(const DUIEvent& info)
{
	IDUIControl* pControl = info.pSender;
	if(pControl == NULL) 
	{
		DUI_ASSERT(FALSE);
		return;
	}

	IDUIScrollBar* pSB = (IDUIScrollBar*)pControl->GetInterface(IScrollBar);
	if(pSB == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	SCROLLINFO si = {0};
	pSB->GetScrollBarInfo(&si);

	if(info.wParam == SB_LINEUP)
	{
		TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
	}
	else if(info.wParam == SB_LINEDOWN)
	{
		TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);

		SCROLLINFO siNew = {0};
		pSB->GetScrollBarInfo(&siNew);
		if(siNew.nPos <= si.nPos) //fix ms bug
		{
			si.nPos = si.nMax;
			si.fMask = SIF_POS;
		}
	}
	else if(info.wParam == SB_PAGEUP)
	{
		TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
	}
	else if(info.wParam == SB_PAGEDOWN)
	{
		TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);

		SCROLLINFO siNew = {0};
		pSB->GetScrollBarInfo(&siNew);
		if(siNew.nPos <= si.nPos) //fix ms bug
		{
			si.nPos = si.nMax;
			si.fMask = SIF_POS;
		}
	}
	else if(info.wParam == SB_THUMBTRACK)
	{
		TxSendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, info.lParam), 0L, 0);
		si.fMask = SIF_POS;
		si.nPos  = info.lParam;
	}

	if(si.fMask == SIF_POS)
	{
		pSB->SetScrollBarInfo(&si);
	}
}

VOID CDUIRichEdit::OnHorizontalSBEvent(const DUIEvent& info)
{
	IDUIControl* pControl = info.pSender;
	if(pControl == NULL) 
	{
		DUI_ASSERT(FALSE);
		return;
	}

	IDUIScrollBar* pSB = (IDUIScrollBar*)pControl->GetInterface(IScrollBar);
	if(pSB == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	SCROLLINFO si = {0};
	pSB->GetScrollBarInfo(&si);

	if(info.wParam == SB_LINELEFT)
	{
		TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
	}
	else if(info.wParam == SB_LINERIGHT)
	{
		TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
	}
	else if(info.wParam == SB_PAGELEFT)
	{
		TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
	}
	else if(info.wParam == SB_PAGERIGHT)
	{
		TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
	}
	else if(info.wParam == SB_THUMBTRACK)
	{
		TxSendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, info.lParam), 0L, 0);
		si.fMask = SIF_POS;
		si.nPos  = info.lParam;
	}

	if(si.fMask == SIF_POS)
	{
		pSB->SetScrollBarInfo(&si);
	}
}

///////////////////  CDUIRichEdit  /////////////////////

DUI_END_NAMESPCE