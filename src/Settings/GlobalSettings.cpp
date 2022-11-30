#include "GlobalSettings.h"

namespace Settings
{
	GlobalSettings* GlobalSettings::GetSingleton()
	{
		static GlobalSettings singleton{};
		return &singleton;
	}

	bool GlobalSettings::AmmoEnchantingEnabled()
	{
		if (auto global = RE::TESForm::LookupByEditorID<RE::TESGlobal>("AmmoEnchantingEnabled")) {
			return global->value;
		}

		return true;
	}

	bool GlobalSettings::AmmoEnchantingAllowDisenchant()
	{
		if (auto global = RE::TESForm::LookupByEditorID<RE::TESGlobal>(
				"AmmoEnchantingAllowDisenchant")) {

			return global->value;
		}

		return true;
	}
}
