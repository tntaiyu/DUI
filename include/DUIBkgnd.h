#ifndef __DUI_BKGND_H__
#define __DUI_BKGND_H__

#include "DUIImage.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUIBkgnd: public CDUIObject
{
public:
	virtual ~IDUIBkgnd() { NULL; }
	
	virtual BOOL IsEmpty() const = 0;
	virtual VOID DrawBK(HDC dc, const RECT& rt) = 0;
};

class  DUILIB_API  CDUIColorBK: public IDUIBkgnd
{
public:
	CDUIColorBK();
	virtual ~CDUIColorBK() { NULL; }

	VOID SetBKColor(const CARGB& clr);
	VOID SetBKColor(const CARGB& clr1, const CARGB& clr2, BOOL bVertical = TRUE);
	//dbPos > 1, use absolute value, else use "length*dbPos"
	VOID SetBKColor(const CARGB& clr1, const CARGB& clr2, const CARGB& clrMid, double dbPos = 0.5, BOOL bVertical = TRUE);
	
	VOID SetBKVertical(BOOL bVertical);
	VOID SetBKSepPos(double dbPos);

	CARGB GetBKColor() const;
	VOID GetBKColor(CARGB& clr1, CARGB& clr2, BOOL& bVertical);
	VOID GetBKCOlor(CARGB& clr1, CARGB& clr2, CARGB& clrMid, double& dbPos, BOOL& bVertical);

	BOOL GetBKVertical() const { return m_bVertical; }
	double GetBKSepPos() const { return m_dbPos; }

	//IDUIBkgnd
	virtual BOOL IsEmpty() const;
	virtual VOID DrawBK(HDC dc, const RECT& rt);

protected:
	BOOL IsOneColorEmpty() const;
	BOOL IsTwoColorEmpty() const;
	BOOL IsTreeColorEmpty() const;

protected:
	CARGB m_clrBK;
	
	CARGB m_clrBK1;
	CARGB m_clrBK2;
	BOOL m_bVertical;

	CARGB m_clrMid;
	double m_dbPos;
};

enum FRAME_SKIN_IMAGE_POS
{
	SKIN_POS_LEFTTOP,
	SKIN_POS_LEFTBOTTOM,
	SKIN_POS_RIGHTTOP,
	SKIN_POS_RIGHTBOTTOM,
	SKIN_POS_CENTER
};

class DUILIB_API CFrameBKImagePos
{
public:
	CFrameBKImagePos()
	{
		m_ePosType = SKIN_POS_LEFTTOP;
		m_nOffsetX = 0;
		m_nOffsetY = 0;
	}

	FRAME_SKIN_IMAGE_POS m_ePosType;
	INT m_nOffsetX;
	INT m_nOffsetY;
};


class DUILIB_API CDUIColorImageBK: public CDUIColorBK
{
public:
	typedef CDUIColorBK theBase;
	virtual ~CDUIColorImageBK() { NULL; }

	VOID SetSkinImage(CRefPtr<CImageList> pImage);
	CRefPtr<CImageList> GetSkinImage() const;

	VOID SetSkinImagePos(const CFrameBKImagePos& pos);
	CFrameBKImagePos GetSkinImagePos() const;

	//IDUIBkgnd
	virtual BOOL IsEmpty() const;
	virtual VOID DrawBK(HDC dc, const RECT& rt);

protected:
	RECT GetSkinImageRect(RECT rtBK);

protected:
	CRefPtr<CImageList> m_pImageSkin;
	CFrameBKImagePos m_skinImagePos;
};


class  DUILIB_API  CDUIFrameBK: public CDUIColorImageBK
{
public:
	typedef CDUIColorImageBK theBase;
	CDUIFrameBK();
	virtual ~CDUIFrameBK() { NULL; }

	VOID SetFrameImage(CRefPtr<CImageList> pImageFrame);

	VOID SetFrameColor(const CARGB& clrFrame);
	VOID SetFrameRound(INT nCornerWidth, INT nCornerHeight);

	VOID SetFrameBorder(const RECT& rtFrame);

	CRefPtr<CImageList> GetFrameImage() const;
	CARGB GetFrameColor() const;
	VOID GetFrameRound(INT& nCornerWidth, INT& nCornerHeight);
	RECT GetFrameBorder() const;

	BOOL SetWinFrameRgn(HWND hWnd);

	//IDUIBkgnd
	virtual BOOL IsEmpty() const;
	virtual VOID DrawBK(HDC dc, const RECT& rt);

protected:
	BOOL IsImageFrameAvailable() const;
	BOOL IsColorFrameAvailable() const;
	BOOL IsFrameEmpty() const; 

protected:
	CARGB m_clrFrame;
	RECT m_rtFrameBorder;
	INT m_nCornerWidth;
	INT m_nCornerHeight;
	CRefPtr<CImageList> m_pImageBorder;
};


//class CDUI

DUI_END_NAMESPCE


#endif //__DUI_BKGND_H__