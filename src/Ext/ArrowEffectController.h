#pragma once

namespace Ext
{
	class ArrowEffectController
	{
	public:
		ArrowEffectController(RE::NiAVObject* a_model);

		void AttachArrow(RE::Actor* a_actor, bool a_firstPerson = false);
		void ResetArrow(RE::Actor* a_actor, bool a_firstPerson = false);

		RE::NiPointer<RE::NiAVObject> model;
		RE::NiPointer<RE::NiAVObject> attachRoot;
	};
}
