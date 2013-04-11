#ifndef __DUI_RES_LOADER_H__
#define __DUI_RES_LOADER_H__

#include "DUIImage.h"
#include "DUICursor.h"

DUI_BGN_NAMESPCE

class DUILIB_API CResLoader
{
public:
	CResLoader();
	~CResLoader();

	CRefPtr<CImageList> LoadImageFromFile(const CDUIString& strFileName);
	CRefPtr<CImageList> LoadImageFromDll(const CDUIString& lpszDllName, const CDUIString& strResID, const CDUIString& strResType);
	CRefPtr<CImageList> LoadImageFromExe(const CDUIString& strResID, const CDUIString& strResType);

	CRefPtr<CCursorObject> LoadCursorFromFile(const CDUIString& strFileName);
	CRefPtr<CCursorObject> LoadCursorFromDll(const CDUIString& lpszDllName, const CDUIString& strResID, const CDUIString& strResType);	
	CRefPtr<CCursorObject> LoadCursorFromExe(const CDUIString& strResID, const CDUIString& strResType);	
	
	BOOL LoadXMLFromFile(const CDUIString& lpszFileName, CDUIString& pContent);
	BOOL LoadXMLFromDll(const CDUIString& lpszDllName, const CDUIString& lpszResID, const CDUIString& lpszResType, CDUIString& pContent);
	BOOL LoadXMLFromExe(const CDUIString& lpszResID, const CDUIString& lpszResType, CDUIString& pContent);

	HINSTANCE LoadLibrary(const CDUIString& strFileName);

protected:
	VOID FreeResDll();
	HINSTANCE GetResInstance(const CDUIString& strDllName);
	
	BOOL LoadStringFromResource(HINSTANCE hRes, const CDUIString& lpszResID, const CDUIString& lpszResType, CDUIString& pContent);
	CRefPtr<CImageList> LoadImageFromResource(HINSTANCE hRes, const CDUIString& strResID, const CDUIString& strResType);
	CRefPtr<CCursorObject> LoadCursorFromResource(HINSTANCE hRes, const CDUIString& strResID, const CDUIString& strResType);

protected:
	HINSTANCE m_hResInstance;
};


DUI_END_NAMESPCE


#endif //__DUI_RES_LOADER_H__