#pragma once

namespace Ext
{
	class ArrowEffectController
	{
	public:
		ArrowEffectController(RE::NiAVObject* a_model, bool a_isBolt);

		void AttachArrow(RE::Actor* a_actor, bool a_firstPerson = false);
		void DetachArrow();
		void ResetArrow(RE::Actor* a_actor, bool a_firstPerson = false);

		RE::NiPointer<RE::NiAVObject> model;
		RE::NiPointer<RE::NiAVObject> attachRoot;
		bool isBolt;
	};
}
