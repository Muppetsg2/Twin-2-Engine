#include <Humans/HumansGenerator.h>

using namespace Humans;
using namespace Generation;
using namespace Generation::Generators;

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace glm;
using namespace std;

SCRIPTABLE_OBJECT_SOURCE_CODE(HumansGenerator, Humans, "HumansGenerator")

SO_SERIALIZATION_BEGIN(HumansGenerator, AMapElementGenerator)
	SO_SERIALIZE_FIELD_F(prefabHuman, PrefabManager::GetPrefabPath)
	SO_SERIALIZE_FIELD(number)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(HumansGenerator, AMapElementGenerator)
	SO_DESERIALIZE_FIELD_F_T(prefabHuman, PrefabManager::LoadPrefab, string)
	SO_DESERIALIZE_FIELD(number)
SO_DESERIALIZATION_END()


void HumansGenerator::Generate(Tilemap::HexagonalTilemap* tilemap)
{
    CitiesManager::MapCitiesConnections(false);
    vector<GameObject*> cities = CitiesManager::GetAllCities();

    GameObject* humanContainer = SceneManager::CreateGameObject();
    humanContainer->SetName("HumansContainer");

    for (unsigned int i = 0; i < number; ++i)
    {
        GameObject* city = cities[Random::Range(0ull, cities.size() - 1ull)];
        GameObject* human = SceneManager::CreateGameObject(prefabHuman, humanContainer->GetTransform());
        human->GetTransform()->SetGlobalPosition(city->GetTransform()->GetGlobalPosition());
        human->GetComponent<HumanMovement>()->SetTilemap(tilemap);
        human->GetComponent<Human>()->StartWorking();
    }
}