#pragma once
#include <SimpleIni.h>

namespace MCM
{
	class Settings
	{
	public:
		void             Init();
		void             ReadAllSettings();
		static Settings* GetSingleton();


		template <typename T>
		bool Get(const char* section, const char* key, T& buf)
		{
			auto sectionIt = iniSectionMap.find(section);
			if (sectionIt == iniSectionMap.end())
			{
				return false;
			}

			auto keyIt = sectionIt->second.find(key);
			if (keyIt == sectionIt->second.end())
			{
				return false;
			}

			std::istringstream iss(keyIt->second);
			iss >> buf;

			return !iss.fail() && iss.eof();
		}

	private:
		Settings() = default;

		void        ReadSettings(const wchar_t* filePath);
		static void OnConfigClosed(RE::TESQuest*);
		static bool RegisterPapyrus(RE::BSScript::IVirtualMachine* a_vm);

	private:
		typedef std::unordered_map<std::string, std::string> KeyValue;
		typedef std::unordered_map<std::string, KeyValue>    SectionMap;

	private:
		SectionMap                      iniSectionMap;
		static constexpr const wchar_t* iniFilePath     = L"Data/MCM/Config/Chiaroscuro/settings.ini";
		static constexpr const wchar_t* userIniFilePath = L"Data/MCM/Settings/Chiaroscuro.ini";
		static Settings                 singleton;
	};
}
