#include "TESAmmo.h"

namespace Ext
{
	RE::BGSExplosion* TESAmmo::GetExplosion(RE::TESAmmo* a_ammo)
	{
		const auto projectile = a_ammo ? a_ammo->data.projectile : nullptr;
		const auto explosion = projectile ? projectile->data.explosionType : nullptr;

		return explosion;
	}

	RE::EnchantmentItem* TESAmmo::GetEnchantment(RE::TESAmmo* a_ammo)
	{
		const auto explosion = GetExplosion(a_ammo);
		const auto enchantment = explosion ? explosion->formEnchanting : nullptr;

		return enchantment;
	}
}
