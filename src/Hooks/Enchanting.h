#pragma once

namespace Hooks
{
	class Enchanting
	{
	public:
		Enchanting() = delete;

		static void Install();

	private:
		// Create enchantments for Ammo items for temporary preview item
		static void ItemPreviewPatch();
		// Create enchantments for Ammo items when crafting real item
		static void EnchantObjectPatch();

		static RE::EnchantmentItem* CreateEnchantment(
			RE::FormType a_formType,
			RE::CraftingSubMenus::EnchantConstructMenu* a_menu);
	};
}
