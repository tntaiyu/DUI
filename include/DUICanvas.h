#ifndef __DUI_CANVAS_H__
#define __DUI_CANVAS_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUICanvas: public CDUIControlBase
{
public:
	virtual ~IDUICanvas() { NULL;}

	virtual VOID SetFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetCanvasUIData() const = 0;
};


class DUILIB_API CDUICanvas: public IDUICanvas
{
public:
	typedef IDUICanvas theBase;

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	//IDUICanvas
	virtual VOID SetFrameBK(CRefPtr<CDUIFrameBK> pFrameBK);
	virtual CRefPtr<CDUIFrameBK> GetCanvasUIData() const;

protected:
	virtual VOID PaintBkgnd(HDC dc);

protected:
	CRefPtr<CDUIFrameBK> m_pFrameBK;
};


DUI_END_NAMESPCE



#endif //__DUI_CANVAS_H__