#include "DUITreeView.h"
#include "DUIRenderEngine.h"



DUI_BGN_NAMESPCE


void	CDUITreeNode::SetItemHeight(INT nHeight)
{
	m_nItemHeight = nHeight;
}

/*virtual */
INT		CDUITreeNode::GetItemHeight() 
{
	return m_nItemHeight;
}

CDUITreeNode::CDUITreeNode(void)
{
	m_bExpand = FALSE;

	m_nItemHeight = 30;

	SetFixedHeight(30);

	m_bSelected = FALSE;
}

CDUITreeNode::~CDUITreeNode(void)
{
}


VOID CDUITreeNode::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	if ( HaveChild() && IsExpand() && IsVisible() )
	{
		
	}
}

/*	virtual */
VOID CDUITreeNode::DoPaint(HDC dc, const RECT& rtDraw)
{
	//DUI_ASSERT(dc && rtDraw.bottom-rtDraw.top >0 && rtDraw.right - rtDraw.left > 0 );

	 theBase::DoPaint(dc,rtDraw);return;

	CDUIRenderClip clip(dc, GetContentRect());

	theBase::PaintBkgnd(dc);

	INT nLevel = GetLevel();

	TCHAR szIndent[32] = {0};

	int i=0;
	for ( i=1;i<nLevel;i++)
	{
		lstrcat(szIndent,_T("     "));
	}

	for (  i=0;i<nLevel;i++)
	{
		lstrcat(szIndent,_T("+"));
	}



	TCHAR szText[128] = {0};
	wsprintf(szText,_T("%s-%x"),szIndent,this);

	RECT rcIndent = rtDraw;
	
	CDUIRenderEngine::DrawText(dc,szText,lstrlen(szText),&rcIndent,DT_SINGLELINE|DT_LEFT);
	
}



//include itself
INT			CDUITreeNode::GetTotalHeight()
{
	if ( !IsVisible() )
		return 0;

	int nTotalHeight = GetFixedSize().cy;

	if (  IsExpand() && HaveChild()  )
	{
		INT nCount = m_arChildControl.size();
		for ( int i=0;i<nCount;i++)
		{
			HDUITREEITEM	hChild = (HDUITREEITEM)m_arChildControl[i]->GetInterface(ITreeViewItemName);
			if ( hChild )
				nTotalHeight += hChild->GetTotalHeight();
		}
	}

	return 	 nTotalHeight;
}

// HDUITREEITEM		CDUITreeNode::GetTopItem(INT nOffsetY)
// {
// 	if ( HaveChild() && IsExpand() )
// 	{
// 		
// 	}
// 
// 	if ( m_rtControl.bottom > nOffsetY )
// 		return this;
// 
// 	return NULL;
// }

LPVOID CDUITreeNode::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ITreeViewItemName) == 0)
	{
		return (IDUITreeViewItem*)this;
	}
	
	return theBase::GetInterface(strName);	
}

CRefPtr<IDUIControl> CDUITreeNode::Clone()
{
	return new CDUITreeNode(*this);;
}


INT			CDUITreeNode::GetLevel()
{
	INT  nLevel = 1;  //root is 1
	HDUITREEITEM hParent = (HDUITREEITEM)GetParent();
	//IDUIControl*	pDUIControl = GetParent();
	while ( hParent )
	{
		if ( hParent->GetInterface(ITreeViewName) )
			break;

		nLevel ++ ;

		hParent = hParent->GetParent();
	}

	return nLevel;
}

BOOL	CDUITreeNode::HaveChild()
{
	return m_arChildControl.size() > 0;
}

BOOL	CDUITreeNode::IsParent()
{
	return HaveChild();
}

BOOL		CDUITreeNode::IsExpand()
{
	//DUI_ASSERT(HaveChild());

	if ( !HaveChild() )
		return FALSE;

	return m_bExpand;
}

BOOL		CDUITreeNode::SetExpand(BOOL bExpand)
{
	//DUI_ASSERT(HaveChild());

	BOOL bOld = IsExpand();
	if ( bOld != bExpand )
	{
		m_bExpand = bExpand;

		INT nCount = GetControlCount();
		for ( int i=0;i<nCount;i++)
		{
			HDUITREEITEM pItem = GetChild(i);
			if ( pItem )
			{
				if ( bExpand )
					pItem->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
				else
					pItem->ModifyStyle(CONTROL_STYLE_VISIBLE,0);
			}
		}
	}

	return bOld;
}

HDUITREEITEM CDUITreeNode::GetParent()
{
	return (HDUITREEITEM)theBase::GetParent();
}

HDUITREEITEM	CDUITreeNode::GetNextSibling()
{
	HDUITREEITEM hParent = GetParent();
	if ( hParent )
		return (HDUITREEITEM)hParent->GetNextSiblingControl(this);

	return NULL;
}

HDUITREEITEM	CDUITreeNode::GetPrevSibling()
{
	HDUITREEITEM hParent = GetParent();
	if ( hParent )
		return (HDUITREEITEM)hParent->GetPrevSiblingControl(CRefPtr<IDUIControl>(this));

	return NULL;
}

HDUITREEITEM	CDUITreeNode::GetFirstChild()
{
	if ( !HaveChild())
		return NULL;

	return (HDUITREEITEM)GetControlByIndex(0);
}

HDUITREEITEM	CDUITreeNode::GetChild(int nIndex)
{
	if ( !HaveChild())
		return NULL;
	
	return (HDUITREEITEM)GetControlByIndex(nIndex);
}

void			CDUITreeNode::SetSel(BOOL bSel)
{
	BOOL bOld = IsSelected();
	if ( bOld != bSel )
	{
		m_bSelected = bSel;

		//update
	}
}
BOOL			CDUITreeNode::IsSelected()
{
	return m_bSelected;
}


LRESULT CDUITreeNode::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);
	switch(info.dwEventType)
	{
	case DUI_EVENT_MOUSEENTER:
		ModifyStatus(0, CONTROL_STATUS_HOVER);
		if(m_pParentControl != NULL)
		{
			DUIEvent event = info;
			event.pSender = this;
			event.dwEventType = DUI_EVENT_ELE_ITEM_MOUSEENTER;
			m_pParentControl->OnDUIEvent(event, bHandled);
		}
		
		break;
		
	case DUI_EVENT_MOUSELEAVE:
		ModifyStatus(CONTROL_STATUS_HOVER, 0);
		if(m_pParentControl != NULL)
		{
			DUIEvent event = info;
			event.pSender = this;
			event.dwEventType = DUI_EVENT_ELE_ITEM_MOUSELEAVE;
			m_pParentControl->OnDUIEvent(event, bHandled);
		}
		break;
		
	case DUI_EVENT_LBUTTONDOWN:
		ModifyStatus(0, CONTROL_STATUS_PRESSED);
		if(m_pParentControl != NULL)
		{
			m_pParentControl->SetFocus(TRUE);
		}
		break;
		
	case DUI_EVENT_LBUTTONDBCLK:
		ModifyStatus(0, CONTROL_STATUS_PRESSED);
		if(m_pParentControl != NULL)
		{
			DUIEvent event = info;
			event.pSender = this;
			event.dwEventType = DUI_EVENT_ELE_ITEM_DBCLICK;
			m_pParentControl->OnDUIEvent(event, bHandled);
		}
		
		break;		
		
	case DUI_EVENT_LBUTTONUP:
		ModifyStatus(CONTROL_STATUS_PRESSED, 0);
		if(PtInRect(&m_rtControl, info.ptMouse))
		{
// 			if(m_pParentControl != NULL)
// 			{
 				DUIEvent event = info;
 				event.pSender = this;
 				event.dwEventType = DUI_EVENT_ELE_ITEM_CLICK;
// 				m_pParentControl->OnDUIEvent(event, bHandled);
// 			}
			FireNotifyToTreeView(event,bHandled);
		}
		break;
		
	default:
		break;
	}
	
	if(bHandled) return lRet; 
	
	return theBase::ProcessEvent(info, bHandled);
}

void			CDUITreeNode::FireNotifyToTreeView(const DUIEvent& event,BOOL& bHandled)
{
	IDUIControl* pControl = NULL;
	IDUIControl* hParent = m_pParentControl;
	while ( hParent )
	{
		pControl = (IDUIControl*)hParent->GetInterface(ITreeViewName);
		if ( pControl )
			break;

		hParent = hParent->GetParent();
	}

	if ( pControl )
		pControl->OnDUIEvent(event,bHandled);
}

VOID CDUITreeNode::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
// 	if(strName.compare(_T("back_image")) == 0)
// 	{
// 		m_pImgListBK = GetSkinMgr()->GetImage(strValue);
// 		bHandled = TRUE;
// 	}
// 	else if(strName.compare(_T("icon")) == 0)
// 	{
// 		m_pImgListIcon = GetSkinMgr()->GetImage(strValue);
// 		bHandled = TRUE;
// 	}
// 	else if(strName.compare(_T("button_style")) == 0)
// 	{
// 		m_dwButtonStyle = ParseButtonStyle(strValue);
// 		bHandled = TRUE;
// 	}
	
	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

//////////////////////////////////////////////////////////////////////////
//implementation of treeview
//////////////////////////////////////////////////////////////////////////

CDUITreeViewImpl::CDUITreeViewImpl()
{
	m_hTopItem = NULL;

	m_nTotalHeight = 0;
	m_nExtraHeight = 50;

	m_nMaxRight = 0;
}
CDUITreeViewImpl::~CDUITreeViewImpl()
{

}

/*virtual*/ CRefPtr<IDUIControl> CDUITreeViewImpl::Clone()
{
	return new CDUITreeViewImpl(*this);
}

HDUITREEITEM CDUITreeViewImpl::AddRoot(HDUITREEITEM hItem)  
{
	DUI_ASSERT(hItem);

	if ( theBase::AddControl(hItem) )
		return hItem;

	return NULL;
}

HDUITREEITEM	CDUITreeViewImpl::AddChildItem(HDUITREEITEM hParent , HDUITREEITEM hChild) 
{
	DUI_ASSERT(hChild);

	if ( hParent == NULL )
		return AddRoot(hChild);

	if ( hParent->AddControl(hChild) )
		return hChild;

	return NULL;
}

HDUITREEITEM	CDUITreeViewImpl::GetTopItem()
{
	return m_hTopItem;
}

HDUITREEITEM	CDUITreeViewImpl::GetRoot()
{
	return (HDUITREEITEM)GetControlByIndex(0);
}

VOID CDUITreeViewImpl::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	
	int nMaxWidth = 0;
	m_nTotalHeight =CalculateTotalHeight();
	
	RECT rtAvailable = GetContentRect();

	BOOL bUpdateScroll = ProcessVerticalSB(rtAvailable, m_nTotalHeight, m_nExtraHeight);
	if(bUpdateScroll) return; //layout at next time
	
	if(!m_pVertSB.IsNull() &&
		(m_pVertSB->IsVisible()))
	{
		INT nThumbWidth = m_pVertSB->GetFixedSize().cx;
		RECT rtScrollBar = rtAvailable;
		rtScrollBar.left = rtScrollBar.right - nThumbWidth;
		
		m_pVertSB->SetControlRect(rtScrollBar);
		
		rtAvailable.right -= nThumbWidth;
	}
	
	IDUIControl* pControl = NULL;
	INT nCurrentHeight(0), nControlHeight(0);;
	
	RECT rtControl = rtAvailable;

	m_hTopItem = CalculateTopItem();
	CTrace::TraceInfo(_T("2011-09-18,hTopItem = %x"),m_hTopItem);

	HDUITREEITEM hFirst = m_hTopItem;
	for ( hFirst;hFirst!=NULL;hFirst=GetNextVisibleItem(hFirst) )
	{
		CTrace::TraceInfo(_T("2011-09-18,hFirst = %x"),hFirst);

		rtControl.top = rtAvailable.top + nCurrentHeight;
		rtControl.bottom = rtControl.top + hFirst->GetItemHeight();

		hFirst->SetControlRect(rtControl);

		nCurrentHeight += hFirst->GetItemHeight();
		if ( nCurrentHeight >= rtAvailable.bottom - rtAvailable.top )
			break;
	}
}

LRESULT CDUITreeViewImpl::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);
	switch(info.dwEventType)
	{
	case DUI_EVENT_ELE_ITEM_CLICK:
		OnItemClick(info);
		bHandled = TRUE;
		break;
	case DUI_EVENT_ELE_ITEM_DBCLICK:
		//OnItemDBClick(info);
		bHandled = TRUE;
		break;
		
	case DUI_EVENT_ELE_ITEM_MOUSEENTER:
	case DUI_EVENT_ELE_ITEM_MOUSELEAVE:
		//OnItemHover(info);
		bHandled = TRUE;
		break;
		
	case DUI_EVENT_KEYDOWN:
		//OnKeyDown(info);
		bHandled = TRUE;
		break;
		
	default:
		break;
	}
	
	if(bHandled) return lRet;
	
	return theBase::ProcessEvent(info, bHandled);
}

LPVOID CDUITreeViewImpl::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ITreeViewName) == 0)
	{
		return (IDUITreeView*)this;
	}

	return theBase::GetInterface(strName);	
}

VOID CDUITreeViewImpl::DoPaint(HDC dc, const RECT& rtDraw)
{
	DUI_ASSERT(dc && rtDraw.bottom-rtDraw.top >0 && rtDraw.right - rtDraw.left > 0 );
	
	RECT rtAvailable = GetContentRect();
	HDUITREEITEM hFirst = m_hTopItem;

	for ( hFirst;hFirst!=NULL;hFirst=GetNextVisibleItem(hFirst) )
	{
		CDUIRenderClip clip(dc, GetContentRect());

		RECT rtControl = hFirst->GetControlRect();
		hFirst->DoPaint(dc,rtControl);
		
		
		if ( rtControl.bottom >= rtAvailable.bottom )
			break;
	}
	
	if(m_pVertSB != NULL 
		&& (m_pVertSB->IsVisible()))
	{
		CDUIRenderClip clip(dc, GetContentRect());
		
		m_pVertSB->DoPaint(dc, rtDraw);
	}
}

IDUIControl* CDUITreeViewImpl::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if((uFlags & FIND_FLAG_VISIBLE)
		&& !(GetStyle() & CONTROL_STYLE_VISIBLE)) return NULL;
	
	if((uFlags & FIND_FLAG_ENABLED)
		&& (GetStatus() & CONTROL_STATUS_DISABLE)) return NULL;
	
	if((uFlags & FIND_FLAG_HITTEST)
		&& 
		(!(PtInRect(&m_rtControl, *static_cast<LPPOINT>(pData)))
		|| (GetStyle() & CONTROL_STYLE_TRANSPARENT))) return NULL;
	
	IDUIControl* pRet = NULL;
	if((uFlags & FIND_FLAG_MEFIRST))
	{
		pRet = Proc(this, pData);
		if(pRet != NULL) return pRet;
	}


	RECT rtAvailable = GetContentRect();
	HDUITREEITEM hFirst = m_hTopItem;
	
	for ( hFirst;hFirst!=NULL;hFirst=GetNextVisibleItem(hFirst) )
	{
		RECT rtControl = hFirst->GetControlRect();

		pRet =	hFirst->FindControl(Proc, pData, uFlags);
		if ( pRet )  return pRet;		
		
		if ( rtControl.bottom >= rtAvailable.bottom )
			break;
	}

// 	INT nCount = m_arChildControl.size();
// 	for(int i=m_nTopIndex; (i<nCount && i<=m_nLastIndex); ++i)
// 	{
// 		pRet = m_arChildControl[i]->FindControl(Proc, pData, uFlags);
// 		if(pRet != NULL) return pRet;
// 	}
	
	IDUIScrollBar* pScrollBar = GetVerticalSB();
	if(pScrollBar != NULL)
	{
		pRet = pScrollBar->FindControl(Proc, pData, uFlags);
		if(pRet != NULL) return pRet;
	}
	
	return Proc(this, pData);
}

INT		CDUITreeViewImpl::CalculateTotalHeight()
{
	m_nMaxRight = 0;

	m_nTotalHeight = 0 ;
	
	INT nItemHeight(0);
	INT nCount = m_arChildControl.size();
	for(INT i=0; i<nCount; ++i)
	{
		HDUITREEITEM pItem = (HDUITREEITEM)m_arChildControl[i]->GetInterface(ITreeViewItemName) ;
		 
		if ( pItem && pItem->IsVisible() )
		{
			//nItemHeight = pItem->GetFixedSize().cy;
			//m_nTotalHeight += nItemHeight;

			//if ( pItem->IsExpand() && pItem->HaveChild() )
				m_nTotalHeight += pItem->GetTotalHeight();

			RECT rt = pItem->GetControlRect();
			if ( rt.right > m_nMaxRight )
				m_nMaxRight = rt.right;
		}
	}

	CTrace::TraceInfo(_T("2011-09-18 totalheight=%d"),m_nTotalHeight);
	
	return m_nTotalHeight;
}

HDUITREEITEM		CDUITreeViewImpl::CalculateTopItem()
{
	HDUITREEITEM hTopItem = NULL;

	if(m_pVertSB.IsNull()
		|| !(m_pVertSB->IsVisible()))
	{
		if ( m_arChildControl.size() > 0)
			hTopItem = (HDUITREEITEM)m_arChildControl[0]->GetInterface(ITreeViewItemName);

		return hTopItem;
	}
	
	SCROLLINFO si = {0};
	m_pVertSB->GetScrollBarInfo(&si);
	INT nOffsetY = si.nPos;
	
	INT nTempY(0);

	INT nCount = m_arChildControl.size();  //root items
	for(INT i=0; i<nCount; ++i)
	{
		HDUITREEITEM pItem = (HDUITREEITEM)m_arChildControl[i]->GetInterface(ITreeViewItemName);
		if ( pItem==NULL || !pItem->IsVisible())
			continue;

		nTempY += pItem->GetFixedSize().cy;
		if ( nTempY > nOffsetY )
		{
			return pItem;
		}
	
		if ( pItem->HaveChild() && pItem->IsExpand() )
		{
			HDUITREEITEM pItemFind = CalculateTopItem(pItem,nTempY,nOffsetY);
			if ( pItemFind != NULL )
				return pItemFind;
		}

	}

	return hTopItem;
}


HDUITREEITEM		CDUITreeViewImpl::CalculateTopItem(HDUITREEITEM pItem,INT& nTempY,INT nOffsetY)
{
	if ( pItem == NULL )
		return NULL;

	HDUITREEITEM hTopItem = NULL;

	//if ( pItem->IsExpand() )
	{
		INT nCount = pItem->GetControlCount();
		for ( int i=0;i<nCount;i++)
		{
			HDUITREEITEM pChildItem = (HDUITREEITEM)pItem->GetChild(i);
			if ( pChildItem ==NULL || !pChildItem->IsVisible() )
				continue;

			nTempY += pChildItem->GetFixedSize().cy;
			if ( nTempY > nOffsetY )
			{
				return pChildItem;
			}
			
			if ( pChildItem->HaveChild() && pChildItem->IsExpand() )
			{
				hTopItem = CalculateTopItem(pChildItem,nTempY,nOffsetY);
				if ( hTopItem )
					return hTopItem;
			}
		}
	}

	return hTopItem;
}

HDUITREEITEM		CDUITreeViewImpl::GetNextVisibleItem(HDUITREEITEM pItem)
{
	if ( pItem == NULL || !pItem->IsVisible() )
		return NULL;

	HDUITREEITEM hNextItemRet = NULL;

	if ( pItem->IsParent() )
	{
		if ( pItem->IsExpand() )
		{
			INT nCount = pItem->GetControlCount();
			if ( nCount > 0)
			{
				HDUITREEITEM hNextItem = pItem->GetFirstChild();
				if ( hNextItem )
				{
					if ( hNextItem->IsVisible() )
						return hNextItem;
					else
						hNextItemRet = GetNextVisibleItem(hNextItem);
				}
			}
		}
		else
		{
			HDUITREEITEM hNextItem = pItem->GetNextSibling();
			if ( hNextItem )
			{
				if ( hNextItem->IsVisible() )
					return hNextItem;
				else
				{
					hNextItemRet = GetNextVisibleItem(hNextItem);
				}
			}
		}
	}
	else
	{
		HDUITREEITEM hNextItem = pItem->GetNextSibling();
		if ( hNextItem )
		{
			if ( hNextItem->IsVisible() )
				return hNextItem;
			else
			{
				hNextItemRet = GetNextVisibleItem(hNextItem);
			}
		}
		else  // return to the parent
		{
			hNextItem = pItem->GetParent()->GetNextSibling();
			return GetNextVisibleItem(hNextItem);
		}
	}

	return hNextItemRet;
}

IDUIScrollBar* CDUITreeViewImpl::GetVerticalSB()
{
	return m_pVertSB.GetPtr();
}

VOID CDUITreeViewImpl::OnCreate()
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
	
	//CheckInitGroupStatus();
}

VOID CDUITreeViewImpl::OnDestroy()
{
	if(!m_pVertSB.IsNull())
	{
		m_pVertSB->DetachWindow();
		m_pVertSB->SetScrollOwner(NULL);
	}
	
	m_pVertSB = NULL;
	//m_nHoverIndex = -1;
}

VOID CDUITreeViewImpl::OnItemClick(const DUIEvent& info)
{
	HDUITREEITEM	hItem = (HDUITREEITEM)info.pSender;
	if ( hItem && hItem->HaveChild() )
	{
		hItem->SetExpand(!hItem->IsExpand());

		UpdateLayout(TRUE);
	}
}

DUI_END_NAMESPCE 