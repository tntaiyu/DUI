#ifndef __DUI_LABEL_H__
#define __DUI_LABEL_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUILabel: public CDUIControlBase
{
public:
};

class DUILIB_API CDUILabel: public IDUILabel
{
public:
	typedef IDUILabel theBase;
	CDUILabel();
	
	virtual CRefPtr<IDUIControl> Clone();
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual LPVOID GetInterface(const CDUIString& strName);

	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet);

protected:
	COLORREF GetLabelTextColor();	
	virtual VOID PaintText(HDC dc);
};

class DUILIB_API IDUIHyperLink: public CDUILabel
{
public:
	virtual VOID SetHyperLink(const CDUIString& strLink) = 0;
	virtual const CDUIString& GetHyperLink() const = 0;
};

class DUILIB_API CDUIHyperLink: public IDUIHyperLink
{
public:
	typedef IDUIHyperLink theBase;
	CDUIHyperLink();

	virtual CRefPtr<IDUIControl> Clone();
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual BOOL Activate();
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	virtual VOID SetHyperLink(const CDUIString& strLink);
	virtual const CDUIString& GetHyperLink() const;
	
protected:
	CDUIString m_strHyperLink;
};


DUI_END_NAMESPCE


#endif //__DUI_LABEL_H__