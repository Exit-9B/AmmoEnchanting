#pragma once

namespace Hooks
{
	class Misc
	{
	public:
		Misc() = delete;

		static void Install();

	private:
		// Makes the game engine recognize ExtraEnchantment data on Ammo items
		static void GetEnchantmentPatch();
		// Scale the gold value of enchanted ammo
		static void GoldValuePatch();
		// Handle extra enchantment data failing to find temporary forms
		static void LoadGamePatch();

		static RE::TESEnchantableForm* AsEnchantableForm(RE::TESForm* a_form);

		static float CalculateCost(
			RE::TESObject* a_object,
			RE::ExtraDataList* a_extraList,
			RE::MagicItem* a_magicItem,
			float a_baseValue);

		static RE::EnchantmentItem* LookupEnchantment(
			RE::FormID a_formID,
			RE::ExtraEnchantment* a_exEnchantment);
	};
}
