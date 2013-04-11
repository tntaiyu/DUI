#include "DUIBase.h"

DUI_BGN_NAMESPCE

CDUIObject::CDUIObject()
{
}

CDUIObject::~CDUIObject()
{

}

//avoid call default, and change ref count
CDUIObject::CDUIObject(const CDUIObject& obj)
{

}

//avoid call default, and change ref count
CDUIObject& CDUIObject::operator = (const CDUIObject& obj)
{
	return *this;
}

DUI_END_NAMESPCE