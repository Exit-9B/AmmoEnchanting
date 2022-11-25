#pragma once

#include "Ext/AmmoEnchantmentController.h"
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

		void AttachArrow(RE::Actor* a_actor);

	private:
		struct EnchantFX
		{
			Ext::AmmoEnchantmentController* quiverFXController = nullptr;
			RE::NiPointer<RE::NiAVObject> arrowEffectModel;
		};

		EnchantArtManager() = default;

		static RE::NiAVObject* GetArrowAttachRoot(RE::Actor* a_actor);

		std::mutex _mutex;
		RE::BSTHashMap<RE::ActorHandle, EnchantFX> _fxMap;
	};
}
