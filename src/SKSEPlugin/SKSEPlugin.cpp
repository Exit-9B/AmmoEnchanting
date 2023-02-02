#include "Hooks/Disenchant.h"
#include "Hooks/Enchanting.h"
#include "Hooks/FilterFlags.h"
#include "Hooks/Gameplay.h"
#include "Hooks/Misc.h"
#include "Hooks/SkyUI.h"
#include "Hooks/VFX.h"
#include "Papyrus/AmmoEnchanting.h"
#include "Serialization/Serialization.h"
#include "Settings/INISettings.h"

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
{
	SKSE::PluginVersionData v{};

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Parapets"sv);

	v.UsesAddressLibrary(true);

	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(259);

	Hooks::Misc::Install();
	Hooks::Enchanting::Install();
	Hooks::FilterFlags::Install();
	Hooks::Disenchant::Install();
	Hooks::Gameplay::Install();
	Hooks::VFX::Install();
	Hooks::SkyUI::Install();

	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(Serialization::ID);
	serialization->SetSaveCallback(&Serialization::SaveCallback);
	serialization->SetLoadCallback(&Serialization::LoadCallback);
	serialization->SetRevertCallback(&Serialization::RevertCallback);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(&Papyrus::AmmoEnchanting::RegisterFuncs);

	Settings::INISettings::GetSingleton()->LoadSettings();

	return true;
}
