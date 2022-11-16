#pragma once

namespace Hooks
{
	class Disenchant
	{
	public:
		Disenchant() = delete;

		static void Install();

	private:
		// Save explosion data when learning enchantment from Ammo item
		static void GetEnchantmentPatch();

		static RE::EnchantmentItem* GetEnchantment(RE::InventoryEntryData* a_entry);

		inline static REL::Relocation<decltype(&GetEnchantment)> _GetEnchantment;
	};
}
