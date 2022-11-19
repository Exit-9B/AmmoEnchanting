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
		// Change confirmation message when not enough arrows
		static void EnchantConfirmPatch();
		// Enchant multiple arrows at once
		static void AmmoQuantityPatch();
		// Show multiple items being added
		static void InventoryNotificationPatch();

		static RE::EnchantmentItem* CreateEnchantment(
			RE::FormType a_formType,
			RE::CraftingSubMenus::EnchantConstructMenu* a_menu);

		static void SetConfirmText(RE::BSString* a_str, const char* a_sEnchantItem);

		static std::uint16_t GetCount(RE::ExtraDataList* a_extraList);

		static void SetBaseItemCount(RE::ExtraDataList* a_extraList, std::uint16_t a_count);

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

		inline static const char* customName = nullptr;
		inline static std::uint16_t availableCount = 0;
		inline static std::uint16_t creatingCount = 1;

		inline static REL::Relocation<decltype(&SetConfirmText)> _SetStr;
		inline static REL::Relocation<decltype(&GetCount)> _GetCount;
		inline static REL::Relocation<decltype(&SetBaseItemCount)> _SetBaseItemCount;
		inline static REL::Relocation<decltype(&SetExtraData)> _SetEnchantment;
		inline static REL::Relocation<decltype(&InventoryNotification)> _InventoryNotification;
	};
}
