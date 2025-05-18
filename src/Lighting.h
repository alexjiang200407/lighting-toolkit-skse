#pragma once
#include "ColorPalette.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"
#include "SKSE/CoSaveIO.h"

class Lighting;
typedef std::unique_ptr<Lighting> LightingPtr;

class Lighting
{
public:
	Lighting(
		RE::TESObjectREFRPtr    ref,
		preset::PresetDatabase* presetDB,
		preset::LightingPreset  lightPreset);
	Lighting(
		RE::TESObjectREFRPtr    ref,
		preset::Color           color,
		preset::PresetDatabase* presetDB,
		preset::LightingPreset  lightPreset);
	Lighting(
		RE::TESObjectREFRPtr                     ref,
		preset::PresetDatabase*                  presetDB,
		RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightPreset,
		float                                    fade,
		float                                    radius,
		bool                                     hideLight  = false,
		bool                                     hideMarker = false);
	Lighting(
		RE::TESObjectREFRPtr                     ref,
		preset::Color                            color,
		preset::PresetDatabase*                  presetDB,
		RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightPreset,
		float                                    fade,
		float                                    radius,
		bool                                     hideLight  = false,
		bool                                     hideMarker = false);

public:
	bool                           DrawTabItem(bool& active);
	void                           UpdateLightColor();
	void                           UpdateLightTemplate();
	virtual void                   DrawControlPanel();
	void                           Remove();
	virtual void                   MoveToCameraLookingAt(bool resetOffset = false);
	virtual void                   MoveTo(RE::NiPoint3 newPos);
	void                           MoveToCurrentPosition();
	virtual void                   OnEnterCell();
	RE::FormID                     GetCellID() const;
	void                           Rotate(float delta);
	virtual void                   Rotate(RE::NiMatrix3 rotation);
	static RE::NiPoint3            GetCameraLookingAt(float distanceFromCamera);
	virtual RE::BSFadeNode*        Attach3D();
	virtual void                   Init3D();
	static std::optional<Lighting> Deserialize(SKSE::CoSaveIO io, preset::PresetDatabase* presetDB);
	void                           DrawCameraOffsetSlider();
	void                           Serialize(SKSE::CoSaveIO io) const;

private:
	static RE::NiPoint3 GetCameraPosition();

private:
	bool                                     hideLight  = false;
	bool                                     hideMarker = false;
	float                                    fade       = 2.0f;
	RE::NiPointer<RE::BSLight>               bsLight    = nullptr;
	RE::NiPointer<RE::NiPointLight>          niLight    = nullptr;
	RE::NiPoint3                             radius{ 500, 500, 500 };
	ColorPalette                             colorPalette;
	RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightCreateParams;
	RE::TESObjectREFRPtr                     ref;
	RE::NiPoint3                             cameraOffset = { 50.0f, 0.0f, 0.0f };
	RE::NiPoint3                             worldTranslate;
};
