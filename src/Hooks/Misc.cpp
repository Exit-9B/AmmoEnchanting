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
	}

	void Misc::GetEnchantmentPatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::TESForm::GetEnchantment,
			0x23);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Misc::GetEnchantmentPatch failed to install"sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_AsEnchantableForm = trampoline.write_call<5>(hook.address(), &Misc::AsEnchantableForm);
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

	RE::TESEnchantableForm* Misc::AsEnchantableForm(
		RE::TESForm* a_inptr,
		std::int32_t a_vfDelta,
		void* a_srcType,
		void* a_targetType,
		std::int32_t a_isReference)
	{
		auto inptr = a_inptr;

		if (const auto ammo = a_inptr->As<RE::TESAmmo>()) {
			const auto projectile = ammo ? ammo->data.projectile : nullptr;
			inptr = projectile ? projectile->data.explosionType : nullptr;
		}

		return _AsEnchantableForm(inptr, a_vfDelta, a_srcType, a_targetType, a_isReference);
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
}
