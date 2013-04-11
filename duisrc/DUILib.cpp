// DUILib.cpp : Defines the entry point for the DLL application.
//

#include "DUIBase.h"
#include "DUILib.h"
#include "DUIApp.h"

#include "DUIAtl.h"

#ifdef DUI_USE_WTL
CAppModule _Module;
#else
CComModule _Module;
#endif//DUI_USE_WTL

BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
   switch( ul_reason_for_call ) {
   case DLL_PROCESS_ATTACH:
	   {
		   _Module.Init(NULL, hModule);
		   DisableThreadLibraryCalls(hModule);
		   break;
	   }

   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
	   break;

   case DLL_PROCESS_DETACH:
	   {
		   _Module.Term();
		   break;
	   }
      
   }
   return TRUE;
}

DUI_BGN_NAMESPCE


CDUIApp _theDUIApp;

IDUIApp*  WINAPI DUIGetAppInstance()
{
	return &_theDUIApp;
}



DUI_END_NAMESPCE