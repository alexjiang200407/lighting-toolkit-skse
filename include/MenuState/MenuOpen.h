#pragma once
#include "LightingToolkit.h"
#include "MenuState.h"

class MenuOpen :
	public MenuState
{
public:
	MenuOpen() = default;
	MenuOpen(Input::MenuInputContext* inputCtx);

public:
	MenuStatePtr Transition(Input::MenuInputContext* inputCtx) override;
	void         DoFrame(LightingToolkit* menu) final override;
	virtual void DrawMenu(LightingToolkit* menu);

private:
	static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
	bool                              previouslyInFreeCameraMode;
	bool                              previouslyFreezeTime;
};
