#include "ColorDesigner.h"

ColorDesigner::ColorDesigner(preset::PresetDatabase* presetDB) :
	ImGui::ImGuiSelector<preset::Color>("Color Designer##ColorDesigner", {}), presetDB(presetDB)
{
}

bool ColorDesigner::DrawValueEditor()
{
	if (!ImGui::ImGuiSelector<preset::Color>::selection.has_value())
		return false;

	bool           changed = false;
	preset::Color& color   = ImGui::ImGuiSelector<preset::Color>::selection.value();

	ImGui::InputText("Color Name", &color.GetName());

	float rgb[3] = { color.red, color.green, color.blue };
	if (ImGui::ColorEdit3("Color Picker", rgb))
	{
		color.red   = rgb[0];
		color.green = rgb[1];
		color.blue  = rgb[2];
		changed     = true;
	}
	return changed;
}
