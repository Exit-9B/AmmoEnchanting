#include "EnchantArtManager.h"

#include "Ext/NiAVObject.h"

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
		if (auto it = _fxMap.find(handle); it != _fxMap.end()) {
			auto& fx = it->second;
			if (fx.quiverFXController) {
				fx.quiverFXController->Stop();
				delete fx.quiverFXController;
			}

			if (fx.arrowFXController) {
				delete fx.arrowFXController;
			}

			_fxMap.erase(it);
		}

		if (a_enchantment) {
			const auto costliestEffect = a_enchantment->GetCostliestEffectItem();
			const auto baseEffect = costliestEffect ? costliestEffect->baseEffect : nullptr;

			EnchantFX fx{};

			if (baseEffect) {
				if (auto& enchantArt = baseEffect->data.enchantEffectArt) {
					fx.quiverFXController = new Ext::AmmoEnchantmentController(
						a_actor,
						enchantArt);
					fx.quiverFXController->Start();
				}

				if (auto& castingArt = baseEffect->data.castingArt) {

					RE::BSResource::ModelID* id = nullptr;
					auto modelResult = RE::BSResource::RequestModelDirect(
						baseEffect->data.castingArt->model.c_str(),
						id);

					if (modelResult == 0) {
						const auto clone = Ext::NiAVObject::Clone(id->data.get());
						fx.arrowFXController = new Ext::ArrowEffectController(clone);
					}

					RE::BSResource::FreeRequestedModel(id);
				}
			}

			_fxMap.insert({ handle, std::move(fx) });
		}
	}

	void EnchantArtManager::AttachArrow(RE::Actor* a_actor, bool a_firstPerson)
	{
		std::scoped_lock lk{ _mutex };

		auto handle = a_actor->GetHandle();
		if (auto it = _fxMap.find(handle); it != _fxMap.end()) {
			auto& fx = it->second;
			if (fx.arrowFXController) {
				fx.arrowFXController->AttachArrow(a_actor, a_firstPerson);
			}
		}
	}

	void EnchantArtManager::ResetArrow(RE::Actor* a_actor, bool a_firstPerson)
	{
		std::scoped_lock lk{ _mutex };

		auto handle = a_actor->GetHandle();
		if (auto it = _fxMap.find(handle); it != _fxMap.end()) {
			auto& fx = it->second;
			if (fx.arrowFXController) {
				fx.arrowFXController->ResetArrow(a_actor, a_firstPerson);
			}
		}
	}
}
