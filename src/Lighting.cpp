#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, int colorIdx, int lightTemplateIdx) :
	Prop(ref), palette(colorIdx), lightTemplate(lightTemplateIdx)
{
	// TODO Perform Error check here
	ref->Load3D(false);
	auto* niRoot   = ref->Get3D()->AsFadeNode();
	auto* niAvNode = niRoot->children[0].get();

	niLight.reset(skyrim_cast<RE::NiPointLight*>(niAvNode));

	niLight->radius = RE::NiPoint3(500, 500, 500);
	niLight->fade   = 2;

	// Remove original Light
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	shadowSceneNode->RemoveLight(niLight.get());

	UpdateLightColor();
	UpdateLightTemplate();
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

void Lighting::ShadowSceneRemove(RE::ShadowSceneNode* shadowSceneNode)
{
	if (bsLight.get())
	{
		shadowSceneNode->RemoveLight(bsLight);
	}
}
