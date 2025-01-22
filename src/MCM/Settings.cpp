#include "MCM/Settings.h"

MCM::Settings MCM::Settings::singleton;

void MCM::Settings::Init()
{
	SKSE::GetPapyrusInterface()->Register(RegisterPapyrus);
	ReadAllSettings();
}

void MCM::Settings::OnConfigClosed(RE::TESQuest*)
{
	singleton.ReadAllSettings();
}

void MCM::Settings::ReadAllSettings()
{
	logger::info("Reading all MCM settings");
	iniSectionMap.clear();
	ReadSettings(iniFilePath);
	ReadSettings(userIniFilePath);
}

MCM::Settings* MCM::Settings::GetSingleton()
{
	return &singleton;
}

bool MCM::Settings::RegisterPapyrus(RE::BSScript::IVirtualMachine* a_vm)
{
	if (!a_vm)
	{
		return false;
	}

	a_vm->RegisterFunction("OnConfigClose"sv, "Chiaroscuro_MCM"sv, OnConfigClosed);
	logger::info("Registered {} class", "Chiaroscuro_MCM");

	return true;
}

void MCM::Settings::ReadSettings(const wchar_t* filePath)
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error rc = ini.LoadFile(filePath);
	if (rc < 0)
	{
		logger::error("Ini File not found");
		return;
	}

	CSimpleIniA::TNamesDepend sections;
	ini.GetAllSections(sections);
	for (const auto& section : sections)
	{
		const char* sectionName = section.pItem;

		const CSimpleIniA::TKeyVal* keys = ini.GetSection(sectionName);
		if (!keys)
			continue;

		for (const auto& keyValue : *keys)
		{
			const char* key   = keyValue.first.pItem;
			const char* value = keyValue.second;

			logger::trace("Reading Setting {} {} {}", sectionName, key, value);

			iniSectionMap[sectionName][key] = std::string(value);
		}
	}

}
