#pragma once

namespace Hooks
{
	class Gameplay
	{
	public:
		Gameplay() = delete;

		static void Install();

	private:
		// Copy enchantment from ExtraEnchantment to Projectile explosion
		static void LaunchProjectilePatch();

		// Remove correct ammo with ExtraData
		static void UseAmmoPatch();

		// TODO: Fix HUD display count

		static RE::BGSExplosion* GetProjectileExplosion(RE::Projectile::LaunchData* a_launchData);

		static std::int32_t UseAmmo(RE::PlayerCharacter* a_player, std::int32_t a_shotCount);

		inline static REL::Relocation<decltype(&UseAmmo)> _UseAmmo;
	};
}
