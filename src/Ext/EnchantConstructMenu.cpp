#include "EnchantConstructMenu.h"
#include <algorithm>

#undef GetObject

namespace Ext
{
	bool EnchantConstructMenu::CanSelectEntry(
		Menu* a_menu,
		std::uint32_t a_index,
		bool a_showNotification)
	{
		if (a_index >= a_menu->listEntries.size()) {
			return false;
		}

		bool armorCompatible = true;

		auto& entry = a_menu->listEntries[a_index];
		auto filterFlag = entry->filterFlag.underlying();

		if (filterFlag == FilterFlag::EnchantArmor) {
			if (a_menu->selected.effects.empty()) {
				auto obj = static_cast<ItemChangeEntry*>(entry.get())->data->GetObject();

				if (auto armor = obj->As<RE::TESObjectARMO>()) {
					for (auto& effect : a_menu->selected.effects) {
						auto& wornRestrictions = effect->data->data.wornRestrictions;

						armorCompatible = armorCompatible && std::ranges::any_of(
							std::span(armor->keywords, armor->numKeywords),
							[&](auto keyword)
							{
								return wornRestrictions->HasForm(keyword);
							});
					}
				}
			}
		}
		else if (filterFlag == FilterFlag::EffectArmor) {
			auto& item = a_menu->selected.item;

			if (item && item->filterFlag.underlying() == FilterFlag::EnchantArmor) {
				if (auto armor = item->data->GetObject()->As<RE::TESObjectARMO>()) {

					auto wornRestrictions =
						static_cast<EnchantmentEntry*>(entry.get())->data->data.wornRestrictions;

					armorCompatible = armorCompatible && std::ranges::any_of(
						std::span(armor->keywords, armor->numKeywords),
						[&](auto keyword)
						{
							return wornRestrictions->HasForm(keyword);
						});
				}
			}
		}

		if (!armorCompatible) {
			if (a_showNotification) {
				static const auto setting =
					RE::GameSettingCollection::GetSingleton()
					->GetSetting("sEnchantArmorIncompatible");

				if (setting) {
					RE::DebugNotification(setting->data.s);
				}
			}

			return false;
		}

		switch (filterFlag) {
		case FilterFlag::EnchantWeapon:
			if (!a_menu->selected.effects.empty()) {
				return a_menu->selected.effects[0]->filterFlag.underlying() ==
					FilterFlag::EffectWeapon;
			}

			return true;

		case FilterFlag::EnchantArmor:
			if (!a_menu->selected.effects.empty()) {
				return a_menu->selected.effects[0]->filterFlag.underlying() ==
					FilterFlag::EffectArmor;
			}

			return true;

		case FilterFlag::EnchantAmmo:
			if (!a_menu->selected.effects.empty()) {
				return a_menu->selected.effects[0]->filterFlag.underlying() ==
					FilterFlag::EffectAmmo;
			}

			return true;

		case FilterFlag::EffectWeapon:
			if (a_menu->selected.item) {
				return a_menu->selected.item->filterFlag.underlying() == FilterFlag::EnchantWeapon;
			}

			return true;

		case FilterFlag::EffectArmor:
			if (a_menu->selected.item) {
				return a_menu->selected.item->filterFlag.underlying() == FilterFlag::EnchantArmor;
			}

			return true;

		case FilterFlag::EffectAmmo:
			if (a_menu->selected.item) {
				return a_menu->selected.item->filterFlag.underlying() == FilterFlag::EnchantAmmo;
			}

			return true;

		default:
			return true;
		}
	}
}
