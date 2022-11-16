#include "Misc.h"

#include "RE/Offset.h"

namespace Hooks
{
	void Misc::Install()
	{
		GetEnchantmentPatch();
	}

	void Misc::GetEnchantmentPatch()
	{
		// TODO: Scale gold cost of enchanted ammo
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::TESForm::GetEnchantment,
			0x23);

		auto& trampoline = SKSE::GetTrampoline();
		_AsEnchantableForm = trampoline.write_call<5>(
			hook.address(),
			&Misc::AsEnchantableForm);
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
}
