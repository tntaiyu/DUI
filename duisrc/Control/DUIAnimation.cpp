#include "DUIAnimation.h"
#include <algorithm>

DUI_BGN_NAMESPCE

CDUIAnimation::CDUIAnimation(IDUIControl* pOwner)
{
	DUI_ASSERT(pOwner != NULL);
	m_pControl = pOwner;
}

 BOOL CDUIAnimation::StartAnimation(INT nElapse, INT nTotalFrame, INT nAnimationID /*= 0*/, BOOL bLoop/* = FALSE*/)
 {
	CRefPtr<CAnimationData> pData = GetAnimationByID(nAnimationID);
	if(!pData.IsNull() 
		|| nElapse <= 0
		|| nTotalFrame <= 0
		|| m_pControl == NULL)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	CRefPtr<CAnimationData> pAnimation = new CAnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
	if(pAnimation.IsNull()) return FALSE;

	if(m_pControl->SetTimer(nAnimationID, nElapse))
	{
		m_arAnimations.push_back(pAnimation);
		return TRUE;
	}

	return FALSE;
 }

 VOID CDUIAnimation::StopAnimation(INT nAnimationID /*= 0*/)
 {
	 if(m_pControl == NULL) return;

	 if(nAnimationID  != 0)
	 {
		CRefPtr<CAnimationData> pData = GetAnimationByID(nAnimationID);
		if(!pData.IsNull())
		{
			m_pControl->KillTimer(nAnimationID);
			m_arAnimations.erase(std::remove(m_arAnimations.begin(), m_arAnimations.end(), pData)
				, m_arAnimations.end());
			return;
		}
	 }
	else
	{
		INT nCount = m_arAnimations.size();
		for(INT i=0; i<nCount; ++i)
		{
			m_pControl->KillTimer(m_arAnimations[i]->m_nAnimationID);
		}
		m_arAnimations.clear();
	}
 }

 BOOL CDUIAnimation::IsAnimationRunning(INT nAnimationID)
 {
	CRefPtr<CAnimationData> pData = GetAnimationByID(nAnimationID);
	return !pData.IsNull();
 }

 INT CDUIAnimation::GetCurrentFrame(INT nAnimationID/* = 0*/)
 {
	 CRefPtr<CAnimationData> pData = GetAnimationByID(nAnimationID);
	 if(pData.IsNull())
	 {
		 DUI_ASSERT(FALSE);
		 return -1;
	 }
	
	 return pData->m_nCurFrame;
 }

 BOOL CDUIAnimation::SetCurrentFrame(INT nFrame, INT nAnimationID/* = 0*/)
 {
	 CRefPtr<CAnimationData> pData = GetAnimationByID(nAnimationID);
	 if(pData.IsNull())
	 {
		 DUI_ASSERT(FALSE);
		 return FALSE;
	 }

	 if(nFrame >= 0 && nFrame <= pData->m_nTotalFrame)
	 {
		 pData->m_nCurFrame = nFrame;
		 return TRUE;
	 }
	 else
	 {
		 DUI_ASSERT(FALSE);
	 }

	 return FALSE;
 }

 VOID CDUIAnimation::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
 {

 }

 VOID CDUIAnimation::OnAnimationStart(INT nAnimationID, BOOL bFirstLoop)
 {

 }

 VOID CDUIAnimation::OnAnimationStop(INT nAnimationID)
 {

 }

 VOID CDUIAnimation::OnAnimationElapse(INT nAnimationID)
 {
	if(m_pControl == NULL) return;

	CRefPtr<CAnimationData> pData = GetAnimationByID(nAnimationID);
	if(pData.IsNull()) return;
	
	INT nCurFrame = pData->m_nCurFrame;
	if(nCurFrame == 0)
	{
		OnAnimationStart(nAnimationID, pData->m_bFirstLoop);
		pData->m_bFirstLoop = FALSE;
	}

	OnAnimationStep(pData->m_nTotalFrame, nCurFrame, nAnimationID);
	
	if(nCurFrame >= pData->m_nTotalFrame)
	{
		OnAnimationStop(nAnimationID);
		if(pData->m_bLoop)
		{
			pData->m_nCurFrame = 0;
		}
		else
		{
			m_pControl->KillTimer(nAnimationID);
			m_arAnimations.erase(std::remove(m_arAnimations.begin(), m_arAnimations.end(), pData)
				, m_arAnimations.end());
			pData = NULL;
		}
	}

	if(!pData.IsNull())
	{
		++(pData->m_nCurFrame);
	}
 }

 CRefPtr<CAnimationData> CDUIAnimation::GetAnimationByID(INT nAnimationID)
 {
	CRefPtr<CAnimationData> pRet = NULL;
	INT nCount = m_arAnimations.size();
	for(INT i=0; i<nCount; ++i)
	{
		if(m_arAnimations[i]->m_nAnimationID == nAnimationID)
		{
			pRet = m_arAnimations[i];
			break;
		}
	}

	 return pRet;
 }

DUI_END_NAMESPCE