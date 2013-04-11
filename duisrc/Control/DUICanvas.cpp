#include "DUICanvas.h"
#include "DUIRenderEngine.h"
#include "DUIDdb.h"

DUI_BGN_NAMESPCE


///////////////   CDUICanvas  ///////////////////////

LPVOID CDUICanvas::GetInterface(const CDUIString& strName)
{
	if(strName.compare(ICanvas) == 0)
	{
		return (IDUICanvas*)this;
	}
	
	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUICanvas::Clone()
{
	return new CDUICanvas(*this);
}

VOID CDUICanvas::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);
	if(strName.compare(_T("frame_style")) == 0)
	{
		m_pFrameBK = GetSkinMgr()->GetFrameBK(strValue);
		bHandled = TRUE;
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}


VOID CDUICanvas::PaintBkgnd(HDC dc)
{
	if(!m_pFrameBK.IsNull() && !m_pFrameBK->IsEmpty())
	{
		RECT rtDraw = GetContentRect();
		m_pFrameBK->DrawBK(dc, rtDraw);
	}
}

VOID CDUICanvas::SetFrameBK(CRefPtr<CDUIFrameBK> pFrameBK)
{
	m_pFrameBK = pFrameBK;
}

CRefPtr<CDUIFrameBK> CDUICanvas::GetCanvasUIData() const
{
	return m_pFrameBK;
}

///////////////   CDUICanvas  ///////////////////////






DUI_END_NAMESPCE