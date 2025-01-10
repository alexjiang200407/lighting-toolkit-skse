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
	void DrawControlPanel() override;
	void UpdateLightColor();
	void UpdateLightTemplate();
	void Remove() override;
	void MoveToCameraLookingAt(float distanceFromCamera) override;
	void OnEnterCell() override;

private:
	void FindOrCreateLight();

private:
	RE::NiPointer<RE::BSLight>               bsLight = nullptr;
	RE::NiPointer<RE::NiPointLight>          niLight = nullptr;
	ColorPalette                             colorPalette;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightCreateParams;
};
