#include "Enchanting.h"

#include "Data/CreatedObjectManager.h"
#include "Ext/EnchantConstructMenu.h"
#include "Ext/ExtraDataList.h"
#include "RE/Offset.h"
#include "Settings/INISettings.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Enchanting::Install()
	{
		ItemPreviewPatch();
		CraftItemPatch();
		EnchantConfirmPatch();
		AmmoQuantityPatch();
		InventoryNotificationPatch();
		EnchantmentChargePatch();
	}

	void Enchanting::ItemPreviewPatch()
	{
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x208);

		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x299);

		if (!REL::make_pattern<"41 83 FE 29">().match(hook1.address()) ||
			!REL::make_pattern<"41 83 FE 1A">().match(hook2.address())) {

			util::report_and_fail("Enchanting::ItemPreviewPatch failed to install"sv);
		}

		// jmp from 0x208 to 0x299
		std::array<std::uint8_t, 5> patch1{ 0xE9, 0x8C, 0x00, 0x00, 0x00 };

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				Xbyak::Label retn;

				mov(rdx, rsi);
				mov(ecx, r14d);
				mov(rax, util::function_ptr(&Enchanting::CreateEnchantment));
				call(rax);
				test(rax, rax);
				jz(retn);
				nop(0x2);
				// hook + 0x19
				mov(ptr[rsi + offsetof(Menu, createEffectFunctor) +
						offsetof(Menu::CreateEffectFunctor, createdEnchantment)],
					rax);
				// hook + 0x20
				L(retn);
			}
		};

		Patch patch2{};
		patch2.ready();

		assert(patch2.getSize() == 0x20);

		REL::safe_fill(hook1.address(), REL::NOP, 0x22);
		REL::safe_write(hook1.address(), patch1.data(), patch1.size());
		REL::safe_write(hook2.address(), patch2.getCode(), patch2.getSize());
	}

	void Enchanting::CraftItemPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CraftItem,
			0x98);

		if (!REL::make_pattern<"8B 43">().match(hook.address())) {
			util::report_and_fail("Enchanting::CraftItemPatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rdx, rsi);
				mov(ecx, dword[rsi + 0x1E8]);  // createEffectFunctor.formtype
				mov(rax, util::function_ptr(&Enchanting::CreateEnchantment));
				call(rax);
			}
		};

		Patch patch{};

		assert(patch.getSize() <= 0x35);

		REL::safe_fill(hook.address(), REL::NOP, 0x35);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void Enchanting::EnchantConfirmPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::ProcessUserEvent,
			0x1B6);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			logger::error("Enchanting::EnchantConfirmPatch failed to install"sv);
			return;
		}

		auto& trampoline = SKSE::GetTrampoline();
		_SetStr = trampoline.write_call<5>(hook.address(), &Enchanting::SetConfirmText);
	}

	void Enchanting::AmmoQuantityPatch()
	{
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::InventoryChanges::EnchantObject,
			0x8C);

		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::InventoryChanges::EnchantObject,
			0xA5);

		static const auto hook3 = REL::Relocation<std::uintptr_t>(
			RE::Offset::InventoryChanges::EnchantObject,
			0x14F);

		if (!REL::make_pattern<"E8">().match(hook1.address()) ||
			!REL::make_pattern<"E8">().match(hook2.address()) ||
			!REL::make_pattern<"E8">().match(hook3.address())) {

			util::report_and_fail("Enchanting::AmmoQuantityPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();

		_GetCount = trampoline.write_call<5>(hook1.address(), &Enchanting::GetCount);

		_SetBaseItemCount = trampoline.write_call<5>(
			hook2.address(),
			&Enchanting::SetBaseItemCount);

		_SetEnchantment = trampoline.write_call<5>(hook3.address(), &Enchanting::SetExtraData);
	}

	void Enchanting::InventoryNotificationPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CraftItem,
			0x2A3);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			logger::error("Enchanting::InventoryNotificationPatch failed to install"sv);
			return;
		}

		auto& trampoline = SKSE::GetTrampoline();

		_InventoryNotification = trampoline.write_call<5>(
			hook.address(),
			&Enchanting::InventoryNotification);
	}

	void Enchanting::EnchantmentChargePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x174);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Enchanting::EnchantmentChargePatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();

		_ApplyPerkEntries = trampoline.write_call<5>(
			hook.address(),
			&Enchanting::ApplyPerkEntries);
	}

	RE::EnchantmentItem* Enchanting::CreateEnchantment(
		RE::FormType a_formType,
		RE::CraftingSubMenus::EnchantConstructMenu* a_menu)
	{
		customName = a_menu->customName.c_str();
		availableCount = a_menu->selected.item
			? static_cast<std::uint16_t>(a_menu->selected.item->data->countDelta)
			: 0;

		switch (a_formType) {

		case RE::FormType::Armor:
			creatingCount = 1;
			return RE::BGSCreatedObjectManager::GetSingleton()->CreateArmorEnchantment(
				a_menu->createEffectFunctor.createdEffects);

		case RE::FormType::Weapon:
			creatingCount = 1;
			return RE::BGSCreatedObjectManager::GetSingleton()->CreateWeaponEnchantment(
				a_menu->createEffectFunctor.createdEffects);

		case RE::FormType::Ammo:
			creatingCount = Ext::EnchantConstructMenu::GetAmmoEnchantQuantity(a_menu);
			return Data::CreatedObjectManager::GetSingleton()->CreateAmmoEnchantment(
				a_menu->createEffectFunctor.createdEffects);

		default:
			return nullptr;
		}
	}

	void Enchanting::SetConfirmText(RE::BSString* a_str, const char* a_sEnchantItem)
	{
		if (availableCount < creatingCount) {
			_SetStr(
				a_str,
				fmt::format(
					"Enchantment charges: {}. You only have {} of the selected item."sv,
					creatingCount,
					availableCount)
					.data());
		}
		else {
			_SetStr(a_str, a_sEnchantItem);
		}
	}

	std::uint16_t Enchanting::GetCount(RE::ExtraDataList* a_extraList)
	{
		std::uint16_t count = _GetCount(a_extraList);
		if (creatingCount > count) {
			creatingCount = count;
		}

		return count - creatingCount + 1;
	}

	void Enchanting::SetBaseItemCount(RE::ExtraDataList* a_extraList, std::uint16_t a_count)
	{
		return _SetBaseItemCount(a_extraList, a_count - creatingCount);
	}

	void Enchanting::SetExtraData(
		RE::ExtraDataList* a_extraList,
		RE::EnchantmentItem* a_enchantment,
		std::uint16_t a_chargeAmount,
		bool a_removeOnUnequip)
	{
		_SetEnchantment(a_extraList, a_enchantment, a_chargeAmount, a_removeOnUnequip);

		if (creatingCount > availableCount) {
			creatingCount = availableCount;
		}

		if (creatingCount > 1) {
			Ext::ExtraDataList::SetCount(a_extraList, creatingCount);
		}
	}

	void Enchanting::InventoryNotification(
		RE::TESForm* a_item,
		[[maybe_unused]] std::int32_t a_count,
		bool a_itemAdded,
		bool a_playSound,
		const char* a_name)
	{
		// Vanilla only shows the original item name, so patch that here
		const char* name = a_name;
		if (customName && customName[0] != '\0') {
			name = customName;
		}

		_InventoryNotification(a_item, creatingCount, a_itemAdded, a_playSound, name);
	}

	void Enchanting::ApplyPerkEntries(
		RE::BGSEntryPoint::ENTRY_POINT a_entryPoint,
		RE::Actor* a_perkOwner,
		RE::EnchantmentItem* a_enchantment,
		RE::TESForm* a_item,
		float& a_value)
	{
		assert(a_entryPoint == RE::BGSEntryPoint::ENTRY_POINT::kModSoulGemEnchanting);

		_ApplyPerkEntries(a_entryPoint, a_perkOwner, a_enchantment, a_item, a_value);

		if (Data::CreatedObjectManager::GetSingleton()->IsBaseAmmoEnchantment(a_enchantment)) {
			a_value *= Settings::INISettings::GetSingleton()->fAmmoChargeMult;
		}
	}
}
