#include "ArrowEffectController.h"

#include "Ext/NiAVObject.h"

namespace Ext
{
	ArrowEffectController::ArrowEffectController(RE::NiAVObject* a_model, bool a_isBolt)
		: isBolt{ a_isBolt }
	{
		Ext::NiAVObject::SetValueNodeHidden(a_model, true);

		if (a_isBolt) {
			// Account for offset of bolt placement
			const auto adjustNode = RE::NiNode::Create(1);
			adjustNode->local.translate.y = 7.0f;

			RE::TaskQueueInterface::GetSingleton()->Attach3D(a_model, adjustNode);

			model.reset(adjustNode);
		}
		else {
			model.reset(a_model);
		}
	}

	void ArrowEffectController::AttachArrow(RE::Actor* a_actor, bool a_firstPerson)
	{
		const auto actor3D = a_actor->Get3D(a_firstPerson);
		const auto actorNode = actor3D ? actor3D->AsNode() : nullptr;

		if (!actorNode)
			return;

		const RE::BSFixedString name = isBolt ? "NPC R MagicNode [RMag]"sv : "Weapon"sv;
		const auto fxAttachRoot = actorNode->GetObjectByName(name);

		RE::TaskQueueInterface::GetSingleton()->Attach3D(model.get(), fxAttachRoot);

		attachRoot.reset(fxAttachRoot);
	}

	void ArrowEffectController::DetachArrow()
	{
		if (!attachRoot || !model)
			return;

		SKSE::GetTaskInterface()->AddTask([parent = attachRoot, child = model]
		{
			if (const auto node = parent->AsNode()) {
				node->DetachChild(child.get());
			}
		});
	}

	void ArrowEffectController::ResetArrow(RE::Actor* a_actor, bool a_firstPerson)
	{
		if (!attachRoot || !attachRoot->parent || !model || !model->parent) {
			return;
		}

		AttachArrow(a_actor, a_firstPerson);
	}
}
