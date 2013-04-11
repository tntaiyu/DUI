#include "DUIControlFactory.h"
#include "DUIControlBase.h"
#include "DUIXml.h"
#include "DUIResLoader.h"


DUI_BGN_NAMESPCE

BOOL RegisterDUIDefaultControls();

CDUIControlFactory::CDUIControlFactory()
{
	m_hInstanceRichEdit = NULL;
}

CDUIControlFactory::~CDUIControlFactory()
{
	UninitializeFactory();
}


BOOL CDUIControlFactory::InitializeFactory()
{
	if(m_hInstanceRichEdit == NULL)
	{
		m_hInstanceRichEdit = LoadLibrary(RICHEDIT_DLL_NAME);
	}
	DUI_ASSERT(m_hInstanceRichEdit != NULL);

	if(!RegisterDUIDefaultControls()) return FALSE;
	
	ApplyDefaultControlStyle();

	return TRUE;
}

VOID CDUIControlFactory::UninitializeFactory()
{
	if(!m_mapControl.empty())
	{
		m_mapControl.clear();
	}

	if(m_hInstanceRichEdit != NULL)
	{
		FreeLibrary(m_hInstanceRichEdit);
		m_hInstanceRichEdit = NULL;
	}
}

BOOL CDUIControlFactory::RegisterControl(const CDUIString& strName,
									  CRefPtr<IDUIControl> pControl)
{
	DUI_ASSERT(!pControl.IsNull());
	DUI_ASSERT(strName.size() > 0);

	if(!pControl.IsNull() && strName.size() > 0)
	{
		m_mapControl.insert(std::make_pair(strName, pControl));

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CDUIControlFactory::UnregisterControl(const CDUIString& strName)
{
	m_mapControl.erase(strName);

	return TRUE;
}

CRefPtr<IDUIControl> CDUIControlFactory::NewControl(const CDUIString& strName)
{
	CControlMap::iterator itr = m_mapControl.find(strName);
	if(itr == m_mapControl.end())
	{
		CRefPtr<IDUIControl> pPlugin = CreateFromPlugin(strName);
		DUI_ASSERT(!pPlugin.IsNull());
		if(!pPlugin.IsNull())
		{
			RegisterControl(strName, pPlugin);
			return pPlugin->Clone();
		}
		else
		{
			return NULL;
		}
	}
	
	CRefPtr<IDUIControl> pControl = (*itr).second->Clone();
	if(pControl == NULL) return NULL;
	
	return pControl;
}

VOID CDUIControlFactory::CheckControlStyle(CDUIString& strControlName)
{
	CControlMap::iterator itr = m_mapControl.find(strControlName);
	if(itr == m_mapControl.end())
	{
		DUI_ASSERT(FALSE);
		return;
	}

	CRefPtr<IDUIControl> pControl = itr->second;
	DUI_ASSERT(!pControl.IsNull());


	pControl->SetAttribute(_T("control_style"), strControlName);
}

VOID CDUIControlFactory::ApplyDefaultControlStyle()
{
	CDUIString strVertSB(DUI_VERT_SCROLLBAR);
	CheckControlStyle(strVertSB);
	
	CDUIString strHorzSB(DUI_HORZ_SCROLLBAR);
	CheckControlStyle(strHorzSB);	
}

BOOL CDUIControlFactory::GetPlugin(const CDUIString& strName, CDUIString& strDllName, CDUIString& strEntryFunction)
{
	IDUISkinMgr* pSkinMgr = GetSkinMgr();
	if(pSkinMgr == NULL) return FALSE;

	CDUIString strPluginXML;
	if(!pSkinMgr->GetPlugin(strName, strPluginXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	CDUIXml xml;
	if(!xml.SetDoc(strPluginXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	if(!xml.FindElem(_T("plugin")))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	strDllName = xml.GetAttrib(_T("file_name"));
	strEntryFunction = xml.GetAttrib(_T("entry_function"));
	if(strDllName.size() == 0
		|| strEntryFunction.size() == 0)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}

CRefPtr<IDUIControl> CDUIControlFactory::CreateFromPlugin(const CDUIString& strName)
{
	CDUIString strDllName, strEntryFunction;
	if(!GetPlugin(strName, strDllName, strEntryFunction))
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}

	CResLoader resLoader;
	HINSTANCE hPlugin = resLoader.LoadLibrary(strDllName);
	if(hPlugin == NULL) 
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}

	typedef BOOL (WINAPI* CreateObjectProc)(VOID** ppPlugin);

	char szFunName[256] = {0};
	WideCharToMultiByte(CP_ACP, 0, strEntryFunction.c_str(), 
		strEntryFunction.size(), szFunName, sizeof(szFunName)/sizeof(char), NULL, NULL);
	CreateObjectProc pFun = (CreateObjectProc)GetProcAddress(hPlugin, szFunName);
	if(pFun == NULL)
	{
		DUI_ASSERT(FALSE);
		return NULL;
	}

	IDUIControl* pControlRet = NULL;
	BOOL bRet = pFun((VOID**)&pControlRet);
	return bRet ? pControlRet : NULL;
}

DUI_END_NAMESPCE