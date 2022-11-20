#pragma once

namespace RE
{
	class AmmoHUDData : public IUIMessageData
	{
	public:
		~AmmoHUDData() override;  // 00

		// members
		std::uint32_t type;       // 10
		std::uint32_t pad14;      // 14
		BSString      text;       // 18
		std::uint32_t unk28;      // 28
		std::uint32_t pad2C;      // 2C
		void*         unk30;      // 30
		std::uint64_t unk38;      // 38
		std::uint8_t  unk40;      // 40
		std::uint8_t  pad41;      // 41
		std::uint16_t pad42;      // 42
		std::int32_t  count;      // 44
	};
	static_assert(sizeof(AmmoHUDData) == 0x48);
}
