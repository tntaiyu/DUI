#include "DUIResLoader.h"
#include "DUIUtil.h"
#include "DUIXml.h"

DUI_BGN_NAMESPCE

class CRelativeToAbsolutePath
{
public:
	CRelativeToAbsolutePath(const CDUIString& strRelativePath)
	{
		BuilderAbsolutePath(strRelativePath);
	}
	
	operator CDUIString()
	{
		return m_strAbsolutePath;
	}
	
protected:
	VOID BuilderAbsolutePath(const CDUIString& strRelativePath)
	{
		CDUIString strPath;
		GetCurrentPath(strPath);
		
		CDUIString strTemp = stringReplace(strRelativePath, _T("/"), _T("\\"));
		
		if(strTemp.size() > 2) //for ./a.txt
		{
			CDUIString strBegin(strTemp, 0, 2);
			if(strBegin.compare(_T(".\\")) == 0)
			{
				strTemp = strTemp.substr(2);
			}
		}

		while(strTemp.size() > 3) //for ../../c.txt
		{
			CDUIString strBegin(strTemp, 0, 3);
			if(strBegin.compare(_T("..\\")) == 0)
			{
				strTemp = strTemp.substr(3);
			}
			else
			{
				break;
			}

			INT nPos = strPath.find_last_of(_T("\\"));
			if(nPos != -1 && nPos != 2)
			{
				strPath = strPath.substr(0, nPos);
			}
			else
			{
				DUI_ASSERT(FALSE);
			}
		}


		m_strAbsolutePath = strPath;
		if(strTemp.size() > 0 && strTemp[0] != _T('\\')) //for \a\b.txt
		{
			m_strAbsolutePath += _T("\\");
		}
		m_strAbsolutePath += strTemp;
	}


	BOOL GetCurrentPath(CDUIString& strPath)
	{
		TCHAR szExePath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szExePath, sizeof(szExePath) / sizeof(TCHAR) - 1);
		TCHAR* p = _tcsrchr(szExePath, _T('\\'));
		if(p != NULL)
		{
			*p = _T('\0');
			strPath = szExePath;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
protected:
	CDUIString m_strAbsolutePath;
};


///////////////////  CResLoader  /////////////////////
CResLoader::CResLoader()
{
	m_hResInstance = NULL;
}
CResLoader::~CResLoader()
{
	FreeResDll();
}

VOID CResLoader::FreeResDll()
{
	if(m_hResInstance != NULL)
	{
		::FreeLibrary(m_hResInstance);
		m_hResInstance = NULL;
	}
}

HINSTANCE CResLoader::LoadLibrary(const CDUIString& strFileName)
{
	CRelativeToAbsolutePath path(strFileName);
	CDUIString strDllName = path;

	return ::LoadLibrary(strDllName.c_str());
}

HINSTANCE CResLoader::GetResInstance(const CDUIString& strDllName)
{
	HINSTANCE hNewInstance = ::LoadLibrary(strDllName.c_str());
	if(hNewInstance == NULL) return NULL;

	if(m_hResInstance == NULL) //keep last instance and do not free it
	{
		m_hResInstance = hNewInstance;
		return m_hResInstance;
	}

	if(m_hResInstance == hNewInstance)
	{
		::FreeLibrary(hNewInstance);
	}
	else
	{
		::FreeLibrary(m_hResInstance);
		m_hResInstance = hNewInstance;
	}

	return m_hResInstance;
}

CRefPtr<CImageList> CResLoader::LoadImageFromFile(const CDUIString& lpszFileName)
{
	CRelativeToAbsolutePath path(lpszFileName);
	CDUIString strFullPath = path;
	
	CRefPtr<CImageList> pImg = new CImageList;
	if(pImg.IsNull()) return NULL;

	if(!pImg->CreateFromFile(strFullPath.c_str()))
	{
		DUI_ASSERT(FALSE);
		CTrace::TraceError(_T("CResLoader::LoadImageFromFile: failed to load image(%s)"), lpszFileName.c_str());
	}
	
	return pImg;
}

CRefPtr<CImageList> CResLoader::LoadImageFromResource(HINSTANCE hRes, const CDUIString& strResID, const CDUIString& strResType)
{
	CRefPtr<CImageList> pImg = new CImageList;
	if(pImg.IsNull()) return NULL;

	INT nResID = _ttoi(strResID.c_str());
	if(!pImg->CreateFromResource(hRes, MAKEINTRESOURCE(nResID), strResType.c_str()))
	{
		DUI_ASSERT(FALSE);
		CTrace::TraceError(_T("CResLoader::LoadImageFromResource: failed to load image(id-%s, type:%s)")
			, strResID.c_str(), strResType.c_str());
	}
	
	return pImg;
}

CRefPtr<CImageList> CResLoader::LoadImageFromDll(const CDUIString& lpszDllName, const CDUIString& lpszResID, const CDUIString& lpszResType)
{
	CRelativeToAbsolutePath path(lpszDllName);
	CDUIString strFullPath = path;

	HINSTANCE hRes = GetResInstance(strFullPath);
	if(hRes == NULL) return NULL;
	
	return LoadImageFromResource(hRes, lpszResID, lpszResType);
}

CRefPtr<CImageList> CResLoader::LoadImageFromExe(const CDUIString& lpszResID, const CDUIString& lpszResType)
{
	HINSTANCE hRes = GetModuleHandle(NULL);
	if(hRes == NULL) return FALSE;
	
	return LoadImageFromResource(hRes, lpszResID, lpszResType);
}

CRefPtr<CCursorObject> CResLoader::LoadCursorFromResource(HINSTANCE hRes, const CDUIString& strResID, const CDUIString& strResType)
{
	CRefPtr<CCursorObject> pCursor = new CCursorObject;
	if(pCursor.IsNull()) return NULL;

	INT nResID = _ttoi(strResID.c_str());
	if(!pCursor->CreateFromResource(hRes, MAKEINTRESOURCE(nResID)))
	{
		DUI_ASSERT(FALSE);
		CTrace::TraceError(_T("CResLoader::LoadCursorFromResource: failed to load cursor(id-%s, type:%s)")
			, strResID.c_str(), strResType.c_str());
	}

	return pCursor;
}

CRefPtr<CCursorObject> CResLoader::LoadCursorFromFile(const CDUIString& lpszFileName)
{
	CRelativeToAbsolutePath path(lpszFileName);
	CDUIString strFullPath = path;
	
	CRefPtr<CCursorObject> pCursor = new CCursorObject;
	if(pCursor.IsNull()) return NULL;

	if(!pCursor->CreateFromFile(strFullPath.c_str()))
	{
		DUI_ASSERT(FALSE);
		CTrace::TraceError(_T("CResLoader::LoadCursorFromFile: failed to load cursor(%s)"), lpszFileName.c_str());
	}
 
	return pCursor;
}

CRefPtr<CCursorObject> CResLoader::LoadCursorFromDll(const CDUIString& lpszDllName, const CDUIString& lpszResID, const CDUIString& lpszResType)
{
	CRelativeToAbsolutePath path(lpszDllName);
	CDUIString strFullPath = path;

	HINSTANCE hRes = GetResInstance(strFullPath);
	if(hRes == NULL) return FALSE;
	
	return LoadCursorFromResource(hRes, lpszResID, lpszResType);
}

CRefPtr<CCursorObject> CResLoader::LoadCursorFromExe(const CDUIString& lpszResID, const CDUIString& lpszResType)
{
	HINSTANCE hRes = GetModuleHandle(NULL);
	if(hRes == NULL) return FALSE;
	
	return LoadCursorFromResource(hRes, lpszResID, lpszResType);
}

BOOL CResLoader::LoadXMLFromFile(const CDUIString& lpszFileName, CDUIString& strContent)
{
	CRelativeToAbsolutePath path(lpszFileName);
	CDUIString strFullPath = path;
	
	CDUIXml xml;
	if(!xml.Load(strFullPath))
	{
		CTrace::TraceError(_T("CResLoader::LoadXMLFromFile: can not open file(%s)"), strFullPath.c_str());
		DUI_ASSERT(FALSE);
		return FALSE;
	}
	
	strContent = xml.GetDoc();

	return TRUE;
}

BOOL CResLoader::LoadXMLFromDll(const CDUIString& lpszDllName, const CDUIString& lpszResID, const CDUIString& lpszResType, CDUIString& strContent)
{
	CRelativeToAbsolutePath path(lpszDllName);
	CDUIString strFullPath = path;

	HINSTANCE hRes = GetResInstance(strFullPath);
	if(hRes == NULL) return FALSE;

	return LoadStringFromResource(hRes, lpszResID, lpszResType, strContent);

}

BOOL CResLoader::LoadXMLFromExe(const CDUIString& lpszResID, const CDUIString& lpszResType, CDUIString& strContent)
{
	HINSTANCE hRes = GetModuleHandle(NULL);
	if(hRes == NULL) return FALSE;

	return LoadStringFromResource(hRes, lpszResID, lpszResType, strContent);
}

BOOL CResLoader::LoadStringFromResource(HINSTANCE hRes, const CDUIString& lpszResID, const CDUIString& lpszResType, CDUIString& strContent)
{
	INT nResID = _ttoi(lpszResID.c_str());
	HRSRC hRsrc = FindResource (hRes, MAKEINTRESOURCE(nResID), lpszResType.c_str());
    if (hRsrc == NULL) return FALSE;
  
    BYTE* ptr = (BYTE*)LoadResource(hRes, hRsrc);
    DWORD nSize = SizeofResource(hRes, hRsrc);
		
	CEncode code;
	LPWSTR lpszUnicode = code.Utf8ToUnicode((char*)ptr, nSize);
	if(lpszUnicode == NULL) return FALSE;

	strContent = lpszUnicode;

	return TRUE;
}

///////////////////  CResLoader  /////////////////////

DUI_END_NAMESPCE
