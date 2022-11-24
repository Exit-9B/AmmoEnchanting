#pragma once

namespace Hooks
{
	class VFX
	{
	public:
		VFX() = delete;

		static void Install();

	private:
		static void EquipAmmoPatch();
		static void UnequipAmmoPatch();

		static void DoEquipObject(
			RE::ActorEquipManager* a_equipManager,
			RE::Actor* a_actor,
			RE::TESBoundObject* a_object,
			RE::ObjectEquipParams* a_equipParams);

		static void DoUnequipObject(
			RE::ActorEquipManager* a_equipManager,
			RE::Actor* a_actor,
			RE::TESBoundObject* a_object,
			RE::ObjectEquipParams* a_equipParams);

		inline static REL::Relocation<decltype(&DoEquipObject)> _DoEquipObject;
		inline static REL::Relocation<decltype(&DoUnequipObject)> _DoUnequipObject;
	};
}
