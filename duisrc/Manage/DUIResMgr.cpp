#include "DUIResMgr.h"
#include "DUIXml.h"
#include "DUILibDef.h"
#include <algorithm>
#include <sstream>

using namespace std;

#define CONFIG_TYPE_DLL			_T("dll")
#define CONFIG_TYPE_FILE		_T("file")
#define CONFIG_TYPE_EXE			_T("exe")

DUI_BGN_NAMESPCE

CResMgr::CResMgr()
{

}

CResMgr::~CResMgr()
{
	ClearAllString();
	ClearTemplate();
	ClearDUIResource();
}

BOOL CResMgr::GetXMLByConfigInfo(CResourceFileInfo& info, CDUIString& strXML)
{
	if(info.m_strFileType.size() <=0)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	BOOL bRet(FALSE);
	if(lstrcmpi(info.m_strFileType.c_str(), CONFIG_TYPE_DLL) == 0)
	{
		bRet =	m_ResLoader.LoadXMLFromDll( info.m_strFileName,
			info.m_strResID, info.m_strResType, strXML);
	}
	else if(lstrcmpi(info.m_strFileType.c_str(), CONFIG_TYPE_FILE) == 0)
	{
		bRet =  m_ResLoader.LoadXMLFromFile(info.m_strFileName, strXML);		
	}
	else if(lstrcmpi(info.m_strFileType.c_str(), CONFIG_TYPE_EXE) == 0)
	{
		bRet =	m_ResLoader.LoadXMLFromExe(info.m_strResID, info.m_strResType, strXML);		
	}	
	else
	{
		DUI_ASSERT(FALSE);
	}

	return bRet;
}

BOOL CResMgr::LoadAllString(CResourceFileInfo& info)
{
	CDUIString strXML;
	if(!GetXMLByConfigInfo(info, strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	if(strXML.size() > 0)
	{
		return ParseAllString(strXML);
	}
	else
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
}

VOID CResMgr::ClearAllString()
{
	m_mapLanguage.clear();
}

BOOL CResMgr::LoadTemplateStyle(CResourceFileInfo& info)
{
	CDUIString strXML;
	if(!GetXMLByConfigInfo(info, strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	if(strXML.size() > 0)
	{
		return ParseTemplateStyle(strXML);
	}
	else
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
}

VOID CResMgr::ClearTemplate()
{
	m_mapWindowTemplate.clear();
	m_mapWindowStyle.clear();
	m_mapTextStyle.clear();
	m_mapFrameBK.clear();
	m_mapMenuTemplate.clear();
	m_mapPlugin.clear();
}

BOOL CResMgr::LoadDUIResource(CResourceFileInfo& info)
{
	CDUIString strXML;
	if(!GetXMLByConfigInfo(info, strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	if(strXML.size() > 0)
	{
		return ParseDUIResource(strXML);
	}
	else
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}
}

VOID CResMgr::ClearDUIResource()
{
	m_mapColor.clear();
	m_mapFont.clear();
	m_mapImage.clear();
	m_mapCursor.clear();
	m_pDefaultFont = NULL;
}

BOOL CResMgr::ParseAllString(CDUIString& strXML)
{
	CDUIXml xml;
	if(!xml.SetDoc(strXML.c_str())) 
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	while(xml.FindChildElem(_T("string")))
	{
		CDUIString strID = xml.GetChildAttrib(_T("id"));
		CDUIString strText = xml.GetChildData();
		if(strID.size() > 0 && strText.size() > 0)
		{
			m_mapLanguage.insert(std::make_pair(strID, strText));
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
	}	

	return TRUE;
}

VOID CResMgr::ParseFrameBKImagePos(const CDUIString& strValue, CRefPtr<CDUIFrameBK> pFrameBK)
{
	CFrameBKImagePos pos = pFrameBK->GetSkinImagePos();

	FRAME_SKIN_IMAGE_POS imagePos = SKIN_POS_LEFTTOP;
	if(strValue.find(_T("left_top")) != -1)
	{
		imagePos = SKIN_POS_LEFTTOP;
	}
	else if(strValue.find(_T("left_bottom")) != -1)
	{
		imagePos = SKIN_POS_LEFTBOTTOM;
	}
	else if(strValue.find(_T("right_bottom")) != -1)
	{
		imagePos = SKIN_POS_RIGHTBOTTOM;
	}
	else if(strValue.find(_T("right_top")) != -1)
	{
		imagePos = SKIN_POS_RIGHTTOP;
	}
	else if(strValue.find(_T("center")) != -1)
	{
		imagePos = SKIN_POS_CENTER;
	}

	pos.m_ePosType = imagePos;

	pFrameBK->SetSkinImagePos(pos);
}

VOID CResMgr::ParseFrameBKAttriColors(const CDUIString& strValue, CRefPtr<CDUIFrameBK> pFrameBK)
{
	DUI_ASSERT(!pFrameBK.IsNull());

	const TCHAR ch = _T(',');
	CDUIString str, str1, str2;
	INT nLen = strValue.size();
	INT nPos = strValue.find_first_of (ch);
	if(nPos != -1)
	{
		str = strValue.substr(0, nPos);
		++nPos;
		if(nPos < nLen)
		{
			INT nPos1 = strValue.find(ch, nPos);
			if(nPos1 != -1)
			{
				str1 = strValue.substr(nPos, nPos1 - nPos);
				++nPos1;
				if(nPos1<nLen)
				{
					INT nPos2 = strValue.find(ch, nPos1);
					if(nPos2 != -1)
					{
						str2 = strValue.substr(nPos1, nPos2 - nPos1);
					}
					else
					{
						str2 = strValue.substr(nPos1, nLen-nPos1);
					}
				}
			}
			else
			{
				str1 = strValue.substr(nPos, nLen-nPos);
			}
		}
	}
	else
	{
		str = strValue;
	}

	BOOL bVertical = pFrameBK->GetBKVertical();
	double dbPos = pFrameBK->GetBKSepPos();

	CARGB clrBK, clrBK1, clrBK2, clrMid;
	if(str.size() > 0) 
	{
		clrBK = GetColor(str);
	}

	if(str1.size() > 0)
	{
		clrBK1 = clrBK;
		clrBK2 = GetColor(str1);
	}

	if(str2.size() > 0)
	{
		clrMid = GetColor(str2);
	}

	if(!clrMid.IsEmpty()
		&& !clrBK1.IsEmpty()
		&& !clrBK2.IsEmpty())
	{
		pFrameBK->SetBKColor(clrBK1, clrBK2, clrMid, dbPos, bVertical);
	}
	else if(!clrBK1.IsEmpty()
		&& !clrBK2.IsEmpty())
	{
		pFrameBK->SetBKColor(clrBK1, clrBK2, bVertical);
	}
	else if(!clrBK.IsEmpty())
	{
		pFrameBK->SetBKColor(clrBK);
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

BOOL CResMgr::ParseTemplateStyle(CDUIString& strXML)
{
	CDUIXml xml;
	if(!xml.SetDoc(strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	while(xml.FindChildElem(_T("text_style_set")))
	{
		xml.IntoElem();
		
		while(xml.FindChildElem(_T("text_style")))
		{
			CRefPtr<CTextStyle> pTextStyle = new CTextStyle;
			CDUIString strName = xml.GetChildAttrib(_T("name"));
			if(strName.size() <= 0 || pTextStyle == NULL)
			{
				DUI_ASSERT(FALSE);
				continue;
			}
			else
			{
				m_mapTextStyle.insert(std::make_pair(strName, pTextStyle));
			}

			CDUIString strTextFormat = xml.GetChildAttrib(_T("format"));
			pTextStyle->SetTextFormat(ParseTextFormat(strTextFormat));

			CDUIString strFontName = xml.GetChildAttrib(_T("font"));
			pTextStyle->SetFont(GetFont(strFontName));

			CDUIString strNormalColor = xml.GetChildAttrib(_T("normal_color"));
			if(strNormalColor.size() > 0)
			{
				pTextStyle->SetNormalColor(GetColor(strNormalColor));
			}

			CDUIString strDisableColor = xml.GetChildAttrib(_T("disable_color"));
			if(strDisableColor.size() > 0)
			{
				pTextStyle->SetDisableColor(GetColor(strDisableColor));		
			}

			CDUIString strHoverColor = xml.GetChildAttrib(_T("hover_color"));
			if(strHoverColor.size() > 0)
			{
				pTextStyle->SetHoverColor(GetColor(strHoverColor));		
			}

			CDUIString strSelectColor = xml.GetChildAttrib(_T("select_color"));
			if(strSelectColor.size() > 0)
			{
				pTextStyle->SetSelectColor(GetColor(strSelectColor));		
			}

			CDUIString strDefault = xml.GetChildAttrib(_T("default"));
			if(strDefault.size() > 0 && lstrcmpi(strDefault.c_str(), DUI_STR_TRUE) == 0)
			{
				m_pDefaultTextStyle = pTextStyle;
			}
		}

		xml.OutOfElem();
	}

	xml.ResetMainPos();

	while(xml.FindChildElem(_T("frame_style_set")))
	{
		xml.IntoElem();
		
		while(xml.FindChildElem(_T("frame_style")))
		{
			CRefPtr<CDUIFrameBK> pFrameBK = new CDUIFrameBK;
			CDUIString strName = xml.GetChildAttrib(_T("name"));
			if(pFrameBK.IsNull() || strName.size() <= 0)
			{
				DUI_ASSERT(FALSE);
				continue;
			}
			else
			{
				m_mapFrameBK.insert(std::make_pair(strName, pFrameBK));
			}

			CDUIString strValue = xml.GetChildAttrib(_T("back_color"));
			if(strValue.size() > 0)
			{
				ParseFrameBKAttriColors(strValue, pFrameBK);
			}

			strValue = xml.GetChildAttrib(_T("back_sep"));
			if(strValue.size() > 0)
			{
				wstringstream os;
				os << strValue;
				double dbPos;
				os >> dbPos;

				pFrameBK->SetBKSepPos(dbPos);
			}

			strValue = xml.GetChildAttrib(_T("back_vertical"));
			if(strValue.size() > 0)
			{
				BOOL bVertical(FALSE);
				if(strValue.compare(DUI_STR_TRUE) == 0)
				{
					bVertical = TRUE;
				}

				pFrameBK->SetBKVertical(bVertical);
			}
			
			strValue = xml.GetChildAttrib(_T("back_image"));
			if(strValue.size() > 0)
			{
				pFrameBK->SetSkinImage(GetImage(strValue));
			}
		
			strValue = xml.GetChildAttrib(_T("back_image_pos"));
			if(strValue.size() > 0)
			{
				ParseFrameBKImagePos(strValue, pFrameBK);	
			}

			strValue = xml.GetChildAttrib(_T("border_image"));
			if(strValue.size() > 0)
			{
				pFrameBK->SetFrameImage(GetImage(strValue));
			}	

			strValue = xml.GetChildAttrib(_T("border_color"));
			if(strValue.size() > 0)
			{
				pFrameBK->SetFrameColor(GetColor(strValue));
			}
			
			strValue = xml.GetChildAttrib(_T("border_rect"));
			if(strValue.size() > 0)
			{
				TCHAR szTextBuffer[128] = {0};
				lstrcpyn(szTextBuffer, strValue.c_str(), sizeof(szTextBuffer) / sizeof(TCHAR) - 1);

				RECT rtTemp = {0};
				LPTSTR pstr = NULL;
				rtTemp.left = _tcstol(szTextBuffer, &pstr, 10);
				rtTemp.top = _tcstol(pstr + 1, &pstr, 10);
				rtTemp.right = _tcstol(pstr + 1, &pstr, 10);
				rtTemp.bottom = _tcstol(pstr + 1, &pstr, 10);
				
				pFrameBK->SetFrameBorder(rtTemp);
			}
			
			strValue = xml.GetChildAttrib(_T("border_round"));
			if(strValue.size() > 0)
			{
				TCHAR szTextBuffer[128] = {0};
				lstrcpyn(szTextBuffer, strValue.c_str(), sizeof(szTextBuffer) / sizeof(TCHAR) - 1);

				SIZE size = {0};
				LPTSTR pstr = NULL;
				size.cx = _tcstol(szTextBuffer, &pstr, 10);
				size.cy = _tcstol(pstr + 1, &pstr, 10);
				
				pFrameBK->SetFrameRound(size.cx, size.cy);
			}			
		}

		xml.OutOfElem();
	}

	xml.ResetMainPos();

	while(xml.FindChildElem(_T("control_style_set")))
	{
		xml.IntoElem();

		while(xml.FindChildElem(_T("control_style")))
		{
			xml.IntoElem();

			CDUIString strName = xml.GetAttrib(_T("name"));
			CDUIString strEelement = xml.GetSubDoc();
			m_mapWindowStyle.insert(std::make_pair(strName, strEelement));

			xml.OutOfElem();
		}
		
		xml.OutOfElem();
	}

	xml.ResetMainPos();

	while (xml.FindChildElem(_T("menu_template")))
	{
		xml.IntoElem();
		
		while(xml.FindChildElem(_T("menu")))
		{
			xml.IntoElem();
			
			CDUIString strName = xml.GetAttrib(_T("name"));
			CDUIString strEelement = xml.GetSubDoc();

			DUI_ASSERT(strName.size() > 0);
			m_mapMenuTemplate.insert(std::make_pair(strName, strEelement));
			
			xml.OutOfElem();
		}
		
		xml.OutOfElem();
	}

	xml.ResetMainPos();

	while(xml.FindChildElem(_T("window_template")))
	{
		xml.IntoElem();

		while(xml.FindChildElem(_T("window")))
		{
			xml.IntoElem();

			CDUIString strName = xml.GetAttrib(_T("name"));
			CDUIString strEelement = xml.GetSubDoc();

			DUI_ASSERT(strName.size() > 0);
			m_mapWindowTemplate.insert(std::make_pair(strName, strEelement));

			xml.OutOfElem();
		}
		
		xml.OutOfElem();
	}

	xml.ResetMainPos();

	while(xml.FindChildElem(_T("plugin_set")))
	{
		xml.IntoElem();

		while(xml.FindChildElem(_T("plugin")))
		{
			xml.IntoElem();

			CDUIString strName = xml.GetAttrib(_T("name"));
			CDUIString strEelement = xml.GetSubDoc();

			DUI_ASSERT(strName.size() > 0);
			m_mapPlugin.insert(std::make_pair(strName, strEelement));

			xml.OutOfElem();
		}

		xml.OutOfElem();
	}

	return TRUE;
}

CARGB CResMgr::GetColor(const CDUIString& strColorOrID)
{
	CARGB clrRet;
	//parse as color value(#00ff00)
	if(GetColorFromString(strColorOrID, clrRet))
	{
		return clrRet;
	}

	//find from color table
	typedef CStringColorMap::iterator ITR;
	ITR itr = m_mapColor.find(strColorOrID);
	if(itr != m_mapColor.end())
	{
		clrRet = itr->second;
	}
	else
	{
		DUI_ASSERT(FALSE);
	}

	return clrRet;
}	

BOOL CResMgr::GetString(const CDUIString& strID, CDUIString& strRet)
{
	if(strID.size() > 0)
	{
		if(strID[0] == _T('#'))
		{
			strRet = strID.substr(1);
			return TRUE;
		}
	}

	typedef CStringMap::iterator ITR;
	BOOL bRet(FALSE);
	ITR itr = m_mapLanguage.find(strID);
	if(itr != m_mapLanguage.end())
	{
		strRet = itr->second;
		bRet = TRUE;
	}

	DUI_ASSERT(strRet.size() >0);

	return bRet;
}

BOOL CResMgr::GetControlStyle(const CDUIString& strID, CDUIString& strRet)
{
	typedef CStringMap::iterator ITR;
	BOOL bRet(FALSE);
	ITR itr = m_mapWindowStyle.find(strID);
	if(itr != m_mapWindowStyle.end())
	{
		strRet = itr->second;
		bRet = TRUE;
	}
	
	return bRet;
}

BOOL CResMgr::GetWindowTemplate(const CDUIString& strID, CDUIString& strRet)
{
	typedef CStringMap::iterator ITR;
	BOOL bRet(FALSE);
	ITR itr = m_mapWindowTemplate.find(strID);
	if(itr != m_mapWindowTemplate.end())
	{
		strRet = itr->second;
		bRet = TRUE;
	}
	
	return bRet;
}

BOOL CResMgr::GetMenuTemplate(const CDUIString& strID, CDUIString& strRet)
{
	typedef CStringMap::iterator ITR;
	BOOL bRet(FALSE);
	ITR itr = m_mapMenuTemplate.find(strID);
	if(itr != m_mapMenuTemplate.end())
	{
		strRet = itr->second;
		bRet = TRUE;
	}
	
	return bRet;
}

BOOL CResMgr::GetPlugin(const CDUIString& strID, CDUIString& strRet)
{
	typedef CStringMap::iterator ITR;
	BOOL bRet(FALSE);
	ITR itr = m_mapPlugin.find(strID);
	if(itr != m_mapPlugin.end())
	{
		strRet = itr->second;
		bRet = TRUE;
	}

	return bRet;
}

CRefPtr<CTextStyle> CResMgr::GetTextStyle(const CDUIString& strName)
{
	if(strName.size() <= 0)
	{
		return GetDefaultTextStyle();
	}

	typedef CStringTextStyleMap::iterator ITR;
	ITR itr = m_mapTextStyle.find(strName);
	if(itr != m_mapTextStyle.end())
	{
		return itr->second;
	}	
	else
	{
		DUI_ASSERT(FALSE);
		return GetDefaultTextStyle();
	}
}

CRefPtr<CCursorObject> CResMgr::CreateCursor(const CRefPtr<CResourceCursorData>& pData)
{
	CRefPtr<CCursorObject> pCursorRet;	

	if(lstrcmpi(pData->m_strFileType.c_str(), CONFIG_TYPE_DLL) == 0)
	{
		pCursorRet = m_ResLoader.LoadCursorFromDll(pData->m_strFileName, pData->m_strResID, pData->m_strResType);
	}
	else if(lstrcmpi(pData->m_strFileType.c_str(), CONFIG_TYPE_FILE) == 0)
	{
		pCursorRet = m_ResLoader.LoadCursorFromFile(pData->m_strFileName);
	}
	else if(lstrcmpi(pData->m_strFileType.c_str(), CONFIG_TYPE_EXE) == 0)
	{
		pCursorRet = m_ResLoader.LoadCursorFromExe(pData->m_strResID, pData->m_strResType);
	}
	
	return pCursorRet;
}

CRefPtr<CCursorObject> CResMgr::GetCursor(const CDUIString& strName, BOOL bCache /*= TRUE*/)
{
	typedef CStringCursorMap::iterator ITR;
	ITR itr = m_mapCursor.find(strName);
	CRefPtr<CCursorObject> pCursorRet;	
	if(itr != m_mapCursor.end())
	{
		CRefPtr<CResourceCursorData>& pData = itr->second;
		DUI_ASSERT(!pData.IsNull());
		
		if(!bCache)
		{
			pCursorRet = CreateCursor(pData);
		}
		else
		{
			if(pData->m_pCursor.IsNull())
			{
				pCursorRet = CreateCursor(pData);
				DUI_ASSERT(!pCursorRet.IsNull());	

				if(!pCursorRet.IsNull())
				{
					pData->m_pCursor = pCursorRet;
				}
			}
			else
			{
				pCursorRet = pData->m_pCursor;
			}
		}
	}

	return pCursorRet;
}

CRefPtr<CImageList> CResMgr::CreateImage(const CRefPtr<CResourceImageData>& pData)
{
	CRefPtr<CImageList> pImageRet;	
	if(lstrcmpi(pData->m_strFileType.c_str(), CONFIG_TYPE_DLL) == 0)
	{
		pImageRet = m_ResLoader.LoadImageFromDll(pData->m_strFileName, pData->m_strResID, pData->m_strResType);
	}
	else if(lstrcmpi(pData->m_strFileType.c_str(), CONFIG_TYPE_FILE) == 0)
	{
		pImageRet = m_ResLoader.LoadImageFromFile(pData->m_strFileName);
	}
	else if(lstrcmpi(pData->m_strFileType.c_str(), CONFIG_TYPE_EXE) == 0)
	{
		pImageRet = m_ResLoader.LoadImageFromExe(pData->m_strResID, pData->m_strResType);
	}
	
	if(!pImageRet.IsNull())
	{
			if(pData->m_strColorKey.size() > 0) //set color key
			{
				pImageRet->SetColorKey(GetColor(pData->m_strColorKey));
			}

			if(pData->m_strAlpha.size() > 0) //set alpha
			{
				INT nAlpha = _ttol(pData->m_strAlpha.c_str());
				if(nAlpha > 0 && nAlpha < 256)
				{
				 	pImageRet->SetAlpha(nAlpha);
				}
				else
				{
					DUI_ASSERT(FALSE);
				}
			}
			
			if(pData->m_strCount.size() > 0) //set item count
			{
				INT nCount = _ttol(pData->m_strCount.c_str());
				if(nCount > 0)
				{
				 	pImageRet->SetItemCount(nCount);
				}
 			}
	}

	return pImageRet;
}

CRefPtr<CImageList> CResMgr::GetImage(const CDUIString& strName, BOOL bCache/* = TRUE*/)
{
	typedef CStringIamgeMap::iterator ITR;
	ITR itr = m_mapImage.find(strName);
	CRefPtr<CImageList> pImageRet;	
	if(itr != m_mapImage.end())
	{
		CRefPtr<CResourceImageData>& pData = itr->second;
		DUI_ASSERT(!pData.IsNull());
		if(!bCache)
		{
			pImageRet = CreateImage(pData);
		}
		else
		{
			if(pData->m_pImagelist.IsNull())
			{
				pImageRet = CreateImage(pData);
				DUI_ASSERT(!pImageRet.IsNull());

				if(!pImageRet.IsNull())
				{
					pData->m_pImagelist = pImageRet;
				}
			}
			else
			{
				pImageRet = pData->m_pImagelist;
			}
		}

	}
	else
	{
		DUI_ASSERT(FALSE);
	}
	
	return pImageRet;
}

CRefPtr<CFontObject> CResMgr::CreateFont(const CRefPtr<CResourceFontData>&  pData)
{
	CRefPtr<CFontObject> pFontRet = new CFontObject;
	if(pFontRet == NULL) return NULL;

	if(pData->m_strHeight.size() > 0)
	{
		INT nHeight = _ttoi(pData->m_strHeight.c_str());	
		pFontRet->SetHeight( nHeight);
	}

	if(pData->m_strWeight.size() > 0)
	{
		INT nWeight = _ttoi(pData->m_strWeight.c_str());
		if(nWeight >=0 && nWeight < 1000)
		{
			pFontRet->SetWeight( nWeight);
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
	}

	if(pData->m_strItalic.size() > 0)
	{
		if(lstrcmpi(pData->m_strItalic.c_str(), DUI_STR_TRUE) == 0)
		{
			pFontRet->SetItalic(TRUE);
		}
		else
		{
			pFontRet->SetItalic(FALSE);
		}
	}

	if(pData->m_strUnderline.size() > 0)
	{
		if(lstrcmpi(pData->m_strUnderline.c_str(), DUI_STR_TRUE) == 0)
		{
			pFontRet->SetUnderline(TRUE);
		}
		else
		{
			pFontRet->SetUnderline(FALSE);
		}
	}

	if(pData->m_strStrikeout.size() > 0)
	{
		if(lstrcmpi(pData->m_strStrikeout.c_str(), DUI_STR_TRUE) == 0)
		{
			pFontRet->SetStrikeout(TRUE);
		}
		else
		{
			pFontRet->SetStrikeout(FALSE);
		}
	}

	if(pData->m_strFaceName.size() > 0)
	{
		CDUIString strTempName;
		if(GetString(pData->m_strFaceName, strTempName))
		{
			pFontRet->SetFaceName(strTempName.c_str());
		}
		else
		{
			DUI_ASSERT(FALSE);
		}
	}

	pFontRet->CreateFont();

	return pFontRet;
}

CRefPtr<CFontObject> CResMgr::GetDefaultFont()
{
	if(m_pDefaultFont.IsNull())
	{
		if(m_mapFont.size() > 0)
		{
			m_pDefaultFont = m_mapFont.begin()->second->m_pFont;
		}
		else 
		{
			m_pDefaultFont = new CFontObject;
			if(!m_pDefaultFont.IsNull())
			{
				m_pDefaultFont->CreateFont();
			}
		}
	}

	return m_pDefaultFont;
}

CRefPtr<CTextStyle> CResMgr::GetDefaultTextStyle()
{
	if(m_pDefaultTextStyle.IsNull())
	{
		if(m_mapTextStyle.size() > 0)
		{
			m_pDefaultTextStyle = m_mapTextStyle.begin()->second;
		}
		else
		{
			m_pDefaultTextStyle = new CTextStyle;
			if(!m_pDefaultTextStyle.IsNull())
			{
				CARGB clr;
				clr.SetColor(RGB(0, 0, 0));

				m_pDefaultTextStyle->SetFont(GetDefaultFont());
				m_pDefaultTextStyle->SetNormalColor(clr);
				m_pDefaultTextStyle->SetDisableColor(clr);
				m_pDefaultTextStyle->SetHoverColor(clr);
				m_pDefaultTextStyle->SetSelectColor(clr);
				m_pDefaultTextStyle->SetTextFormat(DT_LEFT);
			}
		}
	}

	return m_pDefaultTextStyle;
}

CRefPtr<CDUIFrameBK> CResMgr::GetFrameBK(const CDUIString& strName)
{
	typedef CStringFrameBKMap::iterator ITR;
	ITR itr = m_mapFrameBK.find(strName);
	if(itr != m_mapFrameBK.end())
	{
		CRefPtr<CDUIFrameBK> pFrameBK = itr->second;
		DUI_ASSERT(!pFrameBK.IsNull());
		return pFrameBK;
	}

	DUI_ASSERT(FALSE);
	return NULL;
}

CRefPtr<CFontObject> CResMgr::GetFont(const CDUIString& strName, BOOL bCache /* = TRUE */)
{
	//if name is empty, get default font
	if(strName.size() <= 0)
	{
		return GetDefaultFont();
	}

	typedef CStringFontMap::iterator ITR;
	ITR itr = m_mapFont.find(strName);
	CRefPtr<CFontObject> pFontRet;	
	if(itr != m_mapFont.end())
	{
		CRefPtr<CResourceFontData>& pData = itr->second;
		DUI_ASSERT(!pData.IsNull());
		if(!bCache)
		{
			pFontRet = CreateFont(pData);
		}
		else
		{
			if(pData->m_pFont.IsNull())
			{
				pFontRet = CreateFont(pData);
				DUI_ASSERT(!pFontRet.IsNull());

				if(!pFontRet.IsNull())
				{
					pData->m_pFont = pFontRet;
				}
			}
			else
			{
				pFontRet = pData->m_pFont;
			}
		}
	}
	else
	{
		DUI_ASSERT(FALSE);
		pFontRet = GetDefaultFont();
	}
	
	return pFontRet;
}

BOOL CResMgr::GetColorFromString(const CDUIString& strColor, CARGB& clrRet)
{
	if(strColor.size() < 2) return FALSE;
	if(strColor[0] != _T('#'))
	{
	//	DUI_ASSERT(FALSE);
		return FALSE;
	}

	CDUIString strTemp = strColor.substr(1);
	clrRet.SetColor(strTemp.c_str());
	
	return TRUE;
}

BOOL CResMgr::ParseDUIResource(CDUIString& strXML)
{
	CDUIXml	 xml;
	if(!xml.SetDoc(strXML))
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	//load color table
	while(xml.FindChildElem(_T("color_set")))
	{
		xml.IntoElem();

		while(xml.FindChildElem(_T("color")))
		{
			CDUIString strName = xml.GetChildAttrib(_T("name"));
			CDUIString strColor = xml.GetChildAttrib(_T("value"));
			if(strName.size() > 0 && strColor.size() > 0)
			{
				CARGB clr;
				if(GetColorFromString(strColor, clr))
				{
					m_mapColor.insert(std::make_pair(strName, clr));
				}
				else
				{
					DUI_ASSERT(FALSE);
				}
			}
		}

		xml.OutOfElem();
	}

	xml.ResetMainPos();
	
	//load curor table
	while(xml.FindChildElem(_T("cursor_set")))
	{
		xml.IntoElem();

		while(xml.FindChildElem(_T("cursor")))
		{
			CRefPtr<CResourceCursorData> pData = new CResourceCursorData;
			if(!pData.IsNull())
			{
				pData->m_strName = xml.GetChildAttrib(_T("name"));
				pData->m_strFileType = xml.GetChildAttrib(_T("config_type"));
				pData->m_strFileName = xml.GetChildAttrib(_T("file_name"));
				pData->m_strResID = xml.GetChildAttrib(_T("resource_id"));
				pData->m_strResType = xml.GetChildAttrib(_T("resource_type"));
				
				if(pData->m_strName.size() > 0)
				{
					m_mapCursor.insert(std::make_pair(pData->m_strName, pData));
				}
				else
				{
					DUI_ASSERT(FALSE);
				}
			}
		}
		
		xml.OutOfElem();
	}
	
	
	xml.ResetMainPos();
	//load font
	while(xml.FindChildElem(_T("font_set")))
	{
		xml.IntoElem();
		
		while(xml.FindChildElem(_T("font")))
		{
			CRefPtr<CResourceFontData> pData = new CResourceFontData;
			if(!pData.IsNull())
			{
				pData->m_strName = xml.GetChildAttrib(_T("name"));
				pData->m_strFaceName = xml.GetChildAttrib(_T("facename"));
				pData->m_strItalic = xml.GetChildAttrib(_T("italic"));
				pData->m_strUnderline = xml.GetChildAttrib(_T("underline"));
				pData->m_strHeight = xml.GetChildAttrib(_T("height"));
				pData->m_strWeight = xml.GetChildAttrib(_T("weight"));
				pData->m_strStrikeout = xml.GetChildAttrib(_T("strikeout"));

				CDUIString strDefault = xml.GetChildAttrib(_T("default"));
				if(strDefault.size() > 0 && lstrcmpi(strDefault.c_str(), DUI_STR_TRUE) == 0)
				{
					m_pDefaultFont = CreateFont(pData);
					pData->m_pFont = m_pDefaultFont;
				}

				if(pData->m_strName.size() > 0)
				{
					m_mapFont.insert(std::make_pair(pData->m_strName, pData));
				}
				else
				{
					DUI_ASSERT(FALSE);
				}
			}
		}
		
		xml.OutOfElem();
	}


	xml.ResetMainPos();
	//load imagelist
	while(xml.FindChildElem(_T("image_set")))
	{
		xml.IntoElem();

		while(xml.FindChildElem(_T("image")))
		{
			CRefPtr<CResourceImageData> pData = new CResourceImageData;
			if(!pData.IsNull())
			{
				pData->m_strName = xml.GetChildAttrib(_T("name"));
				pData->m_strFileType = xml.GetChildAttrib(_T("config_type"));
				pData->m_strFileName = xml.GetChildAttrib(_T("file_name"));
				pData->m_strResID = xml.GetChildAttrib(_T("resource_id"));
				pData->m_strResType = xml.GetChildAttrib(_T("resource_type"));

				pData->m_strAlpha = xml.GetChildAttrib(_T("alpha"));
				pData->m_strColorKey = xml.GetChildAttrib(_T("mask_color"));
				pData->m_strCount = xml.GetChildAttrib(_T("count"));

				if(pData->m_strName.size() > 0)
				{
					m_mapImage.insert(std::make_pair(pData->m_strName, pData));
				}
				else
				{
					DUI_ASSERT(FALSE);
				}
			}
		}
		
		xml.OutOfElem();
	}		

	return TRUE;	
}

#ifndef DT_HIDEPREFIX
enum { DT_HIDEPREFIX = 0x00100000 };
#endif //!DT_HIDEPREFIX

#ifndef DT_NOFULLWIDTHCHARBREAK
enum { DT_NOFULLWIDTHCHARBREAK =  0x00080000 };
#endif //!DT_NOFULLWIDTHCHARBREAK

#ifndef DT_PREFIXONLY
enum { DT_PREFIXONLY =  0x00200000 };
#endif //!DT_PREFIXONLY

DWORD CResMgr::ParseTextFormat(const CDUIString& strTextFormat)
{
	DWORD dwRet(0);

	CDUIString strTemp(strTextFormat);
	std::transform(strTemp.begin(), strTemp.end(), strTemp.begin(), toupper);

	if(strTemp.find(_T("DT_BOTTOM")) != -1)
	{
		dwRet |= DT_BOTTOM;		
	}

	if(strTemp.find(_T("DT_CENTER")) != -1)
	{
		dwRet |= DT_CENTER;		
	}
	
	if(strTemp.find(_T("DT_EDITCONTROL")) != -1)
	{
		dwRet |= DT_EDITCONTROL;		
	}

	if(strTemp.find(_T("DT_END_ELLIPSIS")) != -1)
	{
		dwRet |= DT_END_ELLIPSIS;		
	}

	if(strTemp.find(_T("DT_EXPANDTABS")) != -1)
	{
		dwRet |= DT_EXPANDTABS;		
	}

	if(strTemp.find(_T("DT_EXTERNALLEADING")) != -1)
	{
		dwRet |= DT_EXTERNALLEADING;		
	}

	if(strTemp.find(_T("DT_HIDEPREFIX")) != -1)
	{
		dwRet |= DT_HIDEPREFIX;		
	}

	if(strTemp.find(_T("DT_INTERNAL")) != -1)
	{
		dwRet |= DT_INTERNAL;		
	}

	if(strTemp.find(_T("DT_LEFT")) != -1)
	{
		dwRet |= DT_LEFT;		
	}

	if(strTemp.find(_T("DT_MODIFYSTRING")) != -1)
	{
		dwRet |= DT_MODIFYSTRING;		
	}

	if(strTemp.find(_T("DT_NOCLIP")) != -1)
	{
		dwRet |= DT_NOCLIP;		
	}

	if(strTemp.find(_T("DT_NOFULLWIDTHCHARBREAK")) != -1)
	{
		dwRet |= DT_NOFULLWIDTHCHARBREAK;		
	}

	if(strTemp.find(_T("DT_NOPREFIX")) != -1)
	{
		dwRet |= DT_NOPREFIX;		
	}

	if(strTemp.find(_T("DT_PATH_ELLIPSIS")) != -1)
	{
		dwRet |= DT_PATH_ELLIPSIS;		
	}

	if(strTemp.find(_T("DT_PREFIXONLY")) != -1)
	{
		dwRet |= DT_PREFIXONLY;		
	}

	if(strTemp.find(_T("DT_RIGHT"))!= -1)
	{
		dwRet |= DT_RIGHT;		
	}

	if(strTemp.find(_T("DT_RTLREADING")) != -1)
	{
		dwRet |= DT_RTLREADING;		
	}

	if(strTemp.find(_T("DT_SINGLELINE")) != -1)
	{
		dwRet |= DT_SINGLELINE;		
	}

	if(strTemp.find(_T("DT_TABSTOP")) != -1)
	{
		dwRet |= DT_TABSTOP;		
	}

	if(strTemp.find(_T("DT_TOP")) != -1)
	{
		dwRet |= DT_TOP;		
	}
	
	if(strTemp.find(_T("DT_VCENTER")) != -1)
	{
		dwRet |= DT_VCENTER;		
	}
	
	if(strTemp.find(_T("DT_WORDBREAK")) != -1)
	{
		dwRet |= DT_WORDBREAK;		
	}
	
	if(strTemp.find(_T("DT_WORD_ELLIPSIS")) != -1)
	{
		dwRet |= DT_WORD_ELLIPSIS;		
	}

	return dwRet;
}

DUI_END_NAMESPCE