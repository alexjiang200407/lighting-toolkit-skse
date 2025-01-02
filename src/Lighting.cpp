#include "Lighting.h"
#include "Color.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref) :
	Prop(ref)
{
	niLight.reset(RE::NiPointLight::Create());
	FindOrCreateLight();
}

Lighting::Lighting(RE::TESObjectREFRPtr ref, PresetID colorId, PresetID lightTemplateId) :
	Prop(ref), colorPalette(colorId), lightingPreset(lightTemplateId)
{
	niLight.reset(RE::NiPointLight::Create());
	FindOrCreateLight();
}

void Lighting::DrawControlPanel(PresetDatabase& config)
{
	if (auto* opt = colorPalette.DrawSelectionComboBox(config, "Color"))
	{
		niLight->diffuse = opt->GetColor();
	}

	if (auto* opt = lightingPreset.DrawSelectionComboBox(config, "Lighting"))
	{
		UpdateLightTemplate(*opt);
	}

	ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
	ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);
}

void Lighting::UpdateLightTemplate()
{
	UpdateLightTemplate(LightingPreset());
}

void Lighting::UpdateLightTemplate(const LightingPreset& lightingPreset)
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), lightingPreset));
}

void Lighting::Remove()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	Prop::Remove();

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);
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
	UpdateLightTemplate();
	//RE::UpdateNode(niLight.get());
}
