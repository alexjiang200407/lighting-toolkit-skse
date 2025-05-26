#include "Environment/Environment.h"
#include "MCM/Settings.h"

void Environment::OnDataLoaded()
{
	BaseFormItem<RE::TESWeather>::GetAllFormsOfType<WeatherItem>(
		weathers,
		[](RE::TESWeather* weather) { return WeatherItem{ weather }; });

	BaseFormItem<RE::BGSLightingTemplate>::GetAllFormsOfType<LightingTemplateItem>(
		lightingTemplates,
		[](auto* lightingTemplate) { return LightingTemplateItem(lightingTemplate); });
}

static float* GetSunXExtreme()
{
	static auto* sunX = REL::Relocation<float*>{ RELOCATION_ID(502621, 370250) }.get();
	return sunX;
}

static float* GetSunYExtreme()
{
	static auto* sunY = REL::Relocation<float*>{ RELOCATION_ID(502623, 370253) }.get();
	return sunY;
}

[[maybe_unused]] static float* GetSunZExtreme()
{
	static auto* sunZ = REL::Relocation<float*>{ RELOCATION_ID(502625, 370256) }.get();
	return sunZ;
}

void Environment::DrawWindow()
{
	ImGui::PushID("environment");
	{
		DrawWeatherControl();
		DrawLightingTemplateControl();
		if (ImGui::Button("Reset Environment"))
		{
			Restore();
		}
	}
	ImGui::PopID();
}

void Environment::OnMenuOpened()
{
	auto* sky = RE::Sky::GetSingleton();

	initialSunExtreme[0] = *GetSunXExtreme();
	initialSunExtreme[1] = *GetSunYExtreme();

	if (!sky)
	{
		logger::error("Could not find sky");
		return;
	}

	if (auto* skyWeather = sky->currentWeather)
	{
		originalWeather.emplace(skyWeather);
	}

	if (auto* sun = sky->sun; sun && sun->light)
	{
		initialSunIntensity = sun->light->fade;
	}
	else
	{
		logger::error("Sun not found");
	}

	if (auto* cell = RE::PlayerCharacter::GetSingleton()->GetParentCell();
	    cell && cell->IsInteriorCell())
	{
		originalLightingTemplate = cell->lightingTemplate;
	}
}

void Environment::OnMenuClosed()
{
	Restore();
	originalWeather.reset();
	originalLightingTemplate = nullptr;
}

template <typename T>
static bool ColorEditor4(const char* label, T* color, ImGuiColorEditFlags flags = 0)
{
	float colors[4];

	if (color)
	{
		colors[0] = color->red / 255.0f;
		colors[1] = color->green / 255.0f;
		colors[2] = color->blue / 255.0f;
		colors[3] = color->alpha / 255.0f;
	}
	else
	{
		std::fill(colors, colors + sizeof(colors) / sizeof(float), 0.0f);
	}

	ImGui::PushFullItemWidth(label);
	bool retVal = ImGui::ColorEdit4(label, colors, flags);
	ImGui::PopItemWidth();

	if (color)
	{
		color->red   = static_cast<uint8_t>(std::clamp(colors[0] * 255.0f, 0.0f, 255.0f));
		color->green = static_cast<uint8_t>(std::clamp(colors[1] * 255.0f, 0.0f, 255.0f));
		color->blue  = static_cast<uint8_t>(std::clamp(colors[2] * 255.0f, 0.0f, 255.0f));
		color->alpha = static_cast<uint8_t>(std::clamp(colors[3] * 255.0f, 0.0f, 255.0f));
	}
	return retVal;
}

void Environment::DrawColorDataEditor(
	const char* label,
	size_t      colorType,
	WeatherItem currentWeather)
{
	assert(currentWeather.base);

	using ColorTimes = RE::TESWeather::ColorTimes;

	ImGui::PushID(label);
	if (ImGui::CollapsingHeader(label))
	{
		ColorEditor4("Day", &currentWeather.base->colorData[colorType][ColorTimes::kDay]);
		ColorEditor4("Night", &currentWeather.base->colorData[colorType][ColorTimes::kNight]);
		ColorEditor4("Sunrise", &currentWeather.base->colorData[colorType][ColorTimes::kSunrise]);
		ColorEditor4("Sunset", &currentWeather.base->colorData[colorType][ColorTimes::kSunset]);
	}
	ImGui::PopID();
}

void Environment::Restore()
{
	*GetSunXExtreme() = initialSunExtreme[0];
	*GetSunYExtreme() = initialSunExtreme[1];

	if (auto currentWeather = GetCurrentWeather(); currentWeather != weathers.end())
	{
		currentWeather->RestoreOriginalData();
	}

	auto* sky = RE::Sky::GetSingleton();

	if (!sky)
	{
		logger::error("Could not get Sky");
		return;
	}

	if (auto* sun = sky->sun; sun && sun->light)
		sun->light->fade = initialSunIntensity;
	else
		logger::error("Could not get Sun or Sun directional light");

	auto* cell = RE::PlayerCharacter::GetSingleton()->parentCell;

	if (originalWeather)
	{
		if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton())
			taskQueueInterface->QueueForceWeather(originalWeather->base, true);
		else
			logger::error("Could not get task queue interface to force weather");
	}

	if (cell)
	{
		auto currentLightingTemplate = GetCurrentLightingTemplate(cell);
		if (currentLightingTemplate != lightingTemplates.end())
			currentLightingTemplate->RestoreOriginalData();
	}

	if (originalLightingTemplate && cell)
	{
		cell->lightingTemplate = originalLightingTemplate;
	}
}

Environment::WeatherItem::iterator Environment::GetCurrentWeather()
{
	if (auto* sky = RE::Sky::GetSingleton(); sky && sky->currentWeather)
	{
		return FindCurrent(sky->currentWeather, weathers);
	}

	return weathers.end();
}

Environment::LightingTemplateItem::iterator
	Environment::GetCurrentLightingTemplate(const RE::TESObjectCELL* cell)
{
	if (!cell || !cell->lightingTemplate)
		return lightingTemplates.end();

	return FindCurrent(cell->lightingTemplate, lightingTemplates);
}

template <typename T, bool HAS_NONE = false, typename It>
void TESFormComboBox(
	const char*                     label,
	It&                             current,
	std::vector<T>&                 allForms,
	std::function<bool(It, It, It)> handleChanged)
{
	ImGui::PushFullItemWidth(label);
	std::vector<std::string> labels;
	labels.reserve(allForms.size() + 1);

	int currentItem = -1;

	if constexpr (HAS_NONE)
	{
		labels.push_back("None");
		if (current != allForms.end())
			currentItem = 0;
	}

	{
		int i = 0;

		for (auto it = allForms.begin(); it != allForms.end(); it++)
		{
			labels.push_back(it->GetName());
			if (current == it)
			{
				currentItem = i;
			}
			i++;
		}
	}

	if (ImGui::ComboWithFilter(label, &currentItem, labels, -1))
	{
		It newValue;
		if (currentItem >= 0 && currentItem < static_cast<int>(allForms.size()))
		{
			newValue = allForms.begin() + currentItem;
		}
		else if constexpr (HAS_NONE)
		{
			newValue = allForms.begin();
		}

		if (handleChanged(current, newValue, allForms.end()))
		{
			current = newValue;
		}
	}

	ImGui::PopItemWidth();
}

template <typename T, typename U, typename K>
bool EnumSetCheckbox(const char* label, REX::EnumSet<T, U>* enumSet, K keys)
{
	bool enabled = enumSet ? enumSet->all(keys) : false;
	if (ImGui::Checkbox(label, &enabled))
	{
		if (enabled && enumSet)
			enumSet->set(keys);
		else if (enumSet)
			enumSet->reset(keys);
		return true;
	}
	return false;
}

void Environment::DrawLightingTemplateControl()
{
	const auto* player = RE::PlayerCharacter::GetSingleton();
	auto*       cell   = player->parentCell;

	if (!cell)
		return;

	if (ImGui::BeginPanel("LightingTemplateEditor"))
	{
		ImGui::BeginDisabled(!cell->IsInteriorCell());
		{
			auto current = GetCurrentLightingTemplate(cell);
			TESFormComboBox<LightingTemplateItem, true, LightingTemplateItem::iterator>(
				"Lighting Template",
				current,
				lightingTemplates,
				[cell](auto, auto newLightingTemplate, auto end) {
					if (newLightingTemplate != end)
					{
						cell->lightingTemplate = newLightingTemplate->base;
					}
					return true;
				});
		}
		ImGui::EndDisabled();
	}
	ImGui::EndPanel();
}

void Environment::DrawWeatherControl()
{
	using MCM        = MCM::Settings;
	using ColorTypes = RE::TESWeather::ColorTypes;

	auto currentWeather = GetCurrentWeather();

	if (ImGui::BeginPanel("##WeatherControl"))
	{
		TESFormComboBox<WeatherItem, false, WeatherItem::iterator>(
			"Weather",
			currentWeather,
			weathers,
			[](WeatherItem::iterator oldWeather,
		       WeatherItem::iterator newWeather,
		       WeatherItem::iterator end) {
				if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton();
			        taskQueueInterface && newWeather != end)
				{
					taskQueueInterface->QueueForceWeather(newWeather->base, true);
					if (oldWeather != end)
						oldWeather->RestoreOriginalData();
					return true;
				}
				return false;
			});
		if (currentWeather != weathers.end())
		{
			ImGui::SliderAutoFill(
				"Wind Speed",
				&currentWeather->GetData().windSpeed,
				std::numeric_limits<uint8_t>::min(),
				std::numeric_limits<uint8_t>::max());
		}
	}
	ImGui::EndPanel();

	if (auto* sun = RE::Sky::GetSingleton()->sun; sun && sun->light)
	{
		static const float sunMaxIntensity = MCM::GetOrDefault_s("Sun", "fMaxIntensity", 10.0f);

		if (ImGui::BeginPanel("##SunProperties"))
		{
			auto* player = RE::PlayerCharacter::GetSingleton();
			ImGui::BeginDisabled(player->parentCell && player->parentCell->IsInteriorCell());
			ImGui::SliderAutoFill("Sun Intensity", &sun->light->fade, 0.0f, sunMaxIntensity);
			if (currentWeather != weathers.end())
			{
				DrawColorDataEditor("Sun Color", ColorTypes::kSun, *currentWeather);
				DrawColorDataEditor("Sun Light", ColorTypes::kSunlight, *currentWeather);
				DrawColorDataEditor("Sun Glare", ColorTypes::kSunGlare, *currentWeather);
				DrawColorDataEditor("Moon Glare", ColorTypes::kMoonGlare, *currentWeather);
			}

			float&       sunX         = *GetSunXExtreme();
			float&       sunY         = *GetSunYExtreme();
			const double distance     = std::sqrt((sunX * sunX) + (sunY * sunY));
			float        currentAngle = std::atan2(sunX, sunY);

			ImGui::PushFullItemWidth("Sun Rotation");
			if (ImGui::SliderAngle("Sun Rotation", &currentAngle, -180.0f, 179.0f))
			{
				sunX = (float)(std::sin(currentAngle) * distance);
				sunY = (float)(std::cos(currentAngle) * distance);
			}
			ImGui::PopItemWidth();
			ImGui::EndDisabled();
		}
		ImGui::EndPanel();
	}
	else
	{
		logger::warn("Could not find sun or sun directional light");
	}
}

Environment::WeatherItem::WeatherItem(RE::TESWeather* weather) :
	BaseFormItem<RE::TESWeather>(weather), originalData(weather->data)
{
	memcpy(oldColorData, weather->colorData, sizeof(oldColorData));
}

void Environment::WeatherItem::RestoreOriginalData()
{
	base->data = originalData;
	memcpy(base->colorData, oldColorData, sizeof(oldColorData));
}

inline RE::TESWeather::Data& Environment::WeatherItem::GetData() { return base->data; }

Environment::LightingTemplateItem::LightingTemplateItem(RE::BGSLightingTemplate* lightingTemplate) :
	BaseFormItem<RE::BGSLightingTemplate>(lightingTemplate)
{
	assert(lightingTemplate);
	memcpy(&originalData, &lightingTemplate->data, sizeof(lightingTemplate->data));
	memcpy(
		&originalDirectionalAmbientLightingColors,
		&lightingTemplate->directionalAmbientLightingColors,
		sizeof(lightingTemplate->directionalAmbientLightingColors));
}

void Environment::LightingTemplateItem::RestoreOriginalData()
{
	memcpy(&base->data, &originalData, sizeof(originalData));
	memcpy(
		&base->directionalAmbientLightingColors,
		&originalDirectionalAmbientLightingColors,
		sizeof(originalDirectionalAmbientLightingColors));
}
