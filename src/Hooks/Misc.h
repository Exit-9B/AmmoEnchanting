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

		static RE::TESEnchantableForm* AsEnchantableForm(
			RE::TESForm* a_inptr,
			std::int32_t a_vfDelta = 0,
			void* a_srcType = REL::Relocation<void*>(RE::TESForm::RTTI).get(),
			void* a_targetType = REL::Relocation<void*>(RE::TESEnchantableForm::RTTI).get(),
			std::int32_t a_isReference = 0);

		inline static REL::Relocation<decltype(&AsEnchantableForm)> _AsEnchantableForm;
	};
}
