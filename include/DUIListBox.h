#ifndef __DUI_LIST_BOX_H__
#define __DUI_LIST_BOX_H__

#include "DUICanvas.h"
#include "DUIControlContainer.h"

DUI_BGN_NAMESPCE

class IDUIListBox;
class DUILIB_API IDUIListItem: public CDUIControlBase
{
public:
	virtual ~IDUIListItem() { NULL; }
	
	virtual VOID SetSelect(BOOL bSet) = 0;
	virtual BOOL GetSelect() const = 0;

	virtual VOID SetExpand(BOOL bExpand) = 0;
	virtual BOOL GetExpand() const = 0;

	virtual VOID SetGroup(BOOL bGroup) = 0;
	virtual BOOL GetGroup() const = 0;

	virtual VOID SetHeight(BOOL bExpand, INT nValue) = 0;
	virtual INT GetHeight(BOOL bExpand) const = 0;

	virtual VOID SetItemData(DWORD dwData) = 0;
	virtual DWORD GetItemData() const = 0;

	virtual VOID SetHoverFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetHoverFrameBK() const = 0;

	virtual VOID SetSelectFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetSelectFrameBK() const = 0;
};

class DUILIB_API CIconTextItemUIData
{
public:
	CRefPtr<CImageList> m_pNormalIcon;
	CRefPtr<CImageList> m_pExpandIcon;
};

class DUILIB_API IDUIIconTextItem: public IDUIListItem
{
public: 
	virtual ~IDUIIconTextItem() { NULL; }
	typedef IDUIListItem theBase;
	
	virtual VOID SetUIData(const CIconTextItemUIData& data) = 0;
	virtual const CIconTextItemUIData& GetUIdata() const = 0;

	virtual VOID SetIconIndex(INT nIndex) = 0;
	virtual INT GetIconIndex() const = 0;
	
	
	//IDUIListItem
	virtual VOID SetSelect(BOOL bSet) = 0;
	virtual BOOL GetSelect() const = 0;

	virtual VOID SetExpand(BOOL bExpand) = 0;
	virtual BOOL GetExpand() const = 0;

	virtual VOID SetGroup(BOOL bGroup) = 0;
	virtual BOOL GetGroup() const = 0;

	virtual VOID SetHeight(BOOL bExpand, INT nValue) = 0;
	virtual INT GetHeight(BOOL bExpand) const = 0;

	virtual VOID SetItemData(DWORD dwData) = 0;
	virtual DWORD GetItemData() const = 0;

	virtual VOID SetHoverFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetHoverFrameBK() const = 0;

	virtual VOID SetSelectFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetSelectFrameBK() const = 0;
};

class DUILIB_API CDUIIconTextItem: public IDUIIconTextItem
{
public:
	typedef IDUIIconTextItem theBase;
	CDUIIconTextItem();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);	
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	//IDUIIconTextItem
	virtual VOID SetUIData(const CIconTextItemUIData& data);
	virtual const CIconTextItemUIData& GetUIdata() const;
	
	virtual VOID SetIconIndex(INT nIndex);
	virtual INT GetIconIndex() const;

	
	//IDUIListItem
	virtual VOID SetSelect(BOOL bSet);
	virtual BOOL GetSelect() const;


	virtual VOID SetExpand(BOOL bExpand);
	virtual BOOL GetExpand() const;

	virtual VOID SetGroup(BOOL bGroup);
	virtual BOOL GetGroup() const;

	virtual VOID SetHeight(BOOL bExpand, INT nValue);
	virtual INT GetHeight(BOOL bExpand) const;

	virtual VOID SetItemData(DWORD dwData);
	virtual DWORD GetItemData() const;

	virtual VOID SetHoverFrameBK(CRefPtr<CDUIFrameBK> pFrameBK);
	virtual CRefPtr<CDUIFrameBK> GetHoverFrameBK() const;

	virtual VOID SetSelectFrameBK(CRefPtr<CDUIFrameBK> pFrameBK);
	virtual CRefPtr<CDUIFrameBK> GetSelectFrameBK() const;

protected:
	virtual VOID PaintBkgnd(HDC dc);
	virtual VOID PaintForgnd(HDC dc);
	virtual VOID PaintText(HDC dc);

	RECT GetTextRect();
	RECT GetIconRect();
	RECT GetBKRect();

protected:
	BOOL m_bSelect;
	BOOL m_bExpand;
	BOOL m_bGroup;
	INT m_nIconIndex;
	INT m_nNormalHeight;
	INT m_nExpandHeight;
	DWORD m_dwItemData;

	CIconTextItemUIData m_uiData;

	CRefPtr<CDUIFrameBK> m_pHoverFrameBK;
	CRefPtr<CDUIFrameBK> m_pSelectFrameBK;
};

class DUILIB_API IDUIListBox: public CDUIControlBase
{
public:
	virtual INT GetCurSel() const = 0;
	virtual BOOL SetCurSel(INT nIndex) = 0;

	virtual INT GetTopIndex() const = 0;
	virtual BOOL SetTopIndex(INT nIndex) = 0;
	virtual INT GetLastIndex() const = 0;

	virtual BOOL ExpandItem(INT nIndex, BOOL bExpand) = 0;

	virtual VOID SetSelectExpand(BOOL bSet) = 0;
	virtual BOOL GetSelectExpand() const = 0;

	virtual VOID SetHoverExpand(BOOL bSet) = 0;
	virtual BOOL GetHoverExpand() const = 0;

	virtual VOID SetHoverSelect(BOOL bSet) = 0;
	virtual BOOL GetHoverSelect() const = 0;

	virtual VOID SetExtraExpandHeight(INT nHeight) = 0;
};

class DUILIB_API CDUIListBox: public IDUIListBox
{
public:
	typedef IDUIListBox theBase;
	CDUIListBox();
	
	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();

	virtual VOID SetControlRect(const RECT& rt);
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual VOID DoPaint(HDC dc, const RECT& rtDraw);
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	virtual IDUIControl* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	//IDUIListBox
	virtual INT GetCurSel() const;
	virtual BOOL SetCurSel(INT nIndex);

	virtual INT GetTopIndex() const;
	virtual BOOL SetTopIndex(INT nIndex);

	virtual BOOL ExpandItem(INT nIndex, BOOL bExpand);

	virtual VOID SetSelectExpand(BOOL bSet);
	virtual BOOL GetSelectExpand() const;
	virtual INT GetLastIndex() const;

	virtual VOID SetHoverExpand(BOOL bSet);
	virtual BOOL GetHoverExpand() const;

	virtual VOID SetHoverSelect(BOOL bSet);
	virtual BOOL GetHoverSelect() const;

	virtual VOID SetExtraExpandHeight(INT nHeight);

protected:
	virtual VOID OnCreate();
	virtual VOID OnDestroy();
	virtual	IDUIScrollBar* GetVerticalSB();

	virtual BOOL BeforeAddControl(CRefPtr<IDUIControl> pItem);
	virtual VOID AfterRemoveControl(CRefPtr<IDUIControl> pItem);

	virtual INT GetScrollLineStep();

protected:
	INT CalculateTotalHeight();
	INT CalculateTopIndex();

	INT GetVisibleItemIndex(IDUIControl* pItem);
	VOID OnItemClick(const DUIEvent& info);
	VOID OnItemDBClick(const DUIEvent& info);
	VOID OnItemHover(const DUIEvent& info);
	BOOL OnKeyDown(const DUIEvent& info);

	BOOL SetCurHoverExpand(INT nIndex);
	VOID ExpandGroup(INT nIndex, BOOL bExpand);

	VOID CheckInitGroupStatus();

protected:
	friend class CDUIComboBox;

	CRefPtr<IDUIScrollBar> m_pVertSB;
	INT m_nTopIndex;
	INT m_nLastIndex;
	INT m_nTotalHeight;
	INT m_nExtraHeight;
	
	INT m_nSelectIndex;
	INT m_nHoverIndex;
	
	BOOL m_bSelectExpand;
	BOOL m_bHoverExpand;
	BOOL m_bHoverSelect;
};


DUI_END_NAMESPCE

#endif //__DUI_LIST_BOX_H__