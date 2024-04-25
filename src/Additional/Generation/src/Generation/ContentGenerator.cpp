#include <Generation/ContentGenerator.h>

using namespace Generation;
using namespace Generation::Generators;
using namespace Tilemap;

void ContentGenerator::GenerateContent(HexagonalTilemap* targetTilemap)
{
    for (AMapElementGenerator* generator : mapElementGenerators)
    {
        generator->Generate(targetTilemap);
    }
}

YAML::Node ContentGenerator::Serialize() const
{
    YAML::Node node = Twin2Engine::Core::Component::Serialize();
    size_t index = 0;
    for (AMapElementGenerator* generator : mapElementGenerators)
    {
        node["mapElementGenerators"][index++] = Twin2Engine::Manager::ScriptableObjectManager::SceneSerialize(generator->GetId());
    }
    node.remove("type");
    node.remove("subTypes");
    return node;
}