#pragma once

namespace Settings
{
	class INISettings
	{
	public:
		static INISettings* GetSingleton();

		void LoadSettings();

		float fAmmoChargeMult;
		float fAmmoEffectCostMult;
	};
}
