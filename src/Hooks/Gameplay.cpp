#include "Gameplay.h"

#include "Data/CreatedObjectManager.h"
#include "RE/Offset.h"

#include <xbyak/xbyak.h>

#undef GetObject

namespace Hooks
{
	void Gameplay::Install()
	{
		EquipAmmoPatch();
		AutoEquipAmmoPatch();
		LaunchProjectilePatch();
		HUDAmmoPatch();
		UseAmmoPatch();
	}

	void Gameplay::LaunchProjectilePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::Projectile::Launch,
			0x6FF);

		if (!REL::make_pattern<"48 83 7F">().match(hook.address())) {
			util::report_and_fail("Gameplay::LaunchProjectilePatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rcx, rdi);
				mov(rax, util::function_ptr(&Gameplay::GetProjectileExplosion));
				call(rax);
				mov(ptr[rbx + offsetof(RE::Projectile, explosion)], rax);
			}
		};

		Patch patch{};

		assert(patch.getSize() <= 0x5C);

		REL::safe_fill(hook.address(), REL::NOP, 0x5C);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void Gameplay::EquipAmmoPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::Actor::AddWornItem,
			0x36A);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Gameplay::EquipAmmoPatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(r9, rdi);
				mov(r8, r14);
				mov(rdx, rbx);
				mov(rax, util::function_ptr(&Gameplay::UnequipAmmoIfDifferent));
				call(rax);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() <= 0x59);

		REL::safe_fill(hook.address(), REL::NOP, 0x59);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void Gameplay::AutoEquipAmmoPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::Actor::AutoEquipNewAmmo,
			0x122);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Gameplay::AutoEquipAmmoPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_GetCount = trampoline.write_call<5>(hook.address(), &Gameplay::GetCount);
	}

	void Gameplay::HUDAmmoPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(RE::Offset::ShowHUDAmmo, 0xBF);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Gameplay::HUDAmmoPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_AddMessage = trampoline.write_call<5>(hook.address(), &Gameplay::AddMessage);
	}

	void Gameplay::UseAmmoPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::PlayerCharacter::UseAmmo,
			0x43);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Gameplay::UseAmmoPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_UseAmmo = trampoline.write_call<5>(hook.address(), &Gameplay::UseAmmo);
	}

	void Gameplay::UnequipAmmoIfDifferent(
		RE::AIProcess* a_process,
		RE::TESAmmo* a_baseForm,
		RE::ExtraDataList* a_extraList,
		RE::Actor* a_actor)
	{
		const auto bothHands = a_process->middleHigh ? a_process->middleHigh->bothHands : nullptr;
		if (!bothHands)
			return;

		const auto extraList = !bothHands->extraLists->empty()
			? bothHands->extraLists->front()
			: nullptr;

		if (bothHands->object != a_baseForm || extraList != a_extraList) {
			RE::ActorEquipManager::GetSingleton()->UnequipObject(
				a_actor,
				bothHands->object,
				extraList,
				1,
				nullptr,
				false,
				false,
				true,
				false,
				nullptr);
		}
	}

	std::uint32_t Gameplay::GetCount(RE::InventoryEntryData* a_entry)
	{
		if (a_entry->extraLists && !a_entry->extraLists->empty()) {
			auto& extraList = a_entry->extraLists->front();
			return extraList->GetCount();
		}
		else {
			return _GetCount(a_entry);
		}
	}

	RE::BGSExplosion* Gameplay::GetProjectileExplosion(RE::Projectile::LaunchData* a_launchData)
	{
		const auto keywordConditionalExplosion =
			RE::BGSDefaultObjectManager::GetSingleton()->GetObject<RE::BGSKeyword>(
				RE::DEFAULT_OBJECTS::kKeywordConditionalExplosion);

		if (a_launchData->sourceAmmo &&
			a_launchData->sourceAmmo->HasKeyword(keywordConditionalExplosion) &&
			(!a_launchData->sourceWeapon ||
			 !a_launchData->sourceWeapon->HasKeyword(keywordConditionalExplosion))) {

			return nullptr;
		}

		auto explosion = a_launchData->projectile->data.explosionType;

		if (!a_launchData->aggressor || !a_launchData->sourceAmmo)
			return explosion;

		const auto& actor = a_launchData->aggressor;
		const auto& actorProcess = actor->currentProcess;
		const auto middleHigh = actorProcess ? actorProcess->middleHigh : nullptr;
		const auto bothHands = middleHigh ? middleHigh->bothHands : nullptr;
		const auto extraLists = bothHands ? bothHands->extraLists : nullptr;

		if (!extraLists)
			return explosion;

		for (const auto& extraList : *bothHands->extraLists) {
			const auto exEnch = extraList->GetByType<RE::ExtraEnchantment>();
			if (exEnch && exEnch->enchantment) {
				const auto createdObjectManager = Data::CreatedObjectManager::GetSingleton();
				return createdObjectManager->GetExplosion(exEnch->enchantment);
			}
		}

		return explosion;
	}

	void Gameplay::AddMessage(
		RE::UIMessageQueue* a_queue,
		const RE::BSFixedString& a_menuName,
		RE::UI_MESSAGE_TYPE a_type,
		RE::HUDData* a_data)
	{
		const auto player = RE::PlayerCharacter::GetSingleton();
		const auto& actorProcess = player->currentProcess;
		const auto middleHigh = actorProcess ? actorProcess->middleHigh : nullptr;
		const auto bothHands = middleHigh ? middleHigh->bothHands : nullptr;
		const auto extraLists = bothHands ? bothHands->extraLists : nullptr;
		const auto extraList = extraLists && !extraLists->empty() ? extraLists->front() : nullptr;

		if (!extraList) {
			return _AddMessage(a_queue, a_menuName, a_type, a_data);
		}

		std::int32_t delta = a_data->data - bothHands->countDelta;
		a_data->data = static_cast<std::uint32_t>(extraList->GetCount() + delta);

		return _AddMessage(a_queue, a_menuName, a_type, a_data);
	}

	std::int32_t Gameplay::UseAmmo(RE::PlayerCharacter* a_player, std::int32_t a_shotCount)
	{
		const auto& actorProcess = a_player->currentProcess;
		const auto middleHigh = actorProcess ? actorProcess->middleHigh : nullptr;
		const auto bothHands = middleHigh ? middleHigh->bothHands : nullptr;

		if (!bothHands) {
			return 0;
		}

		const auto& extraLists = bothHands->extraLists;
		const auto extraList = extraLists && !extraLists->empty() ? extraLists->front() : nullptr;
		if (!extraList) {
			return _UseAmmo(a_player, a_shotCount);
		}

		const auto equippedWeapon = a_player->GetEquippedObject(false);

		if (!equippedWeapon || equippedWeapon->GetFormType() != RE::FormType::Weapon) {
			return 0;
		}

		auto invCount = extraList->GetCount();

		if (a_shotCount < 0) {
			a_shotCount = middleHigh->unk310;
		}

		if (a_shotCount > invCount) {
			a_shotCount = invCount;
		}

		invCount -= a_shotCount;

		a_player->RemoveItem(
			bothHands->GetObject(),
			a_shotCount,
			RE::ITEM_REMOVE_REASON::kRemove,
			extraList,
			nullptr);

		return invCount;
	}
}
