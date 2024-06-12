#include <UIScripts/PopularityGainingBonusBarController.h>


using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

PopularityGainingBonusBarController* PopularityGainingBonusBarController::_instance = nullptr;

PopularityGainingBonusBarController* PopularityGainingBonusBarController::Instance()
{
	return _instance;
}

void PopularityGainingBonusBarController::Initialize()
{
	_currentBonusBar = GetTransform()->GetChildAt(0ull)->GetGameObject()->GetComponent<Image>();
	_possibleBonusBar = GetTransform()->GetChildAt(1ull)->GetGameObject()->GetComponent<Image>();

	if (!_instance)
	{
		_instance = this;
	}
}

void PopularityGainingBonusBarController::OnDestroy()
{
	if (_instance == this)
	{
		_instance = nullptr;
	}
}

void PopularityGainingBonusBarController::SetCurrentBonus(float bonus)
{
	_currentBonus = bonus;

	_currentBonusCurrentLength = _currentBonus / _maxBonus * _maxLength;
	_currentBonusBar->SetWidth(_currentBonusCurrentLength);
	_currentBonusBar->GetTransform()->SetLocalPosition(vec3((_currentBonusCurrentLength - _maxLength) * 0.5f, 0.0f, 0.0f));
}

void PopularityGainingBonusBarController::AddCurrentBonus(float additionalCurrentBonus)
{
	_currentBonus += additionalCurrentBonus;

	_currentBonusCurrentLength = _currentBonus / _maxBonus * _maxLength;
	_currentBonusBar->SetWidth(_currentBonusCurrentLength);
	_currentBonusBar->GetTransform()->SetLocalPosition(vec3((_currentBonusCurrentLength - _maxLength) * 0.5f, 0.0f, 0.0f));
}

void PopularityGainingBonusBarController::RemoveCurrentBonus(float removedCurrentBonus)
{
	_currentBonus -= removedCurrentBonus;

	_currentBonusCurrentLength = _currentBonus / _maxBonus * _maxLength;
	_currentBonusBar->SetWidth(_currentBonusCurrentLength);
	_currentBonusBar->GetTransform()->SetLocalPosition(vec3((_currentBonusCurrentLength - _maxLength) * 0.5f, 0.0f, 0.0f));
}


void PopularityGainingBonusBarController::AddPossibleBonus(float additionalPossibleBonus)
{
	_possibleBonus += additionalPossibleBonus;

	_possibleBonusCurrentLength = _possibleBonus / _maxBonus * _maxLength;
	_possibleBonusBar->SetWidth(_possibleBonusCurrentLength);
	_possibleBonusBar->GetTransform()->SetLocalPosition(vec3((_possibleBonusCurrentLength - _maxLength) * 0.5f + _currentBonusCurrentLength, 0.0f, 0.0f));
}

void PopularityGainingBonusBarController::RemovePossibleBonus(float removedPossibleBonus)
{
	_possibleBonus -= removedPossibleBonus;

	_possibleBonusCurrentLength = _possibleBonus / _maxBonus * _maxLength;
	_possibleBonusBar->SetWidth(_possibleBonusCurrentLength);
	_possibleBonusBar->GetTransform()->SetLocalPosition(vec3((_possibleBonusCurrentLength - _maxLength) * 0.5f + _currentBonusCurrentLength, 0.0f, 0.0f));
}

YAML::Node PopularityGainingBonusBarController::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "PopularityGainingBonusBarController";
    node["maxBonus"] = _maxBonus;
    node["maxLength"] = _maxLength;

    return node;
}

bool PopularityGainingBonusBarController::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) return false;

	_maxBonus = node["maxBonus"].as<float>();
	_maxLength = node["maxLength"].as<float>();

    return true;
}

#if _DEBUG

bool PopularityGainingBonusBarController::DrawInheritedFields()
{
    if (Component::DrawInheritedFields()) return true;

    bool changed = ImGui::InputFloat("MaxBonus", &_maxBonus);
	changed = ImGui::InputFloat("MaxLength", &_maxLength);

	if (changed)
	{
		_currentBonusCurrentLength = _currentBonus / _maxBonus * _maxLength;
		_currentBonusBar->SetWidth(_currentBonusCurrentLength);
		_currentBonusBar->GetTransform()->SetLocalPosition(vec3((_currentBonusCurrentLength - _maxLength) * 0.5f, 0.0f, 0.0f));
		_possibleBonusCurrentLength = _possibleBonus / _maxBonus * _maxLength;
		_possibleBonusBar->SetWidth(_possibleBonusCurrentLength);
		_possibleBonusBar->GetTransform()->SetLocalPosition(vec3((_possibleBonusCurrentLength - _maxLength) * 0.5f + _currentBonusCurrentLength, 0.0f, 0.0f));

	}

	ImGui::Text("CurrentBonus: %f", _currentBonus);
	ImGui::Text("CurrentBonusCurrentLength: %f", _currentBonusCurrentLength);
	ImGui::Text("PossibleBonus: %f", _possibleBonus);
	ImGui::Text("PossibleBonusCurrentLength: %f", _possibleBonusCurrentLength);

    return false;
}

void PopularityGainingBonusBarController::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("PopularityGainingBonusBarController##Component").append(id);

    if (ImGui::CollapsingHeader(name.c_str())) {

        if (DrawInheritedFields()) return;
    }
}

#endif