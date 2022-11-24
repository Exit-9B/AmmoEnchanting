#include "VFX.h"

#include "Data/EnchantArtManager.h"
#include "Ext/TESAmmo.h"

namespace Hooks
{
	void VFX::Install()
	{
		EquipAmmoPatch();
		UnequipAmmoPatch();
	}

	void VFX::EquipAmmoPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::ActorEquipManager::EquipObject,
			0x170);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("VFX::EquipAmmoPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_DoEquipObject = trampoline.write_call<5>(hook.address(), &VFX::DoEquipObject);
	}

	void VFX::UnequipAmmoPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::ActorEquipManager::UnequipObject,
			0x1B9);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("VFX::UnequipAmmoPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_DoUnequipObject = trampoline.write_call<5>(hook.address(), &VFX::DoUnequipObject);
	}

	void VFX::DoEquipObject(
		RE::ActorEquipManager* a_equipManager,
		RE::Actor* a_actor,
		RE::TESBoundObject* a_object,
		RE::ObjectEquipParams* a_equipParams)
	{
		_DoEquipObject(a_equipManager, a_actor, a_object, a_equipParams);

		if (const auto ammo = a_object->As<RE::TESAmmo>()) {

			RE::EnchantmentItem* enchantment = Ext::TESAmmo::GetEnchantment(ammo);

			const auto extraList = a_equipParams->extraDataList;
			const auto exEnch = extraList ? extraList->GetByType<RE::ExtraEnchantment>() : nullptr;
			if (exEnch) {
				enchantment = exEnch->enchantment;
			}

			Data::EnchantArtManager::GetSingleton()->UpdateAmmoEnchantment(a_actor, enchantment);
		}
	}

	void VFX::DoUnequipObject(
		RE::ActorEquipManager* a_equipManager,
		RE::Actor* a_actor,
		RE::TESBoundObject* a_object,
		RE::ObjectEquipParams* a_equipParams)
	{
		_DoUnequipObject(a_equipManager, a_actor, a_object, a_equipParams);

		if (const auto ammo = a_object->As<RE::TESAmmo>()) {
			Data::EnchantArtManager::GetSingleton()->UpdateAmmoEnchantment(a_actor, nullptr);
		}
	}
}
