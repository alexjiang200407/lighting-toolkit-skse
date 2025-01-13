#pragma once
#include "ImGui/ImGuiPresetSelector.h"
#include "ImGui/ImGuiTabBar.h"
#include "Preset/PresetDatabase.h"

class Prop :
	public ImGui::ImGuiTabBarItemRemovable
{
public:
	Prop(RE::TESObjectREFRPtr ref);

public:
	virtual bool            DrawTabItem(bool& active) override;
	virtual void            DrawControlPanel() = 0;
	virtual void            Remove() override;
	virtual void            MoveToCameraLookingAt(bool resetOffset = false);
	virtual void            MoveTo(RE::NiPoint3 newPos);
	void                    MoveToCurrentPosition();
	virtual void            OnEnterCell();
	RE::FormID              GetCellID();
	void                    Rotate(float delta);
	virtual void            Rotate(RE::NiMatrix3 rotation);
	static RE::NiPoint3     GetCameraLookingAt(float distanceFromCamera);
	virtual RE::BSFadeNode* Attach3D();
	virtual void            Init3D();
	void                    DrawCameraOffsetSlider();

private:
	static RE::NiPoint3 GetCameraPosition();

protected:
	RE::TESObjectREFRPtr ref;

private:
	RE::NiPoint3 cameraOffset = { 50.0f, 0.0f, 0.0f };
	RE::NiPoint3 worldTranslate;
};
