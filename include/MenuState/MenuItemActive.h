#pragma once
#include "MenuState.h"
#include "MenuOpen.h"

class MenuItemActive :
	public MenuOpen
{
public:
	MenuItemActive(Input::MenuInputContext* inputCtx);

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx) override;
};
