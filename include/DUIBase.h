#ifndef __DUI_BASE_H__
#define __DUI_BASE_H__

#include "DUIRef.h"
#include "DUIDebug.h"

DUI_BGN_NAMESPCE

class DUILIB_API CDUIObject: public CRefCountBase
{
public:
	CDUIObject();
	virtual ~CDUIObject();

	CDUIObject(const CDUIObject& obj);
	CDUIObject& operator = (const CDUIObject& obj);

};

DUI_END_NAMESPCE

#endif // __DUI_BASE_H__