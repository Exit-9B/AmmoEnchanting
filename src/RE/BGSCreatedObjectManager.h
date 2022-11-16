#pragma once

namespace RE
{
	class BGSCreatedObjectManager
	{
	public:
		struct EnchantmentEntry
		{
			EnchantmentItem*       enchantment;  // 00
			volatile std::uint32_t refCount;     // 08
			std::uint32_t          pad0C;        // 0C
		};
		static_assert(sizeof(EnchantmentEntry) == 0x10);

		struct UnkData
		{
			void*         unk00;  // 00
			std::uint64_t unk08;  // 08
			void*         unk10;  // 10
			std::uint64_t unk18;  // 18
			std::uint32_t unk20;  // 20
			std::uint32_t unk24;  // 24
			std::uint32_t unk28;  // 28
			std::uint32_t unk2C;  // 2C
		};
		static_assert(sizeof(UnkData) == 0x30);

		static BGSCreatedObjectManager* GetSingleton();

		EnchantmentItem* CreateArmorEnchantment(BSTArray<Effect>& a_effects);
		EnchantmentItem* CreateWeaponEnchantment(BSTArray<Effect>& a_effects);

		// members
		std::uint64_t              unk00;               // 00
		BSTArray<EnchantmentEntry> weaponEnchantments;  // 08
		BSTArray<EnchantmentEntry> armorEnchantments;   // 20
		std::uint64_t              pad38;               // 38
		std::uint32_t              pad40;               // 40
		std::uint32_t              unk44;               // 44
		std::uint32_t              unk48;               // 48
		std::uint32_t              unk4C;               // 4C
		UnkData                    unk50;               // 50
		UnkData                    unk80;               // 80
		void*                      unkB0;               // B0
		std::uint64_t              padB8;               // B8
		std::uint64_t              unkC0;               // C0
		mutable BSSpinLock         lock;                // C8
	};
	static_assert(sizeof(BGSCreatedObjectManager) == 0xD0);
}
