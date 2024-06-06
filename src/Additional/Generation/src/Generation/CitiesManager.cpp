#include <Generation/CitiesManager.h>

using namespace Generation;

using namespace Twin2Engine::Core;

using namespace glm;
using namespace std;

//CitiesManager* CitiesManager::instance = nullptr;

std::vector<GameObject*> CitiesManager::cities;
std::unordered_map<GameObject*, vector<GameObject*>> CitiesManager::citiesGraph;

//CitiesManager* CitiesManager::Instance() {
//    if (instance == nullptr) {
//        instance = new CitiesManager();
//    }
//    return instance;
//}

void CitiesManager::ResetCities() {
    for (auto& pair : citiesGraph) {
        pair.second.clear();
    }
    citiesGraph.clear();
    cities.clear();
}

void CitiesManager::AddCity(GameObject* city) {
    if (city != nullptr) {
        cities.push_back(city);
    }
}

void CitiesManager::RemoveAllCities() {
    citiesGraph.clear();
    cities.clear();
}

void CitiesManager::MapCitiesConnections(bool byRegions) {
    citiesGraph.clear();

    for (const auto& city : cities) {
        citiesGraph[city] = {};
    }

    for (size_t i = 0; i < cities.size(); ++i) {
        auto adjacentSectors = cities[i]->GetTransform()->GetParent()->GetParent()->GetGameObject()->GetComponent<MapSector>()->GetAdjacentSectors();

        for (size_t j = i + 1; j < cities.size(); ++j) {
            if (std::find(adjacentSectors.begin(), adjacentSectors.end(), cities[j]->GetTransform()->GetParent()->GetParent()->GetGameObject()->GetComponent<MapSector>()) != adjacentSectors.end()) {
                citiesGraph[cities[i]].push_back(cities[j]);
                citiesGraph[cities[j]].push_back(cities[i]);
            }
        }
    }
}

vector<GameObject*> CitiesManager::GetAllCities() {
    return cities;
}

vector<GameObject*> CitiesManager::GetConnectedCities(GameObject* city) {
    //auto it = citiesGraph.find(city);
    //if (it != citiesGraph.end()) {
    if (citiesGraph.contains(city)) {
        return citiesGraph[city];
    }
    return {};
}

GameObject* CitiesManager::GetClosestCity(const glm::vec3& position) {
    GameObject* closestCity = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    size_t size = cities.size();
    float distance = 0.0f;
    for (size_t index = 0ull; index < size; ++index)
    {
        distance = glm::distance(position, cities[index]->GetTransform()->GetGlobalPosition());
        if (distance < minDistance) {
            closestCity = cities[index];
            minDistance = distance;
        }
    }

    return closestCity;
}