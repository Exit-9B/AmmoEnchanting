#include "BSResource.h"

#include "RE/Offset.h"

namespace RE
{
	std::int32_t BSResource::RequestModelDirect(
		const char* a_path,
		ModelID*& a_modelOut,
		const ModelLoadParameters& a_params)
	{
		using func_t = decltype(&RequestModelDirect);
		REL::Relocation<func_t> func{ RE::Offset::BSResource::RequestModelDirect };
		return func(a_path, a_modelOut, a_params);
	}

	void BSResource::FreeRequestedModel(ModelID*& a_model)
	{
		using func_t = decltype(&FreeRequestedModel);
		REL::Relocation<func_t> func{ RE::Offset::BSResource::FreeRequestedModel };
		return func(a_model);
	}
}
