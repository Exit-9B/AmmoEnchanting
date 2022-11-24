#include "EnchantArtManager.h"

namespace Data
{
	EnchantArtManager* EnchantArtManager::GetSingleton()
	{
		static EnchantArtManager singleton{};
		return &singleton;
	}

	void EnchantArtManager::UpdateAmmoEnchantment(
		RE::Actor* a_actor,
		RE::EnchantmentItem* a_enchantment)
	{
		std::scoped_lock lk{ _mutex };

		auto handle = a_actor->GetHandle();
		if (auto it = _quiverEffectControllers.find(handle);
			it != _quiverEffectControllers.end()) {

			auto& controller = it->second;
			controller->Stop();
			delete controller;
			_quiverEffectControllers.erase(it);
		}

		if (a_enchantment) {
			auto controller = new Ext::AmmoEnchantmentController(a_actor, a_enchantment);
			controller->Start();

			_quiverEffectControllers.insert({ handle, controller });
		}
	}
}
