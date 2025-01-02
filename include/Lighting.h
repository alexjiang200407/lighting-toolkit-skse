#pragma once
#include "LightingPreset.h"
#include "Prop.h"

class Lighting :
	public Prop
{
public:
	Lighting(RE::TESObjectREFRPtr ref, PresetID colorId, PresetID lightTemplateId);

public:
	void DrawControlPanel(PresetDatabase& config) override;
	void UpdateLightColor();
	void UpdateLightTemplate();
	void Remove() override;
	void MoveToCameraLookingAt(float distanceFromCamera) override;
	void OnEnterCell() override;

private:
	void FindOrCreateLight();

private:
	PresetID                        currentLightTemplate = 0;
	PresetID                        currentLightColor    = 0;
	RE::NiPointer<RE::NiNode>       attachNode = nullptr;
	RE::NiPointer<RE::BSLight>      bsLight    = nullptr;
	RE::NiPointer<RE::NiPointLight> niLight    = nullptr;
	LightingPreset            templateData;
};
