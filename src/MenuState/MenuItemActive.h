#pragma once
#include "MenuOpen.h"
#include "MenuState.h"

class MenuItemActive : public MenuOpen
{
public:
	MenuItemActive(Input::MenuInputContext* inputCtx);

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx) override;
};
