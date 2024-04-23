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
    node.remove("type");
    node.remove("subTypes");
    return node;
}