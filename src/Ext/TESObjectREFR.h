#pragma once

namespace Ext
{
	namespace TESObjectREFR
	{
		RE::ModelReferenceEffect* ApplyArtObject(
			RE::TESObjectREFR* a_ref,
			RE::BGSArtObject* a_artObject,
			float a_duration = -1.0f,
			RE::TESObjectREFR* a_facingRef = nullptr,
			bool a_faceTarget = false,
			bool a_attachToCamera = false,
			RE::NiAVObject* a_attachNode = nullptr,
			bool a_interfaceEffect = false);
	}
}
