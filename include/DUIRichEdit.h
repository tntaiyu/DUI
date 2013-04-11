#ifndef __DUI_RICH_EDIT_H__
#define __DUI_RICH_EDIT_H__

#include "DUIControlBase.h"
#include <richedit.h>


#ifndef EM_GETZOOM
#define EM_GETZOOM				(WM_USER + 224)
#endif //EM_GETZOOM

#ifndef EM_SETZOOM
#define EM_SETZOOM				(WM_USER + 225)
#endif //EM_SETZOOM


DUI_BGN_NAMESPCE

enum EDIT_BACK_IMAGE_INDEX
{
	EDIT_IMAGE_INDEX_NORMAL = 0,
	EDIT_IMAGE_INDEX_HOVER,
	EDIT_IMAGE_INDEX_DISABLE,
	EDIT_IMAGE_INDEX_FOCUS,

	EDIT_IMAGE_INDEX_COUNT
};

class IDUIRichEdit: public CDUIControlBase
{
public:
	virtual ~IDUIRichEdit() { NULL; }

	virtual BOOL GetViewInset(RECT& rt) const = 0;
	virtual BOOL SetViewInset(const RECT& rt) = 0;

    virtual BOOL IsRich() const = 0;
    virtual VOID SetRich(BOOL bRich = TRUE) = 0;

    virtual BOOL IsReadOnly() const = 0;
    virtual VOID SetReadOnly(BOOL bReadOnly = TRUE) = 0;

    virtual BOOL GetWordWrap() const = 0;
    virtual VOID SetWordWrap(BOOL bWordWrap = TRUE) = 0;

    virtual DWORD GetEditStyle() const = 0;
    virtual VOID SetEditStyle(DWORD dwStyle) = 0;
 
	virtual INT GetLimitText() const = 0;
    virtual VOID SetLimitText(INT iChars) = 0;

    virtual DWORD GetTextLength(DWORD dwFlags = GTL_DEFAULT) const = 0;

    virtual BOOL GetModify() const = 0;
    virtual VOID SetModify(BOOL bModified = TRUE) const = 0;

    virtual VOID GetSel(INT& nStartChar, INT& nEndChar) const = 0;
    virtual INT SetSel(INT nStartChar, INT nEndChar) const = 0;

    virtual VOID ReplaceSel(const CDUIString& lpszNewText, BOOL bCanUndo) = 0;
    virtual CDUIString GetSelText() const = 0;

    virtual INT SetSelAll() = 0;
    virtual INT SetSelNone() = 0;

    virtual BOOL GetZoom(int& nNum, int& nDen) const = 0;
    virtual BOOL SetZoom(int nNum, int nDen) = 0;
	virtual BOOL SetZoomOff() = 0;

    virtual WORD GetSelectionType() const = 0;

    virtual BOOL GetAutoURLDetect() const = 0;
    virtual BOOL SetAutoURLDetect(BOOL bAutoDetect = TRUE) = 0;

    virtual DWORD GetEventMask() const = 0;
    virtual VOID SetEventMask(DWORD dwEventMask) = 0;

	virtual CDUIString GetTextRange(long nStartChar, long nEndChar) const = 0;
	virtual VOID HideSelection(BOOL bHide = TRUE) = 0;
	virtual VOID ScrollCaret() = 0;

    virtual INT InsertText(INT nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo = FALSE) = 0;
    virtual INT AppendText(LPCTSTR lpstrText, BOOL bCanUndo = FALSE) = 0;

    virtual DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const = 0;
    virtual BOOL SetDefaultCharFormat(CHARFORMAT2 &cf) = 0;

    virtual DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const = 0;
    virtual BOOL SetSelectionCharFormat(CHARFORMAT2 &cf) = 0;
    virtual BOOL SetWordCharFormat(CHARFORMAT2 &cf) = 0;

    virtual DWORD GetParaFormat(PARAFORMAT2 &pf) const = 0;
    virtual BOOL SetParaFormat(PARAFORMAT2 &pf) = 0;

	virtual BOOL CanUndo() = 0;
	virtual BOOL CanRedo() = 0;
	virtual BOOL CanPaste() = 0;
    virtual BOOL Redo() = 0;
    virtual BOOL Undo() = 0;
    virtual VOID Clear() = 0;
    virtual VOID Copy() = 0;
    virtual VOID Cut() = 0;
    virtual VOID Paste() = 0;

	virtual VOID SetEnableContextMenu(BOOL bEnable) = 0;
	virtual BOOL GetEnableContextMenu() const = 0;

    virtual INT GetLineCount() const = 0;
    virtual CDUIString GetLine(int nIndex, int nMaxLength) const = 0;

    virtual INT LineIndex(int nLine = -1) const = 0;
    virtual INT LineLength(int nLine = -1) const = 0;

	virtual BOOL LineScroll(int nLines, int nChars = 0) = 0;

    virtual long LineFromChar(long nIndex) const = 0;
    virtual POINT PosFromChar(UINT nChar) const = 0;
    virtual INT CharFromPos(POINT pt) const = 0;

    virtual VOID EmptyUndoBuffer() = 0;
	virtual UINT SetUndoLimit(UINT nLimit) = 0;

	virtual long StreamIn(int nFormat, EDITSTREAM &es) = 0;
    virtual long StreamOut(int nFormat, EDITSTREAM &es) = 0;
	
	virtual INT GetFirstVisibleLine() const = 0;

    virtual IDropTarget* GetTxDropTarget() = 0;
	virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult = NULL) const = 0;
};

class CDUIRichEdit: public IDUIRichEdit,
					public IDUIMessageFilter
{
public:
	typedef IDUIRichEdit theBase;
	CDUIRichEdit();
	CDUIRichEdit(const CDUIRichEdit& obj);
	virtual ~CDUIRichEdit();
	
	//IDUIMessageFilter
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//IDUIControl
	virtual CRefPtr<IDUIControl> Clone();
	virtual LRESULT ProcessEvent(const DUIEvent& info, BOOL& bHandled);
	virtual LPVOID GetInterface(const CDUIString& strName);
	virtual VOID SetAttribute(const CDUIString& strName, const CDUIString& strValue);

	virtual VOID SetControlRect(const RECT& rt);
	virtual VOID DoPaint(HDC dc, const RECT& rtDraw);

	virtual	VOID SetText(const CDUIString& strText);
	virtual const CDUIString& GetText();

	virtual VOID SetTextStyle(CRefPtr<CTextStyle> pTextStyle);
	virtual VOID ModifyStatus(DWORD dwRemove, DWORD dwAdd);
	
	//IDUIRichEdit
	virtual BOOL GetViewInset(RECT& rt) const;
	virtual BOOL SetViewInset(const RECT& rt);

    virtual BOOL IsRich() const;
    virtual VOID SetRich(BOOL bRich = TRUE);

    virtual  BOOL IsReadOnly() const;
    virtual  VOID SetReadOnly(BOOL bReadOnly = TRUE);

    virtual  BOOL GetWordWrap() const;
    virtual  VOID SetWordWrap(BOOL bWordWrap = TRUE);

    virtual  DWORD GetEditStyle() const;
    virtual  VOID SetEditStyle(DWORD dwStyle);
 
	virtual  INT GetLimitText() const;
    virtual  VOID SetLimitText(INT iChars);

    virtual  DWORD GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;

    virtual   BOOL GetModify() const;
    virtual  VOID SetModify(BOOL bModified = TRUE) const;

    virtual  VOID GetSel(INT& nStartChar, INT& nEndChar) const;
    virtual  INT SetSel(INT nStartChar, INT nEndChar) const;

    virtual VOID ReplaceSel(const CDUIString& lpszNewText, BOOL bCanUndo);
    virtual CDUIString GetSelText() const;

    virtual INT SetSelAll();
    virtual int SetSelNone();

    virtual BOOL GetZoom(int& nNum, int& nDen) const;
    virtual BOOL SetZoom(int nNum, int nDen);
	virtual BOOL SetZoomOff();

    virtual WORD GetSelectionType() const;

    virtual BOOL GetAutoURLDetect() const;
    virtual BOOL SetAutoURLDetect(BOOL bAutoDetect = TRUE);

    virtual DWORD GetEventMask() const;
    virtual VOID SetEventMask(DWORD dwEventMask);

    virtual CDUIString GetTextRange(long nStartChar, long nEndChar) const;

    virtual VOID HideSelection(BOOL bHide = TRUE);
    virtual VOID ScrollCaret();

    virtual INT InsertText(INT nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo = FALSE);
    virtual INT AppendText(LPCTSTR lpstrText, BOOL bCanUndo = FALSE);

    virtual DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
    virtual BOOL SetDefaultCharFormat(CHARFORMAT2 &cf);

    virtual DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
    virtual BOOL SetSelectionCharFormat(CHARFORMAT2 &cf);
    virtual BOOL SetWordCharFormat(CHARFORMAT2 &cf);

    virtual DWORD GetParaFormat(PARAFORMAT2 &pf) const;
    virtual BOOL SetParaFormat(PARAFORMAT2 &pf);

	virtual BOOL CanUndo();
	virtual BOOL CanRedo();
	virtual BOOL CanPaste();
    virtual BOOL Redo();
    virtual BOOL Undo();
    virtual VOID Clear();
    virtual VOID Copy();
    virtual VOID Cut();
    virtual VOID Paste();

	virtual VOID SetEnableContextMenu(BOOL bEnable);
	virtual BOOL GetEnableContextMenu() const;

    virtual INT GetLineCount() const;
    virtual CDUIString GetLine(int nIndex, int nMaxLength) const;

    virtual INT LineIndex(int nLine = -1) const;
    virtual INT LineLength(int nLine = -1) const;

    virtual BOOL LineScroll(int nLines, int nChars = 0);

    virtual long LineFromChar(long nIndex) const;
    virtual POINT PosFromChar(UINT nChar) const;
    virtual INT CharFromPos(POINT pt) const;

    virtual VOID EmptyUndoBuffer();
    virtual UINT SetUndoLimit(UINT nLimit);

	virtual long StreamIn(int nFormat, EDITSTREAM &es);
    virtual long StreamOut(int nFormat, EDITSTREAM &es);
	
	virtual INT GetFirstVisibleLine() const;

    virtual IDropTarget* GetTxDropTarget();
	virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult = NULL) const;

protected:
	virtual VOID OnCreate();
	virtual VOID OnDestroy();

	virtual IDUIScrollBar* GetVerticalSB();
	virtual IDUIScrollBar* GetHorizontalSB();

	virtual VOID PaintBorder(HDC dc);
	virtual VOID PaintBkgnd(HDC dc);

    virtual BOOL OnTxViewChanged();
    virtual VOID OnTxNotify(DWORD iNotify, void *pv);

protected: 
	VOID SetupMessageFilter(BOOL bAdd);
    VOID GetSel(const CHARRANGE &cr) const;
	INT SetSel(const CHARRANGE &cr) const;

	VOID CreateScrollBar();
	VOID DestroyScrollBar();
	
	VOID OnVertialSBEvent(const DUIEvent& info);
	VOID OnHorizontalSBEvent(const DUIEvent& info);	
	VOID OnMouseWheel(const DUIEvent& info);
	VOID OnContextMenu(const DUIEvent& info);

	VOID ParseScrollBarStyle(const CDUIString& strValue);

	BOOL IsEditAvailable(MSG* pMsg);

protected:
	friend class CTxtWinHost;
	CTxtWinHost* m_pHost;
	CRefPtr<CImageList> m_pImageBK;

	CRefPtr<IDUIScrollBar> m_pHorSB;
	CRefPtr<IDUIScrollBar> m_pVertSB;
    DWORD m_dwEditStyle;
	BOOL m_bEnableContextMenu;
};


DUI_END_NAMESPCE


#endif //__DUI_RICH_EDIT_H__ 




















