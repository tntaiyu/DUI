#include "DUIControlBuilder.h"
#include "DUIXml.h"
#include "DUIComboBox.h"

DUI_BGN_NAMESPCE

BOOL DefAddChildControl(CRefPtr<IDUIControl> pParent, CRefPtr<IDUIControl> pChild)
{
	IDUIComboBox* pComboBox = (IDUIComboBox*)pParent->GetInterface(IComboBox);
	if(pComboBox != NULL)
	{
		pComboBox->AddListItem(pChild);
	}
	else
	{
		pParent->AddControl(pChild);
	}
	
	return TRUE;
}

//////////////////  IDUIControlBuilderCallback  ////////////////////////////////
BOOL IDUIControlBuilderCallback::OnAddChildControl(CRefPtr<IDUIControl> pParent, CRefPtr<IDUIControl> pChild)
{
	return DefAddChildControl(pParent, pChild);	
}
//////////////////  IDUIControlBuilderCallback  ////////////////////////////////

VOID XmlControlParseChild(CDUIControlBuilder* pThis, CDUIXml& xml, CRefPtr<IDUIControl>  pParentControl)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return;
	
	IDUIControlFactory* pControlFactory = pDUIApp->GetControlFactory();
	if(pControlFactory == NULL) return;
	
	CDUIString strControlName, strAttriName, strAttriValue;
	CRefPtr<IDUIControl> pControl;
	INT nIndex(0);
	while(xml.FindChildElem())
	{
		xml.IntoElem();
		
		strControlName = xml.GetTagName();
		pControl = pControlFactory->NewControl(strControlName);
		if(pControl.IsNull() && pThis->GetCallBack() != NULL)
		{
			pControl = pThis->GetCallBack()->OnBuilderNewControl(strControlName);
		}
		DUI_ASSERT(!pControl.IsNull());
		
		if(!pControl.IsNull())
		{
			nIndex = 0;
			do 
			{
				strAttriName = xml.GetAttribName(nIndex++);
				if(strAttriName.size() <= 0) break;
				
				strAttriValue = xml.GetAttrib(strAttriName);
				pControl->SetAttribute(strAttriName, strAttriValue);
				
			} while(TRUE);
			
			if(pThis->GetCallBack() != NULL)
			{
				pThis->GetCallBack()->OnAddChildControl(pParentControl, pControl);
			}
			else
			{
				DefAddChildControl(pParentControl, pControl);
			}
			
			
			XmlControlParseChild(pThis, xml, pControl);
		}
		
		xml.OutOfElem();
	}
}


CRefPtr<IDUIControl> XmlControlParse(CDUIControlBuilder* pThis, CDUIXml& xml)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return NULL;
	
	IDUIControlFactory* pControlFactory = pDUIApp->GetControlFactory();
	if(pControlFactory == NULL) return NULL;
	
	if(!xml.FindElem(_T("window")))
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}
	
	//parse window attribute
	CDUIString strAttriName, strAttriValue;
	INT nIndex(0);
	do 
	{
		strAttriName = xml.GetAttribName(nIndex++);
		if(strAttriName.size() <= 0) break;
		
		strAttriValue = xml.GetAttrib(strAttriName);
		if(pThis->GetCallBack() != NULL)
		{
			pThis->GetCallBack()->OnBuilderWindowAttribute(strAttriName, strAttriValue);
		}
		
	} while(TRUE);
	
	//parse child control
	CDUIString strControlName;
	CRefPtr<IDUIControl> pRootControl;
	if(xml.FindChildElem(NULL))
	{
		xml.IntoElem();

		strControlName = xml.GetTagName();
		pRootControl = pControlFactory->NewControl(strControlName);
		if(pRootControl.IsNull() && pThis->GetCallBack() != NULL)
		{
			pRootControl = pThis->GetCallBack()->OnBuilderNewControl(strControlName);
		}
		DUI_ASSERT(!pRootControl.IsNull());

		if(!pRootControl.IsNull())
		{
			nIndex = 0;
			do 
			{
				strAttriName = xml.GetAttribName(nIndex++);
				if(strAttriName.size() <= 0) break;

				strAttriValue = xml.GetAttrib(strAttriName);
				pRootControl->SetAttribute(strAttriName, strAttriValue);
				
			} while(TRUE);

			XmlControlParseChild(pThis, xml, pRootControl);

			if(pThis->GetCallBack() != NULL)
			{
				pThis->GetCallBack()->OnBuilderControlCreated(pRootControl);
			}
		}
		
		xml.OutOfElem();
	}

	return pRootControl;
}


///////////////////////////   CDUIControlBuilder  /////////////////////////////////
CDUIControlBuilder::CDUIControlBuilder(IDUIControlBuilderCallback* pCallback/* = NULL*/)
{
	m_pCallBack = pCallback;
}

CRefPtr<IDUIControl> CDUIControlBuilder::CreateFromXML(const CDUIString&  strTemplateXML)
{
	CDUIXml xml;

	if(!xml.SetDoc(strTemplateXML))
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}
	else
	{
		return XmlControlParse(this, xml);
	}
}

CRefPtr<IDUIControl> CDUIControlBuilder::CreateFromResource(const CDUIString& strTemplateID)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return NULL;

	IDUISkinMgr* pSkinMgr = pDUIApp->GetSkinMgr();
	if(pSkinMgr == NULL) return NULL;

	CDUIString strXML;
	if(!pSkinMgr->GetWindowTemplate(strTemplateID, strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	return CreateFromXML(strXML);
}

IDUIControlBuilderCallback* CDUIControlBuilder::GetCallBack() const
{
	return m_pCallBack;
}
///////////////////////////   CDUIControlBuilder  /////////////////////////////////




VOID XmlMenuParseChild(CDUIMenuBuilder* pThis, CDUIXml& xml, CRefPtr<IDUIControl>  pParentControl)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return;
	
	IDUIControlFactory* pControlFactory = pDUIApp->GetControlFactory();
	if(pControlFactory == NULL) return;
	
	CDUIString strControlName, strAttriName, strAttriValue;
	CRefPtr<IDUIControl> pControl;
	INT nIndex(0);
	while(xml.FindChildElem())
	{
		xml.IntoElem();
		
		strControlName = xml.GetTagName();
		pControl = pControlFactory->NewControl(strControlName);
		if(pControl.IsNull() && pThis->GetCallBack() != NULL)
		{
			pControl = pThis->GetCallBack()->OnBuilderNewControl(strControlName);
		}
		DUI_ASSERT(!pControl.IsNull());
		
		if(!pControl.IsNull())
		{
			DUI_ASSERT(pControl->GetInterface(IMenu) != NULL 
				|| pControl->GetInterface(IMenuItem) != NULL);

			nIndex = 0;
			do 
			{
				strAttriName = xml.GetAttribName(nIndex++);
				if(strAttriName.size() <= 0) break;
				
				strAttriValue = xml.GetAttrib(strAttriName);
				pControl->SetAttribute(strAttriName, strAttriValue);
				
			} while(TRUE);
			
			if(pThis->GetCallBack() != NULL)
			{
				pThis->GetCallBack()->OnAddChildControl(pParentControl, pControl);
			}
			else
			{
				DefAddChildControl(pParentControl, pControl);
			}
			
			
			XmlMenuParseChild(pThis, xml, pControl);
		}
		
		xml.OutOfElem();
	}
}


CRefPtr<IDUIMenu> XmlControlParse(CDUIMenuBuilder* pThis, CDUIXml& xml)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return NULL;
	
	IDUIControlFactory* pControlFactory = pDUIApp->GetControlFactory();
	if(pControlFactory == NULL) return NULL;
	
	if(!xml.FindElem(_T("menu")))
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}
	CRefPtr<IDUIControl> pRootControl = pControlFactory->NewControl(_T("menu"));
	DUI_ASSERT(!pRootControl.IsNull());
	if(pRootControl.IsNull()) return NULL;

	//parse menu attribute
	CDUIString strAttriName, strAttriValue;
	INT nIndex(0);
	do 
	{
		strAttriName = xml.GetAttribName(nIndex++);
		if(strAttriName.size() <= 0) break;
		
		strAttriValue = xml.GetAttrib(strAttriName);
		pRootControl->SetAttribute(strAttriName, strAttriValue);
		
	} while(TRUE);

	XmlMenuParseChild(pThis, xml, pRootControl);

	if(pThis->GetCallBack() != NULL)
	{
		pThis->GetCallBack()->OnBuilderControlCreated(pRootControl);
	}


	IDUIMenu* pMenuRet =  (IDUIMenu*)pRootControl->GetInterface(IMenu);
	DUI_ASSERT(pMenuRet != NULL);

	return pMenuRet;
}



///////////////////////////   CDUIMenuBuilder  /////////////////////////////////
CDUIMenuBuilder::CDUIMenuBuilder(IDUIControlBuilderCallback* pCallback/* = NULL*/)
{
	m_pCallBack = pCallback;
}

CRefPtr<IDUIMenu> CDUIMenuBuilder::CreateFromXML(const CDUIString&  strTemplateXML)
{
	CDUIXml xml;
	
	if(!xml.SetDoc(strTemplateXML))
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}
	else
	{
		return XmlControlParse(this, xml);
	}
}

CRefPtr<IDUIMenu> CDUIMenuBuilder::CreateFromResource(const CDUIString& strTemplateID)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return NULL;
	
	IDUISkinMgr* pSkinMgr = pDUIApp->GetSkinMgr();
	if(pSkinMgr == NULL) return NULL;
	
	CDUIString strXML;
	if(!pSkinMgr->GetMenuTemplate(strTemplateID, strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	return CreateFromXML(strXML);
}

IDUIControlBuilderCallback* CDUIMenuBuilder::GetCallBack() const
{
	return m_pCallBack;
}
///////////////////////////   CDUIControlBuilder  /////////////////////////////////

DUI_END_NAMESPCE