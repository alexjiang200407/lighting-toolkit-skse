#include "Environment/Environment.h"
#include "MCM/Settings.h"

std::string GetEditorID(const RE::TESForm* form)
{
	assert(form);

	using _GetFormEditorID = const char* (*)(std::uint32_t);

	static auto tweaks = GetModuleHandleW(L"po3_Tweaks");
	static auto function =
		reinterpret_cast<_GetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID"));
	if (function)
	{
		return function(form->formID);
	}
	return fmt::format("0x{:X}", form->formID);
}

void Environment::OnDataLoaded()
{
	auto& allWeathers = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESWeather>();

	weathers.clear();
	std::transform(
		allWeathers.begin(),
		allWeathers.end(),
		std::back_inserter(weathers),
		[](RE::TESWeather* weather) { return WeatherItem{ weather }; });
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
	if (oldWeather)
	{
		if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton())
			taskQueueInterface->QueueForceWeather(oldWeather->tesWeather, true);
		else
			logger::error("Could not get task queue interface to force weather");
	}
	if (currentWeather)
		currentWeather->RestoreOriginalData();
	currentWeather.reset();
	oldWeather.reset();

	auto* sky = RE::Sky::GetSingleton();

	if (!sky)
	{
		logger::error("Could not find sky");
		return;
	}

	if (auto* sun = sky->sun; sun && sun->light)
	{
		sun->light->fade = initialSunIntensity;
	}
	else
	{
		logger::error("Sun not found");
	}
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
	assert(currentWeather.has_value() && currentWeather->tesWeather);

	using ColorTimes = RE::TESWeather::ColorTimes;

	ImGui::PushID(label);
	if (ImGui::CollapsingHeader(label))
	{
		{
			ColorEditor4("Day", currentWeather->tesWeather->colorData[colorType][ColorTimes::kDay]);
			ColorEditor4(
				"Night",
				currentWeather->tesWeather->colorData[colorType][ColorTimes::kNight]);
			ColorEditor4(
				"Sunrise",
				currentWeather->tesWeather->colorData[colorType][ColorTimes::kSunrise]);
			ColorEditor4(
				"Sunset",
				currentWeather->tesWeather->colorData[colorType][ColorTimes::kSunset]);
		}
	}
	ImGui::PopID();
}

void Environment::DrawWeatherControl()
{
	using MCM        = MCM::Settings;
	using ColorTypes = RE::TESWeather::ColorTypes;

	if (ImGui::BeginPanel("##WeatherControl"))
	{
		bool changedWeather = false;
		if (ImGui::BeginCombo("Weather", currentWeather ? currentWeather->GetName() : "None"))
		{
			for (const auto& weather : weathers)
			{
				bool isSelected = currentWeather && weather == *currentWeather;
				if (ImGui::Selectable(weather.GetName(), &isSelected))
				{
					changedWeather = currentWeather != weather;
					if (changedWeather && currentWeather)
					{
						if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton())
						{
							taskQueueInterface->QueueForceWeather(weather.tesWeather, true);
							currentWeather->RestoreOriginalData();
							currentWeather = weather.tesWeather;
						}
						else
							logger::error("Could not get task queue interface to force weather");
					}
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
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
			ImGui::NiPointEditor(
				"Sun Light Direction x",
				"Sun Light Direction y",
				"Sun Light Direction z",
				sun->light->worldDir,
				RE::NiPoint3(-1000.0f, -1000.0f, -1000.0f),
				RE::NiPoint3(1000.0f, 1000.0f, 1000.0f));
		}
		ImGui::EndPanel();

		ImGui::BeginDisabled(!currentWeather.has_value());
		{
			if (ImGui::Button("Reset Weather"))
			{
				currentWeather->RestoreOriginalData();
				sun->light->fade = initialSunIntensity;
			}
		}
		ImGui::EndDisabled();
	}
}

Environment::WeatherItem::WeatherItem(RE::TESWeather* weather) :
	tesWeather(weather),
	weatherDisplayName(fmt::format("{}##{}", GetEditorID(weather), weather->formID)),
	originalData(weather->data)
{
	memcpy(oldColorData, weather->colorData, sizeof(oldColorData));
}

bool Environment::WeatherItem::operator==(const RE::TESWeather* weather) const
{
	return this->tesWeather == weather;
}

bool Environment::WeatherItem::operator==(const WeatherItem weather) const
{
	return *this == weather.tesWeather;
}

const char* Environment::WeatherItem::GetName() const { return weatherDisplayName.c_str(); }

void Environment::WeatherItem::RestoreOriginalData()
{
	tesWeather->data = originalData;
	memcpy(tesWeather->colorData, oldColorData, sizeof(oldColorData));
}

inline RE::TESWeather::Data& Environment::WeatherItem::GetData() { return tesWeather->data; }
