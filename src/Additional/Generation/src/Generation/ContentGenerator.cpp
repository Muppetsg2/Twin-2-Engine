#include <Generation/ContentGenerator.h>

using namespace Generation;
using namespace Generation::Generators;
using namespace Tilemap;

void ContentGenerator::GenerateContent(HexagonalTilemap* targetTilemap)
{
    SPDLOG_INFO("Starting content");
    for (AMapElementGenerator* generator : mapElementGenerators)
    {
        SPDLOG_INFO("Generating element");
        generator->Generate(targetTilemap);
    }
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
        node["mapElementGenerators"][index++] = Twin2Engine::Manager::ScriptableObjectManager::GetPath(generator->GetId());
    }
    return node;
}

bool ContentGenerator::Deserialize(const YAML::Node& node)
{
    if (!node["mapElementGenerators"] || !Component::Deserialize(node)) return false;

    for (YAML::Node soSceneId : node["mapElementGenerators"])
    {
        //AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(ScriptableObjectManager::Deserialize(soSceneId.as<unsigned int>()));
        AMapElementGenerator* generator = dynamic_cast<AMapElementGenerator*>(Twin2Engine::Manager::ScriptableObjectManager::Load(soSceneId.as<string>()));
        SPDLOG_INFO("Adding generator {0}, {1}", soSceneId.as<string>(), (unsigned int)generator);
        if (generator != nullptr)
        {
            mapElementGenerators.push_back(generator);
        }
    }
    //contentGenerator->mapElementGenerators = node["mapElementGenerators"].as<std::list<Generators::AMapElementGenerator*>>();

    return true;
}

void ContentGenerator::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("Content Generator##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str()))
    {
		ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool node_open = ImGui::TreeNodeEx(string("Generators##").append(id).c_str(), node_flag);

        std::list<int> clicked = std::list<int>();
        clicked.clear();
		if (node_open) {
            for (int i = 0; i < mapElementGenerators.size(); ++i) {
                string n = Twin2Engine::Manager::ScriptableObjectManager::GetName(mapElementGenerators[i]->GetId());
                ImGui::BulletText(n.c_str());
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);
                if (ImGui::Button(string("Remove##").append(id).append(std::to_string(i)).c_str())) {
                    clicked.push_back(i);
                }
            }
            ImGui::TreePop();
		}

        if (clicked.size() > 0) {
            clicked.sort();

            for (int i = clicked.size() - 1; i > -1; --i)
            {
                mapElementGenerators.erase(mapElementGenerators.begin() + clicked.back());

                clicked.pop_back();
            }
        }

        clicked.clear();

        // DODAC
        // przenoszenie kolejnosæi generatorow
        /*
        if (ImGui::Button(string("Add Element Generator##").append(id).c_str())) {

        }
        */
    }
}