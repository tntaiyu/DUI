#include "DUISplit.h"

#define SPLIT_BAR_WIDTH	5
#define SPLIT_ZERO_RATIO 0.0001

#define SPLIT_TRACK_NO_HIT 0
#define SPLIT_TRACK_HIT_MIN 1
#define SPLIT_TRACK_HIT_MAX 2

DUI_BGN_NAMESPCE

CDUISplitBase::CDUISplitBase()
{
	m_dwStyle |= CONTROL_STYLE_SETCURSOR;

	m_nSplitPos = 0;
	m_nMinItem1 = 0;
	m_nMinItem2 = 0;

	m_dbRatio = 0.0;

	m_bTracking = FALSE;
	m_dwTrackStatus = SPLIT_TRACK_NO_HIT;
	m_dwResizeStyle = 0;
}

CDUISplitBase::~CDUISplitBase()
{

}

LPVOID CDUISplitBase::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ISplit) == 0)
	{
		return static_cast<IDUISplit*>(this);
	}

	return theBase::GetInterface(strName);
}

VOID CDUISplitBase::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("split_style")) == 0)
	{
		DWORD dwSplitStyle(0);
		if(strValue.find(_T("size_left")) != -1)
		{
			dwSplitStyle = SPLIT_STYLE_SIZE_LEFT;
		}
		else if(strValue.find(_T("size_right")) != -1)
		{
			dwSplitStyle = SPLIT_STYLE_SIZE_RIGHT;
		}
		else if(strValue.find(_T("size_top")) != -1)
		{
			dwSplitStyle = SPLIT_STYLE_SIZE_TOP;
		}
		else if(strValue.find(_T("size_bottom")) != -1)
		{
			dwSplitStyle = SPLIT_STYLE_SIZE_BOTTOM;
		}
		else if(strValue.find(_T("size_all")) != -1)
		{
			dwSplitStyle = SPLIT_STYLE_SIZE_ALL;
		}
		
		DUI_ASSERT(dwSplitStyle != 0);
		SetResizeStyle(dwSplitStyle);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("split_pos")) == 0)
	{
		INT nPos = _ttol(strValue.c_str());
		DUI_ASSERT(nPos >= 0);
		if(nPos < 0) nPos = 0;

		SetSplitPos(nPos);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("split_min")) == 0)
	{
		TCHAR szValue[128] = {0};
		lstrcpyn(szValue, strValue.c_str(), sizeof(szValue) / sizeof(TCHAR) - 1);
		LPTSTR pstr = szValue;
		INT nItem1 = _tcstol(pstr, &pstr, 10);    
		INT nItem2 = _tcstol(pstr + 1, &pstr, 10); 		

		SetSplitMinSize(nItem1, nItem2);
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUISplitBase::SetSplitPos(INT nPos)
{
	if(nPos < 0) nPos = 0;

	m_nSplitPos = nPos;

	if(IsRectEmpty(&m_rtControl)) return;

	RECT rtAvailable = GetContentRect();
	if(IsRectEmpty(&rtAvailable)) return;

	INT nMaxPos(0);
	if(IsVertical())
	{
		nMaxPos = rtAvailable.bottom - rtAvailable.top - SPLIT_BAR_WIDTH;
	}
	else
	{
		nMaxPos = rtAvailable.right - rtAvailable.left - SPLIT_BAR_WIDTH;
	}
	
	if(m_nSplitPos > nMaxPos) m_nSplitPos = nMaxPos;
	if(m_nSplitPos < 0) m_nSplitPos = 0;

	LayoutSplit(m_nSplitPos);
}

INT CDUISplitBase::GetSplitPos() const
{
	return m_nSplitPos;
}

BOOL CDUISplitBase::GetSplitMinSize(INT& nItem1, INT& nItem2)
{
	nItem1 = m_nMinItem1;
	nItem2 = nItem2;

	return TRUE;
}

VOID CDUISplitBase::SetSplitMinSize(INT nItem1, INT nItem2)
{
	DUI_ASSERT(nItem2 >= 0 && nItem1 >= 0);
	if(nItem1 < 0) nItem1 = 0;
	if(nItem2 < 0) nItem2 = 0;

	m_nMinItem1 = nItem1;
	m_nMinItem2 = nItem2;
}

//when resize control
INT CDUISplitBase::CalcSplitPos(RECT& rtOld, RECT& rtNew)
{
	DWORD dwUserStyle =	GetResizeStyle();
	SIZE sizeOld = {rtOld.right - rtOld.left, rtOld.bottom - rtOld.top};
	SIZE sizeNew = {rtNew.right - rtNew.left, rtNew.bottom - rtNew.top};
	INT nRet(m_nSplitPos);

	if(!IsVertical())
	{
		if(dwUserStyle & SPLIT_STYLE_SIZE_LEFT)
		{
			if(m_nSplitPos < sizeOld.cx)
			{
				nRet += (sizeNew.cx  - sizeOld.cx);
			}
		}
		else if(dwUserStyle & SPLIT_STYLE_SIZE_RIGHT)
		{
			NULL;
		}
		else
		{
			if(m_nSplitPos < sizeOld.cx)
			{
				INT nAvailable = sizeOld.cx - SPLIT_BAR_WIDTH;
				if(nAvailable < 0) nAvailable = 0;
				
				if(nAvailable > 0 && m_dbRatio <= SPLIT_ZERO_RATIO)
				{
					m_dbRatio = m_nSplitPos / (double)nAvailable;
				}
				
				if(m_dbRatio > SPLIT_ZERO_RATIO)
				{
					nRet = (INT)(m_dbRatio * (sizeNew.cx - SPLIT_BAR_WIDTH));
				}
			}
		}

// 		if(m_nMinItem2 != 0)
// 		{
// 			if(sizeNew.cx - nRet - SPLIT_BAR_WIDTH < m_nMinItem2)
// 			{
// 				nRet = sizeNew.cx - nRet - SPLIT_BAR_WIDTH;
// 			}
// 		}
// 
// 		if(m_nMinItem1 != 0)
// 		{
// 			if(nRet < m_nMinItem1)
// 			{
// 				nRet = m_nMinItem1;
// 			}
// 		}
		
		if(nRet > sizeNew.cx - SPLIT_BAR_WIDTH) nRet = sizeNew.cx - SPLIT_BAR_WIDTH;
		if(nRet < 0) nRet = 0;
	}
	else
	{
		if(dwUserStyle & SPLIT_STYLE_SIZE_TOP)
		{
			if(m_nSplitPos < sizeOld.cy)
			{
				nRet += (sizeNew.cy  - sizeOld.cy);
			}
		}
		else if(dwUserStyle & SPLIT_STYLE_SIZE_BOTTOM)
		{
			NULL;
		}
		else
		{
			if(m_nSplitPos < sizeOld.cy)
			{
				INT nAvailable = sizeOld.cy - SPLIT_BAR_WIDTH;
				if(nAvailable < 0) nAvailable = 0;
				
				if(nAvailable > 0 && m_dbRatio <= SPLIT_ZERO_RATIO)
				{
					m_dbRatio = m_nSplitPos / (double)nAvailable;
				}

				if(m_dbRatio > SPLIT_ZERO_RATIO)
				{
					nRet = (INT)(m_dbRatio * (sizeNew.cy - SPLIT_BAR_WIDTH));
				}
			}
		}

// 		if(m_nMinItem2 != 0 )
// 		{
// 			if(sizeNew.cy - nRet - SPLIT_BAR_WIDTH < m_nMinItem2)
// 			{
// 				nRet = sizeNew.cy - nRet - SPLIT_BAR_WIDTH;
// 			}
// 		}
// 
// 		if(m_nMinItem1 != 0 )
// 		{
// 			if(nRet < m_nMinItem1)
// 			{
// 				nRet = m_nMinItem1;
// 			}
// 		}

		if(nRet > sizeNew.cy - SPLIT_BAR_WIDTH) nRet = sizeNew.cy - SPLIT_BAR_WIDTH;
		if(nRet < 0) nRet = 0;		
	}

	return nRet;
}

VOID CDUISplitBase::LayoutSplit(INT nPos)
{
	INT nItemCount = GetControlCount();
	DUI_ASSERT(nItemCount == 2);
	if(nItemCount != 2) return;
	
	m_nSplitPos = nPos;

	RECT rtAvailable = GetContentRect();

	IDUIControl* pItem1 = GetControlByIndex(0);
	IDUIControl* pItem2 = GetControlByIndex(1);
	DUI_ASSERT(pItem2 != NULL && pItem1 != NULL);

	RECT rtItem1 = rtAvailable;
	RECT rtItem2 = rtAvailable;
	if(!IsVertical())
	{
		SetRect(&rtItem1, rtAvailable.left, rtAvailable.top, rtAvailable.left + nPos, rtAvailable.bottom );
		SetRect(&rtItem2, rtAvailable.left + nPos + SPLIT_BAR_WIDTH, rtAvailable.top, rtAvailable.right, rtAvailable.bottom );
	}
	else
	{
		SetRect(&rtItem1, rtAvailable.left, rtAvailable.top, rtAvailable.right, rtAvailable.top + nPos );
		SetRect(&rtItem2, rtAvailable.left, rtAvailable.top + nPos + SPLIT_BAR_WIDTH, rtAvailable.right, rtAvailable.bottom );
	}
	
	pItem1->SetControlRect(rtItem1);
	pItem2->SetControlRect(rtItem2);

	UpdateLayout(FALSE);
}

VOID CDUISplitBase::SetControlRect(const RECT& rt)
{
	RECT rtOld = m_rtControl;
	m_rtControl = rt;

	INT nPos = CalcSplitPos(rtOld, GetContentRect());
	
	LayoutSplit(nPos);
}

LRESULT CDUISplitBase::ProcessEvent(const DUIEvent& event, BOOL& bHandled)
{
	LRESULT lRet(S_OK);
	switch(event.dwEventType)
	{
	case DUI_EVENT_SETCURSOR:
		{
			RECT rtSplitter = {0};
			GetSplitterRect(rtSplitter);
			
			if(::PtInRect(&rtSplitter, event.ptMouse))
			{
				if(IsVertical())
				{
					::SetCursor(LoadCursor(NULL, IDC_SIZENS));
				}
				else
				{
					::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				}
			}
		}

		bHandled = TRUE;
		break;
	
	case DUI_EVENT_LBUTTONDOWN:
		{
			RECT rtSplitter = {0};
			GetSplitterRect(rtSplitter);
			
			if(::PtInRect(&rtSplitter, event.ptMouse))
			{
				lRet = OnLButtonDown(event);
			}
		}
		bHandled = TRUE;
		break;

	case DUI_EVENT_LBUTTONUP:
		lRet = OnLButtonUp(event);
		bHandled = TRUE;
		break;

	case DUI_EVENT_MOUSEMOVE:
		lRet = OnMouseMove(event);
		bHandled = TRUE;
		break;

	default:
		break;
	}


	if(!bHandled)
	{
		lRet = theBase::ProcessEvent(event, bHandled);
	}

	return lRet;
}

LRESULT CDUISplitBase::OnLButtonDown(const DUIEvent& info)
{
	m_dwTrackStatus = SPLIT_TRACK_NO_HIT;
	m_bTracking = TRUE;

	return S_OK;
}

LRESULT CDUISplitBase::OnLButtonUp(const DUIEvent& info)
{
	m_dwTrackStatus = SPLIT_TRACK_NO_HIT;
	m_bTracking = FALSE;

	return S_OK;
}

VOID CDUISplitBase::GetSplitterRect(RECT& rtSplitter)
{
	if(IsVertical())
	{
		rtSplitter.left = m_rtControl.left + m_rtMargin.left;
		rtSplitter.right = m_rtControl.right - m_rtMargin.right;
		rtSplitter.top = m_rtControl.top + m_nSplitPos + m_rtMargin.top;
		rtSplitter.bottom = rtSplitter.top + SPLIT_BAR_WIDTH;
	}
	else
	{
		rtSplitter.left = m_rtControl.left + m_nSplitPos + m_rtMargin.left;
		rtSplitter.right = rtSplitter.left + SPLIT_BAR_WIDTH;
		rtSplitter.top = m_rtControl.top + m_rtMargin.top;
		rtSplitter.bottom = m_rtControl.bottom - m_rtMargin.bottom;
	}
}

LRESULT CDUISplitBase::OnMouseMove(const DUIEvent& info)
{
	if(!m_bTracking) return S_OK;

	RECT rtAvailable = GetContentRect();

	if(!IsVertical())
	{
		INT nOffset = info.ptMouse.x - rtAvailable.left - m_nSplitPos;

		if(nOffset > 0 && m_dwTrackStatus != SPLIT_TRACK_HIT_MAX)
		{
			INT nTempSplit = nOffset + m_nSplitPos;
		
			if(rtAvailable.right - rtAvailable.left - nTempSplit - SPLIT_BAR_WIDTH > m_nMinItem2)
			{
				SetSplitPos(nTempSplit);
				m_dwTrackStatus = SPLIT_TRACK_NO_HIT;

				SaveCurrentRadtio();
			}
			else
			{
				nTempSplit = rtAvailable.right - rtAvailable.left - m_nMinItem2 - SPLIT_BAR_WIDTH;
				SetSplitPos( nTempSplit );
				m_dwTrackStatus = SPLIT_TRACK_HIT_MAX;

				SaveCurrentRadtio();
			}
		}
		else if(nOffset < 0 && m_dwTrackStatus != SPLIT_TRACK_HIT_MIN)
		{
			INT nTempSplit = nOffset + m_nSplitPos;

			if(nTempSplit > m_nMinItem1)
			{
				SetSplitPos(nTempSplit);
				m_dwTrackStatus = SPLIT_TRACK_NO_HIT;

				SaveCurrentRadtio();
			}
			else
			{
				nTempSplit = m_nMinItem1;
				SetSplitPos(nTempSplit);
				m_dwTrackStatus = SPLIT_TRACK_HIT_MIN;

				SaveCurrentRadtio();
			}
		}
	}
	else
	{
		INT nOffset = info.ptMouse.y - rtAvailable.top - m_nSplitPos;

		if(nOffset > 0 && m_dwTrackStatus != SPLIT_TRACK_HIT_MAX)
		{
			INT nTempSplit = nOffset + m_nSplitPos;
		
			if(rtAvailable.bottom  - rtAvailable.top - nTempSplit - SPLIT_BAR_WIDTH > m_nMinItem2)
			{
				SetSplitPos(nTempSplit);
				m_dwTrackStatus = SPLIT_TRACK_NO_HIT;

				SaveCurrentRadtio();
			}
			else
			{
				nTempSplit = rtAvailable.bottom - rtAvailable.top - m_nMinItem2 - SPLIT_BAR_WIDTH;
				SetSplitPos( nTempSplit );
				m_dwTrackStatus = SPLIT_TRACK_HIT_MAX;

				SaveCurrentRadtio();
			}
		}
		else if(nOffset < 0 && m_dwTrackStatus != SPLIT_TRACK_HIT_MIN)
		{
			INT nTempSplit = nOffset + m_nSplitPos;

			if(nTempSplit > m_nMinItem1)
			{
				SetSplitPos(nTempSplit);
				m_dwTrackStatus = SPLIT_TRACK_NO_HIT;

				SaveCurrentRadtio();
			}
			else
			{
				nTempSplit = m_nMinItem1;
				SetSplitPos(nTempSplit);
				m_dwTrackStatus = SPLIT_TRACK_HIT_MIN;

				SaveCurrentRadtio();
			}
		}
	}

	return S_OK;
}

VOID CDUISplitBase::SaveCurrentRadtio()
{
	if(!(GetResizeStyle() & SPLIT_STYLE_SIZE_ALL)) return;

	RECT rtAvailable = GetContentRect();
	DUI_ASSERT(!IsRectEmpty(&rtAvailable));
	
	if(IsVertical())
	{
		m_dbRatio = m_nSplitPos / (double)(rtAvailable.bottom - rtAvailable.top);
	}
	else
	{
		m_dbRatio = m_nSplitPos / (double)(rtAvailable.right - rtAvailable.left);
	}
}

VOID CDUISplitBase::SetResizeStyle(DWORD dwStyle)
{
	m_dwResizeStyle = dwStyle;
}

DWORD CDUISplitBase::GetResizeStyle() const
{
	return m_dwResizeStyle;
}


CRefPtr<IDUIControl> CDUIVerticalSplit::Clone()
{
	return new CDUIVerticalSplit(*this);
}


CRefPtr<IDUIControl> CDUIHorizontalSplit::Clone()
{
	return new CDUIHorizontalSplit(*this);
}

DUI_END_NAMESPCE