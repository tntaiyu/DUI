#include "DUIPictureBox.h"
#include "DUIRenderEngine.h"

DUI_BGN_NAMESPCE

CDUIPictureBox::CDUIPictureBox()
{
	m_dwDrawMode = 0;
	m_nCurrentIndex = 0;
}

CDUIPictureBox::~CDUIPictureBox()
{

}

LPVOID CDUIPictureBox::GetInterface(const CDUIString& strName)
{
	if(strName.compare(IPictureBox) == 0)
	{
		return (IDUIPictureBox*)this;
	}

	return theBase::GetInterface(strName);
}

CRefPtr<IDUIControl> CDUIPictureBox::Clone()
{
	return new CDUIPictureBox(*this);
}

VOID CDUIPictureBox::SetAttribute(const CDUIString& strName, const CDUIString& strValue)
{
	BOOL bHandled(FALSE);

	if(strName.compare(_T("icon")) == 0)
	{
		bHandled = TRUE;

		m_pImage = GetSkinMgr()->GetImage(strValue);
		UpdateLayout(FALSE);
	}
	else if(strName.compare(_T("draw_mode")) == 0)
	{
		bHandled = TRUE;

		DWORD dwDrawMode(0);
		if(strValue.find(_T("middle_stretch_image")) != -1)
		{
			dwDrawMode &= ~PICTURE_BOX_DRAW_MODE_MASK;
			dwDrawMode |=  PICTURE_BOX_STRETCH_IMAGE;
		}
		else if(strValue.find(_T("horizontal_stretch_image")) != -1)
		{
			dwDrawMode &= ~PICTURE_BOX_DRAW_MODE_MASK;
			dwDrawMode |=  PICTURE_BOX_HORIZONTAL_STRETCH_IMAGE;
		}
		else if(strValue.find(_T("vertical_stretch_image")) != -1)
		{
			dwDrawMode &= ~PICTURE_BOX_DRAW_MODE_MASK;
			dwDrawMode |=  PICTURE_BOX_VERTICAL_STRETCH_IMAGE;
		}
		else if(strValue.find(_T("stretch_image")) != -1)//note: must be at last, contained  in string "vertical_stretch_image"
		{
			dwDrawMode &= ~PICTURE_BOX_DRAW_MODE_MASK;
			dwDrawMode |=  PICTURE_BOX_STRETCH_IMAGE;
		}
		else if(strValue.find(_T("center_image")) != -1) 
		{
			dwDrawMode &= ~PICTURE_BOX_DRAW_MODE_MASK;
			dwDrawMode |=  PICTURE_BOX_CENTER_IMAGE;
		}

		DUI_ASSERT(dwDrawMode != 0);
		SetDrawMode(dwDrawMode);
	}

	if(!bHandled)
	{
		theBase::SetAttribute(strName, strValue);
	}
}

BOOL CDUIPictureBox::EstimateSize(SIZE sizeAvaiable, SIZE& sizeRet)
{
	SIZE sizIcon = {0};
	if(!m_pImage.IsNull() && !m_pImage->IsNull())
	{
		sizIcon.cx = m_pImage->GetIconWidth();
		sizIcon.cy = m_pImage->GetIconHeight();
	}
	
	sizeRet = sizIcon;
	sizeRet.cx += (m_rtMargin.left + m_rtMargin.right);
	sizeRet.cy += (m_rtMargin.top + m_rtMargin.bottom);	

	//use fixed width and fixed height
	if(m_sizeFixed.cx != 0)
	{
		sizeRet.cx = m_sizeFixed.cx;
	}
	if(m_sizeFixed.cy != 0)
	{
		sizeRet.cy = m_sizeFixed.cy;
	}

	return TRUE;
}

VOID CDUIPictureBox::PaintBkgnd(HDC dc)
{
	if(!(PICTURE_BOX_DRAW_MODE_MASK & m_dwDrawMode)) return;
	if(m_pImage.IsNull() || m_pImage->IsNull()) return;
	INT nIndex = m_nCurrentIndex;
	if(nIndex >= m_pImage->GetItemCount())
	{
		DUI_ASSERT(FALSE);
		nIndex = 0;
	}

	RECT rtImage = GetContentRect();

	if(m_dwDrawMode & PICTURE_BOX_STRETCH_IMAGE)
	{
		m_pImage->Stretch(dc, rtImage, nIndex);
	}
	else if(m_dwDrawMode & PICTURE_BOX_MIDDLE_STRETCH_IMAGE)
	{
		CDUIRenderClip clip(dc, rtImage);
		m_pImage->MiddleStretch(dc, rtImage, nIndex);
	}
	else if(m_dwDrawMode & PICTURE_BOX_HORIZONTAL_STRETCH_IMAGE)
	{
		CDUIRenderClip clip(dc, rtImage);
		m_pImage->HorizontalStretch(dc, rtImage, nIndex);
	}
	else if(m_dwDrawMode & PICTURE_BOX_VERTICAL_STRETCH_IMAGE)
	{
		CDUIRenderClip clip(dc, rtImage);
		m_pImage->VerticalStretch(dc, rtImage, nIndex);
	}
	else if(m_dwDrawMode & PICTURE_BOX_CENTER_IMAGE)
	{
		CDUIRenderClip clip(dc, rtImage);
		m_pImage->DrawInCenter(dc, rtImage, nIndex);
	}
}

//IDUIPictureBox
VOID CDUIPictureBox::SetImage(CRefPtr<CImageList> pImage)
{
	m_pImage = pImage;
}

const CRefPtr<CImageList>& CDUIPictureBox::GetImage() const
{
	return m_pImage;
}

VOID CDUIPictureBox::SetDrawMode(DWORD dwMode)
{
	m_dwDrawMode = dwMode;
}

DWORD CDUIPictureBox::GetDrawMode() const
{
	return m_dwDrawMode;
}

INT CDUIPictureBox::GetIconCount()
{
	INT nRet(0);
	if(!m_pImage.IsNull() && !m_pImage->IsNull())
	{
		return m_pImage->GetItemCount();
	}

	return nRet;
}

VOID CDUIPictureBox::SetCurrentIconIndex(INT nIndex)
{
	INT nCount = GetIconCount();
	if(nIndex >=0 && nIndex < nCount)
	{
		m_nCurrentIndex = nIndex;
	}
	else
	{
		DUI_ASSERT(FALSE);
	}
}

INT CDUIPictureBox::GetCurrentIconIndex() const
{
	return m_nCurrentIndex;
}

DUI_END_NAMESPCE