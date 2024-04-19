#include <Generation/ContentGenerator.h>

using namespace Generation;
using namespace Generation::Generators;
using namespace Tilemap;

void ContentGenerator::GenerateContent(HexagonalTilemap* targetTilemap)
{
    for (AMapElementGenerator* generator : mapElementGenerators)
    {
        SPDLOG_INFO("Tutaj0");
        generator->Generate(targetTilemap);
        SPDLOG_INFO("Tutaj1");
    }
}