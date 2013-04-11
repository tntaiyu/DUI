#ifndef __DUI_RES_MGR_H__
#define __DUI_RES_MGR_H__

#include "DUIResLoader.h"
#include "DUIFont.h"
#include "DUIColor.h"
#include "DUIBkgnd.h"

#include <map>

DUI_BGN_NAMESPCE

class DUILIB_API CResourceFileInfo
{
public:
	CDUIString m_strName;
	CDUIString m_strFileType;
	CDUIString m_strFileName;
	CDUIString m_strResID;
	CDUIString m_strResType;
};

class DUILIB_API CResourceImageData: public CResourceFileInfo,
									 public CRefCountBase
{
public:
	CDUIString m_strColorKey;
	CDUIString m_strAlpha;
	CDUIString m_strCount;
	CRefPtr<CImageList> m_pImagelist;
};

class DUILIB_API CResourceCursorData: public CResourceFileInfo,			
									  public CRefCountBase	
{
public:
	CRefPtr<CCursorObject> m_pCursor;
};

class DUILIB_API CResourceFontData: public CRefCountBase
{
public:
	CDUIString m_strName;
	CDUIString m_strHeight;
	CDUIString m_strWeight;
	CDUIString m_strItalic;
	CDUIString m_strUnderline;
	CDUIString m_strStrikeout;
	CDUIString m_strFaceName;

	CRefPtr<CFontObject> m_pFont;
};

class DUILIB_API CResMgr
{
public:
	CResMgr();
	~CResMgr();
	
	BOOL LoadAllString(CResourceFileInfo& info);
	VOID ClearAllString();
	
	BOOL LoadTemplateStyle(CResourceFileInfo& info);
	VOID ClearTemplate();

	BOOL LoadDUIResource(CResourceFileInfo& info);
	VOID ClearDUIResource();

	BOOL GetString(const CDUIString& strID, CDUIString& strRet);
	BOOL GetControlStyle(const CDUIString& strID, CDUIString& strRet);
	BOOL GetWindowTemplate(const CDUIString& strID, CDUIString& strRet);
	BOOL GetMenuTemplate(const CDUIString& strID, CDUIString& strRet);
	BOOL GetPlugin(const CDUIString& strID, CDUIString& strRet);
	CRefPtr<CTextStyle> GetTextStyle(const CDUIString& strName);
	CARGB GetColor(const CDUIString& strColorOrID);
	CRefPtr<CCursorObject> GetCursor(const CDUIString& strName, BOOL bCache = TRUE);
	CRefPtr<CImageList> GetImage(const CDUIString& strName, BOOL bCache = TRUE);
	CRefPtr<CFontObject> GetFont(const CDUIString& strID, BOOL bCache = TRUE);
	CRefPtr<CDUIFrameBK> GetFrameBK(const CDUIString& strName);

protected:
	BOOL GetXMLByConfigInfo(CResourceFileInfo& info, CDUIString& strXML);

	BOOL ParseAllString(CDUIString& strXML);
	BOOL ParseTemplateStyle(CDUIString& strXML);
	BOOL ParseDUIResource(CDUIString& strXML);
	VOID ParseFrameBKAttriColors(const CDUIString& strValue, CRefPtr<CDUIFrameBK> pFrameBK);
	VOID ParseFrameBKImagePos(const CDUIString& strValue, CRefPtr<CDUIFrameBK> pFrameBK);
	
	BOOL GetColorFromString(const CDUIString& strColor, CARGB& clrRet);
	DWORD ParseTextFormat(const CDUIString& strTextFormat);

	CRefPtr<CFontObject> CreateFont(const CRefPtr<CResourceFontData>& pData);
	CRefPtr<CCursorObject> CreateCursor(const CRefPtr<CResourceCursorData>& pData);
	CRefPtr<CImageList> CreateImage(const CRefPtr<CResourceImageData>& pData);

	CRefPtr<CFontObject> GetDefaultFont();
	CRefPtr<CTextStyle> GetDefaultTextStyle();

protected:
	CResLoader m_ResLoader;

	typedef std::map<CDUIString, CDUIString, CStrLessCmp> CStringMap;
	CStringMap m_mapLanguage;				//all string
	CStringMap m_mapWindowStyle;			//window(control) style template
	CStringMap m_mapWindowTemplate;			//window struct template
	CStringMap m_mapMenuTemplate;			//menu template
	CStringMap m_mapPlugin;				//plugin

	typedef std::map<CDUIString, CRefPtr<CTextStyle>, CStrLessCmp> CStringTextStyleMap;
	CStringTextStyleMap m_mapTextStyle;		//text style

	typedef std::map<CDUIString, CARGB, CStrLessCmp> CStringColorMap;
	CStringColorMap m_mapColor;			//color

	typedef std::map<CDUIString, CRefPtr<CResourceCursorData>, CStrLessCmp> CStringCursorMap;
	CStringCursorMap m_mapCursor;	//cursor

	typedef std::map<CDUIString, CRefPtr<CResourceFontData>, CStrLessCmp> CStringFontMap;
	CStringFontMap m_mapFont;		//font

	typedef std::map<CDUIString, CRefPtr<CResourceImageData>, CStrLessCmp> CStringIamgeMap;
	CStringIamgeMap m_mapImage;		//imagelist

	typedef std::map<CDUIString, CRefPtr<CDUIFrameBK>, CStrLessCmp> CStringFrameBKMap;
	CStringFrameBKMap m_mapFrameBK;

	CRefPtr<CFontObject> m_pDefaultFont;
	CRefPtr<CTextStyle> m_pDefaultTextStyle;
};

DUI_END_NAMESPCE

#endif //__DUI_RES_MGR_H__