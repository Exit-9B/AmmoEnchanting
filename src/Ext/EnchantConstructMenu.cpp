#include "EnchantConstructMenu.h"
#include <algorithm>

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

		bool restrictionsCompatible = true;

		auto& entry = a_menu->listEntries[a_index];
		auto filterFlag = entry->filterFlag.underlying();

		// Enable worn restrictions for weapon and ammo
		switch (filterFlag) {
		case FilterFlag::EnchantWeapon:
		case FilterFlag::EnchantArmor:
		case FilterFlag::EnchantAmmo:
		{
			for (auto& effect : a_menu->selected.effects) {
				const auto item = static_cast<ItemChangeEntry*>(entry.get());
				restrictionsCompatible &= HasCompatibleRestrictions(item, effect.get());
			}
		} break;

		case FilterFlag::EffectWeapon:
		case FilterFlag::EffectArmor:
		case FilterFlag::EffectAmmo:
		{
			const auto item = a_menu->selected.item.get();
			const auto effect = static_cast<EnchantmentEntry*>(entry.get());
			restrictionsCompatible = HasCompatibleRestrictions(item, effect);
		} break;
		}

		if (!restrictionsCompatible) {
			if (a_showNotification) {
				// "Chosen enchantment cannot be applied to this item"
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

	bool EnchantConstructMenu::HasCompatibleRestrictions(
		ItemChangeEntry* a_item,
		EnchantmentEntry* a_effect)
	{
		if (!a_item || !a_effect) {
			return true;
		}

		auto keywordForm = skyrim_cast<RE::BGSKeywordForm*>(a_item->data->object);
		auto restrictions = a_effect->data->data.wornRestrictions;

		if (!restrictions) {
			return true;
		}

		return std::ranges::any_of(
			std::span(keywordForm->keywords, keywordForm->numKeywords),
			[&](auto keyword)
			{
				return restrictions->HasForm(keyword);
			});
	}

	std::uint16_t EnchantConstructMenu::GetAmmoEnchantQuantity(Menu* a_menu)
	{
		float cost = 0.0f;
		for (auto& enchantment : a_menu->selected.effects) {
			float costRatio = 1.0f;
			if (enchantment->maxMagnitude > 0.0f) {
				costRatio = enchantment->magnitude / enchantment->maxMagnitude;
			}

			cost += enchantment->data->CalculateMagickaCost(nullptr) * costRatio;
		}

		float enchantingSkill = RE::PlayerCharacter::GetSingleton()->GetActorValue(
			RE::ActorValue::kEnchanting);

		static RE::Setting*
			fEnchantingSkillCostBase = RE::GameSettingCollection::GetSingleton()->GetSetting(
				"fEnchantingSkillCostBase");
		static RE::Setting*
			fEnchantingSkillCostScale = RE::GameSettingCollection::GetSingleton()->GetSetting(
				"fEnchantingSkillCostScale");
		static RE::Setting*
			fEnchantingCostExponent = RE::GameSettingCollection::GetSingleton()->GetSetting(
				"fEnchantingCostExponent");
		static RE::Setting*
			fEnchantingSkillCostMult = RE::GameSettingCollection::GetSingleton()->GetSetting(
				"fEnchantingSkillCostMult");

		cost = (1.0f -
				std::powf(
					fEnchantingSkillCostBase->GetFloat() * enchantingSkill,
					fEnchantingSkillCostScale->GetFloat())) *
			std::powf(cost, fEnchantingCostExponent->GetFloat()) *
			fEnchantingSkillCostMult->GetFloat();

		if (cost > 0.0f && a_menu->chargeAmount > cost) {
			return static_cast<std::uint16_t>(a_menu->chargeAmount / cost);
		}
		else {
			return 1;
		}
	}
}
