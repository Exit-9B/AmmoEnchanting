#include "RE/BGSSaveLoadGame.h"

#include "RE/Offset.h"

namespace RE
{
	BGSSaveLoadGame* BGSSaveLoadGame::GetSingleton()
	{
		REL::Relocation<BGSSaveLoadGame**> singleton{ Offset::BGSSaveLoadGame::Singleton };
		return *singleton;
	}

	void BGSSaveLoadGame::ClearForm(TESForm* a_form)
	{
		using func_t = decltype(&BGSSaveLoadGame::ClearForm);
		REL::Relocation<func_t> func{ Offset::BGSSaveLoadGame::ClearForm };
		return func(this, a_form);
	}
}
