#ifndef __DUI_ANIMATION_H__
#define __DUI_ANIMATION_H__

#include "DUILib.h"

DUI_BGN_NAMESPCE

class DUILIB_API IDUIAnimation
{
public:
	virtual ~IDUIAnimation() { NULL; }

	virtual BOOL StartAnimation(INT nElapse, INT nTotalFrame, INT nAnimationID = 0, BOOL bLoop = FALSE) = 0;
	virtual VOID StopAnimation(INT nAnimationID = 0) = 0;
	virtual BOOL IsAnimationRunning(INT nAnimationID) = 0;
	virtual INT GetCurrentFrame(INT nAnimationID = 0) = 0;
	virtual BOOL SetCurrentFrame(INT nFrame, INT nAnimationID = 0) = 0;

	virtual VOID OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID) = 0;
	virtual VOID OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) = 0;
	virtual VOID OnAnimationStop(INT nAnimationID) = 0;

	virtual VOID OnAnimationElapse(INT nAnimationID) = 0;
};

class DUILIB_API CAnimationData: public CRefCountBase
{
public:
	CAnimationData(INT nElipse, INT nFrame, INT nID, BOOL bLoop)
	{
		m_bFirstLoop = TRUE;
		m_nCurFrame = 0;
		m_nElapse = nElipse;
		m_nTotalFrame = nFrame;
		m_bLoop = bLoop;
		m_nAnimationID = nID;
	}

protected:
	friend class CDUIAnimation;

	INT m_nAnimationID;
	INT m_nElapse;

	INT m_nTotalFrame;
	INT m_nCurFrame;

	BOOL m_bLoop;
	BOOL m_bFirstLoop;
};

class IDUIControl;
class DUILIB_API CDUIAnimation: public IDUIAnimation
{
public:
	CDUIAnimation(IDUIControl* pOwner);

	virtual BOOL StartAnimation(INT nElapse, INT nTotalFrame, INT nAnimationID = 0, BOOL bLoop = FALSE);
	virtual VOID StopAnimation(INT nAnimationID = 0);
	virtual BOOL IsAnimationRunning(INT nAnimationID);
	virtual INT GetCurrentFrame(INT nAnimationID = 0);
	virtual BOOL SetCurrentFrame(INT nFrame, INT nAnimationID = 0);

	virtual VOID OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
	virtual VOID OnAnimationStart(INT nAnimationID, BOOL bFirstLoop);
	virtual VOID OnAnimationStop(INT nAnimationID);
	
	virtual VOID OnAnimationElapse(INT nAnimationID);

protected:
	CRefPtr<CAnimationData> GetAnimationByID(INT nAnimationID);

protected:
	IDUIControl* m_pControl;
	std::vector<CRefPtr<CAnimationData> > m_arAnimations;
};


DUI_END_NAMESPCE


#endif //__DUI_ANIMATION_H__