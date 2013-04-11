#include "DUIScrollbar.h"
#include "DUIButton.h"
#include "DUIRenderEngine.h"

#define MIN_THUM_WIDTH				16
#define SB_SCROLL_LINE_INTERVAL		50
#define SB_SCROLL_PAGE_INTERVAL		100


//scrollbar element type
#define SB_ARROW_BUTTON1		1
#define SB_ARROW_BUTTON2		2
#define SB_THUM_BUTTON			3


#define SB_NO_HIT  0
#define SB_HIT_MIN 1
#define SB_HIT_MAX 2


DUI_BGN_NAMESPCE


///////////  CDUISBArrowBtn  /////////////////////////////
class CDUISBArrowBtn: public CDUIButton
{
public:
	typedef CDUIButton theBase;

	CDUISBArrowBtn(INT nType);
	VOID SetSink(CDUIScrollBarBase* pSink);

	virtual CRefPtr<IDUIControl> Clone();
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual BOOL Activate() { return FALSE;}

protected:
	VOID StartMessageLoop();
	VOID FireScrollEvent();

protected:
	INT m_nType;
	CDUIScrollBarBase* m_pSink;
	BOOL m_bLoopMessage;
};

CDUISBArrowBtn::CDUISBArrowBtn(INT nType)
{ 
	m_bLoopMessage = FALSE;
	m_pSink = NULL;
	m_nType = nType; 
	m_dwStyle &= ~(CONTROL_STYLE_TABSTOP);
}

VOID CDUISBArrowBtn::SetSink(CDUIScrollBarBase* pSink)
{
	m_pSink = pSink; 
}

CRefPtr<IDUIControl> CDUISBArrowBtn::Clone()
{ 
	return new CDUISBArrowBtn(*this);
}

VOID CDUISBArrowBtn::StartMessageLoop()
{
	MSG msg = {0};
	m_bLoopMessage = TRUE;
	DWORD dwStartTick = GetTickCount();
	DWORD dwCurrentTick(0);

	while(TRUE)
	{
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				PostQuitMessage(0);
				break;
			}
		}
		
		dwCurrentTick = GetTickCount();
		if((dwCurrentTick - dwStartTick > SB_SCROLL_LINE_INTERVAL)
			|| !m_bLoopMessage)
		{
			FireScrollEvent();
			dwStartTick = dwCurrentTick;		
		}

		Sleep(1);

		if(!m_bLoopMessage)
		{
			break;
		}
	}

}

VOID CDUISBArrowBtn::FireScrollEvent()
{
	if(!(GetStatus() & CONTROL_STATUS_HOVER)) return;

	INT nRequest(-1);
	switch(m_nType)
	{
	case SB_ARROW_BUTTON1:
		nRequest = SB_LINEUP;
		break;

	case SB_ARROW_BUTTON2:
		nRequest = SB_LINEDOWN;
		break;

	default:
		DUI_ASSERT(FALSE);
	}

	if(nRequest != -1 && m_pSink != NULL)
	{
		if(m_pSink->IsVerticalSB())
		{
			m_pSink->OnVScrollEvent(nRequest, 0);
		}
		else
		{
			m_pSink->OnHScrollEvent(nRequest, 0);
		}
	}
}

LRESULT CDUISBArrowBtn::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet = theBase::ProcessEvent(info, bHandled);

	switch(info.dwEventType)
	{
		case DUI_EVENT_LBUTTONDOWN:
 		case DUI_EVENT_LBUTTONDBCLK:
			if(IsEnabled())
			{
				StartMessageLoop();

				if(m_pSink != NULL)
				{
					IDUIControl* pOwner = m_pSink->GetScrollOwner();
					if(pOwner != NULL 
						&& (pOwner->GetStyle() & CONTROL_STYLE_TABSTOP))
					{
						pOwner->SetFocus(TRUE);
					}
				}
			}
			break;
		
		case DUI_EVENT_LBUTTONUP:
			if(IsEnabled())
				m_bLoopMessage = FALSE;
			break;

		default:
			break;
	}

	return lRet;
}
///////////  CDUISBArrowBtn  /////////////////////////////




///////////  CDUISBThumButton  /////////////////////////////
class CDUISBThumButton: public CDUIButton
{
public:
	typedef CDUIButton theBase;

	CDUISBThumButton();
	VOID SetSink(CDUIScrollBarBase* pSink);

	virtual CRefPtr<IDUIControl> Clone();
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual BOOL Activate() { return FALSE;}

protected:
	VOID StartTrackThum(POINT ptCursor);
	VOID OnMouseMove(const POINT& ptCursor);

protected:
	CDUIScrollBarBase* m_pSink;

	INT m_nOffset;
	INT m_nThumStatus;
};

CDUISBThumButton::CDUISBThumButton()
{ 
	m_pSink = NULL;
	m_dwStyle &= ~(CONTROL_STYLE_TABSTOP);
	m_nThumStatus = SB_NO_HIT;
}

VOID CDUISBThumButton::SetSink(CDUIScrollBarBase* pSink)
{ 
	m_pSink = pSink; 
}

CRefPtr<IDUIControl> CDUISBThumButton::Clone()
{ 
	return new CDUISBThumButton(*this); 
}

LRESULT CDUISBThumButton::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet = theBase::ProcessEvent(info, bHandled);

	switch(info.dwEventType)
	{
	case DUI_EVENT_LBUTTONDOWN:
		m_nThumStatus = SB_NO_HIT;
		if(IsEnabled())
		{
			StartTrackThum(info.ptMouse);

			if(m_pSink != NULL)
			{
				IDUIControl* pOwner = m_pSink->GetScrollOwner();
				if(pOwner != NULL 
					&& (pOwner->GetStyle() & CONTROL_STYLE_TABSTOP))
				{
					pOwner->SetFocus(TRUE);
				}
			}
		}
		break;

	case DUI_EVENT_MOUSEMOVE:
		if(IsEnabled())
			OnMouseMove(info.ptMouse);
		break;
	
	case DUI_EVENT_LBUTTONUP:
		m_nThumStatus = SB_NO_HIT;
		break;

	default:
		break;
	}
	
	return lRet;
}

VOID CDUISBThumButton::StartTrackThum(POINT ptCursor)
{
	if(!(GetStatus() & CONTROL_STATUS_PRESSED)
		|| m_pSink == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	if(m_pSink->IsVerticalSB())
	{
		m_nOffset = ptCursor.y - m_rtControl.top;
	}
	else
	{
		m_nOffset = ptCursor.x - m_rtControl.left;
	}
}

VOID CDUISBThumButton::OnMouseMove(const POINT& ptCursor)
{
	if(!(GetStatus() & CONTROL_STATUS_PRESSED)
		|| m_pSink == NULL)
	{
		return;
	}

	if(m_pSink->IsVerticalSB())
	{
		INT nMove = ptCursor.y - m_rtControl.top - m_nOffset;

		if(m_nThumStatus == SB_NO_HIT)
		{
			m_nThumStatus = m_pSink->OnVScrollEvent(SB_THUMBTRACK, nMove);
		}
		else if(m_nThumStatus == SB_HIT_MIN)
		{
			if(nMove > 0)
			{
				m_nThumStatus = m_pSink->OnVScrollEvent(SB_THUMBTRACK, nMove);
			}
		}
		else if(m_nThumStatus == SB_HIT_MAX)
		{
			if(nMove < 0)
			{
				m_nThumStatus = m_pSink->OnVScrollEvent(SB_THUMBTRACK, nMove);
			}
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
	}
	else
	{
		INT nMove = ptCursor.x - m_rtControl.left - m_nOffset;
		
		if(m_nThumStatus == SB_NO_HIT)
		{
			m_nThumStatus = m_pSink->OnHScrollEvent(SB_THUMBTRACK, nMove);
		}
		else if(m_nThumStatus == SB_HIT_MIN)
		{
			if(nMove > 0)
			{
				m_nThumStatus = m_pSink->OnHScrollEvent(SB_THUMBTRACK, nMove);
			}
		}
		else if(m_nThumStatus == SB_HIT_MAX)
		{
			if(nMove < 0)
			{
				m_nThumStatus = m_pSink->OnHScrollEvent(SB_THUMBTRACK, nMove);
			}
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
	}	
}
///////////  CDUISBThumButton  /////////////////////////////



//////////////  CDUIScrollBarBase  ////////////////////////
CDUIScrollBarBase::CDUIScrollBarBase()
{
	m_fRatio = 1.0;

	m_nMin = 0;
	m_nPage = 0;
	m_nMax = 0;
	m_nPos = 0;
	m_pNotifier = NULL;

	m_sizeArrow1.cx = m_sizeArrow1.cy = 0;
	m_sizeArrow2.cx = m_sizeArrow2.cy = 0;

	m_bMsgLoop = FALSE;
	m_bScrollPage1 = TRUE;
	m_ptCursor.x = m_ptCursor.y = 0;

	SetRectEmpty(&m_rtScrollPage1);
	SetRectEmpty(&m_rtScrollPage2);

	CreateCtrls();
}

CDUIScrollBarBase::~CDUIScrollBarBase()
{
	
}

BOOL CDUIScrollBarBase::CreateCtrls()
{
	AddControl(new CDUISBArrowBtn(SB_ARROW_BUTTON1));
	AddControl(new CDUISBThumButton);
	AddControl(new CDUISBArrowBtn(SB_ARROW_BUTTON2));

	return TRUE;
}

BOOL CDUIScrollBarBase::SetMsgSink(BOOL bSet)
{
	IDUIButton* pButton1 = NULL;
	IDUIButton* pButton2 = NULL;
	IDUIButton* pButtonThum = NULL;
	
	pButton1 = GetElement(SB_ARROW_BUTTON1);
	pButton2 = GetElement(SB_ARROW_BUTTON2);
	pButtonThum = GetElement(SB_THUM_BUTTON);
	
	if(pButton1 == NULL 
		|| pButton2 == NULL
		|| pButtonThum == NULL)
	{
		return FALSE;
	}
	
	CDUIScrollBarBase* pSink = NULL;
	if(bSet) pSink = this;

	((CDUISBArrowBtn*)pButton1)->SetSink(pSink);
	((CDUISBArrowBtn*)pButton2)->SetSink(pSink);
	((CDUISBThumButton*)pButtonThum)->SetSink(pSink);

	return TRUE;
}

VOID CDUIScrollBarBase::OnCreate()
{
	SetMsgSink(TRUE);
}

VOID CDUIScrollBarBase::OnDestroy()
{
	SetMsgSink(FALSE);
}

LRESULT CDUIScrollBarBase::OnVScrollEvent(WPARAM wParam, LPARAM lParam)
{
	DUI_ASSERT(IsVerticalSB());

	LRESULT lRet(SB_NO_HIT);
	IDUIControl* pOwner = GetScrollOwner();
	if(pOwner == NULL) return lRet;

	BOOL bSend(FALSE);
	SCROLLINFO si = {0};
	GetScrollBarInfo(&si);

	if(wParam == SB_LINEUP || wParam == SB_PAGEUP)
	{
		if(si.nPos > si.nMin)
		{
			bSend = TRUE;
		}
	}
	else if(wParam == SB_LINEDOWN || wParam == SB_PAGEDOWN)
	{
		if(si.nPos < si.nMax)
		{
			bSend = TRUE;
		}
	}
	else if(wParam == SB_THUMBTRACK)
	{
		INT nOldPos = si.nPos;
 		si.nPos += (INT)(lParam / m_fRatio);
		if(si.nPos >= si.nMax)
		{
			si.nPos = si.nMax;
			lRet = SB_HIT_MAX;
		}
		else if(si.nPos <= si.nMin)
		{
			si.nPos = si.nMin;
			lRet = SB_HIT_MIN;
		}
		
		if(si.nPos != nOldPos)
		{
			bSend = TRUE;
			lParam = si.nPos;
		}
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	if(bSend)
	{
		BOOL bHandled(FALSE);

		DUIEvent info = {0};
		info.pSender = this;
		info.wParam = wParam;
		info.lParam = lParam;
		info.dwEventType = DUI_EVENT_VSCROLL;

		pOwner->OnDUIEvent(info, bHandled);
	}

	return lRet;
}

LRESULT CDUIScrollBarBase::OnHScrollEvent(WPARAM wParam, LPARAM lParam)
{
	DUI_ASSERT(!IsVerticalSB());

	LRESULT lRet(SB_NO_HIT);
	IDUIControl* pOwner = GetScrollOwner();
	if(pOwner == NULL) return lRet;

	BOOL bSend(FALSE);
	SCROLLINFO si = {0};
	GetScrollBarInfo(&si);

	if(wParam == SB_LINELEFT || wParam == SB_PAGELEFT)
	{
		if(si.nPos > si.nMin)
		{
			bSend = TRUE;
		}
	}
	else if(wParam == SB_LINERIGHT || wParam == SB_PAGERIGHT)
	{
		if(si.nPos < si.nMax)
		{
			bSend = TRUE;
		}
	}
	else if(wParam == SB_THUMBTRACK)
	{
		INT nOldPos = si.nPos;
 		si.nPos += (INT)(lParam / m_fRatio);
		if(si.nPos >= si.nMax)
		{
			si.nPos = si.nMax;
			lRet = SB_HIT_MAX;
		}
		else if(si.nPos <= si.nMin)
		{
			si.nPos = si.nMin;
			lRet = SB_HIT_MIN;
		}
		
		if(si.nPos != nOldPos)
		{
			bSend = TRUE;
			lParam = si.nPos;
		}
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	if(bSend)
	{
		BOOL bHandled(FALSE);

		DUIEvent info = {0};
		info.pSender = this;
		info.wParam = wParam;
		info.lParam = lParam;
		info.dwEventType = DUI_EVENT_HSCROLL;

		pOwner->OnDUIEvent(info, bHandled);
	}

	return lRet;
}

LRESULT CDUIScrollBarBase::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet = theBase::ProcessEvent(info, bHandled);

	switch(info.dwEventType)
	{
		case DUI_EVENT_LBUTTONDOWN:
 		case DUI_EVENT_LBUTTONDBCLK:
			if(IsEnabled())
			{
				m_ptCursor = info.ptMouse;
				StartMessageLoop();

				IDUIControl* pOwner = GetScrollOwner();
				if( pOwner != NULL 
					&& (pOwner->GetStyle() & CONTROL_STYLE_TABSTOP))
				{
					pOwner->SetFocus(TRUE);
				}
			}

			break;

		case DUI_EVENT_MOUSEMOVE:
			if(IsEnabled())
				m_ptCursor = info.ptMouse;
			break;

		case DUI_EVENT_LBUTTONUP:
			if(IsEnabled())
			{
				m_ptCursor = info.ptMouse;
				m_bMsgLoop = FALSE;
			}
			break;

		default:
			break;
	}

	return lRet;
}

VOID CDUIScrollBarBase::StartMessageLoop()
{
	if(!IsEnabled()) return;

	if(::PtInRect(&m_rtScrollPage1, m_ptCursor))
	{
		m_bScrollPage1 = TRUE;
		m_bMsgLoop = TRUE;
	}
	else if(::PtInRect(&m_rtScrollPage2, m_ptCursor))
	{
		m_bScrollPage1 = FALSE;
		m_bMsgLoop = TRUE;
	}
	else
	{
		m_bMsgLoop = FALSE;
	}

	if(!m_bMsgLoop) return;
	
	DWORD dwStartTick = GetTickCount();
	DWORD dwCurrentTick(0);
	MSG msg = {0};

	while(TRUE)
	{
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				PostQuitMessage(0);
				break;
			}
		}
		
		dwCurrentTick = GetTickCount();
		if((dwCurrentTick - dwStartTick > SB_SCROLL_PAGE_INTERVAL)
			|| !m_bMsgLoop)
		{
			if(m_bScrollPage1)
			{
				if(PtInRect(&m_rtScrollPage1, m_ptCursor))
				{
					if(IsVerticalSB())
					{
						OnVScrollEvent(SB_PAGEUP, 0);
					}
					else
					{
						OnHScrollEvent(SB_PAGELEFT, 0);
					}
				}
			}
			else
			{
				if(PtInRect(&m_rtScrollPage2, m_ptCursor))
				{
					if(IsVerticalSB())
					{
						OnVScrollEvent(SB_PAGEDOWN, 0);
					}
					else
					{
						OnHScrollEvent(SB_PAGERIGHT, 0);
					}
				}
			}

			dwStartTick = dwCurrentTick;		
		}

		Sleep(1);

		if(!m_bMsgLoop)
		{
			break;
		}
	}	
}


LPVOID CDUIScrollBarBase::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IScrollBar) == 0)
	{
		return static_cast<IDUIScrollBar*>(this);
	}

	return theBase::GetInterface(strName);
}

VOID CDUIScrollBarBase::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	UpdateThumUI();
}

IDUIButton* CDUIScrollBarBase::GetElement(INT nType)
{
	DUI_ASSERT(GetControlCount() == 3);
	if(GetControlCount() != 3) return NULL;

	IDUIButton* pRet = NULL;
	switch(nType)
	{
	case SB_ARROW_BUTTON1:
		pRet = (IDUIButton*)GetControlByIndex(0)->GetInterface(IButton);
		break;

	case SB_THUM_BUTTON:
		pRet = (IDUIButton*)GetControlByIndex(1)->GetInterface(IButton);
		break;

	case SB_ARROW_BUTTON2:
		pRet = (IDUIButton*)GetControlByIndex(2)->GetInterface(IButton);
		break;

	default:
		DUI_ASSERT(FALSE);
	}

	return pRet;
}

VOID CDUIScrollBarBase::UpdateCtrlsAttributes()
{
	IDUIButton* pButton1 = GetElement(SB_ARROW_BUTTON1);
	IDUIButton* pButton2 = GetElement(SB_ARROW_BUTTON2);
	IDUIButton* pButtonThum = GetElement(SB_THUM_BUTTON);

	if(pButton1 == NULL 
		|| pButton2 == NULL
		|| pButtonThum == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}
	
	const CButtonUIData& BtnData1 = pButton1->GetButtonUIData();
	CRefPtr<CImageList> pImageBK = BtnData1.m_imageBK;
	if(!pImageBK.IsNull() && !pImageBK->IsNull())
	{
		SIZE sizeIcon = { pImageBK->GetIconWidth(), pImageBK->GetIconHeight() };
		pButton1->SetFixedWidth(sizeIcon.cx);
		pButton1->SetFixedHeight(sizeIcon.cy);
		m_sizeArrow1 = sizeIcon;
		
		if(IsVerticalSB())
		{
			INT nWidth = GetFixedSize().cx;
			SetFixedWidth(max(nWidth, sizeIcon.cx));
		}
		else
		{
			INT nHeight = GetFixedSize().cy;
			SetFixedHeight(max(nHeight, sizeIcon.cy));
		}
	}

	const CButtonUIData& BtnData2 = pButton2->GetButtonUIData();
	pImageBK = BtnData2.m_imageBK;
	if(!pImageBK.IsNull() && !pImageBK->IsNull())
	{
		SIZE sizeIcon = { pImageBK->GetIconWidth(), pImageBK->GetIconHeight() };
		pButton2->SetFixedWidth(sizeIcon.cx);
		pButton2->SetFixedHeight(sizeIcon.cy);
		m_sizeArrow2 = sizeIcon;
		
		if(IsVerticalSB())
		{
			INT nWidth = GetFixedSize().cx;
			SetFixedWidth(max(nWidth, sizeIcon.cx));
		}
		else
		{
			INT nHeight = GetFixedSize().cy;
			SetFixedHeight(max(nHeight, sizeIcon.cy));
		}
	}
}


VOID CDUIScrollBarBase::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("arrow_image1")) == 0)
	{
		IDUIButton* pButton = GetElement(SB_ARROW_BUTTON1);
		if(pButton != NULL)
		{
			pButton->SetAttribute(_T("back_image"), strValue);
		}

		UpdateCtrlsAttributes();
		bHandled = TRUE;
	}
	else if(strName.compare(_T("arrow_image2")) == 0)
	{
		IDUIButton* pButton = GetElement(SB_ARROW_BUTTON2);
		if(pButton != NULL)
		{
			pButton->SetAttribute(_T("back_image"), strValue);
		}
		
		UpdateCtrlsAttributes();
		bHandled =TRUE;
	}
	else if(strName.compare(_T("thumb_image")) == 0)
	{
		IDUIButton* pButton = GetElement(SB_THUM_BUTTON);
		if(pButton != NULL)
		{
			pButton->SetAttribute(_T("back_image"), strValue);
		}
		
		UpdateCtrlsAttributes();
		bHandled = TRUE;
	}
	else if(strName.compare(_T("notifier")) == 0)
	{
		SetScrollOwner(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("back_color")) == 0)
	{
		m_clrBK = GetSkinMgr()->GetColor(strValue);
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUIScrollBarBase::PaintBkgnd(HDC dc)
{
	if(!m_clrBK.IsEmpty())
	{
		CDUIRenderEngine::DrawColor(dc, m_rtControl, m_clrBK);
	}
}

VOID CDUIScrollBarBase::SetScrollOwner(const CDUIString& strOwnerName)
{
	m_strNotifierName  = strOwnerName;
}

VOID CDUIScrollBarBase::SetScrollOwner(IDUIControl* pOwner)
{
	m_pNotifier = pOwner;
}

IDUIControl* CDUIScrollBarBase::GetScrollOwner()
{
	if(m_pNotifier == NULL && m_strNotifierName.size() > 0)
	{
		m_pNotifier = GetSiblingControl(m_strNotifierName);
	}
	
	DUI_ASSERT(m_pNotifier != NULL);

	return m_pNotifier;
}

VOID CDUIScrollBarBase::ModifyStatus(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwOldStatus = GetStatus();
	theBase::ModifyStatus(dwRemove, dwAdd);
	if((dwOldStatus^m_dwStatus) & CONTROL_STATUS_DISABLE)
	{
		EnableScrollBar(IsEnabled());	
	}
}

VOID CDUIScrollBarBase::EnableScrollBar(BOOL bEnable)
{
	IDUIButton* pButton1 = GetElement(SB_ARROW_BUTTON1);
	IDUIButton* pButton2 = GetElement(SB_ARROW_BUTTON2);
	IDUIButton* pButtonThum = GetElement(SB_THUM_BUTTON);
	if(pButton1 == NULL 
		|| pButton2 == NULL
		|| pButtonThum == NULL)
	{
		return;
	}

	if(bEnable)
	{
		pButton1->ModifyStatus(CONTROL_STATUS_DISABLE, 0);
		pButton2->ModifyStatus(CONTROL_STATUS_DISABLE, 0);
		pButtonThum->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
	}
	else
	{
		pButton1->ModifyStatus(0, CONTROL_STATUS_DISABLE);
		pButton2->ModifyStatus(0, CONTROL_STATUS_DISABLE);
		pButtonThum->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
	}

	UpdateLayout(TRUE);
}

VOID CDUIScrollBarBase::UpdateThumUI()
{
	if(m_nMin > m_nMax) m_nMax = m_nMin;
	if(m_nPos < m_nMin) m_nPos = m_nMin;
	if(m_nPos > m_nMax) m_nPos = m_nMax;

	IDUIButton* pButton1 = GetElement(SB_ARROW_BUTTON1);
	IDUIButton* pButton2 = GetElement(SB_ARROW_BUTTON2);
	IDUIButton* pButtonThum = GetElement(SB_THUM_BUTTON);
	if(pButton1 == NULL 
		|| pButton2 == NULL
		|| pButtonThum == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	RECT rtTotal = GetControlRect();

	if(IsVerticalSB())
	{
		RECT rtArrowBtn1 = rtTotal;
		rtArrowBtn1.bottom = rtArrowBtn1.top + m_sizeArrow1.cy;

		RECT rtArrowBtn2 = rtTotal;
		rtArrowBtn2.top = rtArrowBtn2.bottom - m_sizeArrow2.cy;
		if(rtArrowBtn2.top < rtArrowBtn1.bottom) rtArrowBtn2.top = rtArrowBtn2.bottom;
		
		RECT rtBack = rtTotal;
		rtBack.top = rtArrowBtn1.bottom;
		rtBack.bottom = rtArrowBtn2.top;
		if(rtBack.top > rtBack.bottom) rtBack.top = rtBack.bottom;
		INT nBackLen = rtBack.bottom - rtBack.top;
		
		pButton1->SetControlRect(rtArrowBtn1);
		pButton2->SetControlRect(rtArrowBtn2);

		INT nThumLen(0);
		RECT rtThum = rtBack;
		if(m_nPage > 0 
			&& m_nMax > m_nMin 
			&& nBackLen > 0)
		{
			nThumLen = nBackLen * m_nPage / (m_nMax - m_nMin + m_nPage);
			if(nThumLen < MIN_THUM_WIDTH) nThumLen = MIN_THUM_WIDTH;
			if(nThumLen > nBackLen) nThumLen = 0;
			
			m_fRatio = (nBackLen - nThumLen) / (double)(m_nMax - m_nMin);
			rtThum.top = rtBack.top +  (INT)(m_fRatio * (m_nPos - m_nMin)) ;
		}

		rtThum.bottom = rtThum.top + nThumLen;
		if(rtThum.bottom > rtArrowBtn2.top) rtThum.bottom = rtArrowBtn2.top;

		m_rtScrollPage1 = m_rtScrollPage2 = rtBack;
		if(rtThum.bottom - rtThum.top > 0)
		{
			m_rtScrollPage1.bottom = rtThum.top;
			m_rtScrollPage2.top = rtThum.bottom;
		}
		else
		{
			m_rtScrollPage1.bottom = m_rtScrollPage1.top;
			m_rtScrollPage2.top = m_rtScrollPage2.bottom;
		}

		pButtonThum->SetControlRect(rtThum);
	}
	else
	{
		RECT rtArrowBtn1 = rtTotal;
		rtArrowBtn1.right = rtArrowBtn1.left + m_sizeArrow1.cx;

		RECT rtArrowBtn2 = rtTotal;
		rtArrowBtn2.left = rtArrowBtn2.right - m_sizeArrow2.cx;
		if(rtArrowBtn2.left < rtArrowBtn1.right) rtArrowBtn2.left = rtArrowBtn2.right;
		
		RECT rtBack = rtTotal;
		rtBack.left = rtArrowBtn1.right;
		rtBack.right = rtArrowBtn2.left;
		if(rtBack.left > rtBack.right) rtBack.left = rtBack.right;
		INT nBackLen = rtBack.right - rtBack.left;
		
		pButton1->SetControlRect(rtArrowBtn1);
		pButton2->SetControlRect(rtArrowBtn2);

		INT nThumLen(0);
		RECT rtThum = rtBack;
		if(m_nPage > 0 
			&& m_nMax > m_nMin 
			&& nBackLen > 0)
		{
			nThumLen = nBackLen * m_nPage / (m_nMax - m_nMin + m_nPage);
			if(nThumLen < MIN_THUM_WIDTH) nThumLen = MIN_THUM_WIDTH;
			if(nThumLen > nBackLen) nThumLen = 0;
			
			m_fRatio = (nBackLen - nThumLen) / (double)(m_nMax - m_nMin);
			rtThum.left = rtBack.left +  (INT)(m_fRatio * (m_nPos - m_nMin));
		}

		rtThum.right = rtThum.left + nThumLen;
		if(rtThum.right > rtArrowBtn2.left) rtThum.right = rtArrowBtn2.left;

		m_rtScrollPage1 = m_rtScrollPage2 = rtBack;
		if(rtThum.right - rtThum.left > 0)
		{
			m_rtScrollPage1.right = rtThum.left;
			m_rtScrollPage2.left = rtThum.right;
		}
		else
		{
			m_rtScrollPage1.right = m_rtScrollPage1.left;
			m_rtScrollPage2.left = m_rtScrollPage2.right;
		}

		pButtonThum->SetControlRect(rtThum);
	}


	UpdateLayout(FALSE);
}


BOOL CDUIScrollBarBase::SetScrollBarInfo(LPSCROLLINFO pSi)
{
 	DUI_ASSERT(pSi != NULL);
 
 	UINT uMask = pSi->fMask;
 
 	if(uMask & SIF_PAGE)
 	{
 		m_nPage = pSi->nPage;
	}
 
 	if(uMask & SIF_RANGE)
 	{
 		m_nMin = pSi->nMin;
 		m_nMax = pSi->nMax;
 	}

 	if(uMask & SIF_POS)
 	{
 		m_nPos = pSi->nPos;
 	}
 	
	UpdateThumUI();

 	return TRUE;
}

VOID CDUIScrollBarBase::GetScrollBarInfo(LPSCROLLINFO pSi)
{
	DUI_ASSERT(pSi != NULL);

	pSi->nMin = m_nMin;
	pSi->nMax = m_nMax;
	pSi->nPos = m_nPos;
	pSi->nPage = m_nPage;
}
//////////////  CDUIScrollBarBase  ///////////////


//////////////  CDUIVerticalSB  ///////////////
CRefPtr<IDUIControl> CDUIVerticalSB::Clone()
{
	return new CDUIVerticalSB(*this);
}
//////////////  CDUIVerticalSB  ///////////////



//////////////  CDUIHorizontalSB  ///////////////
CRefPtr<IDUIControl> CDUIHorizontalSB::Clone()
{
	return new CDUIHorizontalSB(*this);
}
//////////////  CDUIHorizontalSB  ///////////////


DUI_END_NAMESPCE
