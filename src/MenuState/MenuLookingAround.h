#pragma once
#include "MenuOpen.h"
#include "MenuState.h"

class MenuLookingAround : public MenuOpen
{
public:
	MenuLookingAround(Input::MenuInputContext* inputCtx);

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx) override;
	void         DrawMenu(LightingToolkit* menu) override;
	bool         ShouldDrawCursor() const override;
};
