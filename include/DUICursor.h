#ifndef __DUI_CURSOR_H__
#define __DUI_CURSOR_H__

#include "DUIBase.h"

DUI_BGN_NAMESPCE

class DUILIB_API CCursorObject: public CDUIObject
{
public:
	CCursorObject();
	~CCursorObject();

	BOOL CreateFromFile(LPCTSTR lpszFileName);
	BOOL CreateFromResource(HINSTANCE hRes, LPCTSTR lpszResID);
	
	HCURSOR GetCursor() const;
	BOOL IsNull() const;

protected:
	VOID DeleteObject();

protected:
	HCURSOR m_hCursor;
};

DUI_END_NAMESPCE

#endif //__DUI_CURSOR_H__