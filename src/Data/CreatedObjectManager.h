#pragma once

#include "RE/BGSCreatedObjectManager.h"

namespace Data
{
	// TODO: Implement serialization
	class CreatedObjectManager final
	{
	public:
		using EnchantmentEntry = RE::BGSCreatedObjectManager::EnchantmentEntry;
		using MakeEnchantment_t = RE::EnchantmentItem* (*)(const RE::BSTArray<RE::Effect>&, bool);

		static CreatedObjectManager* GetSingleton();

		void SetBaseExplosion(RE::EnchantmentItem* a_enchantment, RE::BGSExplosion* a_explosion);

		RE::EnchantmentItem* CreateAmmoEnchantment(const RE::BSTArray<RE::Effect>& a_effects);

		bool IsBaseAmmoEnchantment(RE::EnchantmentItem* a_enchantment) const;

		RE::BGSExplosion* GetExplosion(RE::EnchantmentItem* a_enchantment) const;

		CreatedObjectManager(const CreatedObjectManager&) = delete;
		CreatedObjectManager(CreatedObjectManager&&) = delete;
		CreatedObjectManager& operator=(const CreatedObjectManager&) = delete;
		CreatedObjectManager& operator=(CreatedObjectManager&&) = delete;

	private:
		CreatedObjectManager() = default;

		static RE::BGSExplosion* EnchantExplosion(
			RE::BGSExplosion* a_base,
			RE::EnchantmentItem* a_enchantment);

		RE::BSTArray<EnchantmentEntry> ammoEnchantments;
		std::map<RE::EffectSetting*, RE::BGSExplosion*> baseExplosions;
		std::map<RE::EnchantmentItem*, RE::BGSExplosion*> createdExplosions;
	};
}
