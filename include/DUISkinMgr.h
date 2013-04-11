#ifndef __DUI_SKIN_MGR_H__
#define __DUI_SKIN_MGR_H__

#include "DUILib.h"
#include "DUIResMgr.h"
#include <vector>

DUI_BGN_NAMESPCE

class DUILIB_API CDUISkinMgr: public IDUISkinMgr
{
public:
	CDUISkinMgr();
	~CDUISkinMgr();

	virtual BOOL InitializeSkin(const CDUIString& strConfigXML);
	virtual VOID UninitializeSkin();

	virtual BOOL SetSkinUIData(CRefPtr<CDUIFrameBK> pSkinUIData);
	virtual const CRefPtr<CDUIFrameBK> GetSKinUIData() const;

	virtual BOOL SetLanguageType(const CDUIString& strLanType);
	virtual CDUIString GetLanguageType() const;

	virtual BOOL AddSkinObserver(IDUISkinObserver* pSink);
	virtual BOOL RemoveSkinObserver(IDUISkinObserver* pSink);

	virtual BOOL AddLanguageObserver(IDUILanguageObserver* pObserver);
	virtual BOOL RemoveLanguageObserver(IDUILanguageObserver* pObserver);

	virtual CRefPtr<CImageList> GetImage(const CDUIString& strID, BOOL bCache = TRUE);
	virtual CRefPtr<CCursorObject> GetCursor(const CDUIString& strID, BOOL bCache = TRUE);
	virtual CRefPtr<CFontObject> GetFont(const CDUIString& strID, BOOL bCache = TRUE);
	virtual CARGB GetColor(const CDUIString& strID);

	virtual CRefPtr<CTextStyle> GetTextStyle(const CDUIString& strID);
	virtual CRefPtr<CDUIFrameBK> GetFrameBK(const CDUIString& strName);

	virtual BOOL GetWindowTemplate(const CDUIString& strID, CDUIString& strTemplateXML);
	virtual BOOL GetMenuTemplate(const CDUIString& strID, CDUIString& strTemplateXML);
	virtual BOOL GetControlStyle(const CDUIString& strID, CDUIString& strStyleXML);
	virtual BOOL GetStringText(const CDUIString& strID, CDUIString& strContent);
	virtual BOOL GetPlugin(const CDUIString& strID, CDUIString& strPluginXML);

protected:
	BOOL ParseEntryXML(const CDUIString& lpszEntryXML);
	BOOL LoadLanguage();
	BOOL LoadDUIResource();
	BOOL LoadTemplateStyle();

protected:
	CResMgr m_ResMgr;

	typedef std::vector<CResourceFileInfo> CResourceFileInfoVector;
	CResourceFileInfoVector m_arLanguage;
	CResourceFileInfoVector m_arTemplate;

	std::vector<IDUILanguageObserver*> m_arLanObserver;
	std::vector<IDUISkinObserver*> m_arSkinObserver;

	CDUIString m_strLanType;
	CResourceFileInfo m_skinResourceInfo;

	CRefPtr<CDUIFrameBK> m_pSkinUIData;
};

DUI_END_NAMESPCE

#endif