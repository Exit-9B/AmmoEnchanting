#pragma once

#include "PCH/PCH.h"
#include "RE/Offset.h"

namespace RE
{
	namespace BSResource
	{
		struct ModelID : public ID
		{
		public:
			TES_HEAP_REDEFINE_NEW();

			// members
			std::int32_t          unk0C;
			void*                 unk10;
			std::int64_t          unk18;
			void*                 unk20;
			NiPointer<NiAVObject> data;
		};
		static_assert(sizeof(ModelID) == 0x30);

		struct ModelLoadParameters
		{
			std::uint32_t unk0{ 0 };      // 0
			std::uint32_t unk4{ 3 };      // 4
			bool          unk8{ true };   // 8
			bool          unk9{ false };  // 9
			bool          unkA{ true };   // A
			bool          unkB{ true };   // B
		};
		static_assert(sizeof(ModelLoadParameters) == 0xC);

		std::int32_t RequestModelDirect(
			const char* a_path,
			ModelID*& a_modelOut,
			const ModelLoadParameters& a_params = {});

		void FreeRequestedModel(ModelID*& a_model);
	}
}
