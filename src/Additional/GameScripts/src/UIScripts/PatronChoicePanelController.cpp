#include <UIScripts/PatronChoicePanelController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

void PatronChoicePanelController::Initialize()
{

    // size_t size = _patrons.size();
    size_t size = _patronsButtons.size();

    // wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    wstring_convert<codecvt_utf8<wchar_t>> converter;

    FirstStepTutorial = SceneManager::FindObjectByName("FristStepTutorial");

    for (size_t index = 0ull; index < size; ++index)
    {
        _patronsButtons[index]->GetTransform()->GetChildAt(2ull)->GetGameObject()->GetComponent<Text>()->SetText(converter.from_bytes(_patrons[index]->GetPatronName()));

        string description;
        description.append(_patrons[index]->GetPatronDescription()).append("\n").append(_patrons[index]->GetBonusAsString());

        _patronsButtons[index]->GetTransform()->GetChildAt(1ull)->GetGameObject()->GetComponent<Text>()->SetText(converter.from_bytes(description));

        _patronsButtons[index]->GetOnClickEvent().AddCallback([this, index]() -> void
                                                              {
            SPDLOG_INFO("Index chosen: {}", index);
            Choose(_patrons[index]);
            if (FirstStepTutorial != nullptr) {
                FirstStepTutorial->SetActive(true);
            }
            });
    }
}

void PatronChoicePanelController::Choose(PatronData *patron)
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

bool PatronChoicePanelController::Deserialize(const YAML::Node &node)
{
    if (!node["patrons"] || !node["patronsButtons"] || !Component::Deserialize(node))
        return false;

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

void PatronChoicePanelController::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("PatronChoicePanelController##Component").append(id);

    if (ImGui::CollapsingHeader(name.c_str()))
    {
        if (Component::DrawInheritedFields()) return;

        unordered_map<size_t, Component*> btns = SceneManager::GetComponentsOfType<Button>();
        std::map<size_t, ScriptableObject*> datas = ScriptableObjectManager::GetScriptableObjectsDerivedByType<PatronData>();

        for (size_t i = 0; i < _patronsButtons.size(); ++i) {
            size_t choosed_btn = _patronsButtons[i] == nullptr ? 0 : _patronsButtons[i]->GetId();
            size_t choosed_data = _patrons[i] == nullptr ? 0 : _patrons[i]->GetId();

            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);
            if (ImGui::BeginCombo(string("##Button_Patron").append(id).append(std::to_string(i)).c_str(), choosed_btn == 0 ? "None" : btns[choosed_btn]->GetGameObject()->GetName().c_str())) {

                bool clicked = false;
                for (auto& item : btns) {

                    if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == choosed_btn)) {

                        if (clicked) continue;

                        choosed_btn = item.first;
                        clicked = true;
                    }
                }

                if (clicked) {
                    if (choosed_btn != 0) {
                        _patronsButtons[i] = static_cast<Button*>(btns[choosed_btn]);
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::SameLine();

            if (ImGui::BeginCombo(string("Patron_").append(std::to_string(i)).append("##Data_Patron").append(id).append(std::to_string(i)).c_str(), choosed_data == 0 ? "None" : ScriptableObjectManager::GetName(datas[choosed_data]->GetId()).c_str())) {

                bool clicked = false;
                for (auto& item : datas) {

                    if (ImGui::Selectable(ScriptableObjectManager::GetName(item.second->GetId()).append("##").append(id).append(std::to_string(item.first)).c_str(), item.first == choosed_data)) {

                        if (clicked) continue;

                        choosed_data = item.first;
                        clicked = true;
                    }
                }

                if (clicked) {
                    if (choosed_data != 0) {
                        _patrons[i] = static_cast<PatronData*>(datas[choosed_data]);
                    }
                }

                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            /*
            if (ImGui::Button(string(ICON_FA_TRASH_CAN "##Remove Patron").append(std::to_string(i)).c_str())) {
                clicked.push_back(item.first);
            }
            */
        }

        btns.clear();
        datas.clear();

        if (ImGui::Button(string("New Patron##").append(id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
            _patronsButtons.push_back(nullptr);
            _patrons.push_back(nullptr);
        }
    }
}

#endif