#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, int colorIdx, int lightTemplateIdx) :
	Prop(ref), palette(colorIdx), lightTemplate(lightTemplateIdx)
{
	niLight.reset(RE::NiPointLight::Create());
	AttachLight();
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

	ShadowSceneRemove(shadowSceneNode);
	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), params));
}

void Lighting::Remove()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	Prop::Remove();
	ShadowSceneRemove(shadowSceneNode);
}

void Lighting::MoveToCameraLookingAt(float distanceFromCamera)
{
	Prop::MoveToCameraLookingAt(distanceFromCamera);
	RE::UpdateNode(niLight.get());
}

void Lighting::ShadowSceneRemove(RE::ShadowSceneNode* shadowSceneNode)
{
	if (bsLight.get())
	{
		shadowSceneNode->RemoveLight(bsLight);
	}
}

void Lighting::AttachLight()
{
	// TODO Perform Error check here
	ref->Load3D(false);
	auto* niRoot = ref->Get3D()->AsFadeNode();
	auto* niNode = niRoot->GetObjectByName("AttachLight")->AsNode();

	niLight->name = "SceneCraftLight";
	RE::AttachNode(niNode, niLight.get());

	// TODO put these into a settings class
	niLight->ambient = RE::NiColor();
	niLight->radius  = RE::NiPoint3(500, 500, 500);
	niLight->SetLightAttenuation(500);
	niLight->fade = 2;

	UpdateLightColor();
	UpdateLightTemplate();

	RE::UpdateNode(niLight.get());
}
