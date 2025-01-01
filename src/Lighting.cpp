#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, int colorIdx, int lightTemplateIdx) :
	Prop(ref), palette(colorIdx), lightTemplate(lightTemplateIdx)
{
	niLight.reset(RE::NiPointLight::Create());

	FindOrCreateLight();
}

void Lighting::DrawControlPanel()
{
	if (palette.DrawSelectionComboBox() != -1)
	{
		UpdateLightColor();
	}
	if (lightTemplate.DrawSelectionComboBox() != -1)
	{
		UpdateLightTemplate();
	}
	ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
	ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);
}

void Lighting::UpdateLightColor()
{
	niLight->diffuse = palette.GetCurrentSelection();
}

void Lighting::UpdateLightTemplate()
{
	auto  params          = lightTemplate.GetCurrentSelection().ToLightCreateParams();
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), params));
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
	if (ref->GetParentCell() != RE::PlayerCharacter::GetSingleton()->GetParentCell())
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		ref->MoveTo(RE::PlayerCharacter::GetSingleton());
		shadowSceneNode->RemoveLight(bsLight);
		shadowSceneNode->RemoveLight(niLight.get());
		FindOrCreateLight();
	}
	attachNode->world.translate = GetCameraLookingAt(distanceFromCamera);
	niLight->world.translate    = GetCameraLookingAt(distanceFromCamera);
}

void Lighting::OnEnterCell()
{
	// Skyrim creates a new niLight when we enter a cell
	// so we have to update our state
	FindOrCreateLight();
}

void Lighting::FindOrCreateLight()
{
	// TODO Perform Error check here
	if (!ref->Is3DLoaded())
		ref->Load3D(false);
	auto* niRoot = ref->Get3D()->AsFadeNode();

	if (auto* newLight = niRoot->GetObjectByName("SceneCraftLight"))
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		shadowSceneNode->RemoveLight(niLight.get());
		shadowSceneNode->RemoveLight(bsLight);
		niLight.reset(skyrim_cast<RE::NiPointLight*>(newLight));
	}
	else
	{
		auto* niNode  = niRoot->GetObjectByName("AttachLight")->AsNode();
		niLight->name = "SceneCraftLight";
		RE::AttachNode(niNode, niLight.get());
		attachNode.reset(niNode);
		// TODO put these into a settings class
		niLight->ambient = RE::NiColor();
		niLight->radius  = RE::NiPoint3(500, 500, 500);
		niLight->SetLightAttenuation(500);
		niLight->fade = 2;
	}
	UpdateLightColor();
	UpdateLightTemplate();
	RE::UpdateNode(niLight.get());
}
