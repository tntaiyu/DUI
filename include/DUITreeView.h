#ifndef __DUI_TREEVIEW_H__
#define __DUI_TREEVIEW_H__


#pragma once
/*#include "DUIControlBase.h"*/
#include "DUICanvas.h"
#include "DUIControlContainer.h"
#include "DUIButton.h"

DUI_BGN_NAMESPCE

#define  KEEP_TREE_STRU	0 

class IDUITreeViewItem;
class CDUITreeNode;

class IDUITreeView;
class CDUITreeViewImpl;

typedef	CDUITreeNode*		HDUITREEITEM;


class DUILIB_API	IDUITreeViewItem /*: public CDUIControlBase*/
									:public CDUIButton
{
public:
	virtual	void	SetItemHeight(INT nHeight) = 0;
	virtual INT		GetItemHeight() = 0;
};

class DUILIB_API CDUITreeNode :	public IDUITreeViewItem
{
public:
	//IDUITreeViewItem
	virtual	void	SetItemHeight(INT nHeight);
	virtual INT		GetItemHeight() ;

	typedef	IDUITreeViewItem	theBase;

	CDUITreeNode();
	virtual ~CDUITreeNode();

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();

	virtual VOID SetControlRect(const RECT& rt);
	virtual VOID DoPaint(HDC dc, const RECT& rtDraw);


	BOOL		IsParent();
	BOOL		IsExpand();
	BOOL		SetExpand(BOOL bExpand);

	INT			GetTotalHeight();
	//HDUITREEITEM		GetTopItem(INT nOffsetY);
	//! root is 1, child is 2, then +++
	INT			GetLevel();
	BOOL		HaveChild();

	HDUITREEITEM	GetParent();
	HDUITREEITEM	GetNextSibling();
	HDUITREEITEM	GetPrevSibling();
	HDUITREEITEM	GetFirstChild();
	HDUITREEITEM	GetChild(int nIndex);

	void			SetSel(BOOL bSel);
	BOOL			IsSelected();

	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);


	void			FireNotifyToTreeView(const DUIEvent& event,BOOL& bHandled);

	VOID			SetAttribute(const CDUIString& strName, const CDUIString& strValue);
protected:
	BOOL				m_bExpand;
	INT					m_nItemHeight;
	BOOL				m_bSelected;

	//CDUITreeViewImpl	*m_lpTreeViewImpl;

	CRefPtr<CImageList>		m_pImgListBK;
	CRefPtr<CImageList>		m_pImgListIcon;
};



class DUILIB_API IDUITreeView : public CDUIControlBase
{
public:
	virtual HDUITREEITEM	AddRoot(HDUITREEITEM hItem)  = 0;
	virtual HDUITREEITEM	AddChildItem(HDUITREEITEM hParent , HDUITREEITEM hChild) = 0;
	virtual	HDUITREEITEM	GetTopItem() = 0;
	virtual	HDUITREEITEM	GetRoot() = 0;
};

//////////////////////////////////////////////////////////////////////////
//implementation of treeview
//////////////////////////////////////////////////////////////////////////
class DUILIB_API CDUITreeViewImpl : public IDUITreeView
									
{

public:
	
	typedef IDUITreeView	theBase;

	//IDUITreeView
	virtual HDUITREEITEM	AddRoot(HDUITREEITEM hItem)  ;
	virtual HDUITREEITEM	AddChildItem(HDUITREEITEM hParent , HDUITREEITEM hChild) ;
	virtual	HDUITREEITEM	GetTopItem();
	virtual HDUITREEITEM	GetRoot();

	virtual CRefPtr<IDUIControl> Clone();

	CDUITreeViewImpl();
	virtual ~CDUITreeViewImpl();

	virtual VOID SetControlRect(const RECT& rt);
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual LPVOID	GetInterface(const CDUIString& strName);
	virtual VOID	DoPaint(HDC dc, const RECT& rtDraw);
	virtual IDUIControl* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);


	INT					CalculateTotalHeight();
	HDUITREEITEM		CalculateTopItem();

	HDUITREEITEM		CalculateTopItem(HDUITREEITEM pItem,INT& nTempY,INT nOffsetY);
	//! find the next visible item according to pItem
	HDUITREEITEM		GetNextVisibleItem(HDUITREEITEM pItem);

	void				SetItemHeight(INT nHeight);

	BOOL				IsExpand();
	INT					GetItemHeight();

protected:
	virtual	IDUIScrollBar* GetVerticalSB();
	virtual VOID OnCreate();
	virtual VOID OnDestroy();

	VOID OnItemClick(const DUIEvent& info);

protected:
	HDUITREEITEM	m_hTopItem;

	CRefPtr<IDUIScrollBar>	m_pVertSB;
	CRefPtr<IDUIScrollBar>	m_pHorzSB;

	INT						m_nTotalHeight;
	INT						m_nExtraHeight;

	INT						m_nMaxRight;
};


DUI_END_NAMESPCE

#endif  //#ifndef __DUI_TREEVIEW_H__