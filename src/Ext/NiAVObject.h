#pragma once

namespace Ext
{
	namespace NiAVObject
	{
		RE::NiAVObject* Clone(RE::NiAVObject* a_obj);

		void SetValueNodeHidden(RE::NiAVObject* a_obj, bool a_hidden);
	}
}
