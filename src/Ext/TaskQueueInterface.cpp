#include "TaskQueueInterface.h"

#include "RE/Offset.h"

namespace Ext
{
	void TaskQueueInterface::Attach3D(
		RE::TaskQueueInterface* a_taskQueueInterface,
		RE::NiAVObject* a_obj,
		RE::NiAVObject* a_root,
		bool a_arg3,
		bool a_arg4)
	{
		using func_t = decltype(&Attach3D);
		static REL::Relocation<func_t> func{ RE::Offset::TaskQueueInterface::Attach3D };
		return func(a_taskQueueInterface, a_obj, a_root, a_arg3, a_arg4);
	}
}
