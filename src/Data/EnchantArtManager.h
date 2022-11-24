#pragma once

#include "Ext/AmmoEnchantmentController.h"

namespace Data
{
	class EnchantArtManager final
	{
	public:
		static EnchantArtManager* GetSingleton();

		~EnchantArtManager() = default;
		EnchantArtManager(const EnchantArtManager&) = delete;
		EnchantArtManager(EnchantArtManager&&) = delete;
		EnchantArtManager& operator=(const EnchantArtManager&) = delete;
		EnchantArtManager& operator=(EnchantArtManager&&) = delete;

		void UpdateAmmoEnchantment(RE::Actor* a_actor, RE::EnchantmentItem* a_enchantment);

	private:
		EnchantArtManager() = default;

		std::mutex _mutex;
		RE::BSTHashMap<RE::ActorHandle, Ext::AmmoEnchantmentController*> _quiverEffectControllers;
	};
}
