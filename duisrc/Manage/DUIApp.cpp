#include "DUIApp.h"
#include "DUISkinMgr.h"
#include "DUIControlFactory.h"
#include "DUITooltip.h"
#include  <algorithm>

DUI_BGN_NAMESPCE


// Message filter operations
BOOL CDUIMessageLoop::AddMessageFilter(IDUIMessageFilter* pMessageFilter)
{
	if(std::find(m_aMsgFilter.begin(), m_aMsgFilter.end(), pMessageFilter) == m_aMsgFilter.end())
	{
		m_aMsgFilter.push_back(pMessageFilter);
	}

	return TRUE;
}	

VOID CDUIMessageLoop::RemoveMessageFilter(IDUIMessageFilter* pMessageFilter)
{
	m_aMsgFilter.erase(std::remove(m_aMsgFilter.begin(), m_aMsgFilter.end(), pMessageFilter), m_aMsgFilter.end());
}

// Idle handler operations
BOOL CDUIMessageLoop::AddIdleHandler(IDUIIdleHandler* pIdleHandler)
{
	if(std::find(m_aIdleHandler.begin(), m_aIdleHandler.end(), pIdleHandler) == m_aIdleHandler.end())
	{
		m_aIdleHandler.push_back(pIdleHandler);
	}
	
	return TRUE;
}

VOID CDUIMessageLoop::RemoveIdleHandler(IDUIIdleHandler* pIdleHandler)
{
	m_aIdleHandler.erase(std::remove(m_aIdleHandler.begin(), m_aIdleHandler.end(), pIdleHandler), m_aIdleHandler.end());
}


// message loop
int CDUIMessageLoop::Run()
{
	BOOL bDoIdle = TRUE;
	int nIdleCount = 0;
	BOOL bRet;
	
	for(;;)
	{
		while(bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(!OnIdle(nIdleCount++))
				bDoIdle = FALSE;
		}
		
		bRet = ::GetMessage(&m_msg, NULL, 0, 0);
		
		if(bRet == -1)
		{
			continue;   // error, don't process
		}
		else if(!bRet)
		{
			break;   // WM_QUIT, exit message loop
		}
		
		if(!PreTranslateMessage(&m_msg))
		{
			::TranslateMessage(&m_msg);
			::DispatchMessage(&m_msg);
		}
		
		if(IsIdleMessage(&m_msg))
		{
			bDoIdle = TRUE;
			nIdleCount = 0;
		}
	}
	
	return (int)m_msg.wParam;
}

BOOL CDUIMessageLoop::IsIdleMessage(MSG* pMsg)
{
	// These messages should NOT cause idle processing
	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
	case WM_PAINT:
	case 0x0118:	// WM_SYSTIMER (caret blink)
		return FALSE;
	}
	
	return TRUE;
}

// Overrideables
// Override to change message filtering
BOOL CDUIMessageLoop::PreTranslateMessage(MSG* pMsg)
{
	// loop backwards
	for(int i = m_aMsgFilter.size() - 1; i >= 0; i--)
	{
		IDUIMessageFilter* pMessageFilter = m_aMsgFilter[i];
		if(pMessageFilter != NULL && pMessageFilter->PreTranslateMessage(pMsg))
			return TRUE;
	}
	return FALSE;   // not translated
}

// override to change idle processing
BOOL CDUIMessageLoop::OnIdle(int /*nIdleCount*/)
{
	for(UINT i = 0; i < m_aIdleHandler.size(); i++)
	{
		IDUIIdleHandler* pIdleHandler = m_aIdleHandler[i];
		if(pIdleHandler != NULL)
			pIdleHandler->OnIdle();
	}
	return FALSE;   // don't continue
}


CDUIApp::CDUIApp()
{
	m_pSkinMgr = NULL;
	m_pControlFactory = NULL;
}

CDUIApp::~CDUIApp()
{
	if(m_pSkinMgr != NULL) 
	{
		delete m_pSkinMgr;
		m_pSkinMgr = NULL;
	}

	if(m_pControlFactory != NULL) 
	{
		delete m_pControlFactory;
		m_pControlFactory = NULL;
	}
}

BOOL CDUIApp::Init(const CDUIString& strEntryXML)
{
	::CoInitialize(NULL);

	if(m_pSkinMgr == NULL) m_pSkinMgr = new CDUISkinMgr;
	if(m_pControlFactory == NULL) m_pControlFactory = new CDUIControlFactory;

	if(m_pSkinMgr == NULL || m_pControlFactory == NULL) return FALSE;
	
	//skin and resource
	BOOL bRet = m_pSkinMgr->InitializeSkin(strEntryXML);
	if(!bRet)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	
	//control factory
	bRet = m_pControlFactory->InitializeFactory();
	if(!bRet)
	{
		DUI_ASSERT(FALSE);
		return FALSE;
	}

	//setup tooltip
	CDUITooltip* pTooltip = CDUITooltip::Instance();
	if(pTooltip != NULL)
	{
		pTooltip->Setup();
	}
	
	return bRet;
}

void CDUIApp::Term()
{
	//tooltip
	CDUITooltip* pTooltip = CDUITooltip::Instance();
	if(pTooltip != NULL)
	{
		pTooltip->Uninstall();
		CDUITooltip::DestroyInstance();
	}

	//control factory
	if(m_pControlFactory != NULL)
	{
		m_pControlFactory->UninitializeFactory();

		delete m_pControlFactory;
		m_pControlFactory = NULL;
	}
	
	//skin and resource
	if(m_pSkinMgr != NULL)
	{
		m_pSkinMgr->UninitializeSkin();
		
		delete m_pSkinMgr;
		m_pSkinMgr = NULL;
	}

	::CoUninitialize();
}

IDUISkinMgr* CDUIApp::GetSkinMgr() const
{
	return m_pSkinMgr;
}

IDUIControlFactory* CDUIApp::GetControlFactory() const
{
	return m_pControlFactory;
}

CDUIMessageLoop* CDUIApp::GetMessageLoop() 
{
	return &m_messageLoop;
}

DUI_END_NAMESPCE