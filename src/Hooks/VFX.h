#pragma once

namespace Hooks
{
	class VFX
	{
	public:
		VFX() = delete;

		static void Install();

	private:
		// Setup VFX when ammo is equipped
		static void EquipAmmoPatch();
		// Cleanup VFX when ammo is unequipped
		static void UnequipAmmoPatch();
		// Attach Casting Art when bow is drawn
		static void AttachArrowPatch();
		// Attach Casting Art when arrow is fired
		static void FireArrowPatch();
		// Reset visual effects when switching views
		static void CameraSwitchPatch();
		// Apply VFX if loading in with enchanted ammo equipped
		static void InitWornPatch();

		static void UpdateVFX(RE::Actor* a_actor);

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

		static void Character_AttachArrow(
			RE::Character* a_character,
			const RE::BSTSmartPointer<RE::BipedAnim>& a_biped);

		static void PlayerCharacter_AttachArrow(
			RE::PlayerCharacter* a_player,
			const RE::BSTSmartPointer<RE::BipedAnim>& a_biped);

		static void ArrowProjectile_Handle3DLoaded(RE::ArrowProjectile* a_projectile);

		static bool ResetVisuals(RE::PlayerCharacter* a_player, bool a_firstPerson);

		static RE::NiAVObject* InitWornObjects(
			RE::Actor* a_actor,
			bool a_backgroundLoading);

		inline static REL::Relocation<decltype(&DoEquipObject)> _DoEquipObject;
		inline static REL::Relocation<decltype(&DoUnequipObject)> _DoUnequipObject;
		inline static REL::Relocation<decltype(&Character_AttachArrow)> _Character_AttachArrow;
		inline static REL::Relocation<decltype(&PlayerCharacter_AttachArrow)>
			_PlayerCharacter_AttachArrow;
		inline static REL::Relocation<decltype(&ArrowProjectile_Handle3DLoaded)>
			_ArrowProjectile_Handle3DLoaded;
		inline static REL::Relocation<bool(*)()> _IsTaskPoolRequired;
		inline static REL::Relocation<decltype(&InitWornObjects)> _InitWornObjects;
	};
}
