#ifndef __DUI_TOOL_BAR_H__
#define __DUI_TOOL_BAR_H__

#include "DUIControlContainer.h"
#include "DUIButton.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUITabItem: public CDUIButton
{
public:
	virtual ~IDUITabItem() { NULL; }
};

class DUILIB_API CDUITabItem: public IDUITabItem
{ 
public:
	typedef IDUITabItem theBase;
	CDUITabItem();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	virtual BOOL Activate();
};

class DUILIB_API IDUITabBar
{
public:
	virtual ~IDUITabBar() { NULL; }

	virtual IDUITabLayout* GetTabLayout() = 0;
	virtual VOID SetTabLayout(IDUITabLayout* pTabLayout) = 0;

	virtual BOOL SetSelectIndex(INT nIndex) = 0;
	virtual INT GetSelectIndex() const = 0;
	virtual BOOL SelectNextItem(BOOL bNext, BOOL bCircle = FALSE) = 0;
};

template<typename TBase>
class DUILIB_API CDUITabBar: public TBase,
							 public IDUITabBar
{
public:
	typedef TBase theBase;
	CDUITabBar()
	{
		m_pTabLayout = NULL;
		m_nSelectIndex = 0;

		m_dwStyle |= CONTROL_STYLE_TABSTOP;
	}


	virtual LPVOID GetInterface(const CDUIString& strName)
	{
		if(strName.compare(ITabBar) == 0)
		{
			return (IDUITabBar*)this;
		}

		return theBase::GetInterface(strName);
	}

	virtual CRefPtr<IDUIControl> Clone()
	{
		return new CDUITabBar(*this);
	}

	
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue)
	{
		BOOL bHandled(FALSE);

		if(strName.compare(_T("select_index")) == 0)
		{
			m_nSelectIndex = _ttol(strValue.c_str());
			bHandled = TRUE;
		}
		else if(strName.compare(_T("notifier")) == 0)
		{
			m_strTabLayoutName = strValue;
			bHandled = TRUE;
		}

		if(!bHandled)
		{
			theBase::SetAttribute(strName, strValue);
		}
	}

	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled)
	{
		LRESULT lRet(S_OK);

		switch(info.dwEventType)
		{
		case DUI_EVENT_ELE_ITEM_CLICK:
			OnTabButtonClick(info.pSender);
			bHandled = TRUE;
			break;

		case DUI_EVENT_KEYDOWN:
			OnkeyDownEvent(info);
			bHandled = TRUE;
			break;

		case DUI_EVENT_LBUTTONDBCLK:
		case DUI_EVENT_LBUTTONDOWN:
			if(!GetFocus())
			{
				SetFocus(TRUE);
			}
			bHandled =TRUE;
			break;
		}

		if(bHandled) return lRet;

		return theBase::ProcessEvent(info, bHandled);
	}

	//IDUITabBar
	virtual IDUITabLayout* GetTabLayout()
	{
		if(m_pTabLayout == NULL)
		{
			if(m_strTabLayoutName.size() > 0)
			{
				IDUIControl* pControl = GetSiblingControl(m_strTabLayoutName);
				if(pControl != NULL)
				{
					m_pTabLayout = (IDUITabLayout*)pControl->GetInterface(ITabLayout);
				}
			}
		}

		return m_pTabLayout;
	}		

	virtual VOID SetTabLayout(IDUITabLayout* pTabLayout)
	{
		m_pTabLayout = pTabLayout;
	}	

	virtual BOOL SetSelectIndex(INT nIndex)
	{
		if(nIndex != m_nSelectIndex)
		{
			INT nCount = GetControlCount();
			if(nIndex >= 0 && nIndex < nCount)
			{
				m_nSelectIndex = nIndex;
				
				UpdateButtons();

				IDUITabLayout* pTabLayout = GetTabLayout();
				if(pTabLayout != NULL)
				{
					pTabLayout->SetSelectIndex(nIndex);
				}
				return TRUE;
			}
		}

		return FALSE;
	}

	virtual INT GetSelectIndex() const
	{
		return m_nSelectIndex;
	}

	virtual BOOL SelectNextItem(BOOL bNext, BOOL bCircle = FALSE)
	{
		INT nIndex = GetNextTabItem(bNext, bCircle);
		if(nIndex != m_nSelectIndex)
		{
			SetSelectIndex(nIndex);
			return TRUE;
		}

		return FALSE;
	}

protected:
	virtual BOOL BeforeAddControl(CRefPtr<IDUIControl> pItem)
	{
		if(!theBase::BeforeAddControl(pItem)) return FALSE;

		IDUITabItem* pTabItem = (IDUITabItem*)pItem->GetInterface(ITabItem);
		if(pTabItem != NULL)
		{
			SubclassControl(pTabItem, TRUE);
		}

		return TRUE;	
	}

	virtual VOID BeforeRemoveControl(CRefPtr<IDUIControl> pItem)
	{
		IDUITabItem* pTabItem = (IDUITabItem*)pItem->GetInterface(ITabItem);
		if(pTabItem != NULL)
		{
			SubclassControl(pTabItem, FALSE);
		}	
	}

	virtual VOID OnCreate()
	{
		UpdateButtons();
	}


protected:
	INT GetNextTabItem(BOOL bNext, BOOL bCircle)
	{
		INT nCurrent = m_nSelectIndex;
		INT i;
		INT nCount = GetControlCount();
		IDUITabItem* pTabItem = NULL;
		INT nIndexRet(-1);
		if(bNext)
		{
			for(i=nCurrent+1; i<nCount; ++i)
			{
				pTabItem = (IDUITabItem*)GetControlByIndex(i)->GetInterface(ITabItem);
				if(pTabItem == NULL) continue;

				if(!pTabItem->IsEnabled()) continue;
				if(!pTabItem->IsVisible()) continue;

				nIndexRet = i;
				break;
			}

			if(nIndexRet == -1 && bCircle)
			{
				for(i=0; i<nCurrent; ++i)
				{
					pTabItem = (IDUITabItem*)GetControlByIndex(i)->GetInterface(ITabItem);
					if(pTabItem == NULL) continue;

					if(!pTabItem->IsEnabled()) continue;
					if(!pTabItem->IsVisible()) continue;

					nIndexRet = i;
					break;
				}
			}
		}
		else
		{
			for(i=nCurrent-1; i>=0; --i)
			{
				pTabItem = (IDUITabItem*)GetControlByIndex(i)->GetInterface(ITabItem);
				if(pTabItem == NULL) continue;

				if(!pTabItem->IsEnabled()) continue;
				if(!pTabItem->IsVisible()) continue;

				nIndexRet = i;
				break;
			}

			if(nIndexRet == -1 && bCircle)
			{
				for(i=nCount-1; i>nCurrent; --i)
				{
					pTabItem = (IDUITabItem*)GetControlByIndex(i)->GetInterface(ITabItem);
					if(pTabItem == NULL) continue;

					if(!pTabItem->IsEnabled()) continue;
					if(!pTabItem->IsVisible()) continue;

					nIndexRet = i;
					break;
				}
			}
		}

		if(nIndexRet == -1)
		{
			nIndexRet = m_nSelectIndex;
		}

		return nIndexRet;
	}

	VOID OnkeyDownEvent(const DUIEvent& info)
	{
		INT nIndex = m_nSelectIndex;
		switch(info.wParam)
		{
		case VK_DOWN:
		case VK_RIGHT:
			nIndex = GetNextTabItem(TRUE, FALSE);
			break;

		case VK_LEFT:
		case VK_UP:
			nIndex = GetNextTabItem(FALSE, FALSE);
			break;
		}

		if(m_nSelectIndex != nIndex)
		{
			SetSelectIndex(nIndex);
		}
	}

	VOID OnTabButtonClick(const IDUIControl* pButton)
	{
		DUI_ASSERT(pButton != NULL);

		INT nCount = GetControlCount();
		for(INT i=0; i<nCount; ++i)
		{
			if(pButton == GetControlByIndex(i))
			{
				SetSelectIndex(i);
				return;
			}
		}

		DUI_ASSERT(FALSE);
	}

	VOID UpdateButtons()
	{
		INT nCount = GetControlCount();
		if(m_nSelectIndex >= 0 && m_nSelectIndex < nCount)
		{
			for(INT i=0; i<nCount; ++i)
			{
				IDUITabItem* pTabItem = (IDUITabItem*)GetControlByIndex(i)->GetInterface(ITabItem);
				if(pTabItem != NULL)
				{
					pTabItem->SetCheck(i==m_nSelectIndex);
				}
			}
		}
	}

protected:
	IDUITabLayout* m_pTabLayout;
	CDUIString m_strTabLayoutName;

	INT m_nSelectIndex;
};

typedef CDUITabBar<CDUIHorizontalLayout> CDUIHorizontalTabBar;
typedef CDUITabBar<CDUIVerticalLayout>	 CDUIVerticalTabBar;


template<typename TBase>
class DUILIB_API CDUIToolBar: public TBase
{ };

typedef CDUIToolBar<CDUIHorizontalLayout> CDUIHorizontalToolBar;
typedef CDUIToolBar<CDUIVerticalLayout>	 CDUIVerticalToolBar;

DUI_END_NAMESPCE


#endif //__DUI_TOOL_BAR_H__