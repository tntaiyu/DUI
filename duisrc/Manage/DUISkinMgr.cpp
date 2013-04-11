#include "DUISkinMgr.h"
#include "DUIXml.h"
#include <algorithm>

DUI_BGN_NAMESPCE



/////////////////  CDUISkinMgr  //////////////////
CDUISkinMgr::CDUISkinMgr()
{

}

CDUISkinMgr::~CDUISkinMgr()
{

}

BOOL CDUISkinMgr::InitializeSkin(const CDUIString& lpszConfigXML)
{
	if(!ParseEntryXML(lpszConfigXML))
	{
		CTrace::TraceError(_T("CDUISkinMgr::InitializeSkin, ParseEntryXML failed"));
		return FALSE;
	}
	
	if(!LoadLanguage())
	{
		CTrace::TraceError(_T("CDUISkinMgr::InitializeSkin, Load Language failed"));
		return FALSE;
	}
	
	if(!LoadDUIResource())
	{
		CTrace::TraceError(_T("CDUISkinMgr::InitializeSkin, Load Resource failed"));
		return FALSE;
	}

	if(!LoadTemplateStyle())
	{
		CTrace::TraceError(_T("CDUISkinMgr::InitializeSkin, Load template failed"));
		return FALSE;
	}

	return TRUE;
}

VOID CDUISkinMgr::UninitializeSkin()
{
	m_ResMgr.ClearAllString();
	m_ResMgr.ClearDUIResource();
	m_ResMgr.ClearTemplate();
	m_arLanguage.clear();
	m_arTemplate.clear();
}

BOOL CDUISkinMgr::SetSkinUIData(CRefPtr<CDUIFrameBK> pSkinUIData)
{
	DUI_ASSERT(pSkinUIData != NULL);
	if(pSkinUIData == NULL) return FALSE;

	m_pSkinUIData = pSkinUIData;
	typedef std::vector<IDUISkinObserver*>::iterator ITR_SKIN;
	std::vector<IDUISkinObserver*> vecTemp(m_arSkinObserver);
	for(ITR_SKIN itr1 = vecTemp.begin(); itr1 != vecTemp.end(); ++itr1)
	{
		DUI_ASSERT(*itr1 != NULL);
		(*itr1)->OnSkinUIDataChanged(pSkinUIData);
	}

	return TRUE;
}

const CRefPtr<CDUIFrameBK> CDUISkinMgr::GetSKinUIData() const 
{
	return m_pSkinUIData;
}

BOOL CDUISkinMgr::SetLanguageType(const CDUIString& strLanType)
{
	if(lstrcmpi(strLanType.c_str(), m_strLanType.c_str()) == 0)
	{
		return FALSE;
	}
	CDUIString strOldLan(m_strLanType);

	typedef CResourceFileInfoVector::iterator ITR;
	ITR itr;
	BOOL bFind(FALSE);
	for(itr = m_arLanguage.begin(); itr != m_arLanguage.end(); ++itr)
	{
		if(lstrcmpi(itr->m_strName.c_str(), strLanType.c_str()) == 0)
		{
			bFind = TRUE;
			break;
		}
	}

	if(!bFind) return FALSE;
	
	m_ResMgr.ClearAllString();
	m_ResMgr.LoadAllString(*itr);
	m_strLanType = strLanType;

	typedef std::vector<IDUILanguageObserver*>::iterator ITR_LAN;
	std::vector<IDUILanguageObserver*> vecTemp(m_arLanObserver);
	for(ITR_LAN itr1 = vecTemp.begin(); itr1 != vecTemp.end(); ++itr1)
	{
		DUI_ASSERT(*itr1 != NULL);
		(*itr1)->OnLanguageTypeChanged(strOldLan, m_strLanType);
	}

	return TRUE;
}

CDUIString CDUISkinMgr::GetLanguageType() const
{
	return m_strLanType;
}

BOOL CDUISkinMgr::AddSkinObserver(IDUISkinObserver* pObserver)
{
	if(std::find(m_arSkinObserver.begin(), m_arSkinObserver.end(), pObserver) == m_arSkinObserver.end())
	{
		m_arSkinObserver.push_back(pObserver);
	}
	
	return TRUE;
}

BOOL CDUISkinMgr::RemoveSkinObserver(IDUISkinObserver* pObserver)
{
	m_arSkinObserver.erase(std::remove(m_arSkinObserver.begin(), m_arSkinObserver.end(), pObserver), m_arSkinObserver.end());
	return TRUE;
}

BOOL CDUISkinMgr::AddLanguageObserver(IDUILanguageObserver* pObserver)
{
	if(std::find(m_arLanObserver.begin(), m_arLanObserver.end(), pObserver) == m_arLanObserver.end())
	{
		m_arLanObserver.push_back(pObserver);
	}

	return TRUE;
}

BOOL CDUISkinMgr::RemoveLanguageObserver(IDUILanguageObserver* pObserver)
{
	m_arLanObserver.erase(std::remove(m_arLanObserver.begin(), m_arLanObserver.end(), pObserver), m_arLanObserver.end());
	return TRUE;
}

CRefPtr<CImageList> CDUISkinMgr::GetImage(const CDUIString& strID, BOOL bCache /* = TRUE */)
{
	return m_ResMgr.GetImage(strID, bCache);
}

CRefPtr<CCursorObject> CDUISkinMgr::GetCursor(const CDUIString& strID, BOOL bCache /* = TRUE */)
{
	return m_ResMgr.GetCursor(strID, bCache);
}

CRefPtr<CFontObject> CDUISkinMgr::GetFont(const CDUIString& strID, BOOL bCache /* = TRUE */)
{
	return m_ResMgr.GetFont(strID, bCache);
}

CARGB CDUISkinMgr::GetColor(const CDUIString& strID)
{
	return m_ResMgr.GetColor(strID);
}

BOOL CDUISkinMgr::GetWindowTemplate(const CDUIString& strID, CDUIString& strTemplateXML)
{
	return m_ResMgr.GetWindowTemplate(strID, strTemplateXML);
}

BOOL CDUISkinMgr::GetMenuTemplate(const CDUIString& strID, CDUIString& strTemplateXML)
{
	return m_ResMgr.GetMenuTemplate(strID, strTemplateXML);
}

BOOL CDUISkinMgr::GetControlStyle(const CDUIString& strID, CDUIString& strStyleXML)
{
	return m_ResMgr.GetControlStyle(strID, strStyleXML);
}

BOOL CDUISkinMgr::GetPlugin(const CDUIString& strID, CDUIString& strPluginXML)
{
	return m_ResMgr.GetPlugin(strID, strPluginXML);
}

BOOL CDUISkinMgr::GetStringText(const CDUIString& strID, CDUIString& strContent)
{
	return m_ResMgr.GetString(strID, strContent);
}

CRefPtr<CTextStyle> CDUISkinMgr::GetTextStyle(const CDUIString& strID)
{
	return m_ResMgr.GetTextStyle(strID);
}

CRefPtr<CDUIFrameBK> CDUISkinMgr::GetFrameBK(const CDUIString& strName)
{
	return m_ResMgr.GetFrameBK(strName);
}

BOOL CDUISkinMgr::ParseEntryXML(const CDUIString& lpszEntryXML)
{
 	CDUIXml xml;
	if(!xml.SetDoc(lpszEntryXML)) return FALSE;
	
	m_arLanguage.clear();
	m_arTemplate.clear();

	if(xml.FindChildElem(_T("language_set")))
	{
		CDUIString strDefault = xml.GetChildAttrib(_T("default"));
		if(strDefault.size() > 0 && m_strLanType.size() <= 0)
		{
			m_strLanType = strDefault;
		}

		xml.IntoElem();
		while(xml.FindChildElem(_T("language")))
		{
			CDUIString strName = xml.GetChildAttrib(_T("name"));
			CDUIString strType = xml.GetChildAttrib(_T("config_type"));
			CDUIString strFileName = xml.GetChildAttrib(_T("file_name"));
			CDUIString strResID = xml.GetChildAttrib(_T("resource_id"));
			CDUIString strResType = xml.GetChildAttrib(_T("resource_type"));

			CResourceFileInfo info;
			info.m_strName = strName;
			info.m_strFileType = strType;
			info.m_strFileName = strFileName;
			info.m_strResType = strResType;
			info.m_strResID = strResID;

			m_arLanguage.push_back(info);
		}
		xml.OutOfElem();
	}
	
	xml.ResetMainPos();

	if(xml.FindChildElem(_T("skin_set")))
	{


		xml.IntoElem();
		if(xml.FindChildElem(_T("skin")))
		{
			CDUIString strName = xml.GetChildAttrib(_T("name"));
			CDUIString strType = xml.GetChildAttrib(_T("config_type"));
			CDUIString strFileName = xml.GetChildAttrib(_T("file_name"));
			CDUIString strResID = xml.GetChildAttrib(_T("resource_id"));
			CDUIString strResType = xml.GetChildAttrib(_T("resource_type"));

			m_skinResourceInfo.m_strName = strName;
			m_skinResourceInfo.m_strFileType = strType;
			m_skinResourceInfo.m_strFileName = strFileName;
			m_skinResourceInfo.m_strResType = strResType;
			m_skinResourceInfo.m_strResID = strResID;
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
		xml.OutOfElem();
	}

	xml.ResetMainPos();

	if(xml.FindChildElem(_T("template_set")))
	{
		xml.IntoElem();
		while(xml.FindChildElem(_T("template")))
		{
			CDUIString strName = xml.GetChildAttrib(_T("name"));
			CDUIString strType = xml.GetChildAttrib(_T("config_type"));
			CDUIString strFileName = xml.GetChildAttrib(_T("file_name"));
			CDUIString strResID = xml.GetChildAttrib(_T("resource_id"));
			CDUIString strResType = xml.GetChildAttrib(_T("resource_type"));

			CResourceFileInfo info;
			info.m_strName = strName;
			info.m_strFileType = strType;
			info.m_strFileName = strFileName;
			info.m_strResType = strResType;
			info.m_strResID = strResID;

			m_arTemplate.push_back(info);
		}
		xml.OutOfElem();
	}

	return TRUE;
}

BOOL CDUISkinMgr::LoadLanguage()
{
	typedef CResourceFileInfoVector::iterator ITR;

	INT nCount = m_arLanguage.size();
	ITR itr = m_arLanguage.end();
	BOOL bFind(FALSE);
	if(m_strLanType.size() > 0)
	{
		for(itr = m_arLanguage.begin(); itr != m_arLanguage.end(); ++itr)
		{
			if(lstrcmpi((*itr).m_strName.c_str(), m_strLanType.c_str()) == 0)
			{
				bFind = TRUE;
				break;
			}
		}

		if(!bFind)
		{
			DUI_ASSERT(FALSE);
			return FALSE;
		}
	}
	else
	{
		if(nCount > 0)
		{
			itr = m_arLanguage.begin();
			bFind = TRUE;
		}
	}

	if(bFind)
	{
		m_strLanType = (*itr).m_strName;
		return m_ResMgr.LoadAllString(*itr);
	}
	else
	{
		return FALSE;
	}
}

BOOL CDUISkinMgr::LoadDUIResource()
{
	return m_ResMgr.LoadDUIResource(m_skinResourceInfo);
}

BOOL CDUISkinMgr::LoadTemplateStyle()
{
	typedef CResourceFileInfoVector::iterator ITR;	
	INT nCount = m_arTemplate.size();
	for(ITR itr = m_arTemplate.begin(); itr != m_arTemplate.end(); ++itr)
	{
		m_ResMgr.LoadTemplateStyle(*itr);
	}

	return TRUE;
}
/////////////////  CDUISkinMgr  //////////////////

DUI_END_NAMESPCE