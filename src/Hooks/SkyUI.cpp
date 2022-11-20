#include "SkyUI.h"

#include "RE/Offset.h"

#include <xbyak/xbyak.h>

#undef GetObject

namespace Hooks
{
	void SkyUI::Install()
	{
		FilterFlagPatch();
		DisenchantDataPatch();
	}

	void SkyUI::FilterFlagPatch()
	{
		// TODO: Override SkyUI's hardcoded category filters (skyui.defines.Inventory)
	}

	void SkyUI::DisenchantDataPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::PopulateEntryList,
			0x4A6);

		if (!REL::make_pattern<"4C 8D 45 D7 48 8B 56 78">().match(hook.address())) {
			logger::error("SkyUI::DisenchantDataPatch failed to install"sv);
			return;
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				// mov(r8, rsi);
				lea(rdx, ptr[rbp - 0x29]);
				mov(rcx, ptr[rbx]);
				mov(rax, util::function_ptr(&SkyUI::SetDisenchantItemData));
				call(rax);

				lea(r8, ptr[rbp - 0x29]);
				mov(rdx, ptr[rsi + offsetof(Menu, entryList) + 0x10]);  // entryList._value

				jmp(ptr[rip]);
				dq(hook.address() + 0x8);
			}
		};

		Patch patch{};
		patch.ready();

		REL::safe_fill(hook.address(), REL::NOP, 0x8);

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.write_branch<6>(hook.address(), trampoline.allocate(patch));
	}

	void SkyUI::SetDisenchantItemData(
		Menu::ItemChangeEntry* a_entry,
		RE::GFxValue* a_dataContainer)
	{
		if (a_entry->filterFlag.underlying() != FilterFlag::DisenchantAmmo)
			return;

		if (!a_entry->data)
			return;

		RE::InventoryEntryData* data = a_entry->data;
		const auto form = data->GetObject();
		const auto ammo = form ? form->As<RE::TESAmmo>() : nullptr;
		const auto projectile = ammo ? ammo->data.projectile : nullptr;
		const auto explosion = projectile ? projectile->data.explosionType : nullptr;
		const auto enchantment = explosion ? explosion->formEnchanting : nullptr;

		if (enchantment) {
			RE::GFxValue name;
			name.SetString(enchantment->GetName());

			a_dataContainer->SetMember("effectName", name);
		}
	}
}
