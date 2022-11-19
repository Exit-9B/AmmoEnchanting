#include "Disenchant.h"

#include "Data/CreatedObjectManager.h"
#include "Ext/TESAmmo.h"
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

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Disenchant::GetEnchantmentPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_GetEnchantment = trampoline.write_call<5>(
			hook.address(),
			&Disenchant::GetEnchantment);
	}

	RE::EnchantmentItem* Disenchant::GetEnchantment(RE::InventoryEntryData* a_entry)
	{
		const auto form = a_entry->GetObject();
		if (const auto ammo = form ? form->As<RE::TESAmmo>() : nullptr) {

			const auto explosion = Ext::TESAmmo::GetExplosion(ammo);
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
