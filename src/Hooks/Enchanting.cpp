#include "Enchanting.h"

#include "Data/CreatedObjectManager.h"
#include "RE/Offset.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Enchanting::Install()
	{
		ItemPreviewPatch();
		EnchantObjectPatch();
	}

	void Enchanting::ItemPreviewPatch()
	{
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x208);

		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::Update,
			0x299);

		// jmp from 0x208 to 0x299
		std::vector<std::uint8_t> patch1{ 0xE9, 0x8C, 0x00, 0x00, 0x00 };

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
				nop(2);
				mov(ptr[rsi + 0x1E0], rax);  // createEffectFunctor.createdEnchantment
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

	void Enchanting::EnchantObjectPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::EnchantObject,
			0x98);

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

	RE::EnchantmentItem* Enchanting::CreateEnchantment(
		RE::FormType a_formType,
		RE::CraftingSubMenus::EnchantConstructMenu* a_menu)
	{
		switch (a_formType) {
		case RE::FormType::Armor:
			return RE::BGSCreatedObjectManager::GetSingleton()->CreateArmorEnchantment(
				a_menu->createEffectFunctor.createdEffects);
		case RE::FormType::Weapon:
			return RE::BGSCreatedObjectManager::GetSingleton()->CreateWeaponEnchantment(
				a_menu->createEffectFunctor.createdEffects);
		case RE::FormType::Ammo:
			return Data::CreatedObjectManager::GetSingleton()->CreateAmmoEnchantment(
				a_menu->createEffectFunctor.createdEffects);
		default:
			return nullptr;
		}
	}
}
