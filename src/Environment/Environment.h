#pragma once
#include "ColorPalette.h"
#include "Preset/PresetDatabase.h"

class Environment
{
public:
	void OnDataLoaded();
	void DrawWindow();
	void OnMenuOpened();
	void OnMenuClosed();

private:
	template <typename T>
	struct BaseFormItem
	{
		ASSERT_BASE(T, RE::TESForm);

		T*          base;
		std::string displayName;

		BaseFormItem(T* item) :
			base(item), displayName(fmt::format("{}##{}", GetEditorID(item), item->formID)){};

		template <typename U, typename F>
		static void GetAllFormsOfType(std::vector<U>& allForms, F create)
		{
			auto& formArray = RE::TESDataHandler::GetSingleton()->GetFormArray<T>();

			allForms.clear();
			std::transform(
				formArray.begin(),
				formArray.end(),
				std::back_inserter(allForms),
				create);

			std::sort(allForms.begin(), allForms.end());
		}
		const char* GetName() const { return displayName.c_str(); };

		bool operator<(const BaseFormItem<T>& rhs) const { return base->formID < rhs.base->formID; }
		bool operator==(const T* rhs) const { return base->formID == rhs->formID; }
		bool operator==(const BaseFormItem<T> rhs) const { return *this == rhs.base; }
		bool operator!=(const BaseFormItem<T> rhs) const { return !(*this == rhs); }
	};

	struct WeatherItem : public BaseFormItem<RE::TESWeather>
	{
		using ColorTypes = RE::TESWeather::ColorTypes;
		using ColorTime  = RE::TESWeather::ColorTime;

		using BaseFormItem<RE::TESWeather>::operator==;

		RE::TESWeather::Data       originalData;
		RE::Color                  oldColorData[ColorTypes::kTotal][ColorTime::kTotal];
		RE::BGSVolumetricLighting* originalVolumetricLighting[ColorTime::kTotal];

		WeatherItem(RE::TESWeather* weather);

		void                                       RestoreOriginalData(Environment& environment);
		typedef std::vector<WeatherItem>::iterator iterator;
	};

	struct VolumetricLightingItem : public BaseFormItem<RE::BGSVolumetricLighting>
	{
		using ColorTime = RE::TESWeather::ColorTime;

		VolumetricLightingItem(RE::BGSVolumetricLighting* volumetricLighting);

		void RestoreOriginal();

		RE::BSVolumetricLightingRenderData                    originalVolumetricLightingData;
		typedef std::vector<VolumetricLightingItem>::iterator iterator;
	};

private:
	template <typename T, typename U>
	std::vector<U>::iterator FindCurrent(T* current, std::vector<U>& all)
	{
		ASSERT_BASE(U, Environment::BaseFormItem<T>);

		if (!current)
			return all.end();

		auto it = std::lower_bound(all.begin(), all.end(), current, [](const U& lhs, const T* rhs) {
			return lhs.base->formID < rhs->formID;
		});
		if (it != all.end() && *it == current)
		{
			return it;
		}

		return all.end();
	}

	void DrawVolumetricLightingEditor(const char* label, RE::BGSVolumetricLighting*& lighting);
	void DrawWeatherControl();
	void DrawColorDataEditor(const char* label, size_t colorType, WeatherItem currentWeather);
	void Restore();
	WeatherItem::iterator GetCurrentWeather();
	VolumetricLightingItem::iterator
		GetCurrentVolumetricLighting(RE::BGSVolumetricLighting* lighting);

private:
	float                               initialSunExtreme[2] = { 0.0f, 0.0f };
	float                               initialSunIntensity;
	std::vector<WeatherItem>            weathers;
	std::vector<VolumetricLightingItem> volumetricLighting;
	std::optional<WeatherItem>          originalWeather;
};
