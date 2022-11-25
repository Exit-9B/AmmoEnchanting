#pragma once

namespace Ext
{
	namespace TaskQueueInterface
	{
		void Attach3D(
			RE::TaskQueueInterface* a_taskQueueInterface,
			RE::NiAVObject* a_obj,
			RE::NiAVObject* a_root,
			bool a_arg3 = true,
			bool a_arg4 = false);
	}
}
