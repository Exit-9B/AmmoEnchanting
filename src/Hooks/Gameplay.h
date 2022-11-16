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

		// TODO: Fix HUD display count

		// TODO: Fix item removal when firing

		static RE::BGSExplosion* GetProjectileExplosion(RE::Projectile::LaunchData* a_launchData);
	};
}
