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
    //node.remove("type");
    node["type"] = "ContentGenerator";
    node.remove("subTypes");
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