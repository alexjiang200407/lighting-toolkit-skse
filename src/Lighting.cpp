#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	Prop(ref), colorPalette(presetDB), lightCreateParams(lightPreset)
{
	FindOrCreateLight();
	UpdateLightColor();
	UpdateLightTemplate();
	MoveToCameraLookingAt(50.0f);
}

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	Prop(ref), colorPalette(presetDB, color), lightCreateParams(lightPreset)
{
	FindOrCreateLight();
	UpdateLightColor();
	UpdateLightTemplate();
	MoveToCameraLookingAt(50.0f);
}

void Lighting::DrawControlPanel()
{
	if (colorPalette.DrawEditor())
	{
		UpdateLightColor();
	}
	ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
	ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);
	Prop::DrawControlPanel();
}

void Lighting::UpdateLightColor()
{
	if (auto selection = colorPalette.GetSelection())
		niLight->diffuse = *selection;
}

void Lighting::UpdateLightTemplate()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), lightCreateParams));
}

void Lighting::MoveToCameraLookingAt(float distanceFromCamera)
{
	assert(RE::PlayerCharacter::GetSingleton());
	assert(RE::PlayerCharacter::GetSingleton()->GetParentCell());

	if (GetCellID() != RE::PlayerCharacter::GetSingleton()->GetParentCell()->formID)
	{
		auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
		shadowSceneNode->RemoveLight(bsLight);
		shadowSceneNode->RemoveLight(niLight.get());
		FindOrCreateLight();
	}
	Prop::MoveToCameraLookingAt(distanceFromCamera);
}

void Lighting::MoveTo(RE::NiPoint3 point)
{
	niLight->world.translate = point;
	Prop::MoveTo(point);
}

void Lighting::OnEnterCell()
{
	// Skyrim creates a new niLight when we enter a cell
	// so we have to update our state
	FindOrCreateLight();
	UpdateLightColor();
	UpdateLightTemplate();
	MoveToCurrentPosition();
}

void Lighting::Remove()
{
	auto* shadowSceneNode                   = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	shadowSceneNode->allowLightRemoveQueues = false;

	if (niLight.get())
		shadowSceneNode->RemoveLight(niLight.get());

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	shadowSceneNode->allowLightRemoveQueues = true;
	Prop::Remove();
}

void Lighting::Hide()
{
	Prop::Hide();
	niLight->SetAppCulled(true);
}

void Lighting::Show()
{
	niLight->SetAppCulled(false);
	Prop::Show();
}

void Lighting::Rotate(float delta)
{
	RE::NiMatrix3 rotation = {
		{ 1, 0, 0 },
		{ 0, cos(delta), -sin(delta) },
		{ 0, sin(delta), cos(delta) }
	};

	if (niLight)
	{
		niLight->world.rotate = niLight->world.rotate * rotation;
	}
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
		niLight.reset(skyrim_cast<RE::NiPointLight*>(newLight));
	}
	else
	{
		auto* niNode = niRoot->GetObjectByName("AttachLight")->AsNode();
		niLight.reset(RE::NiPointLight::Create());
		niLight->name = "SceneCraftLight";
		RE::AttachNode(niNode, niLight.get());

		// TODO put these into a settings class
		niLight->ambient = RE::NiColor();
		niLight->radius  = RE::NiPoint3(500, 500, 500);
		niLight->SetLightAttenuation(500);
		niLight->fade = 2;
	}
}
