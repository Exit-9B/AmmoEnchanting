#include "INISettings.h"

#include <SimpleIni.h>

namespace Settings
{
	INISettings* INISettings::GetSingleton()
	{
		static INISettings singleton{};
		return &singleton;
	}

	void INISettings::LoadSettings()
	{
		::CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(fmt::format(R"(.\Data\SKSE\Plugins\{}.ini)", Plugin::NAME).c_str());

		fAmmoChargeMult = static_cast<float>(
			ini.GetDoubleValue("AmmoEnchanting", "fAmmoChargeMult", 0.2));

		fAmmoEffectCostMult = static_cast<float>(
			ini.GetDoubleValue("AmmoEnchanting", "fAmmoEffectCostMult", 0.45));
	}
}
