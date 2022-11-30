#include "Translation.h"

namespace Translation
{
	bool ScaleformTranslate(const std::string& a_key, std::string& a_result)
	{
		if (!a_key.starts_with('$')) {
			return false;
		}

		const auto scaleformManager = RE::BSScaleformManager::GetSingleton();
		const auto loader = scaleformManager ? scaleformManager->loader : nullptr;
		const auto translator = loader
			? loader->GetState<RE::GFxTranslator>(RE::GFxState::StateType::kTranslator)
			: nullptr;

		if (!translator) {
			logger::warn("Failed to get Scaleform translator"sv);
			return false;
		}

		std::wstring key_utf16 = util::utf8_to_utf16(a_key).value_or(L""s);
		RE::GFxWStringBuffer result;

		RE::GFxTranslator::TranslateInfo translateInfo;
		translateInfo.key = key_utf16.c_str();
		translateInfo.result = std::addressof(result);

		translator->Translate(std::addressof(translateInfo));
		if (result.empty()) {
			return false;
		}

		std::string result_utf8 = util::utf16_to_utf8(result.c_str()).value_or(""s);

		a_result = result_utf8;
		return true;
	}
}
