#ifndef __DUI_APP_H__
#define __DUI_APP_H__

#include "DUILib.h"

DUI_BGN_NAMESPCE

class DUILIB_API CDUIApp: public IDUIApp
{
public:
	CDUIApp();
	~CDUIApp();
	
	virtual BOOL Init(const CDUIString& strEntryXML);
	virtual void Term();
	
	virtual IDUISkinMgr* GetSkinMgr() const;
	virtual IDUIControlFactory* GetControlFactory() const;
	virtual CDUIMessageLoop* GetMessageLoop();
	
protected:
	IDUISkinMgr*		m_pSkinMgr;
	IDUIControlFactory* m_pControlFactory;
	CDUIMessageLoop		m_messageLoop;
};



DUI_END_NAMESPCE


#endif //__DUI_APP_H__