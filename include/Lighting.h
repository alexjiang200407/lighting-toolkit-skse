#pragma once
#include "LightingTemplate.h"
#include "Palette.h"
#include "Prop.h"

class Lighting :
	public Prop
{
public:
	Lighting(RE::TESObjectREFRPtr ref, int colorIdx, int lightTemplateIdx);

public:
	virtual void DrawControlPanel() override;
	void         UpdateLightColor();
	void         UpdateLightTemplate();
	void         Remove() override;
	void         MoveToCameraLookingAt(float distanceFromCamera) override;
	void         OnEnterCell() override;

private:
	void FindOrCreateLight();

private:
	RE::NiPointer<RE::NiNode>       attachNode = nullptr;
	RE::NiPointer<RE::BSLight>      bsLight    = nullptr;
	RE::NiPointer<RE::NiPointLight> niLight    = nullptr;
	Palette                         palette;
	LightingTemplate                lightTemplate;
};
