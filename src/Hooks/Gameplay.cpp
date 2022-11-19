#include "Gameplay.h"

#include "Data/CreatedObjectManager.h"
#include "RE/Offset.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Gameplay::Install()
	{
		LaunchProjectilePatch();
	}

	void Gameplay::LaunchProjectilePatch()
	{
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::Projectile::Launch,
			0x749);

		// skipping the last 4 bytes to support both 1.6.353 and 1.6.640
		if (!REL::make_pattern<"48 8B 47 ?? "
							   "48 8B 88 ?? ?? ?? ?? "
							   "48 89 8B">()
				 .match(hook.address())) {
			util::report_and_fail("Gameplay::LaunchProjectilePatch failed to install"sv);
		}

		struct Patch : Xbyak::CodeGenerator
		{
			Patch()
			{
				mov(rcx, rdi);
				mov(rax, util::function_ptr(&Gameplay::GetProjectileExplosion));
				call(rax);
				mov(ptr[rbx + offsetof(RE::Projectile, explosion)], rax);

				jmp(ptr[rip]);
				dq(hook.address() + 0x12);
			}
		};

		Patch patch{};

		REL::safe_fill(hook.address(), REL::NOP, 0x12);

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.write_branch<6>(hook.address(), trampoline.allocate(patch));
	}

	RE::BGSExplosion* Gameplay::GetProjectileExplosion(RE::Projectile::LaunchData* a_launchData)
	{
		auto explosion = a_launchData->projectile->data.explosionType;

		if (!a_launchData->aggressor || !a_launchData->sourceAmmo)
			return explosion;

		const auto& actor = a_launchData->aggressor;
		const auto& actorProcess = actor->currentProcess;
		const auto middleHigh = actorProcess ? actorProcess->middleHigh : nullptr;
		const auto bothHands = middleHigh ? middleHigh->bothHands : nullptr;
		const auto extraLists = bothHands ? bothHands->extraLists : nullptr;

		if (!extraLists)
			return explosion;

		for (const auto& extraList : *bothHands->extraLists) {
			const auto exEnch = extraList->GetByType<RE::ExtraEnchantment>();
			if (exEnch && exEnch->enchantment) {
				const auto createdObjectManager = Data::CreatedObjectManager::GetSingleton();
				return createdObjectManager->GetExplosion(exEnch->enchantment);
			}
		}

		return explosion;
	}
}
