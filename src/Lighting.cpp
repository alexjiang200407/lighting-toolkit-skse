#include "Lighting.h"
#include "Color.h"

Lighting::Lighting(RE::TESObjectREFRPtr ref, PresetID colorIdx, PresetID lightTemplateIdx) :
	Prop(ref), currentLightColor(colorIdx), currentLightTemplate(lightTemplateIdx)
{
	niLight.reset(RE::NiPointLight::Create());

	FindOrCreateLight();
}

void Lighting::DrawControlPanel(PresetDatabase& config)
{
	{
		auto opt = config.DrawSelectionComboBox<Color>(PresetTID::kColor, "Color", currentLightColor);
		if (opt.has_value())
		{
			niLight->diffuse = (*opt)->GetColor();
			currentLightColor = (*opt)->GetID();
		}
	}

	{
		auto opt = config.DrawSelectionComboBox<LightingTemplateData>(PresetTID::kLightTemplate, "Light Template", templateData.GetID());
		if (opt.has_value())
		{
			templateData = *opt.value();
			UpdateLightTemplate();
		}
	}
	ImGui::SliderAutoFill("Light Radius", &niLight->radius.x, 32.0f, 1024.0f);
	ImGui::SliderAutoFill("Light Intensity", &niLight->fade, 0.0f, 10.0f);
}

void Lighting::UpdateLightColor()
{
	//niLight->diffuse = palette.GetCurrentSelection();
}

void Lighting::UpdateLightTemplate()
{
	auto*       shadowSceneNode = RE::BSShaderManager::State::GetSingleton().shadowSceneNode[0];

	if (bsLight.get())
		shadowSceneNode->RemoveLight(bsLight);

	bsLight.reset(shadowSceneNode->AddLight(niLight.get(), templateData));
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
		auto* niObj   = niRoot->GetObjectByName("AttachLight");

		if (!niObj)
		{
			logger::error("AttachLight node not found!");
			return;
		}

		auto* niNode  = niObj->AsNode();
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
