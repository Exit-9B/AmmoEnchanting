#pragma once

namespace RE::Offset
{
	namespace Actor
	{
		inline static constexpr REL::ID AddWornItem{ 38004 };
	}

	namespace ArrowProjectile
	{
		inline static constexpr REL::ID Vtbl{ 209891 };
	}

	namespace BGSCreatedObjectManager
	{
		inline static constexpr REL::ID Singleton{ 400320 };
		inline static constexpr REL::ID CreateArmorEnchantment{ 36166 };
		inline static constexpr REL::ID CreateEnchantment{ 36184 };
		inline static constexpr REL::ID CreateWeaponEnchantment{ 36165 };
	}

	namespace BSResource
	{
		inline static constexpr REL::ID FreeRequestedModel{ 13154 };
		inline static constexpr REL::ID RequestModelAsync{ 75780 };
		inline static constexpr REL::ID RequestModelDirect{ 75781 };
	}

	namespace Character
	{
		inline static constexpr REL::ID Vtbl{ 207886 };
	}

	namespace CraftingSubMenus
	{
		namespace EnchantConstructMenu
		{
			inline static constexpr REL::ID Ctor{ 51242 };
			inline static constexpr REL::ID AddEnchantmentIfKnown{ 51285 };
			inline static constexpr REL::ID CalculateCharge{ 51338 };
			inline static constexpr REL::ID CanSelectEntry{ 51461 };
			inline static constexpr REL::ID CraftItem{ 51355 };
			inline static constexpr REL::ID DisenchantItem{ 51363 };
			inline static constexpr REL::ID PopulateEntryList{ 51359 };
			inline static constexpr REL::ID ProcessUserEvent{ 51379 };
			inline static constexpr REL::ID SelectEntry{ 51344 };
			inline static constexpr REL::ID SliderClose{ 51443 };
			inline static constexpr REL::ID Update{ 51449 };
			inline static constexpr REL::ID UpdateEnabledEntries{ 51450 };
			inline static constexpr REL::ID UpdateInterface{ 51459 };

			namespace CreateEffectFunctor
			{
				inline static constexpr REL::ID Invoke{ 51283 };
			}

			namespace Selections
			{
				inline static constexpr REL::ID SelectEntry{ 51410 };
			}
		}
	}

	namespace EnchantmentItem
	{
		inline static constexpr REL::ID Create{ 36178 };
	}

	namespace ExtraDataList
	{
		inline static constexpr REL::ID SetCount{ 11617 };
	}

	namespace InventoryChanges
	{
		inline static constexpr REL::ID EnchantObject{ 16146 };
	}

	namespace InventoryEntryData
	{
		inline static constexpr REL::ID GetEnchantment{ 16026 };
	}

	namespace ItemCard
	{
		inline static constexpr REL::ID GetMagicItemDescription{ 51900 };
	}

	namespace MissileProjectile
	{
		inline static constexpr REL::ID UpdateImpl{ 44027 };
	}

	namespace NiAVObject
	{
		inline static constexpr REL::ID Clone{ 70187 };
		inline static constexpr REL::ID SetValueNodeHidden{ 19641 };
	}

	namespace PlayerCharacter
	{
		inline static constexpr REL::ID SwitchSkeleton{ 40476 };
		inline static constexpr REL::ID UseAmmo{ 41243 };
	}

	namespace Projectile
	{
		inline static constexpr REL::ID GetSpeed{ 44139 };
		inline static constexpr REL::ID Launch{ 44108 };
		inline static constexpr REL::ID Initialize{ 44120 };
	}

	namespace ScriptFunctions
	{
		inline static constexpr REL::ID PlayerEnchantObject{ 22507 };
	}

	namespace StandardItemData
	{
		inline static constexpr REL::ID Init{ 51011 };
	}

	namespace TaskQueueInterface
	{
		inline static constexpr REL::ID Attach3D{ 36897 };
	}

	namespace TESForm
	{
		inline static constexpr REL::ID GetEnchantment{ 14563 };
	}

	namespace TESObjectREFR
	{
		inline static constexpr REL::ID ApplyArtObject{ 22769 };
	}

	inline static constexpr REL::ID ShowHUDAmmo{ 51629 };
	inline static constexpr REL::ID UnarmedWeapon{ 401061 };
}
