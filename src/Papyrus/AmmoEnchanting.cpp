#include "AmmoEnchanting.h"

#include "Data/CreatedObjectManager.h"

#define REGISTER(vm, func) vm->RegisterFunction(#func##sv, "AmmoEnchanting"sv, func)

namespace Papyrus
{
	std::int32_t AmmoEnchanting::GetAPIVersion(RE::StaticFunctionTag*)
	{
		return 1;
	}

	void AmmoEnchanting::LearnEnchantment(RE::StaticFunctionTag*, RE::BGSExplosion* a_explosion)
	{
		if (a_explosion && a_explosion->formEnchanting) {
			const auto& enchantment = a_explosion->formEnchanting;

			auto baseEnchantment = enchantment;
			if (enchantment->data.baseEnchantment) {
				baseEnchantment = enchantment->data.baseEnchantment;
			}

			baseEnchantment->formFlags |= RE::TESForm::RecordFlags::kKnown;
			baseEnchantment->AddChange(RE::TESForm::ChangeFlags::kFlags);

			const auto manager = Data::CreatedObjectManager::GetSingleton();
			manager->SetBaseExplosion(baseEnchantment, a_explosion);
		}
	}

	bool AmmoEnchanting::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
	{
		REGISTER(a_vm, GetAPIVersion);
		REGISTER(a_vm, LearnEnchantment);

		return true;
	}
}
