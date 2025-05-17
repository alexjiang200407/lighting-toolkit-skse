#include "LightingToolkit.h"
#include "MCM/Settings.h"
#include "SKSE/SerializationControl.h"

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path)
	{
		SKSE::stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	auto logLevel = spdlog::level::trace;

	log->set_level(logLevel);
	log->flush_on(logLevel);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);
}

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Version::MAJOR);
	v.PluginName(Version::PROJECT);
	v.AuthorName("shdowraithe101");
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEAPI
	SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name        = Version::PROJECT;
	a_info->version     = Version::MAJOR;

	if (a_skse->IsEditor())
	{
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver
#	ifndef SKYRIMVR
	    < SKSE::RUNTIME_1_5_39
#	else
	    > SKSE::RUNTIME_VR_1_4_15_1
#	endif
	)
	{
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	InitializeLog();

	logger::info("Game version : {}", a_skse->RuntimeVersion().string());
	LightingToolkit::GetSingleton()->Init();
	MCM::Settings::GetSingleton()->Init();

	SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
		if (message->type == SKSE::MessagingInterface::kDataLoaded)
		{
			RE::ConsoleLog::GetSingleton()->Print("LightingToolkit has been loaded");
			SKSE::SerializationControl::GetSingleton()->Init();
			LightingToolkit::GetSingleton()->OnDataLoaded();
		}
		else if (message->type == SKSE::MessagingInterface::kPostLoadGame)
			LightingToolkit::GetSingleton()->OnSavePostLoaded();
	});

	return true;
}
