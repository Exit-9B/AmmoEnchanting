#pragma once

namespace Ext
{
	class AmmoEnchantmentController : public RE::ReferenceEffectController
	{
	public:
		AmmoEnchantmentController(RE::Actor* a_actor, RE::BGSArtObject* a_enchantArt);

		virtual ~AmmoEnchantmentController() = default;  // 00

		// override
		RE::TESObjectREFR*   GetTargetReference() override;          // 0B
		RE::BGSArtObject*    GetHitEffectArt() override;             // 0C
		RE::TESEffectShader* GetHitEffectShader() override;          // 0D
		bool                 GetManagerHandlesSaveLoad() override;   // 0E
		RE::NiAVObject*      GetAttachRoot() override;               // 0F
		float                GetParticleAttachExtent() override;     // 10
		bool                 GetUseParticleAttachExtent() override;  // 11
		bool                 GetDoParticles() override;              // 12
		bool                 GetParticlesUseLocalSpace() override;   // 13
		bool                 GetUseRootWorldRotate() override;       // 14
		bool                 GetIsRootActor() override;              // 15
		bool                 GetShaderUseParentCell() override;      // 19
		bool                 GetAllowTargetRoot() override;          // 2C
		bool                 IsReadyForAttach() override;            // 1D

		bool IsActorVisible();

		// members
		RE::Actor*                    actor;
		RE::BGSArtObject*             effectArt;
		RE::NiPointer<RE::NiAVObject> attachRoot;
	};
}
