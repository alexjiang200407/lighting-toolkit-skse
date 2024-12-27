

void InitializeLog()
{
	auto path = logger::log_directory();
	if (!path) {
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
	v.PluginName("powerofthree's Papyrus Extender");
	v.AuthorName("powerofthree");
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();
#else
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "CineStudio";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
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
	) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}
#endif

void UpdateLight(RE::TESObjectLIGH* a_light, const RE::NiPointer<RE::NiPointLight>& a_ptLight, RE::TESObjectREFR* a_ref, float a_wantDimmer)
{
	using func_t = decltype(&UpdateLight);
	static REL::Relocation<func_t> func{ RELOCATION_ID(17212, 17614) };
	func(a_light, a_ptLight, a_ref, a_wantDimmer);
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	InitializeLog();

	logger::info("Game version : {}", a_skse->RuntimeVersion().string());

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
		if (message->type == SKSE::MessagingInterface::kDataLoaded) {
			RE::ConsoleLog::GetSingleton()->Print("CineStudio has been loaded");
		}
		if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
			// ALTERNATE START TORCH REFID 0x22073cac
			auto*                                  torch = RE::TESForm::LookupByID(0xff000d4e)->As<RE::TESObjectREFR>();
			static RE::NiPointer<RE::NiPointLight> pointLight(RE::NiPointLight::Create());
			auto*                                  baseLight = reinterpret_cast<RE::TESObjectLIGH*>(torch->GetBaseObject());
			pointLight->diffuse.red = 0;
			pointLight->diffuse.blue = 255;
			pointLight->diffuse.green = 0;
			pointLight->radius = RE::NiPoint3(500.0f,500.0f,500.0f);
			UpdateLight(baseLight, pointLight, torch, false);
		}
	});

	return true;
}
