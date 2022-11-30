#pragma once

namespace Papyrus
{
	namespace AmmoEnchanting
	{
		std::int32_t GetAPIVersion(RE::StaticFunctionTag*);

		void LearnEnchantment(RE::StaticFunctionTag*, RE::BGSExplosion* a_explosion);

		bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm);
	}
}
