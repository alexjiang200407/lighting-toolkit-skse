#include "LightDesigner.h"

LightDesigner::LightDesigner(preset::PresetDatabase* presetDB) :
	ImGui::ImGuiSelector<preset::LightingPreset>(
		"Light Designer##LightDesigner",
		preset::LightingPreset()),
	presetDB(presetDB)
{}

bool LightDesigner::DrawValueEditor()
{
	if (!ImGui::ImGuiSelector<preset::LightingPreset>::selection.has_value())
		return false;

	bool                    changed = false;
	preset::LightingPreset& lightPreset =
		ImGui::ImGuiSelector<preset::LightingPreset>::selection.value();

	using FLAGS = preset::LightingPreset::Flags;

	uint32_t flags = lightPreset.flags.underlying();
	ImGui::CheckboxFlags("Affect Land", &flags, static_cast<uint32_t>(FLAGS::kAffectLand));
	ImGui::SameLine();
	ImGui::CheckboxFlags("Affect Water", &flags, static_cast<uint32_t>(FLAGS::kAffectWater));
	ImGui::SameLine();
	ImGui::CheckboxFlags("Affect All", &flags, static_cast<uint32_t>(FLAGS::kAffectAll));

	ImGui::CheckboxFlags("Has Shadow", &flags, static_cast<uint32_t>(FLAGS::kCastsShadow));
	ImGui::SameLine();
	ImGui::CheckboxFlags("Dynamic", &flags, static_cast<uint32_t>(FLAGS::kDynamic));
	ImGui::SameLine();
	ImGui::CheckboxFlags("Never Fades", &flags, static_cast<uint32_t>(FLAGS::kNeverFades));
	ImGui::SameLine();
	ImGui::CheckboxFlags("Portal Strict", &flags, static_cast<uint32_t>(FLAGS::kPortalStrict));

	if (flags != lightPreset.flags.underlying())
	{
		changed           = true;
		lightPreset.flags = flags;
	}

	changed = ImGui::SliderAngle("Fov", &lightPreset.fov, 0.0f, 360.0f) || changed;
	changed = ImGui::SliderAutoFill("Depth Bias", &lightPreset.depthBias, 0.0f, 100.0f) || changed;
	changed = ImGui::SliderAutoFill("Falloff", &lightPreset.falloff, 0.0f, 100.0f) || changed;

	return changed;
}
