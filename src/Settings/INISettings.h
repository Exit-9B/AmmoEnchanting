#pragma once

namespace Settings
{
	class INISettings final
	{
	public:
		static INISettings* GetSingleton();

		~INISettings() = default;
		INISettings(const INISettings&) = delete;
		INISettings(INISettings&&) = delete;
		INISettings& operator=(const INISettings&) = delete;
		INISettings& operator=(INISettings&&) = delete;

		void LoadSettings();

		float fAmmoChargeMult;
		float fAmmoEffectCostMult;

	private:
		INISettings() = default;
	};
}
