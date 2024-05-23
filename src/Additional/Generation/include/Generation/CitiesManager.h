#pragma once

#include <core/Component.h>
#include <core/GameObject.h>
#include <core/Transform.h>

#include <Generation/MapSector.h>

namespace Generation
{

    class CitiesManager {
    public:
        static void ResetCities();
        static void AddCity(Twin2Engine::Core::GameObject* city);
        static void RemoveAllCities();
        static void MapCitiesConnections(bool byRegions);
        static std::vector<Twin2Engine::Core::GameObject*> GetAllCities();
        static std::vector<Twin2Engine::Core::GameObject*> GetConnectedCities(Twin2Engine::Core::GameObject* city);
        static Twin2Engine::Core::GameObject* GetClosestCity(const glm::vec3& position);

    private:
        CitiesManager() = default;
        ~CitiesManager() = default;
        CitiesManager(const CitiesManager&) = delete;
        CitiesManager& operator=(const CitiesManager&) = delete;

        static std::vector<Twin2Engine::Core::GameObject*> cities;
        static std::unordered_map<Twin2Engine::Core::GameObject*, std::vector<Twin2Engine::Core::GameObject*>> citiesGraph;
    };
}