#include "DUIUtil.h"
#include "DUIDebug.h"

DUI_BGN_NAMESPCE

CEncode::CEncode()
{

}

CEncode::~CEncode()
{
	ClearData();
}

VOID CEncode::AddPointer(VOID* p)
{
	DUI_ASSERT(p != NULL);
	m_arData.push_back(p);
}

VOID CEncode::ClearData()
{
	std::vector<VOID*>::iterator itr;
	for(itr = m_arData.begin(); itr != m_arData.end(); ++itr)
	{
		delete [](*itr);
	}

	m_arData.clear();
}

LPWSTR CEncode::Utf8ToUnicode(char* pUtf8, INT nSrcLen/* = -1*/)
{
	DUI_ASSERT(pUtf8 != NULL);
	if(nSrcLen == -1)
	{
		nSrcLen = strlen(pUtf8);
	}
	INT nLen = MultiByteToWideChar(CP_UTF8, 0, pUtf8, nSrcLen, NULL, 0);
	wchar_t* pNewData = new wchar_t[nLen + 1];
	if(pNewData == NULL) return NULL;

	AddPointer(pNewData);
	ZeroMemory(pNewData, nLen + 1);
	MultiByteToWideChar(CP_UTF8, 0, pUtf8, nSrcLen, pNewData, nLen + 1);

	return pNewData;
}

DUI_END_NAMESPCE