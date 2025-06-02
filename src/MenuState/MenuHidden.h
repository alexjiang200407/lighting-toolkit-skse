#pragma once
#include "MenuState/MenuLookingAround.h"

class MenuHidden : public MenuState
{
public:
	MenuHidden(Input::MenuInputContext* inputCtx);
	~MenuHidden() override;

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx) override;
	bool         ShouldDrawCursor() const override;
};
