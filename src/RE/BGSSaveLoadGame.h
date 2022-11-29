#pragma once

namespace RE
{
	class BGSSaveLoadGame
	{
	public:
		static BGSSaveLoadGame* GetSingleton();

		void ClearForm(TESForm* a_form);

		// members
		std::uint64_t unk000[105];
	};
	static_assert(sizeof(BGSSaveLoadGame) == 0x348);
}
