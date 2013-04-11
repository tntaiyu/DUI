#ifndef __DUI_COMBOBOX_H__
#define __DUI_COMBOBOX_H__

#include "DUIListBox.h"

DUI_BGN_NAMESPCE


class IDUIRichEdit;
class IDUILabel;
class IDUIButton;
class CDUIComboList;

class IDUIComboBox: public CDUIControlBase
{
public:
	virtual ~IDUIComboBox() { NULL; }	
	virtual VOID SetComboBoxStyle(DWORD dwStyle) = 0;
	virtual	DWORD GetComboBoxStyle() const = 0;

	virtual BOOL SetSelectIndex(INT nIndex) = 0;
	virtual INT GetSelectIndex() const = 0;

	virtual VOID SetComboListFrameBK(CRefPtr<CDUIFrameBK> pFrameBK) = 0;
	virtual CRefPtr<CDUIFrameBK> GetComboListFrameBK() const = 0;

	virtual INT AddListItem(CRefPtr<IDUIControl> pItem) = 0;
	virtual INT GetListItemCount() const = 0;
	virtual IDUIControl* GetListItemByIndex(INT nIndex) = 0;
	virtual VOID RemoveListItem(CRefPtr<IDUIControl> pItem) = 0;
	virtual VOID RemoveAllListItem() = 0;

	virtual IDUIRichEdit* GetEditControl() const = 0;
};

class CDUIComboBox: public IDUIComboBox
{
public:
	typedef IDUIComboBox theBase;
	CDUIComboBox();
	CDUIComboBox(const CDUIComboBox& obj);

	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual CRefPtr<IDUIControl> Clone();
	virtual BOOL Activate();

	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);
	virtual VOID SetControlRect(const RECT& rt);

	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	
	virtual VOID SetText(const CDUIString& strText);
	virtual VOID SetTextStyle(CRefPtr<CTextStyle> pTextStyle);


//	IDUIComboBox
	virtual VOID SetComboBoxStyle(DWORD dwStyle);
	virtual	DWORD GetComboBoxStyle() const;

	virtual BOOL SetSelectIndex(INT nIndex);
	virtual INT GetSelectIndex() const;

	virtual VOID SetComboListFrameBK(CRefPtr<CDUIFrameBK> pFrameBK);
	virtual CRefPtr<CDUIFrameBK> GetComboListFrameBK() const;

	virtual BOOL AddListItem(CRefPtr<IDUIControl> pItem);
	virtual INT GetListItemCount() const;
	virtual IDUIControl* GetListItemByIndex(INT nIndex);
	virtual VOID RemoveListItem(CRefPtr<IDUIControl> pItem);
	virtual VOID RemoveAllListItem();

	virtual IDUIRichEdit* GetEditControl() const;

protected:
	virtual VOID OnCreate();
	virtual VOID OnDestroy();

	virtual VOID PaintBkgnd(HDC dc);

protected:
	BOOL CreateCtrl();
	VOID InitStatus();

	VOID OnKeyDown(const DUIEvent& info, BOOL& bHandled);
	VOID OnTxNotify(DWORD iNotify, void *pv);

	IDUIButton* GetArrowBtn();
	IDUIRichEdit* GetEdit() const;
	IDUILabel* GetLabel();
	IDUIListBox* GetList();

	VOID ShowListWnd();
	VOID HideListWnd();

	VOID BeforeShowListWnd();
	VOID BeforeHideListWnd();

	CRefPtr<CDUIFrameBK> GetDefaultFrameBK();

	//nType: 0-top_index, 1-down, 2-up
	BOOL SetSelectIndexWrapper(INT nIndex, INT nType);

protected:
	CRefPtr<IDUIListBox> m_pList;
	DWORD m_dwComboBoxStyle;
	CRefPtr<CImageList> m_pImageBK;
	INT m_nListHeight;
	INT m_nLastIndex;

	CRefPtr<CDUIFrameBK> m_pFrameBK;

	friend class CComboEdit;
	friend class CDUIComboList;
	CDUIComboList* m_pListWnd;
};


DUI_END_NAMESPCE


#endif //__DUI_COMBOBOX_H__