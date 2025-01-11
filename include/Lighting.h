#pragma once
#include "ColorPalette.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"
#include "Prop.h"

class Lighting :
	public Prop
{
public:
	Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset);
	Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset);

public:
	void            DrawControlPanel() override;
	void            UpdateLightColor();
	void            UpdateLightTemplate();
	void            MoveToCameraLookingAt(float distanceFromCamera) override;
	void            MoveTo(RE::NiPoint3 newPos) override;
	void            OnEnterCell() override;
	void            Remove() override;
	void            Hide() override;
	void            Show() override;
	void            Rotate(float delta) override;
	RE::BSFadeNode* Attach3D() override;
	void            Init3D() override;

private:
	RE::NiPointer<RE::BSLight>               bsLight = nullptr;
	RE::NiPointer<RE::NiPointLight>          niLight = nullptr;
	ColorPalette                             colorPalette;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightCreateParams;
	RE::NiPoint3                             offset{ 0, 0, 0 };
};
