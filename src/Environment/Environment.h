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

		T*                base;
		const std::string weatherDisplayName;

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
		}
		const char* GetName() const { return weatherDisplayName.c_str(); };

		bool operator==(const T* rhs) const { return base == rhs; }
		bool operator==(const BaseFormItem<T> rhs) const { return *this == rhs.base; }
		bool operator!=(const BaseFormItem<T> rhs) const { return !(*this == rhs); }
	};

	struct ImageSpaceItem : public BaseFormItem<RE::TESImageSpace>
	{
		ImageSpaceItem(RE::TESImageSpace* imageSpace);

		using BaseFormItem<RE::TESImageSpace>::operator==;
	};

	struct WeatherItem : public BaseFormItem<RE::TESWeather>
	{
		using ColorTypes = RE::TESWeather::ColorTypes;
		using ColorTime  = RE::TESWeather::ColorTime;

		using BaseFormItem<RE::TESWeather>::operator==;

		RE::TESWeather::Data originalData;
		RE::Color            oldColorData[ColorTypes::kTotal][ColorTime::kTotal];

		WeatherItem(RE::TESWeather* weather);

		void                  RestoreOriginalData();
		RE::TESWeather::Data& GetData();
	};

private:
	void DrawWeatherControl();
	void DrawImageSpaceControl();
	void DrawColorDataEditor(const char* label, size_t colorType, WeatherItem currentWeather);
	void Restore(std::optional<WeatherItem> currentWeather = std::nullopt);

private:
	float                       initialSunExtreme[2] = { 0.0f, 0.0f };
	float                       initialSunIntensity;
	std::vector<WeatherItem>    weathers;
	std::vector<ImageSpaceItem> imageSpaces;
	std::optional<WeatherItem>  oldWeather;
	RE::ImageSpaceBaseData*     oldImageSpace;
};
