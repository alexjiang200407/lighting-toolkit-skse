#pragma once
#include "LightingToolkit.h"
#include "MenuInputContext.h"

class LightingToolkit;
class MenuState;

typedef std::unique_ptr<MenuState> MenuStatePtr;

class MenuState
{
public:
	virtual ~MenuState()                                               = default;
	virtual MenuStatePtr Transition(Input::MenuInputContext* inputCtx) = 0;
	virtual void         DoFrame(LightingToolkit* chiaroscuro);
	virtual bool         ShouldDrawCursor() const;
};
