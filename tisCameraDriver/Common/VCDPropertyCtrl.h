
#ifndef VCDPROPERTYCTRL_H_INC_
#define VCDPROPERTYCTRL_H_INC_

#include <vector>

//////////////////////////////////////////////////////////////////////////
/*! Base class for the controls
	Allows updating of unknown control types
*/
class IVCDPropertyCtrl
{
public:
	virtual ~IVCDPropertyCtrl() {}
	virtual void update() = 0;
};

typedef std::vector<IVCDPropertyCtrl*>				tControlVector;
typedef smart_ptr<tControlVector>					tControlVectorPtr;

#endif
