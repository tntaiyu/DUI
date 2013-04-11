#ifndef __DUI_CONTROL_FACTORY_H__
#define __DUI_CONTROL_FACTORY_H__

#include "DUILib.h"
#include <map>

DUI_BGN_NAMESPCE

class DUILIB_API CDUIControlFactory: public IDUIControlFactory
{
public:
	CDUIControlFactory();
	~CDUIControlFactory();

	virtual	BOOL InitializeFactory();
	virtual VOID UninitializeFactory();
	
	virtual BOOL RegisterControl(const CDUIString& strName, CRefPtr<IDUIControl> pControl);
	virtual BOOL UnregisterControl(const CDUIString& strName);
	
	virtual CRefPtr<IDUIControl> NewControl(const CDUIString& strName);

protected:
	VOID ApplyDefaultControlStyle();
	VOID CheckControlStyle(CDUIString& strControlName);
	CRefPtr<IDUIControl> CreateFromPlugin(const CDUIString& strName);
	BOOL GetPlugin(const CDUIString& strName, CDUIString& strDllName, CDUIString& strEntryFunction);

protected:
	typedef std::map<CDUIString, CRefPtr<IDUIControl>, CStrLessCmp> CControlMap;
	CControlMap m_mapControl;
	HINSTANCE m_hInstanceRichEdit;
};

DUI_END_NAMESPCE

#endif //__DUI_CONTROL_FACTORY_H__