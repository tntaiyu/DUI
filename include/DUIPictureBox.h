#ifndef __DUI_PICTURE_BOX_H__
#define __DUI_PICTURE_BOX_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUIPictureBox: public CDUIControlBase
{
public:
	virtual ~IDUIPictureBox() { NULL;}
	
	virtual VOID SetImage(CRefPtr<CImageList> pImage) = 0;
	virtual const CRefPtr<CImageList>& GetImage() const = 0;
	
	virtual VOID SetDrawMode(DWORD dwMode) = 0;
	virtual DWORD GetDrawMode() const = 0;
	
	virtual INT GetIconCount() = 0;
	virtual VOID SetCurrentIconIndex(INT nIndex) = 0;
	virtual INT GetCurrentIconIndex() const = 0;
};

class DUILIB_API CDUIPictureBox: public IDUIPictureBox
{
public:
	typedef IDUIPictureBox theBase;
	CDUIPictureBox();
	virtual ~CDUIPictureBox();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual BOOL EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet);

	//IDUIPictureBox
	virtual VOID SetImage(CRefPtr<CImageList> pImage);
	virtual const CRefPtr<CImageList>& GetImage() const;
	
	virtual VOID SetDrawMode(DWORD dwMode);
	virtual DWORD GetDrawMode() const;	

	virtual INT GetIconCount();
	virtual VOID SetCurrentIconIndex(INT nIndex);
	virtual INT GetCurrentIconIndex() const;

protected:
	virtual VOID PaintBkgnd(HDC dc);

protected:
	DWORD m_dwDrawMode;
	CRefPtr<CImageList> m_pImage;
	INT m_nCurrentIndex;
};

DUI_END_NAMESPCE


#endif //__DUI_PICTURE_BOX_H__