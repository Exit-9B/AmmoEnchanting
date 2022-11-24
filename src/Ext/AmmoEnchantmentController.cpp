#include "AmmoEnchantmentController.h"

namespace Ext
{
	AmmoEnchantmentController::AmmoEnchantmentController(
		RE::Actor* a_actor,
		RE::EnchantmentItem* a_enchantment)
	{
		actor = a_actor;

		const auto costliestEffect = a_enchantment->GetCostliestEffectItem();
		const auto baseEffect = costliestEffect ? costliestEffect->baseEffect : nullptr;

		if (baseEffect) {
			effectArt = baseEffect->data.enchantEffectArt;
		}
		else {
			effectArt = nullptr;
		}
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

		if (attachRoot) {
			if (!attachRoot->parent) {
				attachRoot.reset();
			}
		}

		if (!attachRoot) {
			if (const auto equippedAmmo = actor->GetCurrentAmmo()) {
				const auto actor3D = IsActorVisible() ? actor->Get3D() : nullptr;
				const auto node = actor3D ? actor3D->AsNode() : nullptr;
				const auto quiver = node ? node->GetObjectByName("ArrowQuiver"sv) : nullptr;

				attachRoot.reset(quiver);
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
		if (GetAttachRoot()) {
			if (const auto node = attachRoot->AsNode()) {
				return node->children.size();
			}
		}

		return false;
	}

	bool AmmoEnchantmentController::IsActorVisible()
	{
		const auto player = RE::PlayerCharacter::GetSingleton();
		return actor != player || player->Is3rdPersonVisible();
	}
}
