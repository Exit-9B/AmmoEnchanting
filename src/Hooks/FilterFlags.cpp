#include "FilterFlags.h"

#include "Data/CreatedObjectManager.h"
#include "RE/Offset.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void FilterFlags::Install()
	{
		ConstructorPatch();

		EnchantEntryPatch();
		DisenchantEntryPatch();
		EffectEntryPatch();

		DisenchantSelectPatch();
		DisenchantEnablePatch();
		DisenchantLearnPatch();
		ButtonTextPatch();

		CanSelectEntryPatch();
		SelectEntryPatch();
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
				using Menu = RE::CraftingSubMenus::EnchantConstructMenu;

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

	void FilterFlags::EnchantEntryPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::PopulateEntryList,
			0x189);

		if (!REL::make_pattern<"0F 85 85 01 00 00">().match(hook.address())) {
			util::report_and_fail("Failed to install hook"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				Xbyak::Label soulGem;
				Xbyak::Label skip;

				// cmp al, FormType::SoulGem
				jz(soulGem);

				cmp(al, util::to_underlying(RE::FormType::Ammo));
				jnz(skip);

				lea(rcx, ptr[rbx + 0xF8]);  // cast TESAmmo to BGSKeywordForm
				mov(rax, ptr[rcx]);         // get vftable
				mov(rdx, r13);              // arg2 = MagicDisallowEnchanting
				call(ptr[rax + 0x20]);      // HasKeyword
				test(al, al);
				jnz(skip);
				mov(edi, FilterFlag::EnchantAmmo);

				// add the entry to the list
				jmp(ptr[rip]);
				dq(hook.address() + 0x94);

				// skip the entry
				L(skip);
				jmp(ptr[rip]);
				dq(hook.address() + 0x18B);

				// entry is a soul gem
				L(soulGem);
				jmp(ptr[rip]);
				dq(hook.address() + 0x6);
			}
		};

		Patch patch{};

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.write_branch<6>(hook.address(), trampoline.allocate(patch));
	}

	void FilterFlags::DisenchantEntryPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::PopulateEntryList,
			0x23C);

		if (!REL::make_pattern<"B8 08 00 00 00">().match(hook.address())) {
			util::report_and_fail("Failed to install hook"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				Xbyak::Label disenchantAmmo;
				Xbyak::Label retn;

				// cmp edi, FilterFlag::EnchantArmor
				jnz(disenchantAmmo);
				mov(edi, FilterFlag::DisenchantArmor);
				jmp(retn);

				L(disenchantAmmo);
				cmp(edi, FilterFlag::EnchantAmmo);
				mov(eax, FilterFlag::DisenchantAmmo);
				cmovz(edi, eax);

				L(retn);
				jmp(ptr[rip]);
				dq(hook.address() + 0x8);
			}
		};

		Patch patch{};

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.write_branch<5>(hook.address(), trampoline.allocate(patch));
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
				sub(rsp, 0x30);
				mov(rax, util::function_ptr(&Ext::EnchantConstructMenu::CanSelectEntry));
				call(rax);
				add(rsp, 0x30);
				ret();
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
				nop(6);
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

	void FilterFlags::ItemPreviewPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0xAF);

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rax, ptr[rsi + 0x180]);
				mov(rcx, ptr[rax]);
				mov(ecx, dword[rcx + 0xC]);
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
