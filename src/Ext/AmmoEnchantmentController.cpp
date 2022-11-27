#include "AmmoEnchantmentController.h"

#include "Ext/TaskQueueInterface.h"

namespace Ext
{
	AmmoEnchantmentController::AmmoEnchantmentController(
		RE::Actor* a_actor,
		RE::BGSArtObject* a_enchantArt)
		: actor{ a_actor },
		  effectArt{ a_enchantArt }
	{
	}

	RE::TESObjectREFR* AmmoEnchantmentController::GetTargetReference()
	{
		return actor;
	}

	RE::BGSArtObject* AmmoEnchantmentController::GetHitEffectArt()
	{
		return effectArt;
	}

	RE::TESEffectShader* AmmoEnchantmentController::GetHitEffectShader()
	{
		return nullptr;
	}

	bool AmmoEnchantmentController::GetManagerHandlesSaveLoad()
	{
		return false;
	}

	RE::NiAVObject* AmmoEnchantmentController::GetAttachRoot()
	{
		if (!actor->GetCurrentAmmo()) {
			attachRoot.reset();
		}

		if (attachRoot && !attachRoot->parent) {
			attachRoot.reset();
		}

		if (!attachRoot) {
			if (const auto equippedAmmo = actor->GetCurrentAmmo()) {
				const auto actor3D = IsActorVisible() ? actor->Get3D() : nullptr;
				const auto node = actor3D ? actor3D->AsNode() : nullptr;
				const auto quiver = node ? node->GetObjectByName("ArrowQuiver"sv) : nullptr;

				if (quiver && equippedAmmo->IsBolt()) {
					const auto adjustNode = RE::NiNode::Create(1);
					// Hardcoded transform to fit arrow FX to bolt
					adjustNode->local.translate.y = 1.0f;
					adjustNode->local.scale = 0.4f;

					Ext::TaskQueueInterface::Attach3D(
						RE::TaskQueueInterface::GetSingleton(),
						adjustNode,
						quiver);

					attachRoot.reset(adjustNode);
					hasAdjustNode = true;
				}
				else {
					attachRoot.reset(quiver);
				}
			}
		}

		return attachRoot.get();
	}

	float AmmoEnchantmentController::GetParticleAttachExtent()
	{
		if (!actor->GetCurrentAmmo()) {
			return std::numeric_limits<float>::max();
		}

		const auto node = GetAttachRoot();
		if (!node) {
			return std::numeric_limits<float>::max();
		}

		auto radius = node->worldBound.radius;
		if (radius <= 0.0f) {
			RE::NiUpdateData updateData;
			node->UpdateDownwardPass(updateData, 0);
			radius = node->worldBound.radius;
		}

		return radius + node->worldBound.center.GetDistance(node->world.translate);
	}

	bool AmmoEnchantmentController::GetUseParticleAttachExtent()
	{
		return true;
	}

	bool AmmoEnchantmentController::GetDoParticles()
	{
		return true;
	}

	bool AmmoEnchantmentController::GetParticlesUseLocalSpace()
	{
		return true;
	}

	bool AmmoEnchantmentController::GetUseRootWorldRotate()
	{
		return true;
	}

	bool AmmoEnchantmentController::GetIsRootActor()
	{
		return false;
	}

	bool AmmoEnchantmentController::GetShaderUseParentCell()
	{
		return false;
	}

	bool AmmoEnchantmentController::GetAllowTargetRoot()
	{
		return false;
	}

	bool AmmoEnchantmentController::IsReadyForAttach()
	{
		if (const auto root = GetAttachRoot()) {
			RE::NiNode* node;

			if (hasAdjustNode) {
				node = root->parent ? root->parent->AsNode() : nullptr;
			}
			else {
				node = root->AsNode();
			}

			return node && node->children.size() != 0;
		}

		return false;
	}

	bool AmmoEnchantmentController::IsActorVisible()
	{
		const auto player = RE::PlayerCharacter::GetSingleton();
		return actor != player || player->Is3rdPersonVisible();
	}
}
