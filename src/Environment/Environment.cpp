#include "Environment/Environment.h"
#include "MCM/Settings.h"

static void
	GetCurrentImageSpace(RE::TESImageSpace*& imageSpace, RE::ExtraCellImageSpace*& extraImageSpace);

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
		DrawImageSpaceControl();
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
		oldWeather.emplace(skyWeather);
	}

	if (auto* sun = sky->sun; sun && sun->light)
	{
		initialSunIntensity = sun->light->fade;
	}
	else
	{
		logger::error("Sun not found");
	}

	if (auto* imageSpaceManager = RE::ImageSpaceManager::GetSingleton())
	{
		oldImageSpace = imageSpaceManager->currentBaseData;
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

	if (oldImageSpace)
	{
		RE::ImageSpaceManager::GetSingleton()->currentBaseData = oldImageSpace;
	}

	oldWeather.reset();
	oldImageSpace = nullptr;
}

template <typename T>
static bool ColorEditor4(const char* label, T& color, ImGuiColorEditFlags flags = 0)
{
	float colors[4] = { color.red / 255.0f,
		                color.green / 255.0f,
		                color.blue / 255.0f,
		                color.alpha / 255.0f };

	ImGui::PushFullItemWidth(label);
	bool retVal = ImGui::ColorEdit4(label, colors, flags);
	ImGui::PopItemWidth();

	color.red   = static_cast<uint8_t>(std::clamp(colors[0] * 255.0f, 0.0f, 255.0f));
	color.green = static_cast<uint8_t>(std::clamp(colors[1] * 255.0f, 0.0f, 255.0f));
	color.blue  = static_cast<uint8_t>(std::clamp(colors[2] * 255.0f, 0.0f, 255.0f));
	color.alpha = static_cast<uint8_t>(std::clamp(colors[3] * 255.0f, 0.0f, 255.0f));
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
		ColorEditor4("Day", currentWeather.base->colorData[colorType][ColorTimes::kDay]);
		ColorEditor4("Night", currentWeather.base->colorData[colorType][ColorTimes::kNight]);
		ColorEditor4("Sunrise", currentWeather.base->colorData[colorType][ColorTimes::kSunrise]);
		ColorEditor4("Sunset", currentWeather.base->colorData[colorType][ColorTimes::kSunset]);
	}
	ImGui::PopID();
}

void Environment::Restore(std::optional<WeatherItem> currentWeather)
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

	if (auto* cell = RE::PlayerCharacter::GetSingleton()->parentCell; cell && oldImageSpace)
	{
		RE::TESImageSpace*       imageSpace;
		RE::ExtraCellImageSpace* extraImageSpace;
		GetCurrentImageSpace(imageSpace, extraImageSpace);
		if (extraImageSpace)
			cell->extraList.Remove(extraImageSpace);

		if (oldImageSpace)
			RE::ImageSpaceManager::GetSingleton()->currentBaseData = oldImageSpace;
	}
}

template <typename T, bool HAS_NONE = false>
void TESFormComboBox(
	const char*           label,
	std::optional<T>&     current,
	const std::vector<T>& allForms,
	std::function<
		bool(std::optional<T>&, std::conditional_t<HAS_NONE, const std::optional<T>&, const T&>)>
		handleChanged)
{
	ImGui::PushFullItemWidth(label);
	if (ImGui::BeginCombo(label, current ? current->GetName() : "None"))
	{
		if constexpr (HAS_NONE)
		{
			bool isSelected = current == std::nullopt;
			if (ImGui::Selectable("None", &isSelected))
			{
				if (current != std::nullopt)
				{
					if (handleChanged(current, std::nullopt))
					{
						current.reset();
					}
				}
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		for (auto& form : allForms)
		{
			bool isSelected = current && form == *current;
			if (ImGui::Selectable(form.GetName(), &isSelected))
			{
				if (current != form)
				{
					if (handleChanged(current, form))
					{
						current.emplace(form.base);
					}
				}
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}

static void
	GetCurrentImageSpace(RE::TESImageSpace*& imageSpace, RE::ExtraCellImageSpace*& extraImageSpace)
{
	imageSpace      = nullptr;
	extraImageSpace = nullptr;

	const auto* player = RE::PlayerCharacter::GetSingleton();
	if (auto* cell = player->GetParentCell())
	{
		extraImageSpace = static_cast<RE::ExtraCellImageSpace*>(
			cell->extraList.GetByType(RE::ExtraDataType::kCellImageSpace));
		if (extraImageSpace != nullptr)
		{
			imageSpace = extraImageSpace->imageSpace;
		}
	}
}

void Environment::DrawImageSpaceControl()
{
	if (ImGui::BeginPanel("Image Space Editor"))
	{
		RE::TESImageSpace*            imageSpace      = nullptr;
		RE::ExtraCellImageSpace*      extraImageSpace = nullptr;
		std::optional<ImageSpaceItem> currentImageSpace{};

		GetCurrentImageSpace(imageSpace, extraImageSpace);
		if (imageSpace)
			currentImageSpace.emplace(imageSpace);

		TESFormComboBox<ImageSpaceItem, true>(
			"Image Space",
			currentImageSpace,
			imageSpaces,
			[&extraImageSpace, this](auto&, const auto& newImageSpace) {
				const auto* player = RE::PlayerCharacter::GetSingleton();

				auto* cell = player->GetParentCell();

				if (!cell)
				{
					logger::warn("Could not get player parent cell");
					return false;
				}

				if (newImageSpace && newImageSpace->base)
				{
					if (extraImageSpace)
					{
						extraImageSpace->imageSpace = newImageSpace->base;
					}
					else
					{
						extraImageSpace =
							RE::ExtraCellImageSpace::Create<RE::ExtraCellImageSpace>();
						extraImageSpace->imageSpace = newImageSpace->base;
						cell->extraList.Add(extraImageSpace);
					}

					RE::ImageSpaceManager::GetSingleton()->currentBaseData =
						&newImageSpace->base->data;
				}
				else if (extraImageSpace)
				{
					cell->extraList.Remove(extraImageSpace);
					if (oldImageSpace)
						RE::ImageSpaceManager::GetSingleton()->currentBaseData = oldImageSpace;
				}

				return true;
			});
	}
	ImGui::EndPanel();
}

void Environment::DrawWeatherControl()
{
	using MCM        = MCM::Settings;
	using ColorTypes = RE::TESWeather::ColorTypes;

	std::optional<WeatherItem> currentWeather{};

	if (auto* sky = RE::Sky::GetSingleton())
	{
		currentWeather.emplace(sky->currentWeather);
	}

	if (ImGui::BeginPanel("##WeatherControl"))
	{
		TESFormComboBox<WeatherItem>(
			"Weather",
			currentWeather,
			weathers,
			[](std::optional<WeatherItem>& oldWeather, const WeatherItem& newWeather) {
				if (auto* taskQueueInterface = RE::TaskQueueInterface::GetSingleton())
				{
					taskQueueInterface->QueueForceWeather(newWeather.base, true);
					if (oldWeather)
						oldWeather->RestoreOriginalData();
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
			auto* player = RE::PlayerCharacter::GetSingleton();
			ImGui::BeginDisabled(player->parentCell && player->parentCell->IsInteriorCell());
			ImGui::SliderAutoFill("Sun Intensity", &sun->light->fade, 0.0f, sunMaxIntensity);
			if (currentWeather)
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

Environment::ImageSpaceItem::ImageSpaceItem(RE::TESImageSpace* imageSpace) :
	BaseFormItem<RE::TESImageSpace>(imageSpace)
{}
