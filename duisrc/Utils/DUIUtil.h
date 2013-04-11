#ifndef __DUI_UTIL_H__
#define __DUI_UTIL_H__

#include "DUIDef.h"
#include <vector>

DUI_BGN_NAMESPCE

class CEncode
{
public:
	CEncode();
	~CEncode();
	
	LPWSTR Utf8ToUnicode(char* pUtf8, INT nLen = -1);
	
protected:
	VOID ClearData();
	VOID AddPointer(VOID* p);
	
protected:
	std::vector<VOID*> m_arData;
};



DUI_END_NAMESPCE


#endif //__DUI_UTIL_H__