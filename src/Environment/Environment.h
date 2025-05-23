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
	struct WeatherItem
	{
		using ColorTypes = RE::TESWeather::ColorTypes;
		using ColorTime  = RE::TESWeather::ColorTime;

		std::string          weatherDisplayName;
		RE::TESWeather*      tesWeather;
		RE::TESWeather::Data originalData;
		RE::Color            oldColorData[ColorTypes::kTotal][ColorTime::kTotal];

		WeatherItem(RE::TESWeather* weather);

		bool                  operator==(const RE::TESWeather* weather) const;
		bool                  operator==(const WeatherItem weather) const;
		const char*           GetName() const;
		void                  RestoreOriginalData();
		RE::TESWeather::Data& GetData();
		RE::TESWeather::Data& GetColorData();
	};

private:
	void DrawWeatherControl();
	void DrawColorDataEditor(const char* label, size_t colorType);

private:
	float                      initialSunIntensity;
	std::vector<WeatherItem>   weathers;
	std::optional<WeatherItem> currentWeather;
	std::optional<WeatherItem> oldWeather;
};
