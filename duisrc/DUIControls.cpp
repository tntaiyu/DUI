#include "DUIControls.h"

DUI_BGN_NAMESPCE

BOOL RegisterDUIDefaultControls()
{
	IDUIControlFactory* pControlFactory = GetControlFactory();
	if(pControlFactory == NULL) return FALSE;

	pControlFactory->RegisterControl(_T("horz_layout"), new CDUIHorizontalLayout);
	pControlFactory->RegisterControl(_T("vert_layout"), new CDUIVerticalLayout);
	pControlFactory->RegisterControl(_T("tile_layout"), new CDUITileLayout);
	pControlFactory->RegisterControl(_T("dialog_layout"), new CDUIDialogLayout);
	pControlFactory->RegisterControl(_T("dock_layout"), new CDUIDockLayout);
	
	pControlFactory->RegisterControl(_T("vert_split"), new CDUIVerticalSplit);
	pControlFactory->RegisterControl(_T("horz_split"), new CDUIHorizontalSplit);

	pControlFactory->RegisterControl(_T("canvas"), new CDUICanvas);
	pControlFactory->RegisterControl(_T("padding_panel"), new CDUIPaddingPanel);
	pControlFactory->RegisterControl(_T("frame_panel"), new CDUIFramePanel);
	pControlFactory->RegisterControl(_T("picture_box"), new CDUIPictureBox);
	
	pControlFactory->RegisterControl(_T("button"), new CDUIButton);
	pControlFactory->RegisterControl(_T("radio_button"), new CDUIRadioButton);
	pControlFactory->RegisterControl(_T("check_box"), new CDUICheckBox);

	pControlFactory->RegisterControl(_T("label"), new CDUILabel);
	pControlFactory->RegisterControl(_T("hyperlink"), new CDUIHyperLink);
	
	pControlFactory->RegisterControl(DUI_VERT_SCROLLBAR, new CDUIVerticalSB);
	pControlFactory->RegisterControl(DUI_HORZ_SCROLLBAR, new CDUIHorizontalSB);
	
	pControlFactory->RegisterControl(_T("vert_progressbar"), new CDUIVerticalProgressBar);
	pControlFactory->RegisterControl(_T("horz_progressbar"), new CDUIHorizontalProgressBar);

	pControlFactory->RegisterControl(_T("vert_trackbar"), new CDUIVerticalTrackBar);
	pControlFactory->RegisterControl(_T("horz_trackbar"), new CDUIHorizontalTrackBar);

	pControlFactory->RegisterControl(_T("listbox"), new CDUIListBox);
	pControlFactory->RegisterControl(_T("list_icontext_item"), new CDUIIconTextItem);

	pControlFactory->RegisterControl(_T("richedit"), new CDUIRichEdit);
	pControlFactory->RegisterControl(_T("combobox"), new CDUIComboBox);

	pControlFactory->RegisterControl(_T("tab_item"), new CDUITabItem);
	pControlFactory->RegisterControl(_T("horz_tabbar"), new CDUIHorizontalTabBar);
	pControlFactory->RegisterControl(_T("vert_tabbar"), new CDUIVerticalTabBar);
	pControlFactory->RegisterControl(_T("tab_layout"), new CDUITabLayout);

	pControlFactory->RegisterControl(_T("horz_toolbar"), new CDUIHorizontalToolBar);
	pControlFactory->RegisterControl(_T("vert_toolbar"), new CDUIVerticalToolBar);

	pControlFactory->RegisterControl(_T("win_object"), new CDUIWinObject);

	pControlFactory->RegisterControl(_T("menu_item"), new CDUIMenuItem);
	pControlFactory->RegisterControl(_T("menu"), new CDUIMenu);

	pControlFactory->RegisterControl(ITreeViewName,new CDUITreeViewImpl);
	pControlFactory->RegisterControl(ITreeViewItemName, new CDUITreeNode);

	return TRUE;
}

DUI_END_NAMESPCE