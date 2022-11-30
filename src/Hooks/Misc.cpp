#include "Misc.h"

#include "Data/CreatedObjectManager.h"
#include "RE/Offset.h"
#include "Settings/INISettings.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Misc::Install()
	{
		GetEnchantmentPatch();
		GoldValuePatch();
		LoadGamePatch();
	}

	void Misc::GetEnchantmentPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::TESForm::GetEnchantment,
			0xD);

		if (!REL::make_pattern<"4C 8D 0D">().match(hook.address())) {
			util::report_and_fail("Misc::GetEnchantmentPatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				xor_(ebx, ebx);
				mov(rax, util::function_ptr(&Misc::AsEnchantableForm));
				call(rax);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() <= 0x1B);

		REL::safe_fill(hook.address(), REL::NOP, 0x1B);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void Misc::GoldValuePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::InventoryEntryData::GetValue,
			0x11D);

		if (!REL::make_pattern<"48 8B D7 49 8B CE">().match(hook.address())) {
			util::report_and_fail("Misc::GoldValuePatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				movaps(xmm3, xmm0);
				mov(r8, rsi);
				mov(rdx, rdi);
				mov(rcx, r14);
				mov(rax, util::function_ptr(&Misc::CalculateCost));
				call(rax);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() <= 0x31);

		REL::safe_fill(hook.address(), REL::NOP, 0x31);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void Misc::LoadGamePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::ExtraDataList::LoadGame,
			0x1E2D);

		if (!REL::make_pattern<"8B 4D 90">().match(hook.address())) {
			util::report_and_fail("Misc::LoadGamePatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rdx, ptr[rsp + 0x38]);
				mov(ecx, ptr[rbp - 0x70]);
				mov(rax, util::function_ptr(&Misc::LookupEnchantment));
				call(rax);
			}
		};

		Patch patch{};
		patch.ready();

		assert(patch.getSize() < 0x25);

		REL::safe_fill(hook.address(), REL::NOP, 0x25);
		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	RE::TESEnchantableForm* Misc::AsEnchantableForm(RE::TESForm* a_form)
	{
		if (const auto ammo = a_form->As<RE::TESAmmo>()) {
			const auto projectile = ammo ? ammo->data.projectile : nullptr;
			a_form = projectile ? projectile->data.explosionType : nullptr;
		}

		return skyrim_cast<RE::TESEnchantableForm*>(a_form);
	}

	float Misc::CalculateCost(
		RE::TESObject* a_object,
		RE::ExtraDataList* a_extraList,
		RE::MagicItem* a_magicItem,
		float a_baseValue)
	{
		if (a_object->GetFormType() == RE::FormType::Ammo) {
			const auto cost = a_magicItem->CalculateTotalGoldValue();
			const auto effectCostMult = Settings::INISettings::GetSingleton()->fAmmoEffectCostMult;

			return a_baseValue + effectCostMult * cost;
		}
		else {
			std::uint16_t charge = 0;
			if (const auto enchantableForm = skyrim_cast<RE::TESEnchantableForm*>(a_object);
				enchantableForm && enchantableForm->amountofEnchantment != 0) {

				charge = enchantableForm->amountofEnchantment;
			}
			else if (a_extraList) {
				if (const auto exEnch = a_extraList->GetByType<RE::ExtraEnchantment>()) {
					charge = exEnch->charge;
				}
			}

			auto cost = a_magicItem->CalculateTotalGoldValue();

			static const auto fEnchantmentPointsMult =
				RE::GameSettingCollection::GetSingleton()->GetSetting("fEnchantmentPointsMult");

			static const auto fEnchantmentEffectPointsMult =
				RE::GameSettingCollection::GetSingleton()
				->GetSetting("fEnchantmentEffectPointsMult");

			if (cost != 0.0f) {
				cost = a_baseValue + fEnchantmentPointsMult->GetFloat() * charge +
					fEnchantmentEffectPointsMult->GetFloat() * cost;
			}

			return cost;
		}
	}

	RE::EnchantmentItem* Misc::LookupEnchantment(
		RE::FormID a_formID,
		RE::ExtraEnchantment* a_exEnchantment)
	{
		auto enchantment = RE::TESForm::LookupByID<RE::EnchantmentItem>(a_formID);
		if (!enchantment) {
			Data::CreatedObjectManager::GetSingleton()->AddFailedEnchantmentLoad(
				a_exEnchantment,
				a_formID);
		}

		return enchantment;
	}
}
