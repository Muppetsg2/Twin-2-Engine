#include <UIScripts/PatronChoicePanelController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

void PatronChoicePanelController::Initialize()
{

    //size_t size = _patrons.size();
    size_t size = _patronsButtons.size();

    //wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    wstring_convert<codecvt_utf8<wchar_t>> converter;

    for (size_t index = 0ull; index < size; ++index)
    {
        _patronsButtons[index]->GetTransform()->GetChildAt(2ull)->GetGameObject()->GetComponent<Text>()
                              ->SetText(converter.from_bytes(_patrons[index]->GetPatronName()));
        
        
        string description;
        description.append(_patrons[index]->GetPatronDescription()).append("\n").append(_patrons[index]->GetBonusAsString());
        
        _patronsButtons[index]->GetTransform()->GetChildAt(1ull)->GetGameObject()->GetComponent<Text>()
                              ->SetText(converter.from_bytes(description));
        
        _patronsButtons[index]->GetOnClickEvent().AddCallback([this, index]() -> void {
            SPDLOG_INFO("Index chosen: {}", index);
            Choose(_patrons[index]);
            });
    }
}

void PatronChoicePanelController::Choose(PatronData* patron)
{
    GameManager::instance->playersPatron = patron;
    GetGameObject()->SetActive(false);
    GameManager::instance->StartGame();
}

YAML::Node PatronChoicePanelController::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "PatronChoicePanelController";

    vector<string> patronDataPaths;
    patronDataPaths.reserve(_patrons.size());
    for (size_t index = 0ull; index < _patrons.size(); ++index)
    {
        patronDataPaths.push_back(ScriptableObjectManager::GetPath(_patrons[index]->GetId()));
    }
    node["patrons"] = patronDataPaths;
    
    vector<size_t> buttonsIds;
    buttonsIds.reserve(_patronsButtons.size());
    for (size_t index = 0ull; index < _patronsButtons.size(); ++index)
    {
        buttonsIds.push_back(_patronsButtons[index]->GetId());
    }
    node["patronsButtons"] = buttonsIds;


    return node;
}

bool PatronChoicePanelController::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) return false;

    _patrons.reserve(node["patrons"].size());
    for (size_t index = 0ull; index < node["patrons"].size(); ++index)
    {
        _patrons.push_back((PatronData*)ScriptableObjectManager::Get(node["patrons"][index].as<string>()));
    }
    
    _patronsButtons.reserve(node["patronsButtons"].size());
    for (size_t index = 0ull; index < node["patronsButtons"].size(); ++index)
    {
        _patronsButtons.push_back((Button*)SceneManager::GetComponentWithId(node["patronsButtons"][index].as<size_t>()));
    }

    return true;
}

#if _DEBUG

bool PatronChoicePanelController::DrawInheritedFields()
{
    if (Component::DrawInheritedFields()) return true;



    return false;
}

void PatronChoicePanelController::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("PatronChoicePanelController##Component").append(id);

    if (ImGui::CollapsingHeader(name.c_str())) {

        if (DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}

#endif