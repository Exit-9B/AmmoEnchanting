#include "VFX.h"

#include "Data/EnchantArtManager.h"
#include "Ext/TESAmmo.h"
#include "Ext/TESObjectREFR.h"
#include "RE/Offset.h"

namespace Hooks
{
	void VFX::Install()
	{
		EquipAmmoPatch();
		UnequipAmmoPatch();
		AttachArrowPatch();
		FireArrowPatch();
		CameraSwitchPatch();
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

	void VFX::AttachArrowPatch()
	{
		static auto Character_vtbl =
			REL::Relocation<std::uintptr_t>(RE::Offset::Character::Vtbl);

		static auto PlayerCharacter_vtbl =
			REL::Relocation<std::uintptr_t>(RE::Offset::PlayerCharacter::Vtbl);

		_Character_AttachArrow =
			Character_vtbl.write_vfunc(205, &VFX::Character_AttachArrow);

		_PlayerCharacter_AttachArrow =
			PlayerCharacter_vtbl.write_vfunc(205, &VFX::PlayerCharacter_AttachArrow);
	}

	void VFX::FireArrowPatch()
	{
		static auto ArrowProjectile_vtbl =
			REL::Relocation<std::uintptr_t>(RE::Offset::ArrowProjectile::Vtbl);

		_ArrowProjectile_Handle3DLoaded =
			ArrowProjectile_vtbl.write_vfunc(192, &VFX::ArrowProjectile_Handle3DLoaded);
	}

	void VFX::CameraSwitchPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::PlayerCharacter::SwitchSkeleton,
			0xD);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("VFX::CameraSwitchPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_IsTaskPoolRequired = trampoline.write_call<5>(hook.address(), &VFX::ResetVisuals);
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

	void VFX::Character_AttachArrow(
		RE::Character* a_character,
		const RE::BSTSmartPointer<RE::BipedAnim>& a_biped)
	{
		_Character_AttachArrow(a_character, a_biped);
		Data::EnchantArtManager::GetSingleton()->AttachArrow(a_character);
	}

	void VFX::PlayerCharacter_AttachArrow(
		RE::PlayerCharacter* a_player,
		const RE::BSTSmartPointer<RE::BipedAnim>& a_biped)
	{
		_PlayerCharacter_AttachArrow(a_player, a_biped);
		bool firstPerson = !a_player->Is3rdPersonVisible();
		Data::EnchantArtManager::GetSingleton()->AttachArrow(a_player, firstPerson);
	}

	void VFX::ArrowProjectile_Handle3DLoaded(RE::ArrowProjectile* a_projectile)
	{
		const auto& explosion = a_projectile->explosion;
		const auto enchantment = explosion ? explosion->formEnchanting : nullptr;
		const auto costliestEffect = enchantment ? enchantment->GetCostliestEffectItem() : nullptr;
		const auto baseEffect = costliestEffect ? costliestEffect->baseEffect : nullptr;
		const auto castingArt = baseEffect ? baseEffect->data.castingArt : nullptr;

		if (castingArt) {
			Ext::TESObjectREFR::ApplyArtObject(a_projectile, castingArt);
		}

		_ArrowProjectile_Handle3DLoaded(a_projectile);
	}

	bool VFX::ResetVisuals(RE::PlayerCharacter* a_player, bool a_firstPerson)
	{
		Data::EnchantArtManager::GetSingleton()->ResetArrow(a_player, a_firstPerson);
		return _IsTaskPoolRequired();
	}
}
