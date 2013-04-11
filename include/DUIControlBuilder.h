#ifndef __DUI_CONTROL_BUILDER_H__
#define __DUI_CONTROL_BUILDER_H__

#include "DUILib.h"
#include "DUIMenu.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUIControlBuilderCallback
{
public:
	//if can not create a new control from factory, will callback this function
	virtual CRefPtr<IDUIControl> OnBuilderNewControl(const CDUIString& strClassName) = 0;
	//after the root control is created
	virtual VOID OnBuilderControlCreated(CRefPtr<IDUIControl> pRootControl) = 0;
	//after parsed a new attribute for main window
	virtual VOID OnBuilderWindowAttribute(const CDUIString& strAttriName, const CDUIString& strAttriValue) = 0;
	
	virtual BOOL OnAddChildControl(CRefPtr<IDUIControl> pParent, CRefPtr<IDUIControl> pChild);
};

// <window att1="a">
// <rootControl att2="b" att3="c">
// <childControl1/>
// <childControl2/>
//  .....
// <childControlN/>
// </rootControl>
// </window>

class DUILIB_API CDUIControlBuilder
{
public:
	CDUIControlBuilder(IDUIControlBuilderCallback* pCallback = NULL);
	
	CRefPtr<IDUIControl> CreateFromXML(const CDUIString&  strTemplateXML);
	 CRefPtr<IDUIControl> CreateFromResource(const CDUIString& strTemplateID);

	 IDUIControlBuilderCallback* GetCallBack() const;

protected:
	IDUIControlBuilderCallback* m_pCallBack;
};

class DUILIB_API CDUIMenuBuilder
{
public:
	CDUIMenuBuilder(IDUIControlBuilderCallback* pCallback = NULL);

	CRefPtr<IDUIMenu> CreateFromXML(const CDUIString&  strTemplateXML);
	CRefPtr<IDUIMenu> CreateFromResource(const CDUIString& strTemplateID);

	IDUIControlBuilderCallback* GetCallBack() const;
	
protected:
	IDUIControlBuilderCallback* m_pCallBack;
};


DUI_END_NAMESPCE

#endif //__DUI_CONTROL_BUILDER_H__