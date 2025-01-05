#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB) :
	Prop(ref), colorPalette(presetDB), lightingPreset(presetDB)
{
	niLight.reset(RE::NiPointLight::Create());
	FindOrCreateLight();
}
	
Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	Prop(ref), colorPalette(presetDB, color), lightingPreset(presetDB, lightPreset)
{
	niLight.reset(RE::NiPointLight::Create());
	FindOrCreateLight();
}

void Lighting::DrawControlPanel()
{
	if (colorPalette.DrawEditor())
	{
		UpdateLightColor();
	}

	if (lightingPreset.DrawEditor())
	{
		UpdateLightTemplate();
	}

	ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
	ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);
}

void Lighting::UpdateLightColor()
{
	if (auto selection = colorPalette.GetSelection())
		niLight->diffuse = *selection;
}

void Lighting::UpdateLightTemplate()
{
	if (auto selection = lightingPreset.GetSelection())
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

		if (bsLight.get())
			shadowSceneNode->RemoveLight(bsLight);

		bsLight.reset(shadowSceneNode->AddLight(niLight.get(), *selection));
	}
}

void Lighting::Remove()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	Prop::Remove();

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	if (niLight.get())
		shadowSceneNode->RemoveLight(niLight.get());

	niLight->parent->SetAppCulled(true);
	niLight->SetAppCulled(true);
}

void Lighting::MoveToCameraLookingAt(float distanceFromCamera)
{
	assert(RE::PlayerCharacter::GetSingleton());
	assert(RE::PlayerCharacter::GetSingleton()->GetParentCell());

	if (GetCellID() != RE::PlayerCharacter::GetSingleton()->GetParentCell()->formID)
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		ref->MoveTo(RE::PlayerCharacter::GetSingleton());
		shadowSceneNode->RemoveLight(bsLight);
		shadowSceneNode->RemoveLight(niLight.get());
		FindOrCreateLight();
	}
	niLight->world.translate = GetCameraLookingAt(distanceFromCamera);
}

void Lighting::OnEnterCell()
{
	// Skyrim creates a new niLight when we enter a cell
	// so we have to update our state
	FindOrCreateLight();
}

void Lighting::FindOrCreateLight()
{
	if (!ref->Is3DLoaded())
		ref->Load3D(false);

	auto* niRoot = ref->Get3D()->AsFadeNode();

	if (!niRoot)
	{
		logger::error("Base Root Node not found!");
		return;
	}

	if (auto* newLight = niRoot->GetObjectByName("SceneCraftLight"))
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		shadowSceneNode->RemoveLight(niLight.get());
		shadowSceneNode->RemoveLight(bsLight);
		niLight.reset(skyrim_cast<RE::NiPointLight*>(newLight));
	}
	else
	{
		auto* niObj = niRoot->GetObjectByName("AttachLight");

		if (!niObj)
		{
			logger::error("AttachLight node not found!");
			return;
		}

		auto* niNode  = niObj->AsNode();
		niLight->name = "SceneCraftLight";
		RE::AttachNode(niNode, niLight.get());

		// TODO put these into a settings class
		niLight->ambient = RE::NiColor();
		niLight->radius  = RE::NiPoint3(500, 500, 500);
		niLight->SetLightAttenuation(500);
		niLight->fade = 2;
	}
	UpdateLightColor();
	UpdateLightTemplate();
	//RE::UpdateNode(niLight.get());
}
