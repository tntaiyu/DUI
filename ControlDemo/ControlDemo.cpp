// ControlDemo.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "DUILib.h"
#include "DUIPopupWindow.h"
#include "DUIControls.h"
#include "DUISkinMgr.h"
#include "DUIDibProcess.h"
#include <atlstr.h>
#include <atlutil.h>

#include <sstream>
using namespace std;

DUI_USING_NAMESPACE

class CMainWnd: public CDUIPopupWindow
{
	typedef CDUIPopupWindow theBase;
	BEGIN_MSG_MAP(CMainWnd)
		CHAIN_MSG_MAP(theBase)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

	CDUIPopupWindow m_winTest;

	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
	{
		bHandled = FALSE;
		
		if(m_winTest.IsWindow())
		{
			m_winTest.UnregisterNotifyHandler(this);
			m_winTest.DestroyWindow();
		}

		PostQuitMessage(0);

		return S_OK;
	}
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled)
	{
		bHandled = FALSE;
	
		IDUIControl* pControl = GetControlByName(_T("tile_layouQQ"));
		if(pControl != NULL)
		{
			IDUITileLayout* pTilteLayout = (IDUITileLayout*)pControl->GetInterface(ITileLayout);
			if(pTilteLayout != NULL && pTilteLayout->GetControlCount() > 0)
			{
				const INT nNewAdd = 50;
				for(INT i=0; i<nNewAdd; ++i)
				{
					CRefPtr<IDUIControl> pNewControl = pTilteLayout->GetControlByIndex(0)->Clone();
					//pTilteLayout->AddControl(pNewControl);
				}
			}
		}

		ResetSkin();

		//
// 		TCHAR szBuf[256] = {0};
// 		DWORD dwlen = 256;
// 		CString str = _T("¸ÕÓÃ");
//  		ATL::AtlEscapeUrl(str, szBuf, &dwlen, 256);
// 		CString str2 = _T("%E5%88%9A%E7%94%A8");
// 
// 		char szInBuf[ATL_URL_MAX_URL_LENGTH];
// 		char *pszIn = szInBuf;
// 		AtlUnicodeToUTF8(str2, str2.GetLength(), pszIn, 256);
// 
// 		ATL::AtlUnescapeUrl(str, szBuf,&dwlen, 256);
		return S_OK;
	}

	VOID ResetSkin()
	{
		IDUIApp* pDUIApp = DUIGetAppInstance();
		IDUISkinMgr* pSkinMgr = pDUIApp->GetSkinMgr();
		static BOOL bRet = TRUE;
		CRefPtr<CDUIFrameBK> pFrameBK = NULL;
		if(bRet)
		{
			pFrameBK = pSkinMgr->GetFrameBK(_T("skin2_bk"));
		}
		else
		{
			pFrameBK = pSkinMgr->GetFrameBK(_T("skin1_bk"));
		}
		bRet = !bRet;

		pSkinMgr->SetSkinUIData(pFrameBK);
	}

	VOID OnDUINotify(const DUINotify& info, BOOL& bHandled)
	{
		DebugEvent(info);

		if(m_winTest.IsWindow()
			&& info.hWndManager == m_winTest.m_hWnd)
		{
			if(info.dwMsgType == WM_NOTIFY_BUTTON_CLICKED)
			{
				CDUIString strName = info.pSender->GetName();
				if(strName.compare(_T("change_skin")) == 0)
				{
					ResetSkin();
				}
			}

			return;
		}

		if(info.dwMsgType == WM_NOTIFY_BUTTON_CLICKED)
		{
			CDUIString strName = info.pSender->GetName();
			if(strName.compare(_T("start")) == 0)
			{
				StartProgress();
			}
			else if(strName.compare(_T("add_listitem")) == 0)
			{
				AddListItem();
			}
			else if(strName.compare(_T("file_button")) == 0)
			{
				ShowMenu(info.pSender, _T("menu_file"));
			}
			else if(strName.compare(_T("click_me_button")) == 0)
			{
				ShowMenu(info.pSender, _T("menu_click_me"));
			}
			else if(strName.compare(_T("radio1")) == 0
				|| strName.compare(_T("radio2")) == 0)
			{
				ClickRadioButton(info.pSender);
			}
			else if(strName.compare(_T("test_popup")) == 0)
			{
				ShowTestWindow();
			}
			else if ( strName.compare(_T("add_treeitem")) == 0 )
			{
				AddTreeItem();
			}
		}
		else if(info.dwMsgType == WM_NOTIFY_TRACK_POS_CHANGED)
		{
			SetTrackBarTooltip(info);
		}
	}
// 
// 	virtual VOID OnBeforeFlipDIB(CDibSection* pDIB)
// 	{
// 		if(!IsNcActivate())
// 		{
// 			CGrayScaleImageFilter::Process(pDIB);
// 		}
// 	}
// 
	VOID DebugEvent(const DUINotify& info)
	{
		//too many messages from edit
		if(info.pSender->GetInterface(IRichEdit) != NULL) return;

		wstringstream ws;
		ws << L"MsgType:" << info.dwMsgType;
		ws << L" Control:" << info.pSender;
		ws << L" wParam:" << info.wParam;
		ws << L" lParam:" << info.lParam;
		ws << L" hWnd:" << info.hWndManager;

		CDUIString strInfo = ws.str() + _T("\n");
		OutputEvent(strInfo);
	}

	VOID OutputEvent(const CDUIString& strText)
	{
		IDUIRichEdit* pEventEdit = NULL;
		IDUIControl* pControl = GetControlByName(_T("control_event_edit"));
		if(pControl != NULL) pEventEdit = (IDUIRichEdit*)pControl->GetInterface(IRichEdit);
		if(pEventEdit == NULL) return;		

		pEventEdit->AppendText(strText.c_str());
	}
	
	VOID StartProgress()
	{
		IDUIControl* pControl1 = GetControlByName(_T("progressbar1"));
		IDUIControl* pControl2 = GetControlByName(_T("progressbar2"));
		if(pControl1 == NULL || pControl2 == NULL) return;

		IDUIProgressBar* pProgressbar1 = (IDUIProgressBar*)pControl1->GetInterface(IProgressBar);
		if(pProgressbar1 == NULL) return;
		IDUIProgressBar* pProgressbar2 = (IDUIProgressBar*)pControl2->GetInterface(IProgressBar);
		if(pProgressbar2 == NULL) return;

		INT nMin1, nMax1;
		pProgressbar1->GetRange(nMin1, nMax1);
		pProgressbar1->SetPos(nMin1);
		
		INT nMin2, nMax2;
		pProgressbar2->GetRange(nMin2, nMax2);
		pProgressbar2->SetPos(nMin2);

		while(pProgressbar1->GetPos() < nMax1
			|| pProgressbar2->GetPos() < nMax2)
		{
			pProgressbar1->OffsetPos(50);
			pProgressbar2->OffsetPos(100);
			UpdateWindow();
			
			Sleep(200);
		}
	}

	VOID ClickRadioButton(IDUIControl* pControl)
	{
		IDUIControl* pRadio1 = GetControlByName(_T("radio1"));
		IDUIControl* pRadio2 = GetControlByName(_T("radio2"));
		IDUIControl* pUnCheck = NULL;
		if(pControl == pRadio1)
		{
			pUnCheck = pRadio2;
		}
		else if(pControl == pRadio2)
		{
			pUnCheck = pRadio1;
		}

		//only one radio button can be checked at the same time
		if(pUnCheck != NULL) 
		{
			IDUIButton* pRadioUnCheck = (IDUIButton*)pUnCheck->GetInterface(IButton);
			if(pRadioUnCheck != NULL)
			{
				pRadioUnCheck->SetCheck(FALSE);
			}
		}
	}

	VOID ShowMenu(IDUIControl* pControl, const CDUIString& strMenuName)
	{
		CRefPtr<IDUIMenu> pMemu = CDUIMenu::CreateFromResource(strMenuName);
		if(!pMemu.IsNull())
		{
			RECT rtClient;
			GetClientRect(&rtClient);
			::ClientToScreen(m_hWnd, (LPPOINT)&rtClient);
			::ClientToScreen(m_hWnd, ((LPPOINT)&rtClient + 1));
			RECT rtControl = pControl->GetControlRect();
			INT nX = rtClient.left + rtControl.left;
			INT nY = rtClient.top + rtControl.bottom;
			CRefPtr<IDUIMenuItem> pMenuItem = pMemu->TrackPopupMenu(pControl, nX, nY);
			if(pMenuItem != NULL)
			{
				wstringstream ws;
				ws << L"Clicked menu item:" << pMenuItem.GetPtr() << L"(" << pMenuItem->GetText() << L")\n";
				OutputEvent(ws.str());
			}
		}
	}

	VOID SetTrackBarTooltip(const DUINotify& info)
	{
		IDUITrackBar* pTrackBar = (IDUITrackBar*)info.pSender->GetInterface(ITrackBar);
		if(pTrackBar != NULL)
		{
			IDUIButton* pThumbBtn = pTrackBar->GetThumbButton();
			if(pThumbBtn != NULL)
			{
				INT nPos = pTrackBar->GetPos();
				wstringstream ws;
				ws << L"current position:" << nPos;
				pThumbBtn->SetTooltip(ws.str());
			}
		}
	}

	
	VOID ShowTestWindow()
	{
		if(!m_winTest.IsWindow())
		{
			m_winTest.SetUseSkinBK(TRUE);

			RECT rtParent;
			GetWindowRect(&rtParent);

			RECT rtTest;
			rtTest.left = rtParent.left + 100;
			rtTest.top = rtParent.top + 100;
			rtTest.right = rtTest.left + 400;
			rtTest.bottom = rtTest.top + 400;

			m_winTest.CreateFromResource(_T("popup_win"), NULL, rtTest, NULL
				, WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,/* WS_EX_TOPMOST |*/ WS_EX_TOOLWINDOW);

			m_winTest.RegisterNotifyHandler(this);
		}
		else
		{
			::SetActiveWindow(m_winTest.m_hWnd);
		}
	}

	VOID AddListItem()
	{
		IDUIControl* pControl = GetControlByName(_T("test_list"));
		if(pControl == NULL) return;

		IDUIListBox* pList = (IDUIListBox*)pControl->GetInterface(IListBox);
		if(pList == NULL) return;

		INT nCount = pList->GetControlCount();
		IDUIControl* pItemCopy = NULL;
		INT nCopyIndex(0);
		if(nCount > 0)
		{
			for(INT i=0; i<nCount; ++i)
			{
				IDUIListItem* pItem = (IDUIListItem*)(pList->GetControlByIndex(i)->GetInterface(IListItem));
				if(pItem != NULL
					&& !pItem->GetGroup()
					&& pItem->IsVisible()
					&& !pItem->GetExpand())
				{
					pItemCopy = pItem;
					nCopyIndex = i;
					break;
				}
			}
		}

		if(pItemCopy != NULL)
		{
			LockLayout(TRUE);
			const INT nCopyCount = 1000;
			CDUIString strText = pItemCopy->GetText();
			
			wstringstream ws;
			for(INT i=0; i<nCopyCount; ++i)
			{
				CRefPtr<IDUIControl> pControl = pItemCopy->Clone();
				pList->AddControlAt(nCopyIndex, pControl);

				ws.str(L"");
				ws << strText << i;
				pControl->SetText(ws.str());
			}
			LockLayout(FALSE);

			LayoutRootControl();
			Invalidate();
		}
	}

	void	AddTreeItem()
	{
		IDUIControl* pControl = GetControlByName(_T("test_treeview"));
		if(pControl == NULL) return;
		
		IDUITreeView* pTreeView = (IDUITreeView*)pControl->GetInterface(ITreeViewName);
		if(pTreeView == NULL) return;

		if ( pTreeView->GetControlCount() >= 10 )
			return ;

		if ( pTreeView->GetControlCount() > 0 )
		{
			IDUITreeViewItem* pItem = (IDUITreeViewItem*)(pTreeView->GetControlByIndex(0)->GetInterface(ITreeViewItemName));
			if ( pItem )
			{
#if 1
				//HDUITREEITEM	hRoot =  (HDUITREEITEM)(pItem->Clone().GetPtr());
				CRefPtr<IDUIControl> hRoot1 = pItem->Clone();
				HDUITREEITEM	hRoot =  (HDUITREEITEM)hRoot1.GetPtr();
				if ( hRoot )
				{
					pTreeView->AddRoot(hRoot);
				}

#else
				CRefPtr<IDUIControl> hRoot = pItem->Clone();
				if ( !hRoot.IsNull() )
				{
					//pTreeView->AddControl(hRoot);
					pTreeView->AddRoot((HDUITREEITEM)hRoot.GetPtr());
				}
#endif
			}
		}

		if ( pTreeView->GetControlCount() == 10 )
		{
			IDUITreeViewItem* pItem = (IDUITreeViewItem*)(pTreeView->GetControlByIndex(0)->GetInterface(ITreeViewItemName));
			if ( pItem == NULL )
				return;

			HDUITREEITEM hTopItem = pTreeView->GetRoot();
			if ( hTopItem )
			{
				for ( int i=0;i<10;i++)
				{
					//CRefPtr<IDUIControl>	pControl1 = pItem->Clone();
					//HDUITREEITEM	hRoot1 =  (HDUITREEITEM)pControl1.GetPtr();
					HDUITREEITEM	hRoot1 =  new CDUITreeNode();

 					if ( hRoot1 )
 					{
						hRoot1->ApplyControlStyle(_T("def_button"));
						wstringstream ws;
						ws<<i<<_T("-----2 Child");
						
						hRoot1->SetText(ws.str());

						HDUITREEITEM hRoot11 = pTreeView->AddChildItem(hTopItem,hRoot1);
						if ( i == 0 )
						{
							ws<<_T("-Parent");
							hRoot1->SetText(ws.str());

							for ( int j=0;j<100;j++)
							{
								HDUITREEITEM	hRoot111 = new CDUITreeNode();
								hRoot111->ApplyControlStyle(_T("def_button"));
								
								wstringstream ws1;
								ws1<<j<<_T("-----3 Child");
								if ( j == 0 )
									ws1<<_T("-Parent");

								hRoot111->SetText(ws1.str());
								pTreeView->AddChildItem(hRoot11,hRoot111);
							}
						}
					}
				}
			}
		}
	}
};

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	IDUIApp* pDUIApp = DUIGetAppInstance();
	if(pDUIApp == NULL) return -1;

	CResLoader loader;
	CDUIString strXMLEntry;
	loader.LoadXMLFromFile(_T(".\\res\\xml\\dui_control_demo.xml"), strXMLEntry);
	//pDUIApp->Init(strXMLEntry);

	BOOL bRet = pDUIApp->Init(strXMLEntry);
	DUI_ASSERT(bRet);
	if(!bRet) return -1;

	IDUISkinMgr* pSkinMgr = pDUIApp->GetSkinMgr();
	IDUIControlFactory* pControlFactory = pDUIApp->GetControlFactory();

	CDUIString strTitle;
	pSkinMgr->GetStringText(_T("ids_title"), strTitle);
 	CMainWnd main;
	RECT rtMain = {100, 100, 700, 700};
  	main.CreateFromResource(_T("main_wnd"), NULL, rtMain, strTitle.c_str()
  			,WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	
	pDUIApp->GetMessageLoop()->Run();

	pDUIApp->Term();

	return 0;
}



