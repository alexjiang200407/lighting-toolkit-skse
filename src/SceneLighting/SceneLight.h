#pragma once
#include "ColorPalette.h"
#include "LightingPreset.h"
#include "Preset/PresetDatabase.h"
#include "SKSE/CoSaveIO.h"

class SceneLight;
typedef std::unique_ptr<SceneLight> LightingPtr;

class SceneLight
{
public:
	SceneLight(
		RE::TESObjectREFRPtr    ref,
		preset::PresetDatabase* presetDB,
		preset::LightingPreset  lightPreset);
	SceneLight(
		RE::TESObjectREFRPtr    ref,
		preset::Color           color,
		preset::PresetDatabase* presetDB,
		preset::LightingPreset  lightPreset);
	SceneLight(
		RE::TESObjectREFRPtr                     ref,
		preset::PresetDatabase*                  presetDB,
		RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightPreset,
		float                                    fade,
		float                                    radius,
		bool                                     hideLight  = false,
		bool                                     hideMarker = false);
	SceneLight(
		RE::TESObjectREFRPtr                     ref,
		preset::Color                            color,
		preset::PresetDatabase*                  presetDB,
		RE::ShadowSceneNode::LIGHT_CREATE_PARAMS lightPreset,
		float                                    fade,
		float                                    radius,
		bool                                     hideLight  = false,
		bool                                     hideMarker = false);

public:
	bool                DrawTabItem(bool& active);
	void                UpdateLightColor();
	void                UpdateLightTemplate();
	void                DrawControlPanel();
	void                Remove();
	void                MoveToCameraLookingAt(bool resetOffset = false);
	void                MoveTo(RE::NiPoint3 newPos);
	void                MoveToCurrentPosition();
	void                OnEnterCell();
	RE::FormID          GetCellID() const;
	void                Rotate(float delta);
	void                Rotate(RE::NiMatrix3 rotation);
	static RE::NiPoint3 GetCameraLookingAt(float distanceFromCamera);
	RE::BSFadeNode*     Attach3D();
	void                Init3D();

	static std::optional<SceneLight>
		 Deserialize(SKSE::CoSaveIO io, preset::PresetDatabase* presetDB);
	void DrawCameraOffsetSlider();
	void Serialize(SKSE::CoSaveIO io) const;
	void HideGeom(RE::NiAVObject* niAvObj);
	void HideGeom();

private:
	static RE::NiPoint3 GetCameraPosition();

private:
	std::string                              tabLabel;
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
