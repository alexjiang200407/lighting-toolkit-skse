#pragma once
#include "MenuState.h"
#include "MenuLookingAround.h"

class MenuPositioning :
	public MenuLookingAround
{
public:
	MenuPositioning(Input::MenuInputContext* inputCtx);
	~MenuPositioning();

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx);
	void         DrawMenu(Chiaroscuro* menu) override;

private:
	bool prevFreezeTime;
};
