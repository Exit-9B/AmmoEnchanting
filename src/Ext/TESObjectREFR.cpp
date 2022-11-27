#include "TESObjectREFR.h"

#include "RE/Offset.h"

namespace Ext
{
	RE::ModelReferenceEffect* TESObjectREFR::ApplyArtObject(
		RE::TESObjectREFR* a_ref,
		RE::BGSArtObject* a_artObject,
		float a_duration,
		RE::TESObjectREFR* a_facingRef,
		bool a_faceTarget,
		bool a_attachToCamera,
		RE::NiAVObject* a_attachNode,
		bool a_interfaceEffect)
	{
		using func_t = decltype(&ApplyArtObject);
		static REL::Relocation<func_t> func{ RE::Offset::TESObjectREFR::ApplyArtObject };

		return func(
			a_ref,
			a_artObject,
			a_duration,
			a_facingRef,
			a_faceTarget,
			a_attachToCamera,
			a_attachNode,
			a_interfaceEffect);
	}
}
