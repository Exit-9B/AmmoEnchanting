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
		// Add extended data to ammo entries
		static void ItemDataPatch();

		static Menu* SetupSubMenu(
			Menu* a_subMenu,
			RE::GPtr<RE::GFxMovieView>& a_view,
			RE::TESFurniture* a_furniture);

		static void OverrideConfig(RE::GFxValue& a_config);

		static void SetItemData(
			Menu::CategoryListEntry* a_entry,
			RE::GFxValue* a_dataContainer);

		inline static REL::Relocation<decltype(&SetupSubMenu)> _EnchantConstructMenu_ctor;
	};
}
