#ifndef __DUI_LIB_DEF_H__
#define __DUI_LIB_DEF_H__

#include "DUIDef.h"
#include "DUIRef.h"

DUI_BGN_NAMESPCE

class IDUIControl;

#define  WM_NOTIFY_CONTROL_ATTACHED			1
#define  WM_NOTIFY_CONTROL_DETACHED			2

#define  WM_NOTIFY_LINK_CLICKED				9	
#define  WM_NOTIFY_BUTTON_CLICKED			10

#define  WM_NOTIFY_PROGRESS_POS_CHANGED		11
#define  WM_NOTIFY_TRACK_POS_CHANGED		12

#define WM_NOTIFY_EDIT_TX_NOTIFY			13

#define WM_NOTIFY_LISTBOX_NOTIFY			20
		#define LBN_WPARAM_CLCIK		1
		#define LBN_WPARAM_DBCLCIK		2
		#define LBN_WPARAM_SELCHANGE	3
		#define LBN_WPARAM_HOVERCHANGE	4

#define WM_NOTIFY_COMBOBOX_NOTIFY		21
		#define CBN_WPARAM_DROPDOWN		1	//before show list
		#define CBN_WPARAM_CLOSEUP		2   //before close list
		#define CBN_WPARAM_SELCHANGE	3

#define WM_NOTIFY_MENU_NOTIFY		22
		
		#define MENU_WPARAM_SELCHANGE	3


typedef struct tagDUINotify 
{
	DWORD			dwMsgType;
	IDUIControl*	pSender;
	WPARAM			wParam;
	LPARAM			lParam;
	HWND			hWndManager;
} DUINotify;

//control event
enum DUI_EVENT_TYPE
{
	DUI_EVENT_SETCURSOR = 1,
	DUI_EVENT_TIMER,
	DUI_EVENT_SETFOCUS,
	DUI_EVENT_KILLFOCUS,

	DUI_EVENT_WINSIZE,
	DUI_EVENT_WINMOVE,
	
	__DUI_EVENT_MOUSE_FIRST,
	DUI_EVENT_LBUTTONDOWN,
	DUI_EVENT_LBUTTONUP,
	DUI_EVENT_LBUTTONDBCLK,
	DUI_EVENT_MOUSEENTER,
	DUI_EVENT_MOUSELEAVE,
	DUI_EVENT_MOUSEMOVE,
	DUI_EVENT_MOUSEWHEEL,
	DUI_EVENT_MOUSECLICK, //this message only send to to the focused control when click outside of it
	__DUI_EVENT_MOUSE_LAST,
	DUI_EVENT_CONTEXTMENU,
	
	__DUI_EVENT_KEY_FIRST,
	DUI_EVENT_KEYDOWN,
	DUI_EVENT_KEYUP,
	DUI_EVENT_CHAR,
	__DUI_EVENT_KEY_LAST,
	
	DUI_EVENT_VSCROLL,
	DUI_EVENT_HSCROLL,

	DUI_EVENT_ELE_ITEM_CLICK,
	DUI_EVENT_ELE_ITEM_DBCLICK,
	DUI_EVENT_ELE_ITEM_MOUSEENTER,
	DUI_EVENT_ELE_ITEM_MOUSELEAVE,


	DUI_EVENT_MENU_CANCEL,
	DUI_EVENT_MENU_CHAR
};



typedef struct tagDUIEvent
{
	DWORD dwEventType;			//DUI_CONTROL_EVENT_TYPE
	IDUIControl* pSender;
	POINT ptMouse;
	TCHAR chKey;
	WORD wKeyState;
	WPARAM wParam;
	LPARAM lParam;
} DUIEvent;

////////////// control request message to manager  ////////////////
#define WM_CONTROL_REQUEST_REDRAW		1
#define WM_CONTROL_REQUEST_FOCUS		2
		#define REQUEST_WPARAM_SET_FOCUS			1
		#define REQUEST_WPARAM_KILL_FOCUS			2
		#define REQUEST_WPARAM_GET_FOCUS_CONTROL	3

#define	WM_CONTROL_REQUEST_GET_HWND		3
		#define REQUEST_WPARAM_HWND_MANAGER		1
		#define REQUEST_WPARAM_HWND_TOP_PARENT		2

#define	WM_CONTROL_REQUEST_GET_CONTROL	4
		#define REQUEST_WPARAM_CONTROL_BY_POINT		1
		#define REQUEST_WPARAM_CONTROL_BY_NAME		2

#define	WM_CONTROL_REQUEST_LOCK_LAYOUT	5	

	
#define WM_CONTROL_REQUEST_TIMER		10
#define WPARAM_SET_TIMER		1
#define WPARAM_KILL_TIMER		2
typedef struct tagDUITimerInfo
{
	IDUIControl* pControl;
	HWND hWnd;
	UINT nTimerID;	//timer ID for control
	UINT nElapse;
	UINT nTimerIndex; //window unique timer ID
} DUITimerInfo, *LPDUITimerInfo;


#define	WM_CONTROL_REQUEST_WIN_STATUS	11
		#define DUI_WIN_STATUS_MIN			0x00000001
		#define DUI_WIN_STATUS_MAX			0x00000002
		#define DUI_WIN_STATUS_NORMAL		0x00000004
		#define DUI_WIN_STATUS_INVISIBLE	0x00000008
		#define DUI_WIN_STATUS_DISABLE		0x00000010
		#define DUI_WIN_STATUS_ACTIVATE		0x00000020

////////////// control request message to manager  ////////////////



#define WM_DUI_FLIP		WM_USER + 1000


////////////////  DUI window style  BEGIN /////////////////
#define DUI_WS_CHILD			(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPCHILDREN)
#define DUI_WS_CUSTOM_POPUP	    (WS_POPUP | WS_VISIBLE)	 

//attribute for "nc_style"
#define DUI_WS_LAYERED			0x00000001	//layered
#define DUI_WS_CUSTOM_NC		0x00000002  //custom_nc
#define DUI_WS_NC_RGN			0x00000004	//region_nc
#define DUI_WS_NC_SIZABLE		0x00000008	//size_nc
////////////////  DUI window style  END /////////////////


/////////////  control interface name  BEGIN /////////////////
#define IControl		 _T("control")
#define IControlBase	 _T("control_base")
#define ICanvas			 _T("canvas")
#define ITileLayout		 _T("tile_layout")
#define IButton			 _T("button")
#define ILabel			 _T("label")
#define IHyperLink		 _T("hyper_link")
#define IDockLayout		 _T("dock_layout")
#define IScrollBar		 _T("scrollbar")
#define IPictureBox		 _T("picturebox")
#define ISplit			 _T("split")
#define IProgressBar	 _T("progress_bar")
#define ITrackBar	     _T("track_bar")
#define IFramePanel	     _T("frame_panel")
#define IListItem	     _T("listItem")
#define IListIconTextItem _T("list_icontext_item")
#define IListBox	     _T("listbox")
#define IRichEdit	     _T("richedit")
#define IComboBox        _T("combobox")
#define ITabLayout       _T("tab_layout")
#define ITabBar			 _T("tab_bar")
#define ITabItem		 _T("tab_item")
#define IWinObject		 _T("win_object")
#define IMenuItem		 _T("menuItem")
#define IMenu			 _T("menu")
#define	ITreeViewName	 _T("treeview")	
#define ITreeViewItemName	_T("treeview_item") 
/////////////  control interface name  END /////////////////


/////////////////  control style  BEGIN ///////////////////
#define CONTROL_STYLE_VISIBLE			0x00000001	//visible="true"
#define CONTROL_STYLE_SETCURSOR			0x00000002  //cursor
#define CONTROL_STYLE_TABSTOP			0x00000004  
#define CONTROL_STYLE_WANTRETURN		0x00000008
#define CONTROL_STYLE_TOOLTIP			0x00000010  //tooltip
#define CONTROL_STYLE_TRANSPARENT		0x00000020	//did not receive mouse message
/////////////////  control style  END ///////////////////


////////////////  control status BEGIN /////////////////
#define CONTROL_STATUS_DISABLE			0x00000001
#define CONTROL_STATUS_FOCUS			0x00000002
#define CONTROL_STATUS_CHECKED  		0x00000004
#define CONTROL_STATUS_HOVER			0x00000008
#define CONTROL_STATUS_PRESSED  		0x00000010
////////////////  control status END ///////////////////


//////////////  picture box draw mode  BEGIN /////////////////////////////////
// attribute for "draw_mode"
#define PICTURE_BOX_DRAW_MODE_MASK				0x000000FF

#define PICTURE_BOX_STRETCH_IMAGE				0x00000001// stretch_image
#define PICTURE_BOX_MIDDLE_STRETCH_IMAGE		0x00000002 // middle_stretch_image
#define PICTURE_BOX_HORIZONTAL_STRETCH_IMAGE	0x00000004 // horizontal_stretch_image	
#define PICTURE_BOX_VERTICAL_STRETCH_IMAGE		0x00000008 // vertical_stretch_image
#define PICTURE_BOX_CENTER_IMAGE				0x00000010 // center_image
//////////////  picture box draw mode END  //////////////////////////////////




//////////////  frame panel draw mode  BEGIN /////////////////////////////////
// attribute for "draw_mode"
#define FRAME_PANEL_DRAW_MODE_MASK				0x0000000F

#define FRAME_PANEL_SINGLE_COLOR				0x00000001// single_color
#define FRAME_PANEL_USE_IMAGE					0x00000002 // use_image
//////////////  frame panel draw mode  END /////////////////////////////////




//////////////  BUTTON STYLE  BEGIN /////////////////////////////////
//attribute for "button_style"
#define BTN_STYLE_MASK			0x000000FF
#define BTN_FORE_MASK			0x000000FF

#define BTN_STYLE_CENTER_ICON	 0x00000001	//center_icon
#define BTN_STYLE_LEFT_ICON		 0x00000002 //left_icon
#define BTN_STYLE_RIGHT_ICON	 0x00000004 //right_icon
#define BTN_STYLE_TOP_ICON		 0x00000008 //top_icon
#define BTN_STYLE_BOTTOM_ICON	 0x00000010 //bottom_icon

#define BTN_STYLE_NOTEXT		 0x00000100 //no_text
#define BTN_STYLE_AUTO_FIT_BK	 0x00000200	//auto_fix
//////////////  BUTTON STYLE  END /////////////////////////////////




//////////////  COMBOBOX STYLE  BEGIN /////////////////////////////////
#define COMBOBOX_STYLE_DROPDOWN		0x00000001 //can edit
#define COMBOBOX_STYLE_DROPLIST		0x00000002 //can only select from list
//////////////  COMBOBOX STYLE  END /////////////////////////////////




//////////////  SPLIT STYLE  BEGIN /////////////////////////////////
//attribute for "split_style"
#define SPLIT_STYLE_SIZE_LEFT		0x00000001				//size_left
#define SPLIT_STYLE_SIZE_RIGHT		0x00000002				//size_right
#define SPLIT_STYLE_SIZE_TOP		SPLIT_STYLE_SIZE_LEFT   //size_top
#define SPLIT_STYLE_SIZE_BOTTOM		SPLIT_STYLE_SIZE_RIGHT	//size_bottom
#define SPLIT_STYLE_SIZE_ALL		0x00000003				//size_all
//////////////  SPLIT STYLE  END /////////////////////////////////


//////////////  CONTROL extend STYLE  BEGIN //////////////////////////////
#define CONTROL_EX_STYLE_STRETCH_MASK	0x000000FF
#define CONTROL_EX_STYLE_DOCK_MASK		0x00000F00

//attribute for "stretch"
#define CONTROL_EX_STYLE_MOVE_X			0x00000001		//move_x
#define CONTROL_EX_STYLE_MOVE_Y			0x00000002      //move_y
#define CONTROL_EX_STYLE_SIZE_X			0x00000004      //size_x
#define CONTROL_EX_STYLE_SIZE_Y			0x00000008      //size_y
#define CONTROL_EX_STYLE_GROUP			0x00000010      //group

//attribute for "dock"
#define CONTROL_EX_STYLE_DOCK_LEFT		0x00000100		//dock_left
#define CONTROL_EX_STYLE_DOCK_RIGHT		0x00000200		//dock_right
#define CONTROL_EX_STYLE_DOCK_TOP		0x00000400		//dock_top
#define CONTROL_EX_STYLE_DOCK_BOTTOM	0x00000800		//dock_bottom

//////////////  CONTROL extend STYLE  END /////////////////////////////////



//find control callback
typedef IDUIControl* (CALLBACK* FINDCONTROLPROC)(IDUIControl*, LPVOID);
typedef struct tagFINDTABINFO
{
	IDUIControl* pFocus;
	IDUIControl* pLast;
	BOOL bForward;
	BOOL bNextIsIt;
} FINDTABINFO;


//find control flag
#define FIND_FLAG_ALL			0x00000000
#define FIND_FLAG_VISIBLE   	0x00000001
#define FIND_FLAG_ENABLED   	0x00000002
#define FIND_FLAG_HITTEST   	0x00000004
#define FIND_FLAG_MEFIRST   	0x00000008

//button(min, max, close) name on title bar
#define DUI_SYS_MAX_BTN			_T("dui_sys_max_btn")
#define DUI_SYS_MIN_BTN			_T("dui_sys_min_btn")
#define DUI_SYS_RESTORE_BTN		_T("dui_sys_restore_btn")
#define DUI_SYS_CLOSE_BTN		_T("dui_sys_close_btn")

//=================================
#define DUI_STR_TRUE		_T("true")
#define DUI_STR_FALSE		_T("false")

#define DUI_STR_CANCEL		_T("cancel")
#define DUI_STR_OK			_T("ok")

#define DUI_VERT_SCROLLBAR	_T("vert_scrollbar")
#define DUI_HORZ_SCROLLBAR  _T("horz_scrollbar")


DUI_END_NAMESPCE

#endif