#include "Environment/Environment.h"
#include "MCM/Settings.h"

template <typename T, bool HAS_NONE = false, typename It = std::vector<T>::iterator>
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

	if constexpr (HAS_NONE)
	{
		if (currentItem == -1)
			currentItem = 0;
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

void Environment::OnDataLoaded()
{
	BaseFormItem<RE::BGSVolumetricLighting>::GetAllFormsOfType<VolumetricLightingItem>(
		volumetricLighting,
		[](RE::BGSVolumetricLighting* volumetricLighting) {
			return VolumetricLightingItem(volumetricLighting);
		});
	BaseFormItem<RE::TESWeather>::GetAllFormsOfType<WeatherItem>(
		weathers,
		[](RE::TESWeather* weather) { return WeatherItem{ weather }; });
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
}

void Environment::OnMenuClosed()
{
	Restore();
	originalWeather.reset();
}

template <bool HAS_ALPHA = true, typename T>
static bool ColorEditor4(const char* label, T* color, ImGuiColorEditFlags flags = 0)
{
	using U = decltype(color->red);
	float colors[4];

	if (color)
	{
		if constexpr (std::is_same_v<U, uint8_t>)
		{
			colors[0] = color->red / 255.0f;
			colors[1] = color->green / 255.0f;
			colors[2] = color->blue / 255.0f;
			if constexpr (HAS_ALPHA)
				colors[3] = color->alpha / 255.0f;
			else
				colors[3] = 0.0f;
		}
		else
		{
			colors[0] = static_cast<float>(color->red);
			colors[1] = static_cast<float>(color->green);
			colors[2] = static_cast<float>(color->blue);
			if constexpr (HAS_ALPHA)
				colors[3] = static_cast<float>(color->alpha);
			else
				colors[3] = 0.0f;
		}
	}
	else
	{
		std::fill(std::begin(colors), std::end(colors), 0.0f);
	}

	ImGui::PushFullItemWidth(label);
	bool retVal;
	if constexpr (HAS_ALPHA)
		retVal = ImGui::ColorEdit4(label, colors, flags);
	else
		retVal = ImGui::ColorEdit3(label, colors, flags);
	ImGui::PopItemWidth();

	if (color)
	{
		if constexpr (std::is_same_v<U, uint8_t>)
		{
			color->red   = static_cast<U>(std::clamp(colors[0] * 255.0f, 0.0f, 255.0f));
			color->green = static_cast<U>(std::clamp(colors[1] * 255.0f, 0.0f, 255.0f));
			color->blue  = static_cast<U>(std::clamp(colors[2] * 255.0f, 0.0f, 255.0f));
			if constexpr (HAS_ALPHA)
				color->alpha = static_cast<U>(std::clamp(colors[3] * 255.0f, 0.0f, 255.0f));
		}
		else
		{
			color->red   = static_cast<U>(colors[0]);
			color->green = static_cast<U>(colors[1]);
			color->blue  = static_cast<U>(colors[2]);
			if constexpr (HAS_ALPHA)
				color->alpha = static_cast<U>(colors[3]);
		}
	}

	return retVal;
}


void Environment::DrawVolumetricLightingEditor(
	const char*                 label,
	RE::BGSVolumetricLighting*& lighting)
{
	using MCM = MCM::Settings;

	auto current = FindCurrent(lighting, volumetricLighting);

	static float minContribution =
		MCM::GetOrDefault_s("Volumetric Lighting", "fMinContribution", 0.0f);
	static float maxContribution =
		MCM::GetOrDefault_s("Volumetric Lighting", "fMaxContribution", 1.0f);
	static float maxIntensity = MCM::GetOrDefault_s("Volumetric Lighting", "fMaxIntensity", 30.0f);
	static float maxDensity   = MCM::GetOrDefault_s("Volumetric Lighting", "fMaxDensity", 10000.0f);
	static float maxFallingSpeed =
		MCM::GetOrDefault_s("Volumetric Lighting", "fMaxFallingSpeed", 100.0f);
	static float minFallingSpeed =
		MCM::GetOrDefault_s("Volumetric Lighting", "fMinFallingSpeed", 0.0f);
	static float minWindSpeed = MCM::GetOrDefault_s("Volumetric Lighting", "fMinWindSpeed", 0.0f);
	static float maxWindSpeed = MCM::GetOrDefault_s("Volumetric Lighting", "fMaxWindSpeed", 100.0f);

	static float minScattering =
		MCM::GetOrDefault_s("Volumetric Lighting", "fMinScattering", -1.0f);
	static float maxScattering = MCM::GetOrDefault_s("Volumetric Lighting", "fMaxScattering", 1.0f);

	static float minRangeFactor =
		MCM::GetOrDefault_s("Volumetric Lighting", "fMinRangeFactor", 0.0f);
	static float maxRangeFactor =
		MCM::GetOrDefault_s("Volumetric Lighting", "fMaxRangeFactor", 80.0f);

	ImGui::PushID(label);

	if (ImGui::CollapsingHeader(label))
	{
		{
			using It = VolumetricLightingItem::iterator;
			TESFormComboBox<VolumetricLightingItem, true, VolumetricLightingItem::iterator>(
				"Preset",
				current,
				volumetricLighting,
				[&](It oldItem, It newItem, It end) {
					if (newItem != end)
					{
						if (oldItem != end)
							oldItem->RestoreOriginal();
						lighting = newItem->base;
					}
					else
					{
						lighting = nullptr;
					}
					return true;
				});
		}

		ImGui::BeginDisabled(!lighting);
		ColorEditor4<false>("Custom Color", lighting);
		ImGui::SliderAutoFill(
			"Custom Color Intensity",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) { return &lighting->intensity; },
			0.0f,
			maxIntensity);
		ImGui::SliderAutoFill(
			"Custom Color Contribution",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) { return &lighting->customColor.contribution; },
			minContribution,
			maxContribution);

		ImGui::SliderAutoFill(
			"Density Contribution",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) { return &lighting->density.contribution; },
			minContribution,
			maxContribution);

		ImGui::SliderAutoFill(
			"Density Size",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) { return &lighting->density.size; },
			0.0f,
			maxDensity,
			nullptr,
			ImGuiSliderFlags_Logarithmic);

		ImGui::SliderAutoFill(
			"Wind Speed",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) { return &lighting->density.windSpeed; },
			minWindSpeed,
			maxWindSpeed);

		ImGui::SliderAutoFill(
			"Falling Speed",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) { return &lighting->density.fallingSpeed; },
			minFallingSpeed,
			maxFallingSpeed);

		ImGui::SliderAutoFill(
			"Phase Function Contribution",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) {
				return &lighting->phaseFunction.contribution;
			},
			minContribution,
			maxContribution);

		ImGui::SliderAutoFill(
			"Phase Function Scattering",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) { return &lighting->phaseFunction.scattering; },
			minScattering,
			maxScattering);

		ImGui::SliderAutoFill(
			"Sampling Repartition Range Factor",
			lighting,
			[](RE::BGSVolumetricLighting* lighting) {
				return &lighting->samplingRepartition.rangeFactor;
			},
			minRangeFactor,
			maxRangeFactor);

		ImGui::EndDisabled();
	}
	ImGui::PopID();
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
		currentWeather->RestoreOriginalData(*this);
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
}

Environment::WeatherItem::iterator Environment::GetCurrentWeather()
{
	if (auto* sky = RE::Sky::GetSingleton(); sky && sky->currentWeather)
	{
		return FindCurrent(sky->currentWeather, weathers);
	}

	return weathers.end();
}

Environment::VolumetricLightingItem::iterator
	Environment::GetCurrentVolumetricLighting(RE::BGSVolumetricLighting* lighting)
{
	if (!lighting)
		return volumetricLighting.end();

	return FindCurrent(lighting, volumetricLighting);
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
			[this](auto oldWeather, auto newWeather, auto end) {
				if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton();
			        taskQueueInterface && newWeather != end)
				{
					taskQueueInterface->QueueForceWeather(newWeather->base, true);
					if (oldWeather != end)
						oldWeather->RestoreOriginalData(*this);
					return true;
				}
				return false;
			});
		if (currentWeather != weathers.end())
		{
			ImGui::SliderAutoFill(
				"Wind Speed",
				&currentWeather->base->data.windSpeed,
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

	if (ImGui::BeginPanel("##VolumetricLighting") && currentWeather != weathers.end())
	{
		using ColorTimes = RE::TESWeather::ColorTimes;

		auto* weather = currentWeather->base;

		ImGui::Text("Volumetric Lighting");

		if (weather)
		{
			DrawVolumetricLightingEditor("Day", weather->volumetricLighting[ColorTimes::kDay]);
			DrawVolumetricLightingEditor("Night", weather->volumetricLighting[ColorTimes::kNight]);
			DrawVolumetricLightingEditor(
				"Sunrise",
				weather->volumetricLighting[ColorTimes::kSunrise]);
			DrawVolumetricLightingEditor(
				"Sunset",
				weather->volumetricLighting[ColorTimes::kSunset]);
		}
	}
	ImGui::EndPanel();
}

Environment::WeatherItem::WeatherItem(RE::TESWeather* weather) :
	BaseFormItem<RE::TESWeather>(weather), originalData(weather->data)
{
	memcpy(oldColorData, weather->colorData, sizeof(oldColorData));
	memcpy(
		originalVolumetricLighting,
		weather->volumetricLighting,
		sizeof(originalVolumetricLighting));
}

static void CopyVolumetricLightingRenderData(
	RE::BGSVolumetricLighting*                dst,
	const RE::BSVolumetricLightingRenderData& src)
{
	dst->red                 = src.red;
	dst->green               = src.green;
	dst->blue                = src.blue;
	dst->density             = src.density;
	dst->intensity           = src.intensity;
	dst->customColor         = src.customColor;
	dst->phaseFunction       = src.phaseFunction;
	dst->samplingRepartition = src.samplingRepartition;
}

void Environment::WeatherItem::RestoreOriginalData(Environment& environment)
{
	base->data = originalData;
	memcpy(base->colorData, oldColorData, sizeof(oldColorData));

	for (auto* v : base->volumetricLighting)
	{
		if (auto current = environment.GetCurrentVolumetricLighting(v);
		    current != environment.volumetricLighting.end())
		{
			current->RestoreOriginal();
		}
	}

	memcpy(
		base->volumetricLighting,
		originalVolumetricLighting,
		sizeof(originalVolumetricLighting));
}

Environment::VolumetricLightingItem::VolumetricLightingItem(
	RE::BGSVolumetricLighting* volumetricLighting) :
	BaseFormItem<RE::BGSVolumetricLighting>(volumetricLighting)
{
	originalVolumetricLightingData =
		static_cast<RE::BSVolumetricLightingRenderData>(*volumetricLighting);
}

void Environment::VolumetricLightingItem::RestoreOriginal()
{
	CopyVolumetricLightingRenderData(base, originalVolumetricLightingData);
}
