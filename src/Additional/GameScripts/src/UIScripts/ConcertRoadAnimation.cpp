#include <UIScripts/ConcertRoadAnimation.h>
#include <core/Time.h>

#include <ConcertRoad.h>

#include <core/EasingFunctions.h>

#include <core/MathExtensions.h>

#include <glm/glm.hpp>

#include <string>
#include <format>

void ConcertRoadAnimation::Start(ConcertAbilityController* a) {
	if (_exit || _start) return;
	_start = true;
	_ability = a;
	_startAnimCounter = 0.f;
}

void ConcertRoadAnimation::Exit()
{
	_start = false;
	_exit = true;
	_exitAnimCounter = 0.f;
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	if (_timeText != nullptr) _timeText->SetText(converter.from_bytes(std::string("")));
	if (_explenationText != nullptr) _explenationText->SetText(converter.from_bytes(std::string("")));
	_endValue = ConcertRoad::instance->playerCount;
}

void ConcertRoadAnimation::Initialize() {}

void ConcertRoadAnimation::Update() {
	if (_ability != nullptr) {
		wstring_convert<codecvt_utf8<wchar_t>> converter;
		if (_start) {

			_startAnimCounter += Time::GetDeltaTime() * _startTimeFactor;

			if (_startAnimCounter > 1.f) {
				_startAnimCounter = 1.f;
			}

			if (_startAnimCounter <= 0.5f) {
				GetGameObject()->GetTransform()->SetLocalScale(glm::mix(glm::vec3(0.4f), glm::vec3(1.f), getEaseFuncValue(EaseFunction::BACK, EaseFunctionType::IN_OUT_F, fmapf(_startAnimCounter, 0.f, 0.5f, 0.f, 1.f))));
			}
			else {
				GetGameObject()->GetTransform()->SetLocalPosition(glm::mix(glm::vec3(0.f), glm::vec3(.7f, .6f, 0.f), getEaseFuncValue(EaseFunction::BACK, EaseFunctionType::IN_OUT_F, fmapf(_startAnimCounter, 0.5f, 1.f, 0.f, 1.f))));
				GetGameObject()->GetTransform()->SetLocalScale(glm::mix(glm::vec3(1.f), glm::vec3(.6f), getEaseFuncValue(EaseFunction::BACK, EaseFunctionType::IN_OUT_F, fmapf(_startAnimCounter, 0.5f, 1.f, 0.f, 1.f))));
			}

			float t = _ability->GetAbilityRemainingTime();
			if (_timeText != nullptr) _timeText->SetText(converter.from_bytes(std::vformat(std::string("{:.2f}s"), std::make_format_args(t))));
		}
		else if (_exit) {
			_exitAnimCounter += Time::GetDeltaTime() * _exitTimeFactor;

			if (_exitAnimCounter > 1.f) {
				_exitAnimCounter = 1.f;
			}

			if (_exitAnimCounter <= 0.5f) {
				if (_countObj != nullptr) _countObj->GetTransform()->SetLocalPosition(glm::mix(glm::vec3(0.f, -59.f, 0.f), glm::vec3(0.f), getEaseFuncValue(EaseFunction::BACK, EaseFunctionType::IN_OUT_F, fmapf(_exitAnimCounter, 0.f, 0.5f, 0.f, 1.f))));
			}

			if (_countObj != nullptr) _countObj->GetTransform()->SetLocalScale(glm::mix(glm::vec3(1.f), glm::vec3(2.f), getEaseFuncValue(EaseFunction::BACK, EaseFunctionType::IN_OUT_F, _exitAnimCounter)));
			GetGameObject()->GetTransform()->SetLocalPosition(glm::mix(glm::vec3(.7f, .6f, 0.f), glm::vec3(0.f), getEaseFuncValue(EaseFunction::BACK, EaseFunctionType::IN_OUT_F, _exitAnimCounter)));
			GetGameObject()->GetTransform()->SetLocalScale(glm::mix(glm::vec3(.6f), glm::vec3(1.f), getEaseFuncValue(EaseFunction::BACK, EaseFunctionType::IN_OUT_F, _exitAnimCounter)));

			if (_exitAnimCounter >= 1.f) {
				_exitWaitTime -= Time::GetDeltaTime();

				if (_exitWaitTime <= 0.f) {
					SceneManager::DestroyGameObject(GetGameObject());
					return;
				}
			}
		}

		if (_countText != nullptr) {
			if (_start) {
				_countText->SetText(converter.from_bytes(std::vformat(std::string("{} / {}"), std::make_format_args(ConcertRoad::instance->playerCount, _numOfPoints))));
			}
			else if (_exit) {
				_countText->SetText(converter.from_bytes(std::vformat(std::string("{} / {}"), std::make_format_args(_endValue, _numOfPoints))));
			}
		}
	}
}

void ConcertRoadAnimation::OnDestroy() {}

YAML::Node ConcertRoadAnimation::Serialize() const {
	YAML::Node node = Component::Serialize();
	node["type"] = "ConcertRoadAnimation";
	node["startTimeFactor"] = _startTimeFactor;
	node["exitTimeFactor"] = _exitTimeFactor;
	node["countObjId"] = _countObj != nullptr ? _countObj->Id() : 0;
	node["countTextId"] = _countText != nullptr ? _countText->GetId() : 0;
	node["timeTextId"] = _timeText != nullptr ? _timeText->GetId() : 0;
	node["explenationTextId"] = _explenationText != nullptr ? _explenationText->GetId() : 0;

	return node;
}

bool ConcertRoadAnimation::Deserialize(const YAML::Node& node) {
	if (!node["startTimeFactor"] || !node["exitTimeFactor"] || !node["countObjId"] || !node["countTextId"] || !node["timeTextId"] || !node["explenationTextId"] || !Component::Deserialize(node))
		return false;

	_startTimeFactor = node["startTimeFactor"].as<float>();
	_exitTimeFactor = node["exitTimeFactor"].as<float>();

	size_t id = node["countObjId"].as<size_t>();
	_countObj = id != 0 ? SceneManager::GetGameObjectWithId(id) : nullptr;

	id = node["countTextId"].as<size_t>();
	_countText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;

	id = node["timeTextId"].as<size_t>();
	_timeText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;

	id = node["explenationTextId"].as<size_t>();
	_explenationText = id != 0 ? (Text*)SceneManager::GetComponentWithId(id) : nullptr;

	return true;
}

#if _DEBUG
void ConcertRoadAnimation::DrawEditor() {
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("ConcertRoadAnimation##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		auto items = SceneManager::GetComponentsOfType<Text>();
		size_t choosed_1 = _countText == nullptr ? 0 : _countText->GetId();

		if (ImGui::BeginCombo(string("CountText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

					if (clicked) continue;

					choosed_1 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_1 != 0) {
					_countText = static_cast<Text*>(items[choosed_1]);
				}
			}

			ImGui::EndCombo();
		}

		items = SceneManager::GetComponentsOfType<Text>();
		choosed_1 = _timeText == nullptr ? 0 : _timeText->GetId();

		if (ImGui::BeginCombo(string("TimeText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

					if (clicked) continue;

					choosed_1 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_1 != 0) {
					_timeText = static_cast<Text*>(items[choosed_1]);
				}
			}

			ImGui::EndCombo();
		}

		items = SceneManager::GetComponentsOfType<Text>();
		choosed_1 = _explenationText == nullptr ? 0 : _explenationText->GetId();

		if (ImGui::BeginCombo(string("ExplenationText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

					if (clicked) continue;

					choosed_1 = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_1 != 0) {
					_explenationText = static_cast<Text*>(items[choosed_1]);
				}
			}

			ImGui::EndCombo();
		}

		items.clear();
	}
}
#endif