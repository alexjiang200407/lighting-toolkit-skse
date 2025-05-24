#include "Environment/Environment.h"
#include "MCM/Settings.h"

void Environment::OnDataLoaded()
{
	BaseFormItem<RE::TESWeather>::GetAllFormsOfType<WeatherItem>(
		weathers,
		[](RE::TESWeather* weather) { return WeatherItem{ weather }; });

	BaseFormItem<RE::TESImageSpace>::GetAllFormsOfType<ImageSpaceItem>(
		imageSpaces,
		[](RE::TESImageSpace* image) { return ImageSpaceItem{ image }; });
}

static float* GetSunXExtreme()
{
	static auto* sunX = REL::Relocation<float*>{ RELOCATION_ID(370250, 370250) }.get();
	return sunX;
}

static float* GetSunYExtreme()
{
	static auto* sunY = REL::Relocation<float*>{ RELOCATION_ID(370253, 370253) }.get();
	return sunY;
}

static float* GetSunZExtreme()
{
	static auto* sunZ = REL::Relocation<float*>{ RELOCATION_ID(370256, 370256) }.get();
	return sunZ;
}

void Environment::DrawWindow()
{
	ImGui::PushID("environment");
	{
		DrawWeatherControl();
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
		currentWeather = WeatherItem(skyWeather);
		oldWeather     = currentWeather;
	}

	if (auto* sun = sky->sun; sun && sun->light)
	{
		initialSunIntensity = sun->light->fade;
	}
	else
	{
		logger::error("Sun not found");
	}
}

void Environment::OnMenuClosed()
{
	Restore();
	if (oldWeather)
	{
		if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton())
			taskQueueInterface->QueueForceWeather(oldWeather->base, true);
		else
			logger::error("Could not get task queue interface to force weather");
	}
	currentWeather.reset();
	oldWeather.reset();
}

template <typename T>
static bool ColorEditor4(const char* label, T& color, ImGuiColorEditFlags flags = 0)
{
	float colors[4] = { color.red / 255.0f,
		                color.green / 255.0f,
		                color.blue / 255.0f,
		                color.alpha / 255.0f };
	bool  retVal    = ImGui::ColorEdit4(label, colors, flags);
	color.red       = static_cast<uint8_t>(std::clamp(colors[0] * 255.0f, 0.0f, 255.0f));
	color.green     = static_cast<uint8_t>(std::clamp(colors[1] * 255.0f, 0.0f, 255.0f));
	color.blue      = static_cast<uint8_t>(std::clamp(colors[2] * 255.0f, 0.0f, 255.0f));
	color.alpha     = static_cast<uint8_t>(std::clamp(colors[3] * 255.0f, 0.0f, 255.0f));

	return retVal;
}

void Environment::DrawColorDataEditor(const char* label, size_t colorType)
{
	assert(currentWeather.has_value() && currentWeather->base);

	using ColorTimes = RE::TESWeather::ColorTimes;

	ImGui::PushID(label);
	if (ImGui::CollapsingHeader(label))
	{
		ColorEditor4("Day", currentWeather->base->colorData[colorType][ColorTimes::kDay]);
		ColorEditor4("Night", currentWeather->base->colorData[colorType][ColorTimes::kNight]);
		ColorEditor4("Sunrise", currentWeather->base->colorData[colorType][ColorTimes::kSunrise]);
		ColorEditor4("Sunset", currentWeather->base->colorData[colorType][ColorTimes::kSunset]);
	}
	ImGui::PopID();
}

void Environment::Restore()
{
	*GetSunXExtreme() = initialSunExtreme[0];
	*GetSunYExtreme() = initialSunExtreme[1];

	if (currentWeather)
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
}

template <typename T>
bool TESFormComboBox(
	const char*                       label,
	std::optional<T>&                 current,
	const std::vector<T>&             allForms,
	std::function<bool(T&, const T&)> handleChanged)
{
	bool changed = false;
	if (ImGui::BeginCombo(label, current ? current->GetName() : "None"))
	{
		for (const auto& form : allForms)
		{
			bool isSelected = current && form == *current;
			if (ImGui::Selectable(form.GetName(), &isSelected))
			{
				changed = current != form;
				if (changed && current)
				{
					if (handleChanged(*current, form))
					{
						current = form.base;
					}
				}
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	return changed;
}

void Environment::DrawWeatherControl()
{
	using MCM        = MCM::Settings;
	using ColorTypes = RE::TESWeather::ColorTypes;

	if (ImGui::BeginPanel("##WeatherControl"))
	{
		bool changedWeather = false;
		TESFormComboBox<WeatherItem>(
			"Weather",
			currentWeather,
			weathers,
			[](WeatherItem& oldWeather, const WeatherItem& newWeather) {
				if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton())
				{
					taskQueueInterface->QueueForceWeather(newWeather.base, true);
					oldWeather.RestoreOriginalData();
					return true;
				}
				logger::error("Could not get task queue interface to force weather");
				return false;
			});
		if (currentWeather.has_value())
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
			ImGui::SliderAutoFill("Intensity", &sun->light->fade, 0.0f, sunMaxIntensity);
			DrawColorDataEditor("Sun Color", ColorTypes::kSun);
			DrawColorDataEditor("Sun Light", ColorTypes::kSunlight);
			DrawColorDataEditor("Sun Glare", ColorTypes::kSunGlare);
			DrawColorDataEditor("Moon Glare", ColorTypes::kMoonGlare);

			float&       sunX         = *GetSunXExtreme();
			float&       sunY         = *GetSunYExtreme();
			const double distance     = std::sqrt((sunX * sunX) + (sunY * sunY));
			float        currentAngle = std::atan2(sunX, sunY);

			if (ImGui::SliderAngle("Sun Rotation", &currentAngle, -180.0f, 179.0f))
			{
				sunX = (float)(std::sin(currentAngle) * distance);
				sunY = (float)(std::cos(currentAngle) * distance);
			}
		}
		ImGui::EndPanel();

		ImGui::BeginDisabled(!currentWeather.has_value());
		{
			if (ImGui::Button("Reset Environment"))
			{
				Restore();
			}
		}
		ImGui::EndDisabled();
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

//bool Environment::WeatherItem::operator==(const WeatherItem& weather) const
//{
//	return *this == weather.base;
//}

void Environment::WeatherItem::RestoreOriginalData()
{
	base->data = originalData;
	memcpy(base->colorData, oldColorData, sizeof(oldColorData));
}

inline RE::TESWeather::Data& Environment::WeatherItem::GetData() { return base->data; }

Environment::ImageSpaceItem::ImageSpaceItem(RE::TESImageSpace* imageSpace) :
	BaseFormItem<RE::TESImageSpace>(imageSpace)
{}
