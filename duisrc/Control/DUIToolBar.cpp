#include "DUIToolBar.h"

DUI_BGN_NAMESPCE

///////////////   CDUITabItem   //////////////////////
CDUITabItem::CDUITabItem()
{
	m_dwStyle &= ~CONTROL_STYLE_TABSTOP;
}

LPVOID CDUITabItem::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ITabItem) == 0)
	{
		return (IDUITabItem*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUITabItem::Clone()
{
	return new CDUITabItem(*this);
}

BOOL CDUITabItem::Activate()
{
	if(!theBase::Activate()) return FALSE;

	if(m_pParentControl != NULL)
	{
		DUIEvent event = {0};
		event.pSender = this;
		event.dwEventType = DUI_EVENT_ELE_ITEM_CLICK;

		BOOL bHandled(FALSE);
		m_pParentControl->OnDUIEvent(event, bHandled);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	return TRUE;
}
///////////////   CDUITabItem   //////////////////////



DUI_END_NAMESPCE