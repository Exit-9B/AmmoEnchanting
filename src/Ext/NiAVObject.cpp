#include "NiAVObject.h"

#include "RE/Offset.h"

namespace Ext
{
	RE::NiAVObject* NiAVObject::Clone(RE::NiAVObject* a_obj)
	{
		using func_t = decltype(&Clone);
		static REL::Relocation<func_t> func{ RE::Offset::NiAVObject::Clone };
		return func(a_obj);
	}

	void NiAVObject::SetValueNodeHidden(RE::NiAVObject* a_obj, bool a_hidden)
	{
		using func_t = decltype(&SetValueNodeHidden);
		static REL::Relocation<func_t> func{ RE::Offset::NiAVObject::SetValueNodeHidden };
		return func(a_obj, a_hidden);
	}
}
