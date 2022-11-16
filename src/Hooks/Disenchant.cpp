#include "Disenchant.h"

#include "Data/CreatedObjectManager.h"
#include "RE/Offset.h"

namespace Hooks
{
	void Disenchant::Install()
	{
		GetEnchantmentPatch();
	}

	void Disenchant::GetEnchantmentPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::DisenchantItem,
			0x52);

		auto& trampoline = SKSE::GetTrampoline();
		_GetEnchantment = trampoline.write_call<5>(
			hook.address(),
			&Disenchant::GetEnchantment);
	}

	RE::EnchantmentItem* Disenchant::GetEnchantment(RE::InventoryEntryData* a_entry)
	{
		const auto form = a_entry->GetObject();
		if (const auto ammo = form ? form->As<RE::TESAmmo>() : nullptr) {
			const auto projectile = ammo ? ammo->data.projectile : nullptr;
			const auto explosion = projectile ? projectile->data.explosionType : nullptr;
			const auto enchantment = explosion ? explosion->formEnchanting : nullptr;

			if (enchantment) {
				const auto manager = Data::CreatedObjectManager::GetSingleton();
				manager->SetBaseExplosion(enchantment, explosion);
			}

			return enchantment;
		}

		return _GetEnchantment(a_entry);
	}
}
