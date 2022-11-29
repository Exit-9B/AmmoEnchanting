#pragma once

namespace Data
{
	class CreatedObjectManager final
	{
	public:
		using EnchantmentEntry = RE::BGSCreatedObjectManager::EnchantmentEntry;

		static CreatedObjectManager* GetSingleton();

		~CreatedObjectManager() = default;
		CreatedObjectManager(const CreatedObjectManager&) = delete;
		CreatedObjectManager(CreatedObjectManager&&) = delete;
		CreatedObjectManager& operator=(const CreatedObjectManager&) = delete;
		CreatedObjectManager& operator=(CreatedObjectManager&&) = delete;

		void SetBaseExplosion(RE::EnchantmentItem* a_enchantment, RE::BGSExplosion* a_explosion);

		RE::EnchantmentItem* CreateAmmoEnchantment(const RE::BSTArray<RE::Effect>& a_effects);

		bool IsBaseAmmoEnchantment(RE::EnchantmentItem* a_enchantment) const;

		RE::BGSExplosion* GetExplosion(RE::EnchantmentItem* a_enchantment) const;

		void AddFailedEnchantmentLoad(RE::ExtraEnchantment* a_exEnchantment, RE::FormID a_formID);

		bool Save(SKSE::SerializationInterface* a_intfc) const;

		bool Load(SKSE::SerializationInterface* a_intfc);

		void Revert();

	private:
		CreatedObjectManager() = default;

		static RE::BGSExplosion* EnchantExplosion(
			RE::BGSExplosion* a_base,
			RE::EnchantmentItem* a_enchantment,
			RE::FormID a_formID = 0x00000000);

		static RE::EnchantmentItem* CreateEnchantment(
			RE::BSTArray<RE::Effect>& a_effects,
			bool a_isWeapon);

		static bool SaveEnchantment(
			const RE::EnchantmentItem* a_enchantment,
			SKSE::SerializationInterface* a_intfc);

		static bool LoadEnchantment(
			RE::EnchantmentItem*& a_enchantment,
			SKSE::SerializationInterface* a_intfc);

		RE::BSTArray<EnchantmentEntry> ammoEnchantments;
		std::map<RE::EffectSetting*, RE::BGSExplosion*> baseExplosions;
		std::map<RE::EnchantmentItem*, RE::BGSExplosion*> createdExplosions;
		std::map<RE::ExtraEnchantment*, RE::FormID> failedFormLoads;
	};
}
