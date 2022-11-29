#include "CreatedObjectManager.h"

#include "RE/BGSSaveLoadGame.h"

namespace Data
{
	void CreatedObjectManager::AddFailedEnchantmentLoad(
		RE::ExtraEnchantment* a_exEnchantment,
		RE::FormID a_formID)
	{
		failedFormLoads[a_exEnchantment] = a_formID;
	}

	bool CreatedObjectManager::Save(SKSE::SerializationInterface* a_intfc) const
	{
		if (!a_intfc->WriteRecordData(baseExplosions.size())) {
			logger::error("Failed to write size of baseExplosions"sv);
			return false;
		}

		for (const auto& [effect, explosion] : baseExplosions) {
			if (!a_intfc->WriteRecordData(effect->formID)) {
				logger::error("Failed to write FormID ({:08X})"sv, effect->formID);
				return false;
			}

			if (!a_intfc->WriteRecordData(explosion->formID)) {
				logger::error("Failed to write FormID ({:08X})"sv, explosion->formID);
				return false;
			}
		}

		if (!a_intfc->WriteRecordData(ammoEnchantments.size())) {
			logger::error("Failed to write size of ammoEnchantments"sv);
			return false;
		}

		for (const auto& entry : ammoEnchantments) {

			RE::BGSExplosion* createdExplosion;
			try {
				createdExplosion = createdExplosions.at(entry.enchantment);
			}
			catch (std::out_of_range&) {
				logger::error("Failed to look up created explosion"sv);
				return false;
			}

			if (!a_intfc->WriteRecordData(static_cast<std::uint32_t>(entry.refCount))) {
				logger::error("Failed to write refCount ({})"sv, entry.enchantment->formID);
				return false;
			}

			if (!SaveEnchantment(entry.enchantment, a_intfc)) {
				logger::error("Failed to save enchantment"sv);
				return false;
			}

			if (!a_intfc->WriteRecordData(createdExplosion->formID)) {
				logger::error("Failed to write FormID ({:08X})"sv, createdExplosion->formID);
				return false;
			}
		}

		return true;
	}

	bool CreatedObjectManager::Load(SKSE::SerializationInterface* a_intfc)
	{
		Revert();

		std::size_t baseExplosionsSize;
		if (!a_intfc->ReadRecordData(baseExplosionsSize)) {
			logger::error("Failed to read size of baseExplosions"sv);
			return false;
		}

		for (std::size_t i = 0; i < baseExplosionsSize; i++) {

			RE::FormID effectOldFormID;
			if (!a_intfc->ReadRecordData(effectOldFormID)) {
				logger::error("Failed to read FormID"sv);
				return false;
			}

			RE::FormID effectFormID;
			if (!a_intfc->ResolveFormID(effectOldFormID, effectFormID)) {
				logger::error("Failed to resolve FormID ({:08X})"sv, effectOldFormID);
				return false;
			}

			RE::FormID explosionOldFormID;
			if (!a_intfc->ReadRecordData(explosionOldFormID)) {
				logger::error("Failed to read FormID"sv);
				return false;
			}

			RE::FormID explosionFormID;
			if (!a_intfc->ResolveFormID(explosionOldFormID, explosionFormID)) {
				logger::error("Failed to resolve FormID ({:08X})"sv, explosionOldFormID);
				return false;
			}

			auto effect = RE::TESForm::LookupByID<RE::EffectSetting>(effectFormID);
			auto explosion = RE::TESForm::LookupByID<RE::BGSExplosion>(explosionFormID);

			if (effect && explosion) {
				baseExplosions.insert({ effect, explosion });
			}
		}

		RE::BSTArrayBase::size_type ammoEnchantmentsSize;
		if (!a_intfc->ReadRecordData(ammoEnchantmentsSize)) {
			logger::error("Failed to read size of ammoEnchantments"sv);
			return false;
		}

		for (RE::BSTArrayBase::size_type i = 0; i < ammoEnchantmentsSize; i++) {
			EnchantmentEntry entry{};

			std::uint32_t refCount;
			if (!a_intfc->ReadRecordData(refCount)) {
				logger::error("Failed to read refCount"sv);
				return false;
			}

			entry.refCount = refCount;

			RE::EnchantmentItem* enchantment;
			if (!LoadEnchantment(enchantment, a_intfc)) {
				logger::error("Failed to load enchantment"sv);
				return false;
			}

			const auto costliestEffect = enchantment->GetCostliestEffectItem()->baseEffect;
			if (!costliestEffect) {
				logger::error(
					"Failed to look up base effect of enchantment ({:08X})"sv,
					enchantment->formID);
			}


			RE::FormID explosionFormID;
			if (!a_intfc->ReadRecordData(explosionFormID)) {
				logger::error("Failed to read FormID"sv);
				return false;
			}

			RE::BGSExplosion* explosion = nullptr;
			RE::TESForm* explosionForm = RE::TESForm::LookupByID(explosionFormID);
			if (explosionForm) {
				explosion = explosionForm->As<RE::BGSExplosion>();
				if (!explosion) {
					RE::BGSSaveLoadGame::GetSingleton()->ClearForm(explosionForm);
				}
			}
			if (!explosion) {
				RE::BGSExplosion* baseExplosion;
				try {
					baseExplosion = baseExplosions.at(costliestEffect);
				}
				catch (std::out_of_range&) {
					logger::error("Failed to look up base explosion"sv);
					return false;
				}

				explosion = EnchantExplosion(baseExplosion, enchantment, explosionFormID);
			}

			entry.enchantment = enchantment;

			ammoEnchantments.push_back(std::move(entry));
			createdExplosions.insert({ enchantment, explosion });
		}

		for (auto& [exEnchantment, formID] : failedFormLoads) {
			exEnchantment->enchantment = RE::TESForm::LookupByID<RE::EnchantmentItem>(formID);
		}
		failedFormLoads.clear();

		return true;
	}

	void CreatedObjectManager::Revert()
	{
		ammoEnchantments.clear();
		baseExplosions.clear();
		createdExplosions.clear();
	}

	bool CreatedObjectManager::SaveEnchantment(
		const RE::EnchantmentItem* a_enchantment,
		SKSE::SerializationInterface* a_intfc)
	{
		if (!a_intfc->WriteRecordData(a_enchantment->formID)) {
			logger::error("Failed to write FormID ({:08X})"sv, a_enchantment->formID);
			return false;
		}

		if (!a_intfc->WriteRecordData(a_enchantment->effects.size())) {
			logger::error("Failed to write size of enchantment effects"sv);
			return false;
		}

		for (auto& effect : a_enchantment->effects) {
			if (!a_intfc->WriteRecordData(effect->effectItem.magnitude)) {
				logger::error("Failed to write effect magnitude"sv);
				return false;
			}
			if (!a_intfc->WriteRecordData(effect->effectItem.area)) {
				logger::error("Failed to write effect area"sv);
				return false;
			}
			if (!a_intfc->WriteRecordData(effect->effectItem.duration)) {
				logger::error("Failed to write effect duration"sv);
				return false;
			}
			if (!a_intfc->WriteRecordData(effect->baseEffect->formID)) {
				logger::error("Failed to write FormID ({:08X})"sv, effect->baseEffect->formID);
				return false;
			}
			if (!a_intfc->WriteRecordData(effect->cost)) {
				logger::error("Failed to write effect cost"sv);
				return false;
			}

			RE::TESConditionItem* item;
			for (item = effect->conditions.head; item; item = item->next) {
				if (!a_intfc->WriteRecordData(&item, sizeof(void*))) {
					logger::error("Failed to write condition item"sv);
					return false;
				}
				if (!a_intfc->WriteRecordData(item->data)) {
					logger::error("Failed to write condition data"sv);
					return false;
				}
			}

			if (!a_intfc->WriteRecordData(&item, sizeof(void*))) {
				logger::error("Failed to write condition end"sv);
				return false;
			}
		}

		return true;
	}

	bool CreatedObjectManager::LoadEnchantment(
		RE::EnchantmentItem*& a_enchantment,
		SKSE::SerializationInterface* a_intfc)
	{
		RE::FormID formID;
		if (!a_intfc->ReadRecordData(formID)) {
			logger::error("Failed to read FormID"sv);
			return false;
		}

		RE::BSTArray<RE::Effect> effects;

		RE::BSTArrayBase::size_type effectsSize;
		if (!a_intfc->ReadRecordData(effectsSize)) {
			logger::error("Failed to read size of enchantment effects"sv);
			return false;
		}

		effects.resize(effectsSize);

		for (RE::BSTArrayBase::size_type i = 0; i < effectsSize; i++) {
			auto& effect = effects[i];

			if (!a_intfc->ReadRecordData(effect.effectItem.magnitude)) {
				logger::error("Failed to read effect magnitude"sv);
				return false;
			}
			if (!a_intfc->ReadRecordData(effect.effectItem.area)) {
				logger::error("Failed to read effect area"sv);
				return false;
			}
			if (!a_intfc->ReadRecordData(effect.effectItem.duration)) {
				logger::error("Failed to read effect duration"sv);
				return false;
			}
			RE::FormID baseEffectOldFormID;
			if (!a_intfc->ReadRecordData(baseEffectOldFormID)) {
				logger::error("Failed to read FormID"sv);
				return false;
			}
			RE::FormID baseEffectFormID;
			if (!a_intfc->ResolveFormID(baseEffectOldFormID, baseEffectFormID)) {
				logger::error("Failed to resolve FormID ({:08X})"sv, baseEffectOldFormID);
				return false;
			}
			effect.baseEffect = RE::TESForm::LookupByID<RE::EffectSetting>(baseEffectFormID);
			if (!a_intfc->ReadRecordData(effect.cost)) {
				logger::error("Failed to read effect cost"sv);
				return false;
			}

			std::uintptr_t conditionItem;
			if (!a_intfc->ReadRecordData(conditionItem)) {
				logger::error("Failed to read condition head"sv);
				return false;
			}
			if (conditionItem) {
				auto item = effect.conditions.head = new RE::TESConditionItem();
				while (true) {
					if (!a_intfc->ReadRecordData(item->data)) {
						logger::error("Failed to read condition data"sv);
						return false;
					}
					if (!a_intfc->ReadRecordData(conditionItem)) {
						logger::error("Failed to read condition item"sv);
						return false;
					}
					if (!conditionItem)
						break;

					item = item->next = new RE::TESConditionItem();
				}
			}
			else {
				effect.conditions.head = nullptr;
			}
		}

		a_enchantment = nullptr;
		if (const auto enchantmentForm = RE::TESForm::LookupByID(formID)) {
			a_enchantment = enchantmentForm->As<RE::EnchantmentItem>();
			if (!a_enchantment) {
				RE::BGSSaveLoadGame::GetSingleton()->ClearForm(enchantmentForm);
			}
		}
		if (!a_enchantment) {
			a_enchantment = CreateEnchantment(effects, true);
			a_enchantment->SetFormID(formID, false);
		}

		return a_enchantment != nullptr;
	}
}
