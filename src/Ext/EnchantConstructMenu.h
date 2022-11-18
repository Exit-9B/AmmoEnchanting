#pragma once

namespace Ext
{
	namespace EnchantConstructMenu
	{
		using Menu = RE::CraftingSubMenus::EnchantConstructMenu;
		using CategoryListEntry = Menu::CategoryListEntry;
		using EnchantmentEntry = Menu::EnchantmentEntry;
		using ItemChangeEntry = Menu::ItemChangeEntry;

		struct FilterFlag
		{
			FilterFlag() = delete;

			enum FILTER_FLAG : std::uint32_t
			{
				EnchantWeapon = 0x1,
				DisenchantWeapon = 0x2,
				EnchantArmor = 0x4,
				DisenchantArmor = 0x8,
				EffectWeapon = 0x10,
				EffectArmor = 0x20,
				SoulGem = 0x40,

				// Patching is a little easier if Disenchant mask fits in 1 byte
				DisenchantAmmo = 0x80,
				EnchantAmmo = 0x100,
				EffectAmmo = 0x200,

				None = 0x0,
				Disenchant = DisenchantWeapon | DisenchantArmor | DisenchantAmmo,
				Item = EnchantWeapon | EnchantArmor | EnchantAmmo,
				Enchantment = EffectWeapon | EffectArmor | EffectAmmo,

				All = EnchantWeapon | DisenchantWeapon | EnchantArmor | DisenchantArmor |
					EffectWeapon | EffectArmor | SoulGem | EnchantAmmo | DisenchantAmmo |
					EffectAmmo,
			};
		};

		bool CanSelectEntry(Menu* a_menu, std::uint32_t a_entry, bool a_showNotification);

		std::uint16_t GetAmmoEnchantQuantity(Menu* a_menu);
	};
}
