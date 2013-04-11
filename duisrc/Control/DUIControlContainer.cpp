#include "DUIControlContainer.h"

DUI_BGN_NAMESPCE

////////////////////////  CDUIHorizontalLayout  //////////////////
CDUIHorizontalLayout::CDUIHorizontalLayout()
{
	m_bEnableScroll = FALSE;
}

CDUIHorizontalLayout::CDUIHorizontalLayout(const CDUIHorizontalLayout& obj)
:CDUIControlBase(obj)
{
	if(!obj.m_pHorSB.IsNull())
	{
		CRefPtr<IDUIControl> pControl = obj.m_pHorSB->Clone();
		m_pHorSB = (IDUIScrollBar*)pControl->GetInterface(IScrollBar);
	}

	m_bEnableScroll = obj.m_bEnableScroll;
}

CRefPtr<IDUIControl> CDUIHorizontalLayout::Clone()
{
	return new CDUIHorizontalLayout(*this);
}


IDUIScrollBar* CDUIHorizontalLayout::GetHorizontalSB()
{
	return m_pHorSB.GetPtr();
}

VOID CDUIHorizontalLayout::OnCreate()
{
	if(!GetEnableScroll()) return;

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
		m_pHorSB->AttachWindow(m_pControlMgr);
		m_pHorSB->SetScrollOwner(this);
	}
}

VOID CDUIHorizontalLayout::OnDestroy()
{
	if(!m_pHorSB.IsNull())
	{
		m_pHorSB->DetachWindow();
		m_pHorSB->SetScrollOwner(NULL);
	}

	m_pHorSB = NULL;
}

VOID CDUIHorizontalLayout::SetEnableScroll(BOOL bEnable)
{
	m_bEnableScroll = bEnable;
}

BOOL CDUIHorizontalLayout::GetEnableScroll() const
{
	return m_bEnableScroll;
}

VOID CDUIHorizontalLayout::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{	
	BOOL bHandled(FALSE);
	if(strName.compare(_T("enable_scroll")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetEnableScroll(TRUE);
		}
		else
		{
			SetEnableScroll(FALSE);
		}
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUIHorizontalLayout::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	INT nCount = m_arChildControl.size();
	if(nCount <= 0) return;

	RECT rtUsable = GetContentRect();

	INT nScrollPos(0);
	IDUIScrollBar* pSB = GetHorizontalSB();
	if(pSB != NULL && (pSB->IsVisible()))
	{
		SCROLLINFO si = {0};
		pSB->GetScrollBarInfo(&si);
		nScrollPos = si.nPos;

		RECT rtScrollBar = rtUsable;
		INT nThumbWidth = pSB->GetFixedSize().cy;
		rtUsable.bottom -= nThumbWidth;
		
		rtScrollBar.top = rtScrollBar.bottom - nThumbWidth;
		pSB->SetControlRect(rtScrollBar);
	}

	if(::IsRectEmpty(&rtUsable))
	{
		for(int i=0; i<nCount; ++i)
		{
			m_arChildControl[i]->SetControlRect(rtUsable);
		}
		return;
	}

	//calculate size changeable item count
	SIZE szAvailable = { rtUsable.right - rtUsable.left, rtUsable.bottom - rtUsable.top };
	INT nEmptyCount(0);
	SIZE szRemain = szAvailable;
	int i(0);
	INT nVisibleCount(0);
	for(i=0; i<nCount; ++i)
	{
		CRefPtr<IDUIControl> pControl = m_arChildControl[i];
		DUI_ASSERT(!pControl.IsNull());
		
		BOOL bVisible = (pControl->IsVisible());
		if(!bVisible) continue;

		SIZE szControl = {0, 0};
		pControl->EstimateSize(szRemain, szControl);
		if(szControl.cx == 0)
		{
			nEmptyCount++;
		}
		else
		{
			szRemain.cx -= szControl.cx;
		}
		szRemain.cx -= m_nPaddingWidth;
		++nVisibleCount;
	}

	if(nVisibleCount > 1) //remove last padding
	{
		szRemain.cx += m_nPaddingWidth;
	}

	int nAvarage(0); //average size for each size changeable item
	if(szRemain.cx > 0 && nEmptyCount > 0)
	{
		nAvarage = szRemain.cx / nEmptyCount;
	}
	
	//position every item
	szRemain = szAvailable;
	int nPosX = rtUsable.left - nScrollPos;

	INT nNeeded = 0;
	CRefPtr<IDUIControl> pLastControl = NULL;
	for(i = 0; i<nCount; ++i)
	{
		CRefPtr<IDUIControl> pControl = m_arChildControl[i];
		DUI_ASSERT(!pControl.IsNull());

		BOOL bVisible = (pControl->IsVisible());
		if(!bVisible) continue;
		
		SIZE szControl = {0};
		pControl->EstimateSize(szRemain, szControl);
		nNeeded += (szControl.cx + m_nPaddingWidth);
		if(szControl.cx == 0)
		{
			nNeeded += nAvarage;
			szControl.cx = nAvarage;
		}
		
		RECT rtControl = { nPosX, rtUsable.top, nPosX + szControl.cx, rtUsable.bottom };
		pControl->SetControlRect(rtControl);
		pLastControl = pControl;

		nPosX += (szControl.cx + m_nPaddingWidth);
		szRemain.cx -= (szControl.cx + m_nPaddingWidth);

	}

	if(!pLastControl.IsNull() && (szRemain.cx + m_nPaddingWidth > 0))
	{
		RECT rtTemp = pLastControl->GetControlRect();
		rtTemp.right += (szRemain.cx + m_nPaddingWidth);
		pLastControl->SetControlRect(rtTemp);
	}

	if(nVisibleCount > 1)
	{
		nNeeded -= m_nPaddingWidth;
	}
	
	ProcessHorizontalSB(rtUsable, nNeeded);
	
	UpdateLayout(FALSE);
}
////////////////////////  CDUIHorizontalLayout  //////////////////



////////////////////  CDUIVerticalLayout  /////////////////////
CDUIVerticalLayout::CDUIVerticalLayout()
{
	m_bEnableScroll = FALSE;
}

CDUIVerticalLayout::CDUIVerticalLayout(const CDUIVerticalLayout& obj)
:CDUIControlBase(obj)
{
	if(!obj.m_pVertSB.IsNull())
	{
		CRefPtr<IDUIControl> pControl = obj.m_pVertSB->Clone();
		m_pVertSB = (IDUIScrollBar*)pControl->GetInterface(IScrollBar);
	}

	m_bEnableScroll = obj.m_bEnableScroll;
}

CRefPtr<IDUIControl> CDUIVerticalLayout::Clone()
{
	return new CDUIVerticalLayout(*this);
}

IDUIScrollBar* CDUIVerticalLayout::GetVerticalSB()
{
	return m_pVertSB.GetPtr();	
}

VOID CDUIVerticalLayout::SetEnableScroll(BOOL bEnable)
{
	m_bEnableScroll = bEnable;
}

BOOL CDUIVerticalLayout::GetEnableScroll() const
{
	return m_bEnableScroll;
}

VOID CDUIVerticalLayout::OnCreate()
{
	if(!GetEnableScroll()) return;

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
		m_pVertSB->AttachWindow(m_pControlMgr);
		m_pVertSB->SetScrollOwner(this);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

VOID CDUIVerticalLayout::OnDestroy()
{
	if(!m_pVertSB.IsNull())
	{
		m_pVertSB->DetachWindow();
		m_pVertSB->SetScrollOwner(NULL);
	}

	m_pVertSB = NULL;
}

VOID CDUIVerticalLayout::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{	
	BOOL bHandled(FALSE);
	if(strName.compare(_T("enable_scroll")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetEnableScroll(TRUE);
		}
		else
		{
			SetEnableScroll(FALSE);
		}
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}
VOID CDUIVerticalLayout::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	INT nCount = m_arChildControl.size();
	if(nCount <= 0) return;
	

	RECT rtUsable = GetContentRect();
	INT nScrollPos(0);
	IDUIScrollBar* pSB = GetVerticalSB();
	if(pSB != NULL && (pSB->IsVisible()))
	{
		SCROLLINFO si = {0};
		pSB->GetScrollBarInfo(&si);
		nScrollPos = si.nPos;

		RECT rtScrollBar = rtUsable;
		INT nThumbWidth = pSB->GetFixedSize().cx;
		rtUsable.right -= nThumbWidth;
		
		rtScrollBar.left = rtScrollBar.right - nThumbWidth;
		pSB->SetControlRect(rtScrollBar);
	}

	if(::IsRectEmpty(&rtUsable))
	{
		for(int i=0; i<nCount; ++i)
		{
			m_arChildControl[i]->SetControlRect(rtUsable);
		}

		return;
	}

	//calculate size changeable item count
	SIZE szAvailable = { rtUsable.right - rtUsable.left, rtUsable.bottom - rtUsable.top };

	INT nEmptyCount(0);
	SIZE szRemain = szAvailable;
	int i(0);
	INT nVisibleCount(0);
	
	for(i=0; i<nCount; ++i)
	{
		CRefPtr<IDUIControl> pControl = m_arChildControl[i];
		DUI_ASSERT(!pControl.IsNull());
		
		BOOL bVisible = (pControl->IsVisible());
		if(!bVisible) continue;

		SIZE szControl = {0, 0};
		pControl->EstimateSize(szRemain, szControl);
		if(szControl.cy == 0)
		{
			nEmptyCount++;
		}
		else
		{
			szRemain.cy -= szControl.cy;
		}
		szRemain.cy -= m_nPaddingWidth;
		++nVisibleCount;
	}

	if(nVisibleCount > 1) //remove last padding
	{
		szRemain.cy += m_nPaddingWidth;
	}

	int nAvarage(0); //average size for each size changeable item
	if(szRemain.cy > 0 && nEmptyCount > 0)
	{
		nAvarage = szRemain.cy / nEmptyCount;
	}
	

	//position every item
	szRemain = szAvailable;
	int nPosY = rtUsable.top - nScrollPos;

	INT nNeeded(0);
	CRefPtr<IDUIControl> pLastControl = NULL;
	for(i = 0; i<nCount; ++i)
	{
		CRefPtr<IDUIControl> pControl = m_arChildControl[i];
		DUI_ASSERT(!pControl.IsNull());

		BOOL bVisible = (pControl->IsVisible());
		if(!bVisible) continue;

		SIZE szControl = {0};
		pControl->EstimateSize(szRemain, szControl);

		nNeeded += (szControl.cy + m_nPaddingWidth);
		if(szControl.cy == 0)
		{
			szControl.cy = nAvarage;
			nNeeded += (szControl.cy);
		}

		RECT rtControl = {rtUsable.left, nPosY, rtUsable.right, nPosY +  szControl.cy};
		pControl->SetControlRect(rtControl);
		pLastControl = pControl;
		

		nPosY += (szControl.cy + m_nPaddingWidth);
		szRemain.cy -= (szControl.cy + m_nPaddingWidth);
	}

	if(!pLastControl.IsNull() && szRemain.cy + m_nPaddingWidth > 0)
	{
		RECT rtTemp = pLastControl->GetControlRect();
		rtTemp.bottom += (szRemain.cy + m_nPaddingWidth);
		pLastControl->SetControlRect(rtTemp);
	}
	
	if(nVisibleCount > 1)
	{
		nNeeded -= m_nPaddingWidth;
	}

	ProcessVerticalSB(rtUsable, nNeeded);
	
	UpdateLayout(FALSE);
}
////////////////////  CDUIVerticalLayout  /////////////////////




//////////////  CDUITileLayout  ////////////////////
CDUITileLayout::CDUITileLayout()
{
	m_nColumns = 0;
}

CDUITileLayout::CDUITileLayout(const CDUITileLayout& obj)
:IDUITileLayout(obj)
{
	m_nColumns = obj.m_nColumns;
	if(!obj.m_pVertSB.IsNull())
	{
		CRefPtr<IDUIControl> pControl = obj.m_pVertSB->Clone();
		m_pVertSB = (IDUIScrollBar*)pControl->GetInterface(IScrollBar);
	}
}

CRefPtr<IDUIControl> CDUITileLayout::Clone()
{
	return new CDUITileLayout(*this);
}

VOID CDUITileLayout::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	if(strName.compare(_T("column_count")) == 0)
	{
		SetColumnCount(_ttol(strValue.c_str()));
		return;
	}

	theBase::SetAttribute(strName, strValue);
}

LPVOID CDUITileLayout::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ITileLayout) == 0)
	{
		return static_cast<IDUITileLayout*>(this);
	}

	return theBase::GetInterface(strName);
}

VOID CDUITileLayout::SetColumnCount(INT nCount)
{
	DUI_ASSERT(nCount >= 0);

	m_nColumns = max(0, nCount);
}

INT CDUITileLayout::GetColumnCount() const
{
	return m_nColumns;
}

INT CDUITileLayout::CalculateColumnCount(RECT& rtUsable)
{
	INT nCount = GetControlCount();
	INT nWidth(0);
	SIZE sizeAvailable = { rtUsable.right - rtUsable.left, rtUsable.bottom - rtUsable.top};
	//use max element width as average element width

	INT i(0);
	for(i=0; i<nCount; ++i)
	{
		IDUIControl* pControl = GetControlByIndex(i);
		DUI_ASSERT(pControl != NULL);
		if(!(pControl->IsVisible())) continue;

		SIZE sizeRet = {0};
		if(pControl->EstimateSize(sizeAvailable, sizeRet))
		{
			nWidth = max(nWidth, sizeRet.cx);
		}
	}

	INT nSum(0);
	BOOL bFirst(TRUE);
	INT nRet(0);
	for(i=0; i<nCount; ++i)
	{
		IDUIControl* pControl = GetControlByIndex(i);
		DUI_ASSERT(pControl != NULL);
		if(!(pControl->IsVisible())) continue;

		if(bFirst)
		{
			nSum += nWidth;
			bFirst = FALSE;
		}
		else
		{
			nSum += (nWidth + m_nPaddingWidth);
		}

		++nRet;
		if(nSum >= sizeAvailable.cx)
		{
			break;
		}
	}

	return nRet;
}

IDUIScrollBar* CDUITileLayout::GetVerticalSB()
{
	return m_pVertSB.GetPtr();	
}

VOID CDUITileLayout::OnCreate()
{
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
		m_pVertSB->AttachWindow(m_pControlMgr);
		m_pVertSB->SetScrollOwner(this);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

VOID CDUITileLayout::OnDestroy()
{
	if(!m_pVertSB.IsNull())
	{
		m_pVertSB->DetachWindow();
		m_pVertSB->SetScrollOwner(NULL);
	}

	m_pVertSB = NULL;
}


VOID CDUITileLayout::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	INT nCount = m_arChildControl.size();
	if(nCount <= 0) return;
	
	RECT rtUsable = GetContentRect();

	INT nScrollPos(0); //layout scrollbar
	IDUIScrollBar* pSB = GetVerticalSB();
	if(pSB != NULL && (pSB->IsVisible()))
	{
		SCROLLINFO si = {0};
		pSB->GetScrollBarInfo(&si);
		nScrollPos = si.nPos;

		RECT rtScrollBar = rtUsable;
		INT nThumbWidth = pSB->GetFixedSize().cx;
		rtUsable.right -= nThumbWidth;

		rtScrollBar.left = rtScrollBar.right - nThumbWidth;
		pSB->SetControlRect(rtScrollBar);
	}


	if(::IsRectEmpty(&rtUsable))
	{
		for(int i=0; i<nCount; ++i)
		{
			m_arChildControl[i]->SetControlRect(rtUsable);
		}

		return;
	}
	
	INT nColumnCount = m_nColumns;
	if(nColumnCount == 0)
	{
		nColumnCount = CalculateColumnCount(rtUsable);
	}
	if(nColumnCount == 0) return;

	SIZE szAvailable = { rtUsable.right - rtUsable.left, rtUsable.bottom - rtUsable.top };
	INT nWidth = szAvailable.cx / nColumnCount; //width for each child control
	if(nColumnCount > 1)
	{
		INT nTotalChildWidth =  szAvailable.cx - (nColumnCount - 1) * m_nPaddingWidth ;
		if(nTotalChildWidth < 0) nTotalChildWidth = 0;
		nWidth = nTotalChildWidth / nColumnCount;
	}

	INT nHeight(0);
	INT nIndex(0);

	INT nNeededHeight(0);
	POINT ptTile = { rtUsable.left, rtUsable.top};
	ptTile.y -= nScrollPos;

	INT nStartY(ptTile.y);
	
	for(int it1 = 0; it1<nCount; ++it1)
	{
		CRefPtr<IDUIControl> pControl = m_arChildControl[it1];
		DUI_ASSERT(!pControl.IsNull());
		if(!(pControl->IsVisible())) continue;

		RECT rtTile = {ptTile.x, ptTile.y, ptTile.x + nWidth, ptTile.y};

		if(m_sizeFixed.cy == 0)
		{
			if(nHeight == 0) //calculate max element height on the row
			{
				INT nTempIndex(it1);
				INT i(0);
				while(nTempIndex < nCount )
				{
					CRefPtr<IDUIControl> pTempControl = m_arChildControl[nTempIndex];
					DUI_ASSERT(!pTempControl.IsNull());
					if(!(pTempControl->IsVisible())) continue;

					SIZE sizeAvTemp = { rtTile.right - rtTile.left, 0x7FFF};
					SIZE sizeRetTemp = {0};
					if(pTempControl->EstimateSize(sizeAvTemp, sizeRetTemp))
					{
						nHeight = max(nHeight, sizeRetTemp.cy);
					}
					
					++nTempIndex;
					if(++i % nColumnCount == 0) break;
				}
			}
		}
		else //use predefined, note: the size here is not the container size, it is for its element
		{
			nHeight  = m_sizeFixed.cy;
		}

		rtTile.bottom = rtTile.top + nHeight;
		pControl->SetControlRect(rtTile);

		if(nColumnCount == 1 
			|| (++nIndex % nColumnCount == 0))
		{
			ptTile.x = rtUsable.left;
			ptTile.y += (nHeight + m_nPaddingWidth);
			nHeight = 0;
		}
		else
		{
			ptTile.x += (nWidth + m_nPaddingWidth);
		}

		nNeededHeight = rtTile.bottom - nStartY;
	}
	
	ProcessVerticalSB(rtUsable, nNeededHeight);
}
//////////////  CDUITileLayout  ////////////////////



//////////////////////////  CDUIDialogLayout  //////////////////////////////
CDUIDialogLayout::CDUIDialogLayout()
{
	m_bFirstLayout = TRUE;
	ZeroMemory(&m_sizeDialog, sizeof(m_sizeDialog));
}

CDUIDialogLayout::~CDUIDialogLayout()
{
	ClearData();
}

VOID CDUIDialogLayout::ClearData()
{
	INT nCount = m_arElements.size();
	for(int i=0; i<nCount; ++i)
	{
		delete m_arElements[i];
	}

	m_arElements.clear();
}


CRefPtr<IDUIControl> CDUIDialogLayout::Clone()
{
	return new CDUIDialogLayout(*this);
}

BOOL CDUIDialogLayout::EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet)
{
	CalculateDlgSize();
	
	sizeRet = m_sizeDialog;

	return TRUE;
}

VOID CDUIDialogLayout::SetControlRect(const RECT& rt)
{
	CalculateDlgSize();
	m_rtControl = rt;

	RECT rtUsable = m_rtControl;
	INT nPosX = rtUsable.left;
	INT nPosY = rtUsable.top;
	int nDiffX = rtUsable.right - rtUsable.left - m_sizeDialog.cx;
	int nDiffY = rtUsable.bottom - rtUsable.top - m_sizeDialog.cy;

	if(nDiffX < 0) nDiffX = 0;
	if(nDiffY < 0) nDiffY = 0;
	
	//for scrollbar
	IDUIScrollBar* pVerSB = GetVerticalSB();
	if(pVerSB != NULL && (pVerSB->IsVisible()))
	{
		SCROLLINFO si = {0};
		pVerSB->GetScrollBarInfo(&si);
		nPosY -= si.nPos;
	}
	
	IDUIScrollBar* pHorSB = GetHorizontalSB();
	if(pHorSB != NULL && (pHorSB->IsVisible()))
	{
		SCROLLINFO si = {0};
		pHorSB->GetScrollBarInfo(&si);
		nPosX -= si.nPos;
	}
	//
	
	INT nSizeCount(0), nStretchX(0), nStretchY(0), nMoveX(0), nMoveY(0);
	INT nCount = m_arElements.size();
	for(int i=0; i<nCount; ++i)
	{
		CStretchElement* pEle = m_arElements[i];
		DUI_ASSERT(pEle != NULL);
	
		if(!(pEle->m_pControl->IsVisible())) continue;

		if((pEle->m_dwMode & CONTROL_EX_STYLE_GROUP)
			||(i == 0))
		{
			nSizeCount = 0;
			for(int j=i+1; j<nCount; ++j)
			{
				CStretchElement* pNext = m_arElements[j];

				if(!(pNext->m_pControl->IsVisible())) continue;
				if(pNext->m_dwMode & CONTROL_EX_STYLE_GROUP) break;
				if(pNext->m_dwMode & (CONTROL_EX_STYLE_SIZE_X | CONTROL_EX_STYLE_SIZE_Y)) nSizeCount++;
			}

			if(nSizeCount == 0) nSizeCount = 1;
			nStretchX = nDiffX / nSizeCount;
			nStretchY = nDiffY / nSizeCount;
			nMoveX = nMoveY = 0;
		}

		RECT rtControl = pEle->m_rtPos;
	//	::OffsetRect(&rtControl, rtUsable.left, rtUsable.top);
		::OffsetRect(&rtControl, nPosX, nPosY);

		if(pEle->m_dwMode & CONTROL_EX_STYLE_MOVE_X) ::OffsetRect(&rtControl, nMoveX, 0);
		if(pEle->m_dwMode & CONTROL_EX_STYLE_MOVE_Y) ::OffsetRect(&rtControl, 0, nMoveY);
		if(pEle->m_dwMode & CONTROL_EX_STYLE_SIZE_X)
		{
			rtControl.right += nStretchX;
			nMoveX += nStretchX;
		}
		if(pEle->m_dwMode & CONTROL_EX_STYLE_SIZE_Y)
		{
			rtControl.bottom += nStretchY;
			nMoveY += nStretchY;
		}

		if(rtControl.right > rtUsable.right) rtControl.right = rtUsable.right;
		if(rtControl.bottom > rtUsable.bottom) rtControl.bottom = rtUsable.bottom;

		pEle->m_pControl->SetControlRect(rtControl);
	}
	
	ProcessHorizontalSB(rt, m_sizeDialog.cx);
	ProcessVerticalSB(rt, m_sizeDialog.cy);

	UpdateLayout(FALSE);
}

//get the dialog template size
//and cache the dialog layout data
VOID CDUIDialogLayout::CalculateDlgSize()
{
	if( !m_bFirstLayout ) return;
	m_bFirstLayout = FALSE;

	IDUIControl* pControl = NULL;
	CStretchElement* pEle = NULL; 
	DWORD dwMode = 0;
	RECT rtDialog = {0};

	int nCount = m_arChildControl.size();
	for(int i=0; i<nCount; ++i)
	{
		pControl = m_arChildControl[i].GetPtr();
		DUI_ASSERT(pControl != NULL);
		RECT rtControl = pControl->GetControlRect();

		pEle = new CStretchElement;
		if(pEle != NULL)
		{
			pEle->m_pControl = pControl;
			pEle->m_rtPos = rtControl;
			dwMode = (pControl->GetExStyle() & CONTROL_EX_STYLE_STRETCH_MASK);
			if(dwMode == 0)
			{
				DUI_ASSERT(FALSE);
				dwMode = CONTROL_EX_STYLE_GROUP;
			}
			pEle->m_dwMode = dwMode;

			m_arElements.push_back(pEle);
		}

		if(rtControl.left < rtDialog.left)		rtDialog.left = rtControl.left;
		if(rtControl.right > rtDialog.right)	rtDialog.right = rtControl.right;
		if(rtControl.top < rtDialog.top)		rtDialog.top = rtControl.top;
		if(rtControl.bottom > rtDialog.bottom)	rtDialog.bottom = rtControl.bottom;
	}	
	
	m_sizeDialog.cx = rtDialog.right - rtDialog.left;
	m_sizeDialog.cy = rtDialog.bottom - rtDialog.top;
}
//////////////////////////  CDUIDialogLayout  //////////////////////////////




///////////////////////////  CDUIDockLayout  ////////////////////////////////
CDUIDockLayout::CDUIDockLayout()
{
	m_bLastFill = FALSE;
}

VOID CDUIDockLayout::DockControl(CRefPtr<IDUIControl> pControl, RECT& rtAvailable, BOOL bLast)
{
	DWORD dwExStyle = pControl->GetExStyle();
	SIZE sizeControl = {0};
	SIZE sizAvailable = { rtAvailable.right - rtAvailable.left, rtAvailable.bottom - rtAvailable.top };
	pControl->EstimateSize(sizAvailable, sizeControl);
	RECT rtNewControl = rtAvailable;
	RECT rtNewAvailable = rtAvailable;

	if(bLast && m_bLastFill)
	{
		pControl->SetControlRect(rtNewControl);
		rtAvailable.left = rtAvailable.right;
		return;
	}

	if(dwExStyle & CONTROL_EX_STYLE_DOCK_LEFT)
	{
		DUI_ASSERT(sizeControl.cx > 0);	
		rtNewControl.right =  rtNewControl.left + sizeControl.cx;
		if(rtNewControl.right > rtAvailable.right)
		{
			rtNewControl.right = rtAvailable.right;
			rtNewAvailable.left = rtNewAvailable.right;
		}
		else
		{
			rtNewAvailable.left = rtNewControl.right;
		}
	}
	else if(dwExStyle & CONTROL_EX_STYLE_DOCK_RIGHT)
	{
		DUI_ASSERT(sizeControl.cx > 0);	
		rtNewControl.left = rtNewControl.right - sizeControl.cx;
		if(rtNewControl.left < rtAvailable.left)
		{
			rtNewControl.left = rtAvailable.left;
			rtNewAvailable.right = rtNewAvailable.left;
		}
		else
		{
			rtNewAvailable.right = rtNewControl.left;
		}
	}
	else if(dwExStyle & CONTROL_EX_STYLE_DOCK_TOP)
	{
		DUI_ASSERT(sizeControl.cy > 0);	
		rtNewControl.bottom = rtNewControl.top + sizeControl.cy;
		if(rtNewControl.bottom > rtAvailable.bottom)
		{
			rtNewControl.bottom = rtAvailable.bottom;
			rtNewAvailable.top = rtNewAvailable.bottom;
		}
		else
		{
			rtNewAvailable.top = rtNewControl.bottom;
		}
	}
	else if(dwExStyle & CONTROL_EX_STYLE_DOCK_BOTTOM)
	{
		DUI_ASSERT(sizeControl.cy > 0);	
		rtNewControl.top = rtNewControl.bottom - sizeControl.cy;
		if(rtNewControl.top < rtAvailable.top)
		{
			rtNewControl.top = rtAvailable.top;
			rtNewAvailable.bottom = rtNewAvailable.top;
		}
		else
		{
			rtNewAvailable.bottom = rtNewControl.top;
		}
	}
	else
	{
		rtNewControl.right = rtNewControl.left;
		rtNewControl.bottom = rtNewControl.top;
		DUI_ASSERT(FALSE);
	}

	pControl->SetControlRect(rtNewControl);
	rtAvailable = rtNewAvailable;
}

VOID CDUIDockLayout::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	RECT rtAvailable = GetContentRect();

	INT nCount = m_arChildControl.size();
	for(INT i=0; i<nCount; ++i)
	{
		CRefPtr<IDUIControl> pControl = m_arChildControl[i];
		DUI_ASSERT(!pControl.IsNull());

		BOOL bLast = (i == nCount - 1);
		DockControl(pControl, rtAvailable, bLast);
	}
}

VOID CDUIDockLayout::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	if(strName.compare(_T("fill_last")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			m_bLastFill = TRUE;
		}
		else
		{
			m_bLastFill = FALSE;
		}

		return;
	}

	theBase::SetAttribute(strName, strValue);
}

CRefPtr<IDUIControl> CDUIDockLayout::Clone()
{
	return new CDUIDockLayout(*this);
}

LPVOID CDUIDockLayout::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IDockLayout) == 0)
	{
		return (IDUIDockLayout*)this;
	}

	return theBase::GetInterface(strName);
}

//IDUIDockLayout
VOID CDUIDockLayout::SetLastFill(BOOL bSet)
{
	m_bLastFill = bSet;
}

BOOL CDUIDockLayout::GetLastFill() const
{
	return m_bLastFill;
}
///////////////////////////  CDUIDockLayout  ////////////////////////////////



///////////////////////////  CDUITabLayout  ////////////////////////////////
CDUITabLayout::CDUITabLayout()
{
	m_nSelectIndex = 0;
}

VOID CDUITabLayout::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);

	if(strName.compare(_T("select_index")) == 0)
	{
		m_nSelectIndex = _ttol(strValue.c_str());
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

CRefPtr<IDUIControl> CDUITabLayout::Clone()
{
	return new CDUITabLayout(*this);
}

VOID CDUITabLayout::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	RECT rtContent = GetContentRect();
	
	INT nCount = GetControlCount();
	for(INT i=0; i<nCount; ++i)
	{
		IDUIControl* pControl = GetControlByIndex(i);
		DUI_ASSERT(pControl != NULL);
		pControl->SetControlRect(rtContent);
	}

	//UpdateControls();
}

LPVOID CDUITabLayout::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ITabLayout) == 0)
	{
		return (IDUITabLayout*)this;
	}
	
	return theBase::GetInterface(strName);
}

VOID CDUITabLayout::OnCreate()
{
	UpdateControls();
}

VOID CDUITabLayout::CheckAndResetFocus(IDUIControl* pControl)
{
	IDUIControl* pFocusControl = GetFocusControl();
	if(pFocusControl == NULL) return;

	IDUIControl* pParentControl = pFocusControl;
	while (pParentControl != NULL
		&& pParentControl != pControl)
	{
		pParentControl = pParentControl->GetParent();
	}
	
	if(pParentControl != NULL)
	{
		pControl->SetFocus(FALSE);
	}
}

VOID CDUITabLayout::UpdateControls()
{
	INT nCount = GetControlCount();
	if(m_nSelectIndex < nCount 
		&& m_nSelectIndex >= 0)
	{
		LockLayout(TRUE);
		for(INT i=0; i<nCount; ++i)
		{
			IDUIControl* pControl = GetControlByIndex(i);
			DUI_ASSERT(pControl != NULL);
			
			if(i == m_nSelectIndex)
			{
				pControl->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
			}
			else
			{
				pControl->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
				CheckAndResetFocus(pControl);
			}
		}
		LockLayout(FALSE);

		UpdateLayout(FALSE);
	}
}


BOOL CDUITabLayout::SetSelectIndex(INT nIndex)
{
	if(m_nSelectIndex != nIndex)
	{
		INT nCount = GetControlCount();
		if(nIndex >= 0 && nIndex < nCount)
		{
			m_nSelectIndex = nIndex;
			UpdateControls();
			return TRUE;
		}
	}

	return FALSE;
}

INT CDUITabLayout::GetSelectIndex() const
{
	return m_nSelectIndex;
}
///////////////////////////  CDUITabLayout  ////////////////////////////////

DUI_END_NAMESPCE