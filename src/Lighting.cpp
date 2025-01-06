#include "Lighting.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	Prop(ref), colorPalette(presetDB), lightCreateParams(lightPreset)
{
	niLight.reset(RE::NiPointLight::Create());
	FindOrCreateLight();
	UpdateLightColor();
	UpdateLightTemplate();
}

Lighting::Lighting(RE::TESObjectREFRPtr ref, preset::Color color, preset::PresetDatabase* presetDB, preset::LightingPreset lightPreset) :
	Prop(ref), colorPalette(presetDB, color), lightCreateParams(lightPreset)
{
	niLight.reset(RE::NiPointLight::Create());
	FindOrCreateLight();
	UpdateLightColor();
	UpdateLightTemplate();
}

void Lighting::DrawControlPanel()
{
	auto flags = ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysUseWindowPadding;
	ImGui::BeginChild("##LightColorSelector", ImVec2(0, 0), flags);
	{
		if (colorPalette.DrawEditor())
		{
			UpdateLightColor();
		}
	}
	ImGui::EndChild();

	ImGui::BeginChild("##LightSelector", ImVec2(0, 0), flags);
	{
		ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
		ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);
	}
	ImGui::EndChild();
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

void Lighting::Remove()
{
	auto* shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];
	if (niLight && niLight->parent)
	{
		niLight->parent->DetachChild(niLight.get());
	}

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);


	Prop::Remove();
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
		niLight.reset(skyrim_cast<RE::NiPointLight*>(newLight));
		bsLight.reset(shadowSceneNode->GetLight(niLight.get()));
	}
	else
	{
		auto* niNode  = niRoot->GetObjectByName("AttachLight");
		niLight->name = "SceneCraftLight";
		RE::AttachNode(niNode->AsNode(), niLight.get());

		// TODO put these into a settings class
		niLight->ambient = RE::NiColor();
		niLight->radius  = RE::NiPoint3(500, 500, 500);
		niLight->SetLightAttenuation(500);
		niLight->fade = 2;
	}
}
