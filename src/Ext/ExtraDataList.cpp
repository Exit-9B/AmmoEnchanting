#include "ExtraDataList.h"

#include "RE/Offset.h"

namespace Ext
{
	void ExtraDataList::SetCount(RE::ExtraDataList* a_extraList, std::uint16_t a_count)
	{
		static REL::Relocation<decltype(&SetCount)> func{ RE::Offset::ExtraDataList::SetCount };
		return func(a_extraList, a_count);
	}
}
