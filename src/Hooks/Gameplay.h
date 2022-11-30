#pragma once

namespace Hooks
{
	class Gameplay
	{
	public:
		Gameplay() = delete;

		static void Install();

	private:
		// Allow equipping ammo with differing extra data
		static void EquipAmmoPatch();

		// Fix unenchanted ammo being replaced with enchanted ammo on auto-equip
		static void AutoEquipAmmoPatch();

		// Copy enchantment from ExtraEnchantment to Projectile explosion
		static void LaunchProjectilePatch();

		// Display correct count in HUD
		static void HUDAmmoPatch();

		// Remove correct ammo with ExtraData
		static void UseAmmoPatch();

		static void UnequipAmmoIfDifferent(
			RE::AIProcess* a_process,
			RE::TESAmmo* a_baseForm,
			RE::ExtraDataList* a_extraList,
			RE::Actor* a_actor);

		static std::uint32_t GetCount(RE::InventoryEntryData* a_entry);

		static RE::BGSExplosion* GetProjectileExplosion(RE::Projectile::LaunchData* a_launchData);

		static void AddMessage(
			RE::UIMessageQueue* a_queue,
			const RE::BSFixedString& a_menuName,
			RE::UI_MESSAGE_TYPE a_type,
			RE::HUDData* a_data);

		static std::int32_t UseAmmo(RE::PlayerCharacter* a_player, std::int32_t a_shotCount);

		inline static REL::Relocation<decltype(&GetCount)> _GetCount;
		inline static REL::Relocation<decltype(&AddMessage)> _AddMessage;
		inline static REL::Relocation<decltype(&UseAmmo)> _UseAmmo;
	};
}
