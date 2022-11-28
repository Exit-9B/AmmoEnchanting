#pragma once

#include "Ext/AmmoEnchantmentController.h"
#include "Ext/ArrowEffectController.h"
#include "RE/BSResource.h"

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

		void AttachArrow(RE::Actor* a_actor, bool a_firstPerson = false);

		void ResetArrow(RE::Actor* a_actor, bool a_firstPerson = false);

	private:
		struct EnchantFX
		{
			Ext::AmmoEnchantmentController* quiverFXController = nullptr;
			Ext::ArrowEffectController* arrowFXController = nullptr;
		};

		EnchantArtManager() = default;

		std::mutex _mutex;
		RE::BSTHashMap<RE::ActorHandle, EnchantFX> _fxMap;
	};
}
