#pragma once
#include "ColorPalette.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"
#include "ImGui/ImGuiTabBar.h"

class Lighting :
	public ImGui::ImGuiTabBarItemRemovable
{
public:
	Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset);
	Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset);

public:
	virtual bool            DrawTabItem(bool& active) override;
	void                    UpdateLightColor();
	void                    UpdateLightTemplate();
	virtual void            DrawControlPanel();
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

private:
	bool                                     hideLight  = false;
	bool                                     hideMarker = false;
	float                                    fade       = 2.0f;
	RE::NiPoint3                             radius{ 500, 500, 500 };
	RE::NiPointer<RE::BSLight>               bsLight = nullptr;
	RE::NiPointer<RE::NiPointLight>          niLight = nullptr;
	ColorPalette                             colorPalette;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightCreateParams;
	RE::NiPoint3                             offset{ 0, 0, 0 };
	RE::TESObjectREFRPtr                     ref;
	RE::NiPoint3                             cameraOffset = { 50.0f, 0.0f, 0.0f };
	RE::NiPoint3                             worldTranslate;
};
