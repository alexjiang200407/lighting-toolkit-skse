#pragma once
#include "MenuState.h"

class MenuHidden :
	public MenuState
{
public:
	MenuHidden(Input::MenuInputContext* inputCtx);

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx) override;
	bool         ShouldDrawCursor() const override;
};
