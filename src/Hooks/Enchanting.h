#pragma once

namespace Hooks
{
	class Enchanting
	{
	public:
		using Menu = RE::CraftingSubMenus::EnchantConstructMenu;

		Enchanting() = delete;

		static void Install();

	private:
		// Create enchantments for Ammo items for temporary preview item
		static void ItemPreviewPatch();
		// Create enchantments for Ammo items when crafting item
		static void CraftItemPatch();
		// Attach enchant art and not casting art when enchanting worn ammo
		static void EnchantArtPatch();
		// Change confirmation message when not enough arrows
		static void EnchantConfirmPatch();
		// Enchant multiple arrows at once
		static void AmmoQuantityPatch();
		// Show multiple items being added
		static void InventoryNotificationPatch();
		// Modify enchantment charges
		static void EnchantmentChargePatch();

		static RE::EnchantmentItem* CreateEnchantment(
			RE::FormType a_formType,
			RE::CraftingSubMenus::EnchantConstructMenu* a_menu);

		static void UpdateWeaponAbility(
			RE::Actor* a_actor,
			RE::TESForm* a_item,
			RE::ExtraDataList* a_extraList,
			bool a_wornLeft);

		static void SetConfirmText(RE::BSString* a_str, const char* a_sEnchantItem);

		static std::uint16_t GetCount(RE::ExtraDataList* a_extraList);

		static void CopyExtraData(RE::ExtraDataList* a_target, RE::ExtraDataList* a_source);

		static void SetExtraData(
			RE::ExtraDataList* a_extraList,
			RE::EnchantmentItem* a_enchantment,
			std::uint16_t a_chargeAmount,
			bool a_removeOnUnequip);

		static void InventoryNotification(
			RE::TESForm* a_item,
			std::int32_t a_count,
			bool a_itemAdded,
			bool a_playSound,
			const char* a_name);

		static void ApplyPerkEntries(
			RE::BGSEntryPoint::ENTRY_POINT a_entryPoint,
			RE::Actor* a_perkOwner,
			RE::EnchantmentItem* a_enchantment,
			RE::TESForm* a_item,
			float& a_value);

		inline static const char* _customName = nullptr;
		inline static std::uint16_t _availableCount = 0;
		inline static std::uint16_t _creatingCount = 1;

		inline static REL::Relocation<decltype(&UpdateWeaponAbility)> _UpdateWeaponAbility;
		inline static REL::Relocation<decltype(&SetConfirmText)> _SetStr;
		inline static REL::Relocation<decltype(&GetCount)> _GetCount;
		inline static REL::Relocation<decltype(&CopyExtraData)> _CopyExtraData;
		inline static REL::Relocation<decltype(&SetExtraData)> _SetEnchantment;
		inline static REL::Relocation<decltype(&InventoryNotification)> _InventoryNotification;
		inline static REL::Relocation<decltype(&ApplyPerkEntries)> _ApplyPerkEntries;
	};
}
