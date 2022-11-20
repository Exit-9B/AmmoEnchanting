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

		static RE::TESEnchantableForm* AsEnchantableForm(
			RE::TESForm* a_inptr,
			std::int32_t a_vfDelta = 0,
			void* a_srcType = REL::Relocation<void*>(RE::TESForm::RTTI).get(),
			void* a_targetType = REL::Relocation<void*>(RE::TESEnchantableForm::RTTI).get(),
			std::int32_t a_isReference = 0);

		static float CalculateCost(
			RE::TESObject* a_object,
			RE::ExtraDataList* a_extraList,
			RE::MagicItem* a_magicItem,
			float a_temperValue);

		inline static REL::Relocation<decltype(&AsEnchantableForm)> _AsEnchantableForm;
	};
}
