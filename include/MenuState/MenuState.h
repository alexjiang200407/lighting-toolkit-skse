#pragma once
#include "MenuInputContext.h"
#include "LightingToolkit.h"

class LightingToolkit;
class MenuState;

typedef std::unique_ptr<MenuState> MenuStatePtr;

class MenuState
{
public:
	virtual MenuStatePtr Transition(Input::MenuInputContext* inputCtx) = 0;
	virtual void         DoFrame(LightingToolkit* chiaroscuro);
	virtual bool         ShouldDrawCursor() const;
};
