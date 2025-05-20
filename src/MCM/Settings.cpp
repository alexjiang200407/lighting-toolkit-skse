#include "Settings.h"
#include <SimpleIni.h>

MCM::Settings MCM::Settings::singleton;

static constexpr std::string_view iniFilePathJoin[]     = { "Data/MCM/Config/",
	                                                        Version::PROJECT,
	                                                        "/settings.ini" };
static constexpr std::string_view userIniFilePathJoin[] = { "Data/MCM/Settings/",
	                                                        Version::PROJECT,
	                                                        ".ini" };
static constexpr std::string_view MCMFileJoin[]         = { Version::PROJECT, "_MCM" };
static constexpr auto             iniFilePath           = JOIN_V(iniFilePathJoin);
static constexpr auto             userIniFilePath       = JOIN_V(userIniFilePathJoin);
static constexpr auto             MCMScriptID           = JOIN_V(MCMFileJoin);

template <typename T>
using ValueTypeMap = std::unordered_map<std::string, T>;

class MCM::SettingsImpl
{
private:
	enum class Index : size_t
	{
		INT,
		FLOAT,
		UINT,
		BOOL,
		STR,
	};

private:
	template <typename T>
	struct TypeToIndex;

	template <>
	struct TypeToIndex<int>
	{
		static constexpr Index value = Index::INT;
	};
	template <>
	struct TypeToIndex<float>
	{
		static constexpr Index value = Index::FLOAT;
	};
	template <>
	struct TypeToIndex<unsigned int>
	{
		static constexpr Index value = Index::UINT;
	};
	template <>
	struct TypeToIndex<bool>
	{
		static constexpr Index value = Index::BOOL;
	};
	template <>
	struct TypeToIndex<std::string>
	{
		static constexpr Index value = Index::STR;
	};

	template <typename T>
	const ValueTypeMap<T>& GetMap() const
	{
		constexpr size_t idx = static_cast<size_t>(TypeToIndex<T>::value);
		return std::get<idx>(data);
	}

	template <typename T>
	ValueTypeMap<T>& GetMap()
	{
		constexpr size_t idx = static_cast<size_t>(TypeToIndex<T>::value);
		return std::get<idx>(data);
	}

public:
	template <typename T>
	bool Get(std::string_view a_section, std::string_view a_key, T& a_buf) const
	{
		std::string fullKey = std::string(a_section) + "#" + std::string(a_key);
		auto&       map     = GetMap<T>();

		if (auto it = map.find(fullKey); it != map.end())
		{
			a_buf = it->second;
			return true;
		}
		return false;
	}

	template <typename T>
	void Set(std::string_view a_section, std::string_view a_key, const T& a_value)
	{
		std::string fullKey = std::string(a_section) + "#" + std::string(a_key);
		auto&       map     = GetMap<T>();
		map[fullKey]        = a_value;
	}

	void        ReadAllSettings();
	void        ReadSettings(std::string_view filePath, SettingsImpl& newSectionMap);
	static bool RegisterPapyrus(RE::BSScript::IVirtualMachine* a_vm);
	static void OnConfigClosed(RE::TESQuest*) { Settings::singleton.impl->ReadAllSettings(); }

private:
	std::tuple<
		ValueTypeMap<int>,
		ValueTypeMap<float>,
		ValueTypeMap<unsigned int>,
		ValueTypeMap<bool>,
		ValueTypeMap<std::string>>
		data;
};

void MCM::Settings::Init()
{
	SKSE::GetPapyrusInterface()->Register(impl->RegisterPapyrus);
	impl->ReadAllSettings();
}

void MCM::SettingsImpl::ReadAllSettings()
{
	logger::info("Reading all MCM settings");

	auto newSectionMap = std::make_shared<SettingsImpl>();
	Settings::singleton.impl->ReadSettings(iniFilePath.data(), *newSectionMap);
	Settings::singleton.impl->ReadSettings(userIniFilePath.data(), *newSectionMap);

	Settings::singleton.impl = std::move(newSectionMap);
}

MCM::Settings* MCM::Settings::GetSingleton() { return &singleton; }

bool MCM::Settings::Get(const char* a_section, const char* a_key, int& a_buf) const
{
	return impl->Get(a_section, a_key, a_buf);
}

bool MCM::Settings::Get(const char* a_section, const char* a_key, float& a_buf) const
{
	return impl->Get(a_section, a_key, a_buf);
}

bool MCM::Settings::Get(const char* a_section, const char* a_key, std::string& a_buf) const
{
	return impl->Get(a_section, a_key, a_buf);
}

bool MCM::Settings::Get(const char* a_section, const char* a_key, unsigned int& a_buf) const
{
	return impl->Get(a_section, a_key, a_buf);
}

bool MCM::Settings::Get(const char* a_section, const char* a_key, bool& a_buf) const
{
	return impl->Get(a_section, a_key, a_buf);
}

bool MCM::SettingsImpl::RegisterPapyrus(RE::BSScript::IVirtualMachine* a_vm)
{
	if (!a_vm)
	{
		return false;
	}
	a_vm->RegisterFunction("OnConfigClose"sv, MCMScriptID, OnConfigClosed);
	return true;
}

void MCM::SettingsImpl::ReadSettings(std::string_view filePath, SettingsImpl& settings)
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error rc = ini.LoadFile(filePath.data());
	if (rc < 0)
	{
		logger::error("Ini File {} not found", filePath);
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
			const char* key        = keyValue.first.pItem;
			const char* valueAsStr = keyValue.second;

			if (!key || key[0] == 0)
				continue;

			logger::trace("Reading Setting {} {} {}", sectionName, key, valueAsStr);

			if (key[0] == 'b')
			{
				bool value;
				if (!parseStr(valueAsStr, value))
					continue;
				settings.Set(sectionName, key, value);
			}
			else if (key[0] == 'i')
			{
				int value;
				if (!parseStr(valueAsStr, value))
					continue;
				settings.Set(sectionName, key, value);
			}
			else if (key[0] == 'u')
			{
				unsigned int value;
				if (!parseStr(valueAsStr, value))
					continue;
				settings.Set(sectionName, key, value);
			}
			else if (key[0] == 'f')
			{
				float value;
				if (!parseStr(valueAsStr, value))
					continue;
				settings.Set(sectionName, key, value);
			}
			else if (key[0] == 's')
			{
				settings.Set(sectionName, key, std::string(valueAsStr));
			}
			else
			{
				logger::warn("Invalid Setting a_key format {}", key);
			}
		}
	}
}
