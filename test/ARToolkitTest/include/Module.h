#ifndef MODULE_H
#define MODULE_H

#include "flint/app/App.h"
#include "flint/Vector.h"

class Module {
 public:
	virtual ~Module() {}
	virtual void	setup( fli::app::App *app ) = 0;
	virtual void	update( fli::app::App *app, bool visible ) = 0;
	virtual void	draw( fli::app::App *app, const fli::Vec4d &camLoc ) = 0;
};

#endif // MODULE_H