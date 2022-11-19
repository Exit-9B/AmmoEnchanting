#include "FilterFlags.h"

#include "Data/CreatedObjectManager.h"
#include "Ext/TESAmmo.h"
#include "RE/Offset.h"

#include <xbyak/xbyak.h>

#undef GetObject

namespace Hooks
{
	void FilterFlags::Install()
	{
		ConstructorPatch();

		ItemEntryPatch();
		EffectEntryPatch();

		DisenchantSelectPatch();
		DisenchantEnablePatch();
		DisenchantLearnPatch();
		ButtonTextPatch();

		CanSelectEntryPatch();
		SelectEntryPatch();
		ComputeMagnitudePatch();
		ChargeSliderPatch();
		ItemPreviewPatch();
	}

	void FilterFlags::ConstructorPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Ctor,
			0x1B3);

		if (!REL::make_pattern<"4D 8D A6">().match(hook.address())) {
			util::report_and_fail("Failed to install filter flag patch"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				lea(r12, ptr[r14 + offsetof(Menu, filterDisenchant)]);
				mov(dword[r12], FilterFlag::Disenchant);
				mov(ptr[r14 + offsetof(Menu, filterDivider)], esi);
				mov(dword[r14 + offsetof(Menu, filterItem)], FilterFlag::Item);
				mov(dword[r14 + offsetof(Menu, filterEnchantment)], FilterFlag::Enchantment);
				mov(dword[r14 + offsetof(Menu, filterSoulGem)], FilterFlag::SoulGem);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() == 0x37);

		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void FilterFlags::ItemEntryPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::PopulateEntryList,
			0x140);

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				Xbyak::Label skip;

				mov(rax, util::function_ptr(&FilterFlags::GetFilterFlag));
				call(rax);
				test(eax, eax);
				jz(skip);

				mov(edi, eax);

				jmp(ptr[rip]);
				dq(hook.address() + 0xDD);

				L(skip);
				jmp(ptr[rip]);
				dq(hook.address() + 0x1D4);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() <= 0xDD);

		REL::safe_fill(hook.address(), REL::NOP, 0xDD);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void FilterFlags::EffectEntryPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::AddEnchantmentIfKnown,
			0x243);

		auto& trampoline = SKSE::GetTrampoline();
		_PushBack = trampoline.write_call<5>(hook.address(), &FilterFlags::PushBack);
	}

	void FilterFlags::DisenchantSelectPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::SelectEntry,
			0x6E);

		if (!REL::make_pattern<"F6 41 0C 0A">().match(hook.address())) {
			util::report_and_fail("Failed to install"sv);
		}

		static_assert(FilterFlag::Disenchant <= 0xFF);

		std::uint8_t disenchantMask = static_cast<std::uint8_t>(FilterFlag::Disenchant);
		REL::safe_write(hook.address() + 0x3, &disenchantMask, 1);
	}

	void FilterFlags::DisenchantEnablePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::UpdateEnabledEntries,
			0x4E);

		if (!REL::make_pattern<"F6 C1 0A">().match(hook.address())) {
			util::report_and_fail("Failed to install"sv);
		}

		static_assert(FilterFlag::Disenchant <= 0xFF);

		std::uint8_t disenchantMask = static_cast<std::uint8_t>(FilterFlag::Disenchant);
		REL::safe_write(hook.address() + 0x2, &disenchantMask, 1);
	}

	void FilterFlags::DisenchantLearnPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::DisenchantItem,
			0x41);

		if (!REL::make_pattern<"F6 41 0C 0A">().match(hook.address())) {
			util::report_and_fail("Failed to install"sv);
		}

		static_assert(FilterFlag::Disenchant <= 0xFF);

		std::uint8_t disenchantMask = static_cast<std::uint8_t>(FilterFlag::Disenchant);
		REL::safe_write(hook.address() + 0x3, &disenchantMask, 1);
	}

	void FilterFlags::ButtonTextPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::UpdateInterface,
			0x11D);

		if (!REL::make_pattern<"F6 41 0C 0A">().match(hook.address())) {
			util::report_and_fail("Failed to install"sv);
		}

		static_assert(FilterFlag::Disenchant <= 0xFF);

		std::uint8_t disenchantMask = static_cast<std::uint8_t>(FilterFlag::Disenchant);
		REL::safe_write(hook.address() + 0x3, &disenchantMask, 1);
	}

	void FilterFlags::CanSelectEntryPatch()
	{
		static const auto func = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CanSelectEntry);

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rax, util::function_ptr(&Ext::EnchantConstructMenu::CanSelectEntry));
				jmp(rax);
			}
		};

		Patch patch{};
		patch.ready();

		REL::safe_fill(func.address(), REL::INT3, 0x1F0);
		REL::safe_write(func.address(), patch.getCode(), patch.getSize());
	}

	void FilterFlags::SelectEntryPatch()
	{
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Selections::SelectEntry,
			0x42);

		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Selections::SelectEntry,
			0x27F);

		struct Patch1 : Xbyak::CodeGenerator
		{
			Patch1()
			{
				// mov r8d, r11d
				test(r8d, FilterFlag::Item);
				db(0x0F);  // jnz hook + 0x1E7
				db(0x85);
				dd(0x1DA);
				test(r8d, FilterFlag::Enchantment);
				db(0x0F);  // jnz hook + 0x66
				db(0x85);
				dd(0x4C);
				cmp(r8d, FilterFlag::SoulGem);
				db(0x0F);  // jnz hook + 0x1E7
				db(0x85);
				dd(0x1C3);
				nop(0x6);
			}
		};

		Patch1 patch1{};
		patch1.ready();

		assert(patch1.getSize() == 0x2A);

		struct Patch2 : Xbyak::CodeGenerator
		{
			Patch2()
			{
				// we don't have enough space here to add our new flags in the assembly, so just
				// fall through to the next jump and recheck the conditions in the function

				// func+0x27F (after selecting an enchantment)
				nop(0x20);
				// func+0x29F (after selecting an item)
				mov(rcx, rsi);
				mov(rax, util::function_ptr(&FilterFlags::GetEnabledFilters));
				call(rax);
				mov(edi, eax);
				nop(0x3A);
				// func+0x2EA (selecting null, early return)
			}
		};

		Patch2 patch2{};
		patch2.ready();

		assert(patch2.getSize() == 0x6B);

		REL::safe_write(hook1.address(), patch1.getCode(), patch1.getSize());
		REL::safe_write(hook2.address(), patch2.getCode(), patch2.getSize());
	}

	void FilterFlags::ComputeMagnitudePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CreateEffectFunctor::Invoke,
			0x14F);

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				// we shouldn't need an explicit check for EffectArmor
				test(ecx, FilterFlag::EffectWeapon | FilterFlag::EffectAmmo);
				db(0x75);  // jnz hook + 0x1D
				db(0x15);
				nop(0x2);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() == 0xA);

		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void FilterFlags::ChargeSliderPatch()
	{
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::SelectEntry,
			0x2A6);

		// there seems to be some redundant code here, possibly an inlined function call
		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::SelectEntry,
			0x380);

		static const auto hook3 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::SliderClose,
			0x2B);

		static const auto hook4 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::CalculateCharge,
			0x47);

		struct Patch1 : Xbyak::CodeGenerator
		{
			Patch1()
			{
				test(
					eax,
					~(FilterFlag::EffectWeapon | FilterFlag::EffectAmmo | FilterFlag::SoulGem));
			}
		};

		Patch1 patch1{};
		patch1.ready();

		assert(patch1.getSize() <= 0x8);

		struct Patch2 : Xbyak::CodeGenerator
		{
			Patch2()
			{
				mov(rbx, r13);
				test(
					dword[rbx + offsetof(Menu::CategoryListEntry, filterFlag)],
					~(FilterFlag::EffectWeapon | FilterFlag::EffectAmmo));
			}
		};

		Patch2 patch2{};
		patch2.ready();

		assert(patch2.getSize() <= 0x23);

		REL::safe_fill(hook1.address(), REL::NOP, 0x8);
		REL::safe_write(hook1.address(), patch1.getCode(), patch1.getSize());
		REL::safe_fill(hook2.address(), REL::NOP, 0x23);
		REL::safe_write(hook2.address(), patch2.getCode(), patch2.getSize());
		// the flag checks here just get in the way, so nop them
		REL::safe_fill(hook3.address(), REL::NOP, 0x6);
		REL::safe_fill(hook4.address(), REL::NOP, 0xA);
	}

	void FilterFlags::ItemPreviewPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0xAF);

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rax,
					ptr[rsi + offsetof(Menu, selected) + offsetof(Menu::Selections, effects)]);
				mov(rcx, ptr[rax]);
				mov(ecx, dword[rcx + offsetof(Menu::CategoryListEntry, filterFlag)]);
				mov(rax, util::function_ptr(&FilterFlags::GetFormTypeFromEffectFlag));
				call(rax);
				mov(r14d, eax);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() <= 0x1D);

		REL::safe_fill(hook.address(), REL::NOP, 0x1D);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void FilterFlags::PushBack(void* a_arg1, Menu::EnchantmentEntry* a_entry)
	{
		if (a_entry->filterFlag.underlying() == FilterFlag::EffectWeapon) {
			const auto manager = Data::CreatedObjectManager::GetSingleton();

			if (manager->IsBaseAmmoEnchantment(a_entry->data)) {
				a_entry->filterFlag = static_cast<Menu::FilterFlag>(FilterFlag::EffectAmmo);
			}
		}

		return _PushBack(a_arg1, a_entry);
	}

	std::uint32_t FilterFlags::GetFilterFlag(RE::InventoryEntryData* a_entry)
	{
		const auto object = a_entry->GetObject();

		if (a_entry->IsQuestObject() || !object || !object->GetName() || !object->GetPlayable())
			return FilterFlag::None;

		const auto defaultObjects = RE::BGSDefaultObjectManager::GetSingleton();
		const auto disallowEnchanting = defaultObjects->GetObject<RE::BGSKeyword>(
			RE::DEFAULT_OBJECT::kKeywordDisallowEnchanting);

		if (const auto armor = object->As<RE::TESObjectARMO>()) {
			if (disallowEnchanting && armor->HasKeyword(disallowEnchanting)) {
				return FilterFlag::None;
			}
			else if (!a_entry->IsEnchanted()) {
				return FilterFlag::EnchantArmor;
			}
			else {
				return FilterFlag::DisenchantArmor;
			}
		}
		else if (const auto weapon = object->As<RE::TESObjectWEAP>()) {
			static const auto unarmedWeapon =
				REL::Relocation<RE::TESObjectWEAP**>{ RE::Offset::UnarmedWeapon };

			if (weapon->weaponData.animationType == RE::WEAPON_TYPE::kStaff ||
				disallowEnchanting && weapon->HasKeyword(disallowEnchanting) ||
				weapon == *unarmedWeapon.get() ||
				(weapon->weaponData.flags.all(RE::TESObjectWEAP::Data::Flag::kNonPlayable))) {

				return FilterFlag::None;
			}
			else if (!a_entry->IsEnchanted()) {
				return FilterFlag::EnchantWeapon;
			}
			else {
				return FilterFlag::DisenchantWeapon;
			}
		}
		else if (const auto ammo = object->As<RE::TESAmmo>()) {
			if (disallowEnchanting && ammo->HasKeyword(disallowEnchanting)) {
				return FilterFlag::None;
			}
			else if (!a_entry->IsEnchanted() && !Ext::TESAmmo::GetEnchantment(ammo)) {
				return FilterFlag::EnchantAmmo;
			}
			else {
				return FilterFlag::DisenchantAmmo;
			}
		}
		else if (const auto soulGem = object->As<RE::TESSoulGem>()) {
			if (a_entry->GetSoulLevel() == RE::SOUL_LEVEL::kNone) {
				return FilterFlag::None;
			}
			else {
				return FilterFlag::SoulGem;
			}
		}
		else {
			return FilterFlag::None;
		}
	}

	std::uint32_t FilterFlags::GetEnabledFilters(Menu::Selections* a_selected)
	{
		std::uint32_t filters = FilterFlag::SoulGem;

		if (a_selected->item) {
			switch (a_selected->item->filterFlag.underlying()) {
			case FilterFlag::EnchantWeapon:
				filters = FilterFlag::SoulGem | FilterFlag::EffectWeapon |
					FilterFlag::EnchantWeapon;
				break;
			case FilterFlag::EnchantArmor:
				filters = FilterFlag::SoulGem | FilterFlag::EffectArmor | FilterFlag::EnchantArmor;
				break;
			case FilterFlag::EnchantAmmo:
				filters = FilterFlag::SoulGem | FilterFlag::EffectAmmo | FilterFlag::EnchantAmmo;
				break;
			}
		}

		if (!a_selected->effects.empty()) {
			switch (a_selected->effects[0]->filterFlag.underlying()) {
			case FilterFlag::EffectWeapon:
				filters |= FilterFlag::EffectWeapon | FilterFlag::EnchantWeapon;
				break;
			case FilterFlag::EffectArmor:
				filters |= FilterFlag::EffectArmor | FilterFlag::EnchantArmor;
				break;
			case FilterFlag::EffectAmmo:
				filters |= FilterFlag::EffectAmmo | FilterFlag::EnchantAmmo;
				break;
			}
		}

		if (a_selected->item && a_selected->effects.empty()) {
			filters |= FilterFlag::Item;
		}
		else if (!a_selected->item && !a_selected->effects.empty()) {
			filters |= FilterFlag::Enchantment;
		}

		if (!(filters & (FilterFlag::Enchantment | FilterFlag::Item))) {
			filters = FilterFlag::All;
		}

		return filters;
	}

	RE::FormType FilterFlags::GetFormTypeFromEffectFlag(std::uint32_t a_flag)
	{
		switch (a_flag) {
		case FilterFlag::EffectWeapon:
			return RE::FormType::Weapon;
		case FilterFlag::EffectAmmo:
			return RE::FormType::Ammo;
		default:
			return RE::FormType::Armor;
		}
	}
}
