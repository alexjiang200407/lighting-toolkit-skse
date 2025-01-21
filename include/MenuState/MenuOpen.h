#pragma once
#include "Chiaroscuro.h"
#include "MenuState.h"

class MenuOpen :
	public MenuState
{
public:
	MenuOpen() = default;
	MenuOpen(Input::MenuInputContext* inputCtx);

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx) override;
	void         DoFrame(Chiaroscuro* menu) final override;
	virtual void DrawMenu(Chiaroscuro* menu);

private:
	static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
	bool                              previouslyInFreeCameraMode;
	bool                              previouslyFreezeTime;
};
