#include "EnchantArtManager.h"

#include "Ext/NiAVObject.h"
#include "Ext/TaskQueueInterface.h"

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
						fx.arrowEffectModel.reset(Ext::NiAVObject::Clone(id->data.get()));
					}

					RE::BSResource::FreeRequestedModel(id);
				}
			}

			_fxMap.insert({ handle, std::move(fx) });
		}
	}

	void EnchantArtManager::AttachArrow(RE::Actor* a_actor)
	{
		std::scoped_lock lk{ _mutex };

		auto handle = a_actor->GetHandle();
		if (auto it = _fxMap.find(handle); it != _fxMap.end()) {
			auto& fx = it->second;
			if (fx.arrowEffectModel) {

				if (const auto root = GetArrowAttachRoot(a_actor)) {

					const auto clone = Ext::NiAVObject::Clone(fx.arrowEffectModel.get());
					Ext::NiAVObject::SetValueNodeHidden(clone, true);

					Ext::TaskQueueInterface::Attach3D(
						RE::TaskQueueInterface::GetSingleton(),
						clone,
						root);
				}
			}
		}
	}

	RE::NiAVObject* EnchantArtManager::GetArrowAttachRoot(RE::Actor* a_actor)
	{
		const auto process = a_actor->currentProcess;
		const auto middleHigh = process ? process->middleHigh : nullptr;
		const auto rightHand = middleHigh ? middleHigh->rightHand : nullptr;
		const auto weapon = rightHand && rightHand->object
			? rightHand->object->As<RE::TESObjectWEAP>()
			: nullptr;

		if (!weapon)
			return nullptr;

		const auto actor3D = a_actor->Get3D();
		const auto actorNode = actor3D ? actor3D->AsNode() : nullptr;

		if (!actorNode)
			return nullptr;

		const bool isCrossbow = weapon->weaponData.animationType == RE::WEAPON_TYPE::kCrossbow;
		const RE::BSFixedString name = isCrossbow ? "NPC R MagicNode [RMag]"sv : "Weapon"sv;

		const auto arrowAttach = actorNode->GetObjectByName(name);

		if (isCrossbow) {
			// Account for offset of bolt placement
			const auto adjustNode = RE::NiNode::Create(1);
			adjustNode->local.translate.y = 7.0f;

			Ext::TaskQueueInterface::Attach3D(
				RE::TaskQueueInterface::GetSingleton(),
				adjustNode,
				arrowAttach);

			return adjustNode;
		}
		else {
			return arrowAttach;
		}
	}
}
