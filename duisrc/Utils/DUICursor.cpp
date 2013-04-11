#include "DUICursor.h"

DUI_BGN_NAMESPCE

CCursorObject::CCursorObject()
{
	CTrace::TraceInfo(_T("CCursorObject::CCursorObject()"));
	m_hCursor = NULL;
}

CCursorObject::~CCursorObject()
{
	CTrace::TraceInfo(_T("CCursorObject::~CCursorObject(): m_hCursor(%u)"), m_hCursor);
	DeleteObject();
}

BOOL CCursorObject::CreateFromFile(LPCTSTR lpszFileName)
{
	DeleteObject();
	m_hCursor = (HCURSOR)::LoadImage(NULL, lpszFileName, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
	DUI_ASSERT(m_hCursor != NULL);
	
	return (m_hCursor != NULL);
}

BOOL CCursorObject::CreateFromResource(HINSTANCE hRes, LPCTSTR lpszResID)
{
	DeleteObject();
	
	INT nResID = _ttoi(lpszResID);
	m_hCursor = (HCURSOR)::LoadImage(hRes, MAKEINTRESOURCE(nResID), IMAGE_CURSOR, 0, 0, 0);
	DUI_ASSERT(m_hCursor != NULL);
	
	return (m_hCursor != NULL);
}

VOID CCursorObject::DeleteObject()
{
	if(m_hCursor != NULL)
	{
		DestroyCursor(m_hCursor);
		m_hCursor = NULL;
	}
}

HCURSOR CCursorObject::GetCursor() const
{
	return m_hCursor;
}

BOOL CCursorObject::IsNull() const
{
	return m_hCursor == NULL;
}

DUI_END_NAMESPCE