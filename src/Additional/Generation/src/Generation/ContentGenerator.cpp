#include <Generation/ContentGenerator.h>

using namespace Generation;
using namespace Generation::Generators;
using namespace Tilemap;
using namespace Twin2Engine::Manager;

void ContentGenerator::GenerateContent(HexagonalTilemap* targetTilemap)
{
    //if (_tilemap == nullptr)
    //{
    //    _tilemap = targetTilemap;
    //}
    SPDLOG_INFO("Starting content");
    for (AMapElementGenerator* generator : mapElementGenerators)
    {
        SPDLOG_INFO("Generating element");
        generator->Generate(targetTilemap);
    }
}

void ContentGenerator::Initialize()
{
    //_tilemap = GetGameObject()->GetComponent<Tilemap::HexagonalTilemap>();
}

void ContentGenerator::OnDestroy() {
    mapElementGenerators.clear();
}

YAML::Node ContentGenerator::Serialize() const
{
    YAML::Node node = Twin2Engine::Core::Component::Serialize();
    node["type"] = "ContentGenerator";
    size_t index = 0;
    //node["mapElementGenerators"] = std::vector<size_t>();
    node["mapElementGenerators"] = {};
    for (AMapElementGenerator* generator : mapElementGenerators)
    {
        //node["mapElementGenerators"][index++] = Twin2Engine::Manager::ScriptableObjectManager::SceneSerialize(generator->GetId());
        node["mapElementGenerators"][index++] = ScriptableObjectManager::GetPath(generator->GetId());
    }
    return node;
}

bool ContentGenerator::Deserialize(const YAML::Node& node)
{
    if (!node["mapElementGenerators"] || !Component::Deserialize(node)) return false;

    for (YAML::Node soSceneId : node["mapElementGenerators"])
    {
        //AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(ScriptableObjectManager::Deserialize(soSceneId.as<unsigned int>()));
        AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(ScriptableObjectManager::Load(soSceneId.as<string>()));
        SPDLOG_INFO("ContentGenerator::Adding generator {0}, {1}", soSceneId.as<string>(), (unsigned int)generator);
        if (generator != nullptr)
        {
            mapElementGenerators.push_back(generator);
        }
    }
    //contentGenerator->mapElementGenerators = node["mapElementGenerators"].as<std::list<Generators::AMapElementGenerator*>>();

    return true;
}

#if _DEBUG
void ContentGenerator::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("Content Generator##Component").append(id);

    // Edition Type
    // 0 - move
    // 1 - remove
    static int type = 0;

    if (ImGui::CollapsingHeader(name.c_str()))
    {
        if (Component::DrawInheritedFields()) return;

        if (ImGui::RadioButton(std::string("Move##RadioButton").append(id).c_str(), type == 0))
            type = 0;
        ImGui::SameLine();
        if (ImGui::RadioButton(std::string("Remove##RadioButton").append(id).c_str(), type == 1))
            type = 1;

		ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool node_open = ImGui::TreeNodeEx(string("Generators##").append(id).c_str(), node_flag);

        std::list<int> clicked = std::list<int>();
        clicked.clear();
		if (node_open) {
            for (int i = 0; i < mapElementGenerators.size(); ++i) {
                AMapElementGenerator* item = mapElementGenerators[i];
                string n = ScriptableObjectManager::GetName(item->GetId()).append("##").append(id);
                ImGui::Text(to_string(i + 1).append(". "s).c_str());
                ImGui::SameLine();
                ImGui::Selectable(n.c_str(), false, NULL, ImVec2(ImGui::GetContentRegionAvail().x - 80, 0.f));

                bool v = false;
                if (type == 0) v = ImGui::IsItemActive() && !ImGui::IsItemHovered();

                if (type == 1) {
                    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
                    if (ImGui::Button(string("Remove##").append(id).append(std::to_string(i)).c_str())) {
                        clicked.push_back(i);
                    }
                }

                if (type == 0 && v)
                {
                    size_t i_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    if (i_next >= 0 && i_next < mapElementGenerators.size())
                    {
                        mapElementGenerators[i] = mapElementGenerators[i_next];
                        mapElementGenerators[i_next] = item;
                        ImGui::ResetMouseDragDelta();
                    }
                }
            }
            ImGui::TreePop();
		}

        if (clicked.size() > 0 && type == 1) {
            clicked.sort();

            for (int i = clicked.size() - 1; i > -1; --i)
            {
                mapElementGenerators.erase(mapElementGenerators.begin() + clicked.back());

                clicked.pop_back();
            }
        }

        clicked.clear();

        /*
        if (ImGui::Button(string("Add Element Generator##").append(id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
            ImGui::OpenPopup(string("Add Element Generator PopUp##Content Generator").append(id).c_str(), ImGuiPopupFlags_NoReopen);
        }

        if (ImGui::BeginPopup(string("Add Element Generator PopUp##Content Generator").append(id).c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {

            std::map<size_t, ScriptableObject*> types = ScriptableObjectManager::GetScriptableObjectsDerivedByType<AMapElementGenerator>();

            for (auto& item : mapElementGenerators) {
                if (types.contains(item->GetId())) {
                    types.erase(item->GetId());
                }
            }

            std::list<size_t> toErase = std::list<size_t>();
            for (auto ty : types) {
                auto iter = std::find_if(mapElementGenerators.begin(), mapElementGenerators.end(), [&](AMapElementGenerator* item) -> bool {
                    return typeid(*item) == typeid(*(ty.second));
                });

                if (iter != mapElementGenerators.end()) {
                    toErase.push_back(ty.first);
                }
            }

            toErase.sort();

            for (size_t i = toErase.size() - 1; i > -1; --i) {
                types.erase(toErase.back());
                toErase.pop_back();
            }

            toErase.clear();

            size_t choosed = 0;

            if (ImGui::BeginCombo(string("##GO POP UP COMPONENTS").append(id).c_str(), choosed == 0 ? "None" : ScriptableObjectManager::GetName(choosed).c_str())) {

                bool clicked = false;
                for (auto& item : types) {

                    if (ImGui::Selectable(std::string(ScriptableObjectManager::GetName(item.first)).append("##").append(id).c_str(), item.first == choosed)) {

                        if (clicked) continue;

                        choosed = item.first;
                        clicked = true;
                    }
                }

                if (clicked) {
                    if (choosed != 0) {
                        AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(types[choosed]);
                        SPDLOG_INFO("ContentGenerator::Adding generator {0}", (unsigned int)generator);
                        if (generator != nullptr)
                        {
                            mapElementGenerators.push_back(generator);
                        }
                    }
                }

                ImGui::EndCombo();
            }

            if (choosed != 0) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        */
    }
}
#endif