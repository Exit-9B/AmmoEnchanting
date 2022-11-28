#include "ArrowEffectController.h"

#include "Ext/NiAVObject.h"
#include "Ext/TaskQueueInterface.h"

namespace Ext
{
	ArrowEffectController::ArrowEffectController(RE::NiAVObject* a_model) : model{ a_model }
	{
		Ext::NiAVObject::SetValueNodeHidden(a_model, true);
	}

	void ArrowEffectController::AttachArrow(RE::Actor* a_actor, bool a_firstPerson)
	{
		const auto process = a_actor->currentProcess;
		const auto middleHigh = process ? process->middleHigh : nullptr;
		const auto rightHand = middleHigh ? middleHigh->rightHand : nullptr;
		const auto weapon = rightHand && rightHand->object
			? rightHand->object->As<RE::TESObjectWEAP>()
			: nullptr;

		if (!weapon)
			return;

		const auto actor3D = a_actor->Get3D(a_firstPerson);
		const auto actorNode = actor3D ? actor3D->AsNode() : nullptr;

		if (!actorNode)
			return;

		const bool isCrossbow = weapon->weaponData.animationType == RE::WEAPON_TYPE::kCrossbow;
		const RE::BSFixedString name = isCrossbow ? "NPC R MagicNode [RMag]"sv : "Weapon"sv;

		const auto arrowAttach = actorNode->GetObjectByName(name);
		RE::NiAVObject* fxAttachRoot = arrowAttach;

		if (isCrossbow) {
			// Account for offset of bolt placement
			const auto adjustNode = RE::NiNode::Create(1);
			adjustNode->local.translate.y = 7.0f;

			Ext::TaskQueueInterface::Attach3D(
				RE::TaskQueueInterface::GetSingleton(),
				adjustNode,
				arrowAttach);

			fxAttachRoot = adjustNode;
		}

		Ext::TaskQueueInterface::Attach3D(
			RE::TaskQueueInterface::GetSingleton(),
			model.get(),
			fxAttachRoot);

		attachRoot.reset(fxAttachRoot);
	}

	void ArrowEffectController::ResetArrow(RE::Actor* a_actor, bool a_firstPerson)
	{
		if (!attachRoot || !attachRoot->parent || !model || !model->parent) {
			return;
		}

		AttachArrow(a_actor, a_firstPerson);
	}
}
