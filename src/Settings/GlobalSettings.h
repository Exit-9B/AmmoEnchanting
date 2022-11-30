#pragma once

namespace Settings
{
	class GlobalSettings final
	{
	public:
		static GlobalSettings* GetSingleton();

		~GlobalSettings() = default;
		GlobalSettings(const GlobalSettings&) = delete;
		GlobalSettings(GlobalSettings&&) = delete;
		GlobalSettings& operator=(const GlobalSettings&) = delete;
		GlobalSettings& operator=(GlobalSettings&&) = delete;

		bool AmmoEnchantingEnabled();

		bool AmmoEnchantingAllowDisenchant();

	private:
		GlobalSettings() = default;
	};
}
