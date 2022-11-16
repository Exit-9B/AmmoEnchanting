#include "CreatedObjectManager.h"
#include "RE/Offset.h"

namespace Data
{
	CreatedObjectManager* CreatedObjectManager::GetSingleton()
	{
		static CreatedObjectManager singleton{};
		return std::addressof(singleton);
	}

	void CreatedObjectManager::SetBaseExplosion(
		RE::EnchantmentItem* a_enchantment,
		RE::BGSExplosion* a_explosion)
	{
		auto costliestEffect = a_enchantment->GetCostliestEffectItem()->baseEffect;
		baseExplosions[costliestEffect] = a_explosion;
	}

	RE::EnchantmentItem* CreatedObjectManager::CreateAmmoEnchantment(
		const RE::BSTArray<RE::Effect>& a_effects)
	{
		using EnchantmentFactory = RE::EnchantmentItem* (*)(const RE::BSTArray<RE::Effect>&, bool);
		static REL::Relocation<EnchantmentFactory> factory{ RE::Offset::EnchantmentItem::Create };

		using CreateEnchantment_t = RE::EnchantmentItem* (*)(
			RE::BSTArray<EnchantmentEntry>&,
			const RE::BSTArray<RE::Effect>&,
			EnchantmentFactory,
			bool);

		static REL::Relocation<CreateEnchantment_t> create{
			RE::Offset::BGSCreatedObjectManager::CreateEnchantment
		};

		auto enchantment = create(ammoEnchantments, a_effects, factory.get(), true);

		if (enchantment) {
			auto costliestEffect = enchantment->GetCostliestEffectItem()->baseEffect;
			if (auto baseExplosion = baseExplosions[costliestEffect]) {
				auto createdExplosion = EnchantExplosion(baseExplosion, enchantment);
				createdExplosions[enchantment] = createdExplosion;
			}
		}

		return enchantment;
	}

	bool CreatedObjectManager::IsBaseAmmoEnchantment(RE::EnchantmentItem* a_enchantment) const
	{
		auto costliestEffect = a_enchantment->GetCostliestEffectItem()->baseEffect;
		return baseExplosions.contains(costliestEffect);
	}

	RE::BGSExplosion* CreatedObjectManager::GetExplosion(RE::EnchantmentItem* a_enchantment) const
	{
		auto it = createdExplosions.find(a_enchantment);
		return it != createdExplosions.end() ? it->second : nullptr;
	}

	RE::BGSExplosion* CreatedObjectManager::EnchantExplosion(
		RE::BGSExplosion* a_base,
		RE::EnchantmentItem* a_enchantment)
	{
		const auto explosionFactory =
			RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSExplosion>();
		const auto created = explosionFactory->Create();

		created->boundData = a_base->boundData;
		created->fullName = a_base->fullName;
		created->model = a_base->model;
		created->textures = a_base->textures;
		created->addons = a_base->addons;
		created->numTextures = a_base->numTextures;
		created->numAddons = a_base->numAddons;
		created->imageSpaceModifying = a_base->imageSpaceModifying;
		created->data = a_base->data;

		created->formEnchanting = a_enchantment;
		created->castingType = RE::MagicSystem::CastingType::kFireAndForget;
		created->amountofEnchantment = 1;

		return created;
	}
}
