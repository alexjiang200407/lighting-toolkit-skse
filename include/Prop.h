#pragma once
#include "ImGui/ImGuiTabBar.h"
#include "Preset/PresetDatabase.h"

class Prop :
	ImGui::ImGuiTabBarItemRemovable
{
public:
	Prop(RE::TESObjectREFRPtr ref);

public:
	virtual bool        DrawTabItem(bool& active) override;
	virtual void        DrawControlPanel();
	virtual void        Remove() override;
	virtual void        MoveToCameraLookingAt(float distanceFromCamera);
	virtual void        Hide();
	virtual void        OnEnterCell();
	RE::FormID          GetCellID();
	static RE::NiPoint3 GetCameraLookingAt(float distanceFromCamera);

private:
	static RE::NiPoint3 GetCameraPosition();

protected:
	RE::TESObjectREFRPtr ref;
};
