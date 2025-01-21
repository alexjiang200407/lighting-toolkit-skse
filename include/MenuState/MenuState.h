#pragma once
#include "MenuInputContext.h"
#include "Chiaroscuro.h"

class Chiaroscuro;
class MenuState;

typedef std::unique_ptr<MenuState> MenuStatePtr;

class MenuState
{
public:
	virtual MenuStatePtr Transition(Input::MenuInputContext* inputCtx) = 0;
	virtual void         DoFrame(Chiaroscuro* chiaroscuro);
	virtual bool         ShouldDrawCursor() const;
};
