#pragma once

#include "Ext/EnchantConstructMenu.h"

namespace Hooks
{
	class SkyUI
	{
	public:
		using Menu = RE::CraftingSubMenus::EnchantConstructMenu;
		using FilterFlag = Ext::EnchantConstructMenu::FilterFlag;

		SkyUI() = delete;

		static void Install();

	private:
		// Override hardcoded category filters
		static void FilterFlagPatch();
		// Add effect name for disenchanting Ammo items
		static void DisenchantDataPatch();

		static void SetDisenchantItemData(
			Menu::ItemChangeEntry* a_entry,
			RE::GFxValue* a_dataContainer);
	};
}
