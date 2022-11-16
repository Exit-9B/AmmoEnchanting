#include "RE/BGSCreatedObjectManager.h"

#include "RE/Offset.h"

namespace RE
{
	BGSCreatedObjectManager* BGSCreatedObjectManager::GetSingleton()
	{
		static REL::Relocation<BGSCreatedObjectManager**> singleton{
			RE::Offset::BGSCreatedObjectManager::Singleton
		};
		return *singleton.get();
	}

	EnchantmentItem* BGSCreatedObjectManager::CreateArmorEnchantment(BSTArray<Effect>& a_effects)
	{
		static REL::Relocation<decltype(&BGSCreatedObjectManager::CreateArmorEnchantment)> func{
			RE::Offset::BGSCreatedObjectManager::CreateArmorEnchantment
		};
		return func(this, a_effects);
	}

	EnchantmentItem* BGSCreatedObjectManager::CreateWeaponEnchantment(BSTArray<Effect>& a_effects)
	{
		static REL::Relocation<decltype(&BGSCreatedObjectManager::CreateWeaponEnchantment)> func{
			RE::Offset::BGSCreatedObjectManager::CreateWeaponEnchantment
		};
		return func(this, a_effects);
	}
}
