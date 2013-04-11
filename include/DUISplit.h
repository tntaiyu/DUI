#ifndef __DUI_SPLIT_H__
#define __DUI_SPLIT_H__

#include "DUIControlBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUISplit: public CDUIControlBase
{
public:
	virtual ~IDUISplit() { NULL;}

	virtual VOID SetSplitMinSize(INT nItem1, INT nItem2) = 0;
	virtual	BOOL GetSplitMinSize(INT& nItem1, INT& nItem2) = 0;

	virtual	VOID SetSplitPos(INT nPos) = 0;
	virtual INT GetSplitPos() const = 0;

	virtual VOID SetResizeStyle(DWORD dwStyle) = 0;
	virtual DWORD GetResizeStyle() const = 0;
};	 


class DUILIB_API CDUISplitBase: public IDUISplit
{
public:
	typedef IDUISplit theBase;
	CDUISplitBase();
	virtual ~CDUISplitBase();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	virtual LRESULT ProcessEvent(const DUIEvent& event, BOOL& bHandled);
	virtual VOID SetControlRect(const RECT& rt);

	//IDUISplit
	virtual VOID SetSplitMinSize(INT nItem1, INT nItem2);
	virtual	BOOL GetSplitMinSize(INT& nItem1, INT& nItem2);

	virtual	VOID SetSplitPos(INT nPos);
	virtual INT GetSplitPos() const;

	virtual VOID SetResizeStyle(DWORD dwStyle);
	virtual DWORD GetResizeStyle() const;
	
protected:
	virtual BOOL IsVertical() = 0;
	
	INT CalcSplitPos(RECT& rtOld, RECT& rtNew);
	VOID LayoutSplit(INT nPos);
	VOID SaveCurrentRadtio();
	VOID GetSplitterRect(RECT& rtSplitter);
	
	LRESULT OnLButtonDown(const DUIEvent& info);
	LRESULT OnLButtonUp(const  DUIEvent& info);
	LRESULT OnMouseMove(const DUIEvent& info);

protected:
	INT m_nSplitPos;
	INT m_nMinItem1;
	INT m_nMinItem2;

	double m_dbRatio;
	BOOL m_bTracking;
	DWORD m_dwTrackStatus;
	DWORD m_dwResizeStyle;
};

class DUILIB_API CDUIVerticalSplit: public CDUISplitBase
{
public:
	virtual CRefPtr<IDUIControl> Clone();
protected:
	virtual BOOL IsVertical() { return TRUE; }
};

class DUILIB_API CDUIHorizontalSplit: public CDUISplitBase
{
public:
	virtual CRefPtr<IDUIControl> Clone();

protected:
	virtual BOOL IsVertical() { return FALSE; }
};


DUI_END_NAMESPCE

#endif //__DUI_SPLIT_H__