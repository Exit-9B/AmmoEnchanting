#pragma once

#include "Ext/EnchantConstructMenu.h"

namespace Hooks
{
	class SkyUI
	{
	public:
		using FilterFlag = Ext::EnchantConstructMenu::FilterFlag;

		SkyUI() = delete;

		static void Install();

	private:
		// Override hardcoded category filters
		static void FilterFlagPatch();
		// Add effect name for disenchanting Ammo items
		static void DisenchantDataPatch();

		static void SetDisenchantItemData(
			RE::CraftingSubMenus::EnchantConstructMenu::ItemChangeEntry* a_entry,
			RE::GFxValue* a_dataContainer);
	};
}
