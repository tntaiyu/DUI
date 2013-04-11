#ifndef __DUI_TOOLTIP_H__
#define __DUI_TOOLTIP_H__

#include "DUIRef.h"
#include "DUIBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API CDUITooltip: public CSingletonT<CDUITooltip>
{
public:
	CDUITooltip();
	~CDUITooltip();

	BOOL Setup();
	VOID Uninstall();

	BOOL AddTool(HWND hWndTool);
	BOOL RemoveTool(HWND hWndTool);
	
	BOOL Activate(BOOL bActivate);

	HWND GetTooltipWnd();

protected:
	HWND m_hWndTooltip;
};

DUI_END_NAMESPCE


#endif// __DUI_TOOLTIP_H__