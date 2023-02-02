#pragma once

namespace RE::Offset
{
	namespace Actor
	{
		inline constexpr REL::ID AddWornItem{ 38004 };
		inline constexpr REL::ID AutoEquipNewAmmo{ 41244 };
	}

	namespace ArrowProjectile
	{
		inline constexpr REL::ID Vtbl{ 209891 };
	}

	namespace BGSCreatedObjectManager
	{
		inline constexpr REL::ID CreateEnchantment{ 36184 };
	}

	namespace BSResource
	{
		inline constexpr REL::ID FreeRequestedModel{ 13154 };
		inline constexpr REL::ID RequestModelAsync{ 75780 };
		inline constexpr REL::ID RequestModelDirect{ 75781 };
	}

	namespace BGSSaveLoadGame
	{
		inline constexpr REL::ID ClearForm{ 35588 };
		inline constexpr REL::ID Singleton{ 403330 };
	}

	namespace Character
	{
		inline constexpr REL::ID Vtbl{ 207886 };
	}

	namespace CraftingMenu
	{
		inline constexpr REL::ID ProcessMessage{ 51200 };
	}

	namespace CraftingSubMenus
	{
		namespace EnchantConstructMenu
		{
			inline constexpr REL::ID Ctor{ 51242 };
			inline constexpr REL::ID AddEnchantmentIfKnown{ 51285 };
			inline constexpr REL::ID CalculateCharge{ 51338 };
			inline constexpr REL::ID CanSelectEntry{ 51461 };
			inline constexpr REL::ID CraftItem{ 51355 };
			inline constexpr REL::ID DisenchantItem{ 51363 };
			inline constexpr REL::ID PopulateEntryList{ 51359 };
			inline constexpr REL::ID ProcessUserEvent{ 51379 };
			inline constexpr REL::ID SelectEntry{ 51344 };
			inline constexpr REL::ID SliderClose{ 51443 };
			inline constexpr REL::ID Update{ 51449 };
			inline constexpr REL::ID UpdateEnabledEntries{ 51450 };

			namespace CreateEffectFunctor
			{
				inline constexpr REL::ID Invoke{ 51283 };
			}

			namespace Selections
			{
				inline constexpr REL::ID SelectEntry{ 51410 };
			}
		}
	}

	namespace EnchantmentItem
	{
		inline constexpr REL::ID Create{ 36178 };
	}

	namespace ExtraDataList
	{
		inline constexpr REL::ID LoadGame{ 12095 };
	}

	namespace InventoryChanges
	{
		inline constexpr REL::ID EnchantObject{ 16146 };
	}

	namespace InventoryEntryData
	{
		inline constexpr REL::ID GetEnchantment{ 16026 };
	}

	namespace ItemCard
	{
		inline constexpr REL::ID GetMagicItemDescription{ 51900 };
	}

	namespace MissileProjectile
	{
		inline constexpr REL::ID UpdateImpl{ 44027 };
	}

	namespace NiAVObject
	{
		inline constexpr REL::ID Clone{ 70187 };
		inline constexpr REL::ID SetValueNodeHidden{ 19641 };
	}

	namespace PlayerCharacter
	{
		inline constexpr REL::ID SwitchSkeleton{ 40476 };
		inline constexpr REL::ID UseAmmo{ 41243 };
	}

	namespace Projectile
	{
		inline constexpr REL::ID GetSpeed{ 44139 };
		inline constexpr REL::ID Launch{ 44108 };
		inline constexpr REL::ID Initialize{ 44120 };
	}

	namespace ScriptFunctions
	{
		inline constexpr REL::ID PlayerEnchantObject{ 22507 };
	}

	namespace StandardItemData
	{
		inline constexpr REL::ID Init{ 51011 };
	}

	namespace TESForm
	{
		inline constexpr REL::ID GetEnchantment{ 14563 };
	}

	inline constexpr REL::ID ShowHUDAmmo{ 51629 };
	inline constexpr REL::ID UnarmedWeapon{ 401061 };
}
