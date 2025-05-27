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
		std::string weatherDisplayName;

		BaseFormItem(T* item) :
			base(item),
			weatherDisplayName(fmt::format("{}##{}", GetEditorID(item), item->formID)){};

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
		const char* GetName() const { return weatherDisplayName.c_str(); };

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

		RE::TESWeather::Data               originalData;
		RE::Color                          oldColorData[ColorTypes::kTotal][ColorTime::kTotal];
		RE::BSVolumetricLightingRenderData originalVolumetricLightingData[ColorTime::kTotal];
		bool                               hasVolumetricLightingData[ColorTime::kTotal];

		WeatherItem(RE::TESWeather* weather);

		void                                       RestoreOriginalData();
		typedef std::vector<WeatherItem>::iterator iterator;
	};

	struct LightingTemplateItem : public BaseFormItem<RE::BGSLightingTemplate>
	{
		LightingTemplateItem(RE::BGSLightingTemplate* lightingTemplate);
		using BaseFormItem<RE::BGSLightingTemplate>::operator==;
		void RestoreOriginalData();

		RE::INTERIOR_DATA                       originalData;
		RE::BGSDirectionalAmbientLightingColors originalDirectionalAmbientLightingColors;

		typedef std::vector<LightingTemplateItem>::iterator iterator;
	};

private:
	template <typename T, typename U>
	std::vector<U>::iterator FindCurrent(T* current, std::vector<U>& all)
	{
		ASSERT_BASE(U, Environment::BaseFormItem<T>);

		auto it = std::lower_bound(all.begin(), all.end(), current, [](const U& lhs, const T* rhs) {
			return lhs.base->formID < rhs->formID;
		});
		if (it != all.end() && *it == current)
		{
			return it;
		}

		return all.end();
	}

	void DrawWeatherControl();
	void DrawLightingTemplateControl();
	void DrawColorDataEditor(const char* label, size_t colorType, WeatherItem currentWeather);
	void Restore();
	WeatherItem::iterator          GetCurrentWeather();
	LightingTemplateItem::iterator GetCurrentLightingTemplate(const RE::TESObjectCELL* cell);

private:
	float                             initialSunExtreme[2] = { 0.0f, 0.0f };
	float                             initialSunIntensity;
	std::vector<WeatherItem>          weathers;
	std::vector<LightingTemplateItem> lightingTemplates;
	std::optional<WeatherItem>        originalWeather;
	RE::BGSLightingTemplate*          originalLightingTemplate = nullptr;
};
