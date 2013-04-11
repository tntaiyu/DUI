#include "DUIListBox.h"
#include "DUIRenderEngine.h"

DUI_BGN_NAMESPCE


///////////////  CDUIIconTextItem  //////////////////
CDUIIconTextItem::CDUIIconTextItem()
{
	m_bSelect = FALSE;
	m_bExpand = FALSE;
	m_bGroup = FALSE;
	m_nIconIndex = 0;
	m_dwItemData = 0;

	m_nNormalHeight = 25;
	m_nExpandHeight = 50;
	SetFixedHeight(m_nNormalHeight);
}

LPVOID CDUIIconTextItem::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IListItem) == 0)
	{
		return (IDUIListItem*)this;
	}
	else if(strName.compare(IListIconTextItem) == 0)
	{
		return (IDUIIconTextItem*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIIconTextItem::Clone()
{
	return new CDUIIconTextItem(*this);
}

LRESULT CDUIIconTextItem::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
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
			if(m_pParentControl != NULL)
			{
				DUIEvent event = info;
				event.pSender = this;
				event.dwEventType = DUI_EVENT_ELE_ITEM_CLICK;
				m_pParentControl->OnDUIEvent(event, bHandled);
			}
		}
		break;
	
	default:
		break;
	}

	if(bHandled) return lRet; 
	
	return theBase::ProcessEvent(info, bHandled);
}

VOID CDUIIconTextItem::PaintBkgnd(HDC dc)
{
	RECT rtBK = GetBKRect();
	BOOL bFill(FALSE);
	CARGB clrBK;

	CRefPtr<CDUIFrameBK> pFrameBK;
	if(GetStatus() & CONTROL_STATUS_HOVER)
	{
		pFrameBK = GetHoverFrameBK();
		bFill = TRUE;
	}

	if(m_bSelect)
	{
		pFrameBK = GetSelectFrameBK();
		bFill = TRUE;
	}

	if(bFill
		&& !pFrameBK.IsNull()
		&& !pFrameBK->IsEmpty())
	{
		pFrameBK->DrawBK(dc, rtBK);
	}
}

VOID CDUIIconTextItem::PaintForgnd(HDC dc)
{
	CRefPtr<CImageList> pIcon;
	if(m_bExpand)
	{
		pIcon = m_uiData.m_pExpandIcon;
	}
	else
	{
		pIcon = m_uiData.m_pNormalIcon;
	}

	if(pIcon.IsNull() || pIcon->IsNull())
	{
		return;
	}
	
	INT nImageIndex(m_nIconIndex);
	if(nImageIndex >= pIcon->GetItemCount())
	{
		nImageIndex = 0;
	}

	RECT rtIcon = GetIconRect();
	pIcon->DrawInCenter(dc , rtIcon, nImageIndex);
}

VOID CDUIIconTextItem::PaintText(HDC dc)
{
	if(m_pParentControl == NULL)
	{
		DUI_ASSERT(FALSE);
		return;
	}

	CRefPtr<CTextStyle> pTextStyle = m_pParentControl->GetTextStyle();
	if(pTextStyle.IsNull()) return;

	CRefPtr<CFontObject> pFont = pTextStyle->GetFont();
	if(pFont.IsNull() || pFont->IsNull()) return;
	HFONT hFont = pFont->GetFont();
	
	if(m_strText.size() > 0)
	{
		RECT rtText = GetTextRect();
		SelectObject(dc, hFont);
		SetBkMode(dc, TRANSPARENT);

		if(GetSelect())
		{
			SetTextColor(dc, pTextStyle->GetSelectColor().GetColor());
		}
		else if(GetStatus() & CONTROL_STATUS_HOVER)
		{
			SetTextColor(dc, pTextStyle->GetHoverColor().GetColor());
		}
		else
		{
			SetTextColor(dc, pTextStyle->GetNormalColor().GetColor());
		}

		CDUIRenderEngine::DrawText(dc, m_strText.c_str()
			, m_strText.size(), &rtText, pTextStyle->GetTextFormat());
	}
}

RECT CDUIIconTextItem::GetTextRect()
{
	RECT rtBK = GetBKRect();
	RECT rtIcon = GetIconRect();

	RECT rtText = rtBK;
	rtText.left = rtIcon.right + GetPaddingWidth();
	return rtText;
}

RECT CDUIIconTextItem::GetIconRect()
{
	RECT rtBK = GetBKRect();
	RECT rtIcon = rtBK;

	if(!m_bExpand)
	{
		if(m_uiData.m_pNormalIcon.IsNull() 
			|| m_uiData.m_pNormalIcon->IsNull())
		{
			rtIcon.right = rtIcon.left;
			return rtIcon;
		}
	}
	else
	{
		if(m_uiData.m_pExpandIcon.IsNull() 
			|| m_uiData.m_pExpandIcon->IsNull())
		{
			rtIcon.right = rtIcon.left;
			return rtIcon;
		}
	}

	rtIcon.right = rtIcon.left + (rtIcon.bottom - rtIcon.top);
	return rtIcon;
}

RECT CDUIIconTextItem::GetBKRect()
{
	/*return GetContentRect();*/
	return m_rtControl;
}

VOID CDUIIconTextItem::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("normal_icon")) == 0)
	{
		m_uiData.m_pNormalIcon = GetSkinMgr()->GetImage(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("expand_icon")) == 0)
	{
		m_uiData.m_pExpandIcon = GetSkinMgr()->GetImage(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("hover_frame")) == 0)
	{
		SetHoverFrameBK(GetSkinMgr()->GetFrameBK(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("select_frame")) == 0)
	{
		SetSelectFrameBK(GetSkinMgr()->GetFrameBK(strValue));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("normal_height")) == 0)
	{
		SetHeight(FALSE, _ttol(strValue.c_str()));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("expand_height")) == 0)
	{
		SetHeight(TRUE, _ttol(strValue.c_str()));
		bHandled = TRUE;
	}
	else if(strName.compare(_T("group")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetGroup(TRUE);
		}
		else
		{
			SetGroup(FALSE);
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("expand")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetExpand(TRUE);
		}
		else
		{
			SetExpand(FALSE);
		}
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}
VOID CDUIIconTextItem::SetHoverFrameBK(CRefPtr<CDUIFrameBK> pFrameBK)
{
	m_pHoverFrameBK = pFrameBK;
}

CRefPtr<CDUIFrameBK> CDUIIconTextItem::GetHoverFrameBK() const
{
	return m_pHoverFrameBK;
}

VOID CDUIIconTextItem::SetSelectFrameBK(CRefPtr<CDUIFrameBK> pFrameBK)
{
	m_pSelectFrameBK = pFrameBK;
}

CRefPtr<CDUIFrameBK> CDUIIconTextItem::GetSelectFrameBK() const
{
	return m_pSelectFrameBK;
}

VOID CDUIIconTextItem::SetUIData(const CIconTextItemUIData& data)
{
	m_uiData = data;
}

const CIconTextItemUIData& CDUIIconTextItem::GetUIdata() const
{
	return m_uiData;
}

VOID CDUIIconTextItem::SetHeight(BOOL bExpand, INT nValue)
{
	if(bExpand)
	{
		m_nExpandHeight = nValue;
	}
	else
	{
		m_nNormalHeight = nValue;
	}

	SetExpand(m_bExpand);
}

INT CDUIIconTextItem::GetHeight(BOOL bExpand) const
{
	if(bExpand)
	{
		return m_nExpandHeight;
	}
	else
	{
		return m_nNormalHeight;
	}
}
	
VOID CDUIIconTextItem::SetIconIndex(INT nIndex)
{
	m_nIconIndex = nIndex;
}

INT CDUIIconTextItem::GetIconIndex() const
{
	return m_nIconIndex;
}

VOID CDUIIconTextItem::SetSelect(BOOL bSet)
{
	m_bSelect  = bSet;
}
BOOL CDUIIconTextItem::GetSelect() const
{
	return m_bSelect;
}

VOID CDUIIconTextItem::SetExpand(BOOL bExpand)
{
	m_bExpand = bExpand;	
	if(m_bExpand)
	{
		SetFixedHeight(m_nExpandHeight);
	}
	else
	{
		SetFixedHeight(m_nNormalHeight);
	}
}

BOOL CDUIIconTextItem::GetExpand() const
{
	return m_bExpand;
}

VOID CDUIIconTextItem::SetGroup(BOOL bGroup)
{
	m_bGroup = bGroup;
}

BOOL CDUIIconTextItem::GetGroup() const
{
	return m_bGroup;
}

VOID CDUIIconTextItem::SetItemData(DWORD dwData)
{
	m_dwItemData = dwData;
}

DWORD CDUIIconTextItem::GetItemData() const 
{
	return m_dwItemData;
}

///////////////  CDUIIconTextItem  //////////////////



////////////////  CDUIListBox  ////////////////
CDUIListBox::CDUIListBox()
{
	m_nSelectIndex = -1;
	m_nHoverIndex = -1;

	m_nTotalHeight = 0;
	m_nExtraHeight = 50;
	
	m_nTopIndex = 0;
	m_nLastIndex = 0;

	m_bSelectExpand = FALSE;
	m_bHoverExpand = FALSE;
	m_bHoverSelect = FALSE;

	m_dwStyle |= CONTROL_STYLE_TABSTOP;
}

LPVOID CDUIListBox::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IListBox) == 0)
	{
		return (IDUIListBox*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIListBox::Clone()
{
	return new CDUIListBox(*this);
}

IDUIScrollBar* CDUIListBox::GetVerticalSB()
{
	return m_pVertSB.GetPtr();
}

VOID CDUIListBox::OnCreate()
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

	CheckInitGroupStatus();
}

VOID CDUIListBox::OnDestroy()
{
	if(!m_pVertSB.IsNull())
	{
		m_pVertSB->DetachWindow();
		m_pVertSB->SetScrollOwner(NULL);
	}

	m_pVertSB = NULL;
	m_nHoverIndex = -1;
}

VOID CDUIListBox::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;

	CalculateTotalHeight();

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

	CalculateTopIndex();

	IDUIControl* pControl = NULL;
	INT nCurrentHeight(0), nControlHeight(0);;
	RECT rtControl = rtAvailable;
	INT nCount = m_arChildControl.size();
	for(INT i=m_nTopIndex; i<nCount; ++i)
	{
		pControl = m_arChildControl[i].GetPtr();
		if(!(pControl->IsVisible())) continue;
		
		rtControl.top = rtAvailable.top + nCurrentHeight;
		nControlHeight = pControl->GetFixedSize().cy;
		rtControl.bottom = rtControl.top + nControlHeight;
		pControl->SetControlRect(rtControl);
	
		nCurrentHeight	+= nControlHeight;
		m_nLastIndex = i;
		if(nCurrentHeight >= rtAvailable.bottom - rtAvailable.top)
		{
			break;
		}
	}
}


VOID CDUIListBox::DoPaint(HDC dc, const RECT& rtDraw)
{
	IDUIControl* pControl = NULL;
	INT nCount = m_arChildControl.size();
	for(INT i=m_nTopIndex; (i<nCount && i<=m_nLastIndex); ++i)
	{
		CDUIRenderClip clip(dc, GetContentRect());

		pControl = m_arChildControl[i].GetPtr();
		if(!(pControl->IsVisible())) continue;
		
		pControl->DoPaint(dc, rtDraw);
	}
	

	if(m_pVertSB != NULL 
		&& (m_pVertSB->IsVisible()))
	{
		CDUIRenderClip clip(dc, GetContentRect());

		m_pVertSB->DoPaint(dc, rtDraw);
	}
}

IDUIControl* CDUIListBox::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
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

	INT nCount = m_arChildControl.size();
	for(int i=m_nTopIndex; (i<nCount && i<=m_nLastIndex); ++i)
	{
		pRet = m_arChildControl[i]->FindControl(Proc, pData, uFlags);
		if(pRet != NULL) return pRet;
	}

	IDUIScrollBar* pScrollBar = GetVerticalSB();
	if(pScrollBar != NULL)
	{
		pRet = pScrollBar->FindControl(Proc, pData, uFlags);
		if(pRet != NULL) return pRet;
	}

	return Proc(this, pData);
}
BOOL CDUIListBox::BeforeAddControl(CRefPtr<IDUIControl> pItem)
{
	if(!theBase::BeforeAddControl(pItem)) return FALSE;
	IDUIListItem* pListItem = (IDUIListItem*)pItem->GetInterface(IListItem);
	DUI_ASSERT(pListItem != NULL);
	if(pListItem == NULL) return FALSE;

	return TRUE;
}

VOID CDUIListBox::AfterRemoveControl(CRefPtr<IDUIControl> pItem)
{
	if(pItem.IsNull()) return;
	
	theBase::AfterRemoveControl(pItem);
}

INT CDUIListBox::GetVisibleItemIndex(IDUIControl* pItem)
{
	INT nCount = GetControlCount();
	for(INT i=m_nTopIndex; (i<nCount && i<=m_nLastIndex); ++i)
	{
		if(m_arChildControl[i].GetPtr() == pItem)
		{
			return i;
		}
	}

	return -1;
}

VOID CDUIListBox::OnItemClick(const DUIEvent& info)
{
	SendNotify(WM_NOTIFY_LISTBOX_NOTIFY, LBN_WPARAM_CLCIK, (LPARAM)info.pSender);

	IDUIListItem* pListItem = (IDUIListItem*)info.pSender->GetInterface(IListItem);
	if(pListItem != NULL)
	{
		if(!pListItem->GetGroup())
		{
			INT nClickIndex = GetVisibleItemIndex(info.pSender);
			if(nClickIndex != -1)
			{
				SetCurSel(nClickIndex);
			}
		}
		else
		{
			INT nIndex = GetControlIndex(info.pSender);
			if(nIndex != -1)
			{
				ExpandGroup(nIndex, !pListItem->GetExpand());
			}
		}
	}
}

VOID CDUIListBox::OnItemDBClick(const DUIEvent& info)
{
	SendNotify(WM_NOTIFY_LISTBOX_NOTIFY, LBN_WPARAM_DBCLCIK, (LPARAM)info.pSender);
}

VOID CDUIListBox::OnItemHover(const DUIEvent& info)
{
	IDUIListItem* pListItem = (IDUIListItem*)info.pSender->GetInterface(IListItem);
	if(pListItem->GetGroup()) return;


	if(info.dwEventType == DUI_EVENT_ELE_ITEM_MOUSEENTER)
	{
		SendNotify(WM_NOTIFY_LISTBOX_NOTIFY, LBN_WPARAM_HOVERCHANGE, (LPARAM)info.pSender);
	}

	INT nHoverIndex(-1);
	if(info.dwEventType == DUI_EVENT_ELE_ITEM_MOUSEENTER)
	{
		nHoverIndex = GetVisibleItemIndex(info.pSender);
	}
	else if(info.dwEventType == DUI_EVENT_ELE_ITEM_MOUSELEAVE)
	{
		nHoverIndex = -1;
	}

	if(m_nHoverIndex != nHoverIndex)
	{
		SetCurHoverExpand(nHoverIndex);
	}

	if(m_bHoverSelect)
	{
		if(nHoverIndex != -1 && nHoverIndex != m_nSelectIndex)
		{
			SetCurSel(nHoverIndex);
		}
	}
}

LRESULT CDUIListBox::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);
	switch(info.dwEventType)
	{
	case DUI_EVENT_ELE_ITEM_CLICK:
		OnItemClick(info);
		bHandled = TRUE;
		break;
	case DUI_EVENT_ELE_ITEM_DBCLICK:
		OnItemDBClick(info);
		bHandled = TRUE;
		break;

	case DUI_EVENT_ELE_ITEM_MOUSEENTER:
	case DUI_EVENT_ELE_ITEM_MOUSELEAVE:
		OnItemHover(info);
		bHandled = TRUE;
		break;
		
	case DUI_EVENT_KEYDOWN:
		OnKeyDown(info);
		bHandled = TRUE;
		break;

	default:
		break;
	}

	if(bHandled) return lRet;

	return theBase::ProcessEvent(info, bHandled);
}

BOOL CDUIListBox::OnKeyDown(const DUIEvent& info)
{
	BOOL bRet(FALSE);
	switch(info.wParam)
	{
// 	case VK_SPACE:
// 	case VK_RETURN:
// 		if(m_nSelectIndex >=0 && m_nSelectIndex < GetControlCount())
// 		{
// 			IDUIListItem* pItem = (IDUIListItem*)GetControlByIndex(m_nSelectIndex)->GetInterface(IListItem);
// 			if(pItem != NULL 
// 				&& pItem->IsVisible()
// 				&& pItem->IsEnabled()
// 				&& pItem->GetGroup())
// 			{
// 				ExpandGroup(m_nSelectIndex, !pItem->GetExpand());
// 			}
// 		}
// 		break;

	case VK_LEFT:
	case VK_UP:
		{
			INT nIndex = m_nSelectIndex;
			while(TRUE)
			{
				--nIndex;
				if(nIndex>=0 && nIndex<GetControlCount())
				{
					if((GetControlByIndex(nIndex)->IsVisible()))
					{
						SetCurSel(nIndex);
						if(nIndex < m_nTopIndex
							|| nIndex >= m_nLastIndex)
						{
							SetTopIndex(nIndex);
						}
						break;
					}
				}
				else
				{
					break;
				}
			}
		}

		bRet = TRUE;
		break;

	case VK_RIGHT:
	case VK_DOWN:
		{
			INT nIndex = m_nSelectIndex;
			while(TRUE)
			{
				++nIndex;
				if(nIndex>=0 && nIndex<GetControlCount())
				{
					if(GetControlByIndex(nIndex)->IsVisible())
					{
						SetCurSel(nIndex);
						if(nIndex >= m_nLastIndex
							|| nIndex < m_nTopIndex)
						{
							INT nTopIndex = m_nTopIndex;
							//SetTopIndex(++nTopIndex);
							SetTopIndex(nIndex);
						}
						break;
					}
				}
				else
				{
					break;
				}
			}

		}
		
		bRet = TRUE;
		break;
	}

	return bRet;
}

VOID CDUIListBox::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);

	if(strName.compare(_T("select_expand")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetSelectExpand(TRUE);
		}
		else
		{
			SetSelectExpand(FALSE);
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("hover_expand")) == 0)
	{
		if(strValue.compare(DUI_STR_TRUE) == 0)
		{
			SetHoverExpand(TRUE);
		}
		else
		{
			SetHoverExpand(FALSE);
		}
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}
	
INT CDUIListBox::GetCurSel() const
{
	return m_nSelectIndex;
}

BOOL CDUIListBox::SetCurHoverExpand(INT nIndex)
{
	if(nIndex != m_nHoverIndex)
	{
		if(m_nHoverIndex != -1)
		{
			if(m_nHoverIndex >=0 && m_nHoverIndex < GetControlCount())
			{
				IDUIListItem* pListItem = (IDUIListItem*)m_arChildControl[m_nHoverIndex]->GetInterface(IListItem);
				if(pListItem != NULL && m_bHoverExpand)
				{
					pListItem->SetExpand(FALSE);
				}
			}
			m_nHoverIndex = -1;
		}

		if(nIndex != -1)
		{
			if(nIndex >=0 && nIndex < GetControlCount())
			{
				IDUIListItem* pListItem = (IDUIListItem*)m_arChildControl[nIndex]->GetInterface(IListItem);
				if(pListItem != NULL && m_bHoverExpand)
				{
					m_nHoverIndex = nIndex;
					pListItem->SetExpand(TRUE);
				}
			}
		}
		
		UpdateLayout(GetHoverExpand());
	}

	return TRUE;
}

BOOL CDUIListBox::SetCurSel(INT nIndex)
{
	if(nIndex != m_nSelectIndex)
	{
		if(m_nSelectIndex != -1)
		{
			if(m_nSelectIndex >=0 && m_nSelectIndex < GetControlCount())
			{
				IDUIListItem* pListItem = (IDUIListItem*)m_arChildControl[m_nSelectIndex]->GetInterface(IListItem);
				if(pListItem != NULL && !pListItem->GetGroup())
				{
					pListItem->SetSelect(FALSE);

					if(m_bSelectExpand)
					{
						pListItem->SetExpand(FALSE);
					}
				}
			}
			m_nSelectIndex = -1;
		}

		if(nIndex != -1)
		{
			if(nIndex >=0 && nIndex < GetControlCount())
			{	
				m_nSelectIndex = nIndex;
				IDUIListItem* pListItem = (IDUIListItem*)m_arChildControl[nIndex]->GetInterface(IListItem);
				if(pListItem != NULL && !pListItem->GetGroup())
				{
					pListItem->SetSelect(TRUE);

					if(m_bSelectExpand)
					{
						pListItem->SetExpand(TRUE);
					}
					SendNotify(WM_NOTIFY_LISTBOX_NOTIFY, LBN_WPARAM_SELCHANGE, (LPARAM)GetControlByIndex(nIndex));
				}
			}
		}
		
		UpdateLayout(TRUE);
	}
	return TRUE;
}

INT CDUIListBox::GetTopIndex() const
{
	return m_nTopIndex;
}

INT CDUIListBox::GetLastIndex() const
{
	return m_nLastIndex;
}

BOOL CDUIListBox::SetTopIndex(INT nIndex)
{
	IDUIControl* pControl = GetControlByIndex(nIndex);
	if(pControl == NULL
		|| !(pControl->IsVisible()))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	IDUIScrollBar* pSB = GetVerticalSB();
	if(pSB == NULL 
		|| !(pSB->IsVisible()))
	{
		return FALSE;
	}

	INT nTempY(0);
	INT nCount = m_arChildControl.size();
	for(INT i=0; i<nIndex; ++i)
	{
		if(m_arChildControl[i]->IsVisible())
		{
			nTempY += m_arChildControl[i]->GetFixedSize().cy;
		}
	}	

	SCROLLINFO si = {0};
	pSB->GetScrollBarInfo(&si);
	si.nPos = nTempY;
	si.fMask = SIF_POS;
	pSB->SetScrollBarInfo(&si);

	UpdateLayout(TRUE);

	return TRUE;
}

BOOL CDUIListBox::ExpandItem(INT nIndex, BOOL bExpand)
{
	IDUIControl* pItem = GetControlByIndex(nIndex);
	if(pItem != NULL 
		&& (pItem->IsVisible()))
	{
		IDUIListItem* pListItem = (IDUIListItem*)pItem->GetInterface(IListItem);
		if(pListItem != NULL)
		{
			if(!pListItem->GetGroup())
			{
				pListItem->SetExpand(TRUE);
				UpdateLayout(TRUE);
			}
			else
			{
				ExpandGroup(nIndex, bExpand);
			}

			return TRUE;
		}
	}

	return FALSE;
}

VOID CDUIListBox::ExpandGroup(INT nIndex, BOOL bExpand)
{
	IDUIControl* pItem = GetControlByIndex(nIndex);
	if(pItem != NULL 
		&& (pItem->IsVisible()))
	{
		IDUIListItem* pListItem = (IDUIListItem*)pItem->GetInterface(IListItem);
		if(pListItem != NULL && pListItem->GetGroup()
			&& (pListItem->GetExpand() != bExpand))
		{
			LockLayout(TRUE);

			INT nCount = GetControlCount();
			for(INT i=nIndex+1; i<nCount; ++i)
			{
				IDUIListItem* pItem = (IDUIListItem*)m_arChildControl[i]->GetInterface(IListItem);
				if(pItem == NULL) continue;
				if(pItem->GetGroup()) break;

				if(bExpand)
				{
					pItem->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
				}
				else
				{
					pItem->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
				}
			}

			pListItem->SetExpand(bExpand);

			LockLayout(FALSE);
			UpdateLayout(TRUE);
		}
	}
}

VOID CDUIListBox::CheckInitGroupStatus()
{
	INT nCount = GetControlCount();
	BOOL bGroup = FALSE;
	BOOL bExpand = TRUE;

	LockLayout(TRUE);
	for(INT i=0; i<nCount; ++i)
	{
		IDUIListItem* pListItem = (IDUIListItem*)m_arChildControl[i]->GetInterface(IListItem);
		if(pListItem == NULL) continue;
		
		if(pListItem->GetGroup())
		{
			bGroup = TRUE;
			bExpand = pListItem->GetExpand();
			continue;
		}
		
		if(!bGroup) continue;

		if(bExpand)
		{
			pListItem->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		}
		else
		{
			pListItem->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		}	
	}
	LockLayout(FALSE);
}

VOID CDUIListBox::SetSelectExpand(BOOL bSet)
{
	m_bSelectExpand = bSet;
}

BOOL CDUIListBox::GetSelectExpand() const
{
	return m_bSelectExpand;
}

VOID CDUIListBox::SetHoverExpand(BOOL bSet)
{
	m_bHoverExpand = bSet;
}

BOOL CDUIListBox::GetHoverExpand() const
{
	return m_bHoverExpand;
}

VOID CDUIListBox::SetHoverSelect(BOOL bSet)
{
	m_bHoverSelect = bSet;
}

BOOL CDUIListBox::GetHoverSelect() const 
{
	return m_bHoverSelect;
}

VOID CDUIListBox::SetExtraExpandHeight(INT nHeight)
{
	m_nExtraHeight = nHeight;
}

INT CDUIListBox::GetScrollLineStep()
{
	if(GetControlCount() > 0)
	{
		IDUIListItem* pItem = (IDUIListItem*)GetControlByIndex(0)->GetInterface(IListItem);
		if(pItem != NULL)
		{
			return pItem->GetHeight(FALSE);
		}
	}

	return theBase::GetScrollLineStep();
}

INT CDUIListBox::CalculateTotalHeight()
{
	m_nTotalHeight = 0;

	INT nItemHeight(0);
	INT nCount = m_arChildControl.size();
	for(INT i=0; i<nCount; ++i)
	{
		if(m_arChildControl[i]->IsVisible())
		{
			nItemHeight = m_arChildControl[i]->GetFixedSize().cy;
			m_nTotalHeight += nItemHeight;
		}
	}

	return m_nTotalHeight;
}

INT CDUIListBox::CalculateTopIndex()
{
	if(m_pVertSB.IsNull()
		|| !(m_pVertSB->IsVisible()))
	{
		m_nTopIndex = 0;
		return 0;
	}

	SCROLLINFO si = {0};
	m_pVertSB->GetScrollBarInfo(&si);
	INT nOffsetY = si.nPos;

	INT nTempY(0);
	INT nCount = m_arChildControl.size();
	for(INT i=0; i<nCount; ++i)
	{
		if(m_arChildControl[i]->IsVisible())
		{
			nTempY += m_arChildControl[i]->GetFixedSize().cy;
			if(nTempY > nOffsetY)
			{
				m_nTopIndex = i;
				break;
			}
		}
	}

	return m_nTopIndex;
}

////////////////  CDUIListBox  ////////////////



DUI_END_NAMESPCE