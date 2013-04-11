#include "DUIComboBox.h"
#include "DUIButton.h"
#include "DUILabel.h"
#include "DUIRichEdit.h"
#include "DUIPopupWindow.h"

#define DUI_LIST_DEF_HEIGHT 100

DUI_BGN_NAMESPCE	


////////////////  CDUIComboList  ///////////////////
class CDUIComboList: public CDUIPopupWindow
{
public:
	typedef CDUIPopupWindow theBase;

	BEGIN_MSG_MAP(CDUIComboList)
		CHAIN_MSG_MAP(theBase)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	END_MSG_MAP()
		
	CDUIComboList(CDUIComboBox* pOwner);
	VOID OnDUINotify(const DUINotify& info, BOOL& bHandled);

protected:
	LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL& bHandled);
	LRESULT OnMouseActivate(UINT, WPARAM, LPARAM, BOOL& bHandled);

protected:
	CDUIComboBox* m_pOwner;
};

CDUIComboList::CDUIComboList(CDUIComboBox* pOwner)
{
	m_pOwner = pOwner;
}

VOID CDUIComboList::OnDUINotify(const DUINotify& info, BOOL& bHandled)
{
	if(m_pOwner == NULL) return;

	if(info.dwMsgType == WM_NOTIFY_LISTBOX_NOTIFY)
	{
		if(info.wParam == LBN_WPARAM_CLCIK)
		{
			IDUIListBox* pList = m_pOwner->GetList();
			if(pList != NULL)
			{
				IDUIControl* pItem = (IDUIControl*)info.lParam;
 				INT nIndex = pList->GetControlIndex(pItem);
 				if(nIndex != -1)
				{
					m_pOwner->SetSelectIndex(nIndex);
					PostMessage(WM_CLOSE);
				}
			}
		}
	}

	bHandled = TRUE;
}

LRESULT CDUIComboList::OnMouseActivate(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	return MA_NOACTIVATE;
}

LRESULT CDUIComboList::OnClose(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
	bHandled = FALSE;

	if(m_pOwner != NULL)
	{
		m_pOwner->BeforeHideListWnd();
	}

	return S_OK;
}
////////////////  CDUIComboList  ///////////////////



////////////   CComboEdit  ///////////
class CComboEdit: public CDUIRichEdit
{
public:
	typedef CDUIRichEdit theBase;
	CComboEdit();
	VOID SetOwner(CDUIComboBox* pOwner);

	virtual CRefPtr<IDUIControl> Clone();
	virtual void OnCreate();
	virtual VOID OnTxNotify(DWORD iNotify, void *pv);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CDUIComboBox* m_pOwner;
};
CComboEdit::CComboEdit()
{
	m_pOwner = NULL;
}

VOID CComboEdit::SetOwner(CDUIComboBox* pOwner)
{
	m_pOwner = pOwner;
}

CRefPtr<IDUIControl> CComboEdit::Clone()
{
	return new CComboEdit(*this);
}

void CComboEdit::OnCreate()
{
	theBase::OnCreate();
	SetEventMask(ENM_CHANGE);
}

VOID CComboEdit::OnTxNotify(DWORD iNotify, void *pv)
{
	if(m_pOwner == NULL) return;
	if(iNotify == EN_CHANGE)
	{
		m_pOwner->SetText(GetText());
	}
	
	m_pOwner->OnTxNotify(iNotify, pv);
}

BOOL CComboEdit::PreTranslateMessage(MSG* pMsg)
{
	if(!IsEditAvailable(pMsg)) return FALSE;

	UINT nMsgType = pMsg->message;
	WPARAM wParam = pMsg->wParam;
	if(nMsgType >= WM_KEYFIRST && nMsgType <= WM_KEYLAST)
	{
		if(nMsgType = WM_KEYUP || nMsgType == WM_KEYDOWN)
		{
			if(wParam == VK_UP || wParam == VK_DOWN)
			{
				return FALSE;
			}
		}
	}

	return theBase::PreTranslateMessage(pMsg);	
}

////////////   CComboEdit  ///////////


////////////////  CDUIComboBox  ///////////////////
CDUIComboBox::CDUIComboBox()
{
	CreateCtrl();
	InitStatus();

	m_nListHeight = DUI_LIST_DEF_HEIGHT;
	
	m_pListWnd = NULL;
	m_nLastIndex = -1;
}

CDUIComboBox::CDUIComboBox(const CDUIComboBox& obj)
:IDUIComboBox(obj)
{
	if(!obj.m_pList.IsNull())
	{
		m_pList = (IDUIListBox*)obj.m_pList->Clone()->GetInterface(IListBox);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	m_dwComboBoxStyle = obj.m_dwComboBoxStyle;
	m_pImageBK = obj.m_pImageBK;
	m_nListHeight = obj.m_nListHeight;
	m_nLastIndex = obj.m_nLastIndex;
	
	m_pListWnd = NULL;

	m_pFrameBK = obj.m_pFrameBK;
}

LPVOID CDUIComboBox::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IComboBox) == 0)
	{
		return (IDUIComboBox*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIComboBox::Clone()
{
	return new CDUIComboBox(*this);
}

BOOL CDUIComboBox::Activate()
{
	if(!theBase::Activate()) return FALSE;
	
	if(m_pListWnd != NULL 
		&& !m_pListWnd->IsWindow())
	{
		ShowListWnd();
	}

	return TRUE;
}

VOID CDUIComboBox::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	
	if(strName.compare(_T("arrow_image")) == 0)
	{
		IDUIButton* pButton = GetArrowBtn();
		if(pButton != NULL)
		{
			CButtonUIData uiData = pButton->GetButtonUIData();
			uiData.m_imageFore = GetSkinMgr()->GetImage(strValue);
			pButton->SetButtonUIData(uiData);
		}
		bHandled = TRUE;
	}
	else if(strName.compare(_T("combobox_style")) == 0)
	{
		if(strValue.find(_T("dropdown")) != -1)
		{
			SetComboBoxStyle(COMBOBOX_STYLE_DROPDOWN);
		}
		else if(strValue.find(_T("droplist")) != -1)
		{
			SetComboBoxStyle(COMBOBOX_STYLE_DROPLIST);
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
		
		bHandled= TRUE;
	}
	else if(strName.compare(_T("list_frame")) == 0)
	{
		m_pFrameBK = GetSkinMgr()->GetFrameBK(strValue);
		bHandled =TRUE;
	}
	else if(strName.compare(_T("back_image")) == 0)
	{
		m_pImageBK = GetSkinMgr()->GetImage(strValue);
		bHandled = TRUE;
	}
	else if(strName.compare(_T("list_height")) == 0)
	{
		m_nListHeight = _ttol(strValue.c_str());
		if(m_nListHeight <= 0)
		{
			m_nListHeight = DUI_LIST_DEF_HEIGHT;
			DUI_ASSERT(FALSE);
		}

		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

VOID CDUIComboBox::SetControlRect(const RECT& rt)
{
	m_rtControl = rt;
	
	IDUIButton* pButton = GetArrowBtn();
	IDUIRichEdit* pEdit = GetEdit();
	IDUILabel* pLabel = GetLabel();
	if(pEdit == NULL 
		|| pLabel == NULL 
		|| pButton == NULL)
	{
		return;
	}

	RECT rtContent = GetContentRect();
	RECT rtArrowBtn = rtContent;
	SIZE sizeBtn = {rtArrowBtn.right - rtArrowBtn.left, rtArrowBtn.bottom - rtArrowBtn.top};
	SIZE sizBtnRet;
	pButton->EstimateSize(sizeBtn, sizBtnRet);
	rtArrowBtn.left = rtArrowBtn.right - sizBtnRet.cx;

	RECT rtEdit = rtContent;
	rtEdit.right = rtArrowBtn.left;

	if(::IsRectEmpty(&rtArrowBtn)) rtArrowBtn.left = rtArrowBtn.right;
	if(::IsRectEmpty(&rtEdit)) rtEdit.right = rtEdit.left;

	pButton->SetControlRect(rtArrowBtn);

	pEdit->SetControlRect(rtEdit);
	pLabel->SetControlRect(rtEdit);
}

VOID CDUIComboBox::OnKeyDown(const DUIEvent& info, BOOL& bHandled)
{
	if(m_pListWnd == NULL) return;

	INT nKey = info.wParam;

	switch(nKey)
	{
	case VK_UP:
	case VK_DOWN:
		if(!m_pListWnd->IsWindow())
		{
			Activate();
		}
		else
		{
			INT nIndex = m_pList->GetCurSel();
			INT nCount = m_pList->GetControlCount();
			if(nCount > 0)
			{
				if(info.wParam == VK_UP) --nIndex;
				if(info.wParam == VK_DOWN) ++nIndex;
				if(nIndex < 0) nIndex = 0;
				if(nIndex >= nCount) nIndex = nCount - 1;
				
				if(nKey == VK_UP)
				{
					SetSelectIndexWrapper(nIndex, 2);
				}
				else if(nKey == VK_DOWN)
				{
					SetSelectIndexWrapper(nIndex, 1);
				}
				else
				{
					DUI_ASSERT(FALSE);
				}
			}
		}
		break;

	case VK_RETURN:
		{
			if(m_pListWnd->IsWindow())
			{
				INT nListIndex = m_pList->GetCurSel();
				if( nListIndex != -1)
				{
					SetSelectIndex(nListIndex);
					HideListWnd();
				}
			}
		}
		break;

	case VK_ESCAPE:
		{
			if(m_pListWnd->IsWindow())
			{
				INT nListIndex = m_pList->GetCurSel();
				if(m_nLastIndex == -1 )
				{
					m_nLastIndex = 0;
				}
				
				if(m_nLastIndex != -1)
				{
					SetSelectIndex(m_nLastIndex);
					HideListWnd();
				}
			}
		}
		break;

	case VK_F4:
		Activate();
		break;
	
	}
}

LRESULT CDUIComboBox::ProcessEvent(const DUIEvent& info, BOOL& bHandled)
{
	LRESULT lRet(S_OK);
	
	switch(info.dwEventType)
	{
	case DUI_EVENT_LBUTTONDOWN:
	case DUI_EVENT_LBUTTONDBCLK:
		if(IsEnabled())	
		{
			IDUIRichEdit* pEdit = GetEdit();

			if(pEdit->IsVisible() //edit mode
				&& !pEdit->GetFocus())
			{
				pEdit->SetFocus(TRUE);
			}
			else if(!pEdit->IsVisible() //label mode
				&& !GetFocus()) 
			{
				SetFocus(TRUE);
			}
			
			if(m_pListWnd != NULL)
			{
				if( m_pListWnd->IsWindow())
				{
					HideListWnd();
				}
				else if(pEdit->GetInterface(IControl) != info.pSender)
				{
					Activate();
				}
			}

		}
		break;

	case DUI_EVENT_KEYDOWN:
		OnKeyDown(info, bHandled);
		break;

	case DUI_EVENT_KILLFOCUS:
		if(m_pListWnd != NULL && m_pListWnd->IsWindow())
		{
			m_pListWnd->PostMessage(WM_CLOSE);
		}
		break;
		
	case DUI_EVENT_MOUSEENTER:
		if(IsEnabled())
			ModifyStatus(0, CONTROL_STATUS_HOVER);
		bHandled = FALSE;
		break;
		
	case DUI_EVENT_MOUSELEAVE:
		if(IsEnabled())
			ModifyStatus(CONTROL_STATUS_HOVER, 0);
		bHandled = FALSE;
		break;	

	case DUI_EVENT_MOUSEWHEEL:
		if(m_pListWnd != NULL && m_pListWnd->IsWindow())
		{
			if(m_pList != NULL)
			{
				m_pList->OnDUIEvent(info, bHandled);
			}
		}
		bHandled = TRUE;
		break;

	case DUI_EVENT_MOUSECLICK:
		if(m_pListWnd != NULL
			&& m_pListWnd->IsWindow())
		{
			if(!PtInRect(&m_rtControl, info.ptMouse))
			{
				HideListWnd();
			}
		}
		break;

	case DUI_EVENT_WINMOVE:
	case DUI_EVENT_WINSIZE:
		if(m_pListWnd != NULL && m_pListWnd->IsWindow())
		{
			HideListWnd();
		}
		break;
		
	default:
		break;	
	}


	if(bHandled) return lRet;

	return theBase::ProcessEvent(info, bHandled);
}

VOID CDUIComboBox::SetText(const CDUIString& strText)
{
	theBase::SetText(strText);

	IDUIRichEdit* pEdit = GetEdit();
	IDUILabel* pLabel = GetLabel();
	IDUIButton* pButton = GetArrowBtn();
	if(pEdit == NULL 
		|| pLabel == NULL 
		|| pButton == NULL
		|| m_pList.IsNull())
	{
		return;
	}

	pLabel->SetText(strText);
	pEdit->SetText(strText);
}

IDUIRichEdit* CDUIComboBox::GetEditControl() const
{
	if(GetComboBoxStyle()&COMBOBOX_STYLE_DROPDOWN)
	{
		IDUIRichEdit* pEdit = GetEdit();
		if(pEdit->IsVisible())
		{
			return pEdit;
		}
	}

	return NULL;
}
VOID CDUIComboBox::SetComboListFrameBK(CRefPtr<CDUIFrameBK> pFrameBK)
{
	m_pFrameBK = pFrameBK;
}

CRefPtr<CDUIFrameBK> CDUIComboBox::GetComboListFrameBK() const
{
	return m_pFrameBK;
}

VOID CDUIComboBox::SetTextStyle(CRefPtr<CTextStyle> pTextStyle)
{
	theBase::SetTextStyle(pTextStyle);

	IDUIRichEdit* pEdit = GetEdit();
	IDUILabel* pLabel = GetLabel();
	IDUIButton* pButton = GetArrowBtn();
	if(pEdit == NULL 
		|| pLabel == NULL 
		|| pButton == NULL
		|| m_pList.IsNull())
	{
		return;
	}

	m_pList->SetTextStyle(pTextStyle);
	pLabel->SetTextStyle(pTextStyle);
	pEdit->SetTextStyle(pTextStyle);
}

BOOL CDUIComboBox::CreateCtrl()
{
	IDUIButton* pArrowBtn = new CDUIButton;
	IDUIRichEdit* pEdit = new CComboEdit;
	IDUILabel* pLabel = new CDUILabel;

	m_pList = new CDUIListBox;

	if(pArrowBtn == NULL
		|| pEdit == NULL
		|| pLabel == NULL
		|| m_pList == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	AddControl(pEdit);
	AddControl(pLabel);
	AddControl(pArrowBtn);

	pArrowBtn->ModifyStyle(CONTROL_STYLE_TABSTOP, 0);
	pArrowBtn->SetButtonStyle(BTN_STYLE_CENTER_ICON | BTN_STYLE_NOTEXT);

	pEdit->SetEditStyle(ES_AUTOHSCROLL);

	return TRUE;
}

VOID CDUIComboBox::InitStatus()
{
	m_dwComboBoxStyle = 0;

	SetComboBoxStyle(COMBOBOX_STYLE_DROPDOWN);
}

IDUIButton* CDUIComboBox::GetArrowBtn()
{
	if(GetControlCount() >= 3)
	{
		return (IDUIButton*)GetControlByIndex(2)->GetInterface(IButton);
	}

	return NULL;
}

IDUIRichEdit* CDUIComboBox::GetEdit() const
{
	if(GetControlCount() >= 3)
	{
		return (IDUIRichEdit*)GetControlByIndex(0)->GetInterface(IRichEdit);
	}

	return NULL;
}

IDUILabel* CDUIComboBox::GetLabel()
{
	if(GetControlCount() >= 3)
	{
		return (IDUILabel*)GetControlByIndex(1)->GetInterface(ILabel);
	}

	return NULL;
}

IDUIListBox* CDUIComboBox::GetList()
{
	return m_pList.GetPtr();
}

VOID CDUIComboBox::SetComboBoxStyle(DWORD dwStyle)
{
	if(dwStyle == m_dwComboBoxStyle) return;

	m_dwComboBoxStyle = dwStyle;

	IDUILabel* pLabel = GetLabel();
	IDUIRichEdit* pEdit = GetEdit();
	if(pEdit == NULL || pLabel == NULL) return;

	if(dwStyle & COMBOBOX_STYLE_DROPDOWN)
	{
		pEdit->ModifyStyle(0, CONTROL_STYLE_VISIBLE);
		pLabel->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);

		ModifyStyle(CONTROL_STYLE_TABSTOP, 0);
		pEdit->ModifyStyle(0, CONTROL_STYLE_TABSTOP);
	}
	else if(dwStyle & COMBOBOX_STYLE_DROPLIST)
	{
		pEdit->ModifyStyle(CONTROL_STYLE_VISIBLE, 0);
		pLabel->ModifyStyle(0, CONTROL_STYLE_VISIBLE);

		ModifyStyle(0, CONTROL_STYLE_TABSTOP);//edit and combobox, only one item can tab stop
		pEdit->ModifyStyle(CONTROL_STYLE_TABSTOP, 0);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	UpdateLayout(TRUE);
}

DWORD CDUIComboBox::GetComboBoxStyle() const 
{
	return m_dwComboBoxStyle;
}

VOID CDUIComboBox::OnCreate()
{
	IDUIRichEdit* pEdit = GetEdit();
	IDUILabel* pLabel = GetLabel();
	IDUIButton* pButton = GetArrowBtn();
	if(pEdit == NULL 
		|| pLabel == NULL 
		|| pButton == NULL
		|| m_pList.IsNull())
	{
		return;
	}

	SubclassControl(pEdit, TRUE);
	SubclassControl(pLabel, TRUE);
	SubclassControl(pButton, TRUE);


	CRefPtr<CTextStyle> pTextStyle = GetTextStyle();
	SetTextStyle(pTextStyle);
	SetText(m_strText);

	m_pList->SetHoverSelect(TRUE);
	m_pList->SetExtraExpandHeight(0);
	
	if(m_pListWnd == NULL)
	{
		m_pListWnd = new CDUIComboList(this);
	}

	((CComboEdit*)pEdit)->SetOwner(this);
}

VOID CDUIComboBox::OnDestroy()
{
	IDUIRichEdit* pEdit = GetEdit();
	IDUILabel* pLabel = GetLabel();
	IDUIButton* pButton = GetArrowBtn();
	if(pEdit == NULL 
		|| pLabel == NULL 
		|| pButton == NULL
		|| m_pList.IsNull())
	{
		return;
	}

	SubclassControl(pEdit, FALSE);
	SubclassControl(pLabel, FALSE);
	SubclassControl(pButton, FALSE);

	if(m_pListWnd != NULL)
	{
		if(m_pListWnd->IsWindow())
		{
			m_pListWnd->SendMessage(WM_CLOSE);
		}

		delete m_pListWnd;
		m_pListWnd = NULL;
	}

}

VOID CDUIComboBox::PaintBkgnd(HDC dc)
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


BOOL CDUIComboBox::AddListItem(CRefPtr<IDUIControl> pItem)
{
	if(m_pList != NULL)
	{
		return m_pList->AddControl(pItem);
	}

	DUI_ASSERT(FALSE);
	return FALSE;
}

INT CDUIComboBox::GetListItemCount() const
{
	if(m_pList != NULL)
	{
		return m_pList->GetControlCount();
	}

	DUI_ASSERT(FALSE);
	return 0;
}

IDUIControl* CDUIComboBox::GetListItemByIndex(INT nIndex)
{
	if(m_pList != NULL)
	{
		return m_pList->GetControlByIndex(nIndex);
	}

	DUI_ASSERT(FALSE);
	return NULL;
}

VOID CDUIComboBox::RemoveListItem(CRefPtr<IDUIControl> pItem)
{
	if(m_pList != NULL)
	{
		m_pList->RemoveControl(pItem);
	}
}

VOID CDUIComboBox::RemoveAllListItem()
{
	if(m_pList != NULL)
	{
		m_pList->RemoveAllControl();
	}
}

CRefPtr<CDUIFrameBK> CDUIComboBox::GetDefaultFrameBK()
{
	if(!m_pFrameBK.IsNull()) return m_pFrameBK;

	m_pFrameBK = new CDUIFrameBK;
	if(!m_pFrameBK.IsNull())
	{
		CARGB clrBK;
		clrBK.SetColor(RGB(255,255,255));
		m_pFrameBK->SetBKColor(clrBK);

		CARGB clrFrame;
		clrFrame.SetColor(RGB(0,0,0));
		m_pFrameBK->SetFrameColor(clrFrame);

		RECT rtBorder = {1,1,1,1};
		m_pFrameBK->SetFrameBorder(rtBorder);
	}

	return m_pFrameBK;
}

VOID CDUIComboBox::ShowListWnd()
{
	if(m_pListWnd == NULL || m_pList == NULL) return;
	if(m_pListWnd->IsWindow()) return;

	CRefPtr<CDUIFrameBK> pFrameBK = GetDefaultFrameBK();
	if(pFrameBK.IsNull()) return;

	m_pListWnd->SetFrameBkgnd(pFrameBK);

	RECT rtMarin = {2,2,2,2};
	m_pListWnd->SetClientRectMargin(rtMarin);
	
	SendNotify(WM_NOTIFY_COMBOBOX_NOTIFY, CBN_WPARAM_DROPDOWN, (LPARAM)(CDUIPopupWindow*)m_pListWnd);
	
	RECT rtFrame = pFrameBK->GetFrameBorder();
	RECT rtClientMargin = m_pListWnd->GetClietnRectMargin();

	//set region style
	INT nRoundX(0),nRoundY(0);
	m_pFrameBK->GetFrameRound(nRoundX, nRoundY);
	CRefPtr<CImageList> pBorderImage = pFrameBK->GetFrameImage();
	if((!pBorderImage.IsNull() && !pBorderImage->IsNull())
		||(nRoundX > 0 && nRoundY > 0))
	{
		m_pListWnd->SetDUIStyle(0, DUI_WS_CUSTOM_NC | DUI_WS_NC_RGN);
	}
	//

	POINT ptList = { m_rtControl.left, m_rtControl.bottom };
	HWND hWndMain = GetManagerWnd(FALSE);
	ClientToScreen(hWndMain, &ptList);
	RECT rtList = { ptList.x, ptList.y, ptList.x + m_rtControl.right - m_rtControl.left, ptList.y};

	INT nExtraHeight = (rtFrame.top + rtClientMargin.top) + (rtFrame.bottom + rtClientMargin.bottom);
	INT nMaxHeight = ((CDUIListBox*)m_pList.GetPtr())->CalculateTotalHeight();
	nMaxHeight += nExtraHeight;
	if(nMaxHeight < m_nListHeight)
	{
		rtList.bottom = rtList.top + nMaxHeight;
	}
	else
	{
		INT nItemHeight = 0;
		if(m_pList->GetControlCount() > 0)
		{
			IDUIListItem* pItem = (IDUIListItem*)m_pList->GetControlByIndex(0)->GetInterface(IListItem);
			if(pItem != NULL)
			{
				nItemHeight = pItem->GetHeight(FALSE);
			}
		}

		if(nItemHeight != 0) //should always show the whole list item
		{
			INT nTempCount = m_nListHeight / nItemHeight;
			if(nTempCount == 0) nTempCount = 1;
			rtList.bottom = rtList.top + nItemHeight * nTempCount + nExtraHeight;
		}
		else
		{
			rtList.bottom = rtList.top + m_nListHeight;
		}
	}

	CRefPtr<IDUIControl> pRootControl = (IDUIControl*)m_pList->GetInterface(IControl);
	m_pListWnd->CreateFromRootControl(pRootControl, GetManagerWnd(TRUE), rtList, NULL, WS_POPUP,
		WS_EX_NOACTIVATE | WS_EX_TOPMOST);
	
	BeforeShowListWnd();

	m_pListWnd->ShowWindow(SW_SHOWNOACTIVATE);
	m_pListWnd->UpdateWindow();
}

VOID CDUIComboBox::HideListWnd()
{
	if(m_pListWnd == NULL) return;

	if(m_pListWnd->IsWindow())
	{
		m_pListWnd->PostMessage(WM_CLOSE);
	}
}

VOID CDUIComboBox::BeforeHideListWnd()
{
	m_nLastIndex = -1;
	SendNotify(WM_NOTIFY_COMBOBOX_NOTIFY, CBN_WPARAM_CLOSEUP, (LPARAM)(CDUIPopupWindow*)m_pListWnd);
}

VOID CDUIComboBox::BeforeShowListWnd()
{
	if(m_pList == NULL) return;
	INT nCount = m_pList->GetControlCount();
	INT nIndex(-1);
	for(INT i=0; i<nCount; ++i)
	{
		IDUIControl* pItem = m_pList->GetControlByIndex(i);
		IDUIListItem* pListItem = (IDUIListItem*)pItem->GetInterface(IListItem);
		if(pListItem == NULL || !pListItem->IsVisible()) continue;

		CDUIString strText = pListItem->GetText();
		if(m_strText.compare(strText) == 0)
		{
			nIndex = i;
			break;
		}
	}

	if(nIndex != -1)
	{
		SetSelectIndex(nIndex);
		m_nLastIndex = nIndex;
	}
}


//nType: 0-top_index, 1-down, 2-up
BOOL CDUIComboBox::SetSelectIndexWrapper(INT nIndex, INT nType)
{
	if(m_pList == NULL) return FALSE;

	if(nIndex >=0 && nIndex<m_pList->GetControlCount())
	{
		m_pList->SetCurSel(nIndex);
		if(nType == 0)
		{
			m_pList->SetTopIndex(nIndex);
		}
		else if(nType == 1)
		{
			if(nIndex > m_pList->GetLastIndex())
			{
				INT nTop = m_pList->GetTopIndex();
				m_pList->SetTopIndex(++nTop);
			}			
		}
		else if(nType == 2)
		{
			INT nTop = m_pList->GetTopIndex();
			if(nIndex < nTop)
			{
				m_pList->SetTopIndex(--nTop);
			}
		}
		else
		{
			DUI_ASSERT(FALSE);
		}

		IDUIControl* pControl = m_pList->GetControlByIndex(nIndex);
		if(pControl != NULL)
		{
			SetText(pControl->GetText());
			SendNotify(WM_NOTIFY_COMBOBOX_NOTIFY, CBN_WPARAM_SELCHANGE, nIndex);
		}
		
		IDUIRichEdit* pEdit = GetEdit();
		if(pEdit  != NULL && pEdit->IsVisible())
		{
			pEdit->SetSelAll();
		}

		return TRUE;
	}

	return FALSE;
}
BOOL CDUIComboBox::SetSelectIndex(INT nIndex)
{
	return SetSelectIndexWrapper(nIndex, 0);
}

INT CDUIComboBox::GetSelectIndex() const 
{
	if(m_pList != NULL)
	{
		return m_pList->GetCurSel();
	}

	return -1;
}


VOID CDUIComboBox::OnTxNotify(DWORD iNotify, void *pv)
{
	SendNotify(WM_NOTIFY_EDIT_TX_NOTIFY, iNotify, (LPARAM)pv);
}

////////////////  CDUIComboBox  ///////////////////

DUI_END_NAMESPCE