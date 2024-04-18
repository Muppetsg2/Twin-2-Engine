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