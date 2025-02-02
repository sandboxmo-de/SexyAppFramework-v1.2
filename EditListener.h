#ifndef __EDITLISTENER_H__
#define __EDITLISTENER_H__

#include "Common.h"
#include "KeyCodes.h"

namespace Sexy
{

class EditListener
{
public:
	virtual void			EditWidgetText(int theId, const std::string& theString) {};

	virtual bool			AllowKey(int theId, KeyCode theKey) { return true; }
	virtual bool			AllowChar(int theId, char theChar) { return true; }
	virtual bool			AllowText(int theId, const std::string& theText) { return true; }
};

}

#endif //__EDITLISTENER_H__