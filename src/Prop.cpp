#include "Prop.h"

Prop::Prop(RE::TESObjectREFRPtr ref) :
	ref(ref)
{
}

bool Prop::DrawTabItem(bool& active)
{
	bool open     = true;
	bool selected = false;
	if ((selected = ImGui::BeginTabItem(std::format("{} 0x{:X}", ref->GetFormEditorID(), ref->GetFormID()).c_str(), &open)))
	{
		active = selected;
		ImGui::EndTabItem();
	}

	if (!open)
	{
		Remove();
	}

	return open;
}

void Prop::DrawControlPanel(preset::PresetDatabase&)
{
	ImGui::Text("This is a Prop");
}

void Prop::Remove()
{
	ref->Disable();
	ref->SetDelete(true);
}

void Prop::Hide()
{
	ref->Disable();
}

void Prop::OnEnterCell()
{
	// TODO Add default behavior
}

RE::FormID Prop::GetCellID()
{
	assert(ref->GetParentCell());
	return ref->GetParentCell()->formID;
}

RE::NiPoint3 Prop::GetCameraLookingAt(float distanceFromCamera)
{
	auto cameraNode = RE::PlayerCamera::GetSingleton()->cameraRoot.get()->AsNode();
	auto cameraNI   = reinterpret_cast<RE::NiCamera*>((cameraNode->children.size() == 0) ? nullptr : cameraNode->children[0].get());

	if (cameraNI)
		return GetCameraPosition() + (cameraNI->world.rotate * RE::NiPoint3{ distanceFromCamera, 0.0f, 0.0f });

	return RE::NiPoint3();
}

void Prop::MoveToCameraLookingAt(float distanceFromCamera)
{
	auto cameraNode = RE::PlayerCamera::GetSingleton()->cameraRoot.get()->AsNode();
	auto cameraNI   = reinterpret_cast<RE::NiCamera*>((cameraNode->children.size() == 0) ? nullptr : cameraNode->children[0].get());

	if (cameraNI)
		ref->SetPosition(GetCameraPosition() + (cameraNI->world.rotate * RE::NiPoint3{ distanceFromCamera, 0.0f, 0.0f }));
}

RE::NiPoint3 Prop::GetCameraPosition()
{
	RE::NiPoint3 origin;
	if (RE::PlayerCamera::GetSingleton()->IsInFreeCameraMode())
		reinterpret_cast<RE::FreeCameraState*>(RE::PlayerCamera::GetSingleton()->currentState.get())->GetTranslation(origin);
	else
		origin = RE::PlayerCamera::GetSingleton()->pos;

	return origin;
}
