#include <ConcertRoad.h>
#include <GameManager.h>
#include <core/Random.h>
#include <manager/SceneManager.h>
#include <manager/PrefabManager.h>
#include <algorithm>

using namespace Twin2Engine::Manager;

ConcertRoad *ConcertRoad::instance = nullptr;

void ConcertRoad::Initialize()
{
    if (instance == nullptr)
    {
        instance = this;
    }
    else
    {
        Twin2Engine::Manager::SceneManager::DestroyGameObject(this->GetGameObject());
    }
}

void ConcertRoad::Update()
{
    entityPoints.clear();
    entityMultiplier.clear();

    while (entityPoints.size() < GameManager::instance->entities.size())
    {
        entityPoints.push_back(0);
    }

    while (entityMultiplier.size() < GameManager::instance->entities.size())
    {
        entityMultiplier.push_back(1.0f);
    }

    // while (entityPoints.size() > GameManager::instance->entities.size())
    //{
    //     entityPoints.erase(entityPoints.begin()); //RemoveAt(0);
    // }
    //
    // while (entityMultiplier.size() > GameManager::instance->entities.size())
    //{
    //     entityMultiplier.erase(entityMultiplier.begin());
    // }
    //
    // for (int i = 0; i < GameManager::instance->entities.size(); ++i)
    //{
    //     entityPoints[i] = 0;
    //     entityMultiplier[i] = 1.0f;
    // }

    // int LevelOfQuality = 255;

    for (auto tile : RoadMapPoints)
    {
        if (tile->ownerEntity != nullptr)
        {
            auto posItr = std::find(GameManager::instance->entities.begin(), GameManager::instance->entities.end(), tile->ownerEntity);
            int pos = std::distance(GameManager::instance->entities.begin(), posItr);
            entityPoints[pos] += 1;
            // entityPoints[GameManager::instance->entities.IndexOf(tile->takenEntity)] += 1;

            for (int i = 3; i >= 0; --i)
            {
                if (tile->percentage >= mulPercentage[i])
                {
                    float mul = (i != 3 ? mulPercentage[i + 1] : 100.0f) / 100.0f;
                    entityMultiplier[pos] += ((maxMultiplier - minMultiplier) / NumberOfPoints) * mul;
                    break;
                }
            }
        }

        /*
        if (tile.percentage >= 90.0f)
        {
            if (3 < LevelOfQuality)
            {
                LevelOfQuality = 3;
            }
        }
        else if (tile.percentage >= 60.0f)
        {
            if (2 < LevelOfQuality)
            {
                LevelOfQuality = 2;
            }
        }
        else if (tile.percentage >= 30.0f)
        {
            if (1 < LevelOfQuality)
            {
                LevelOfQuality = 1;
            }
        }
        else
        {
            LevelOfQuality = 0;
        }
        */
    }

    /*
    switch (LevelOfQuality)
    {
        case 0:
            ConcertRoadMultiplier = 1.0f;
            break;

        case 1:
            ConcertRoadMultiplier = 1.25f;
            break;

        case 2:
            ConcertRoadMultiplier = 1.5f;
            break;

        case 3:
            ConcertRoadMultiplier = 2.0f;
            break;
    }
    */

    // textField.text = "";
    // textField.text = new StringBuilder()
    //     .Append("Points: ")
    //     //.Append(entityPoints[GameManager.instance.entities.FindIndex(0, (x) => { return x.GetType().Equals(typeof(Player)); })])
    //     .Append($"{((RoadMapPoints.Count > 0) ? (entityPoints[GameManager.instance.entities.FindIndex(0, (x) => { return x.GetType().Equals(typeof(Player)); })]) : (0)):N0}")
    //     .Append("/")
    //     .Append(RoadMapPoints.Count)
    //     .Append("\n")
    //     .Append("Multiplier: x")
    //     .Append($"{((RoadMapPoints.Count > 0) ? (entityMultiplier[GameManager.instance.entities.FindIndex(0, (x) => { return x.GetType().Equals(typeof(Player)); })]):(0)):N1}")
    //     .ToString();
}

void ConcertRoad::OnDestroy()
{
    Finish();
    RoadMapPoints.clear();
    entityPoints.clear();
    entityMultiplier.clear();
    if (instance == this)
    {
        instance = nullptr;
    }
}

void ConcertRoad::Use() {
    //NumberOfPoints = std::min(NumberOfPoints, CitiesManager::Instance->GetAllCities().size());
    Begin();
}

void ConcertRoad::Begin()
{
    // std::vector<Generation::MapHexTile*> tiles();// (Twin2Engine::Manager::SceneManager::FindObjectByName("MapGenerator")->GetComponentsInChildren<MapHexTile>().);

    RoadMapPoints.clear();
    entityPoints.clear();
    entityMultiplier.clear();

    //{
    //    // FindObjectByName("MapGenerator")->GetComponentsInChildren<HexTile>();
    //    HexTile* tile;
    //    auto tiles = Twin2Engine::Manager::SceneManager::GetComponentsOfType<HexTile>();
    //    for (auto& t : tiles)
    //    {
    //        tile = (HexTile*)t.second;
    //        MapHexTile* mapHexTile = tile->GetMapHexTile();
    //        if (mapHexTile->type == Generation::MapHexTile::HexTileType::PointOfInterest)
    //        {
    //            if (tile->percentage == 0.0f || ConsiderInfluenced)
    //            {
    //                RoadMapPoints.insert(tile);
    //            }
    //        }
    //    }
    //}
    for (auto &tile : Twin2Engine::Manager::SceneManager::FindObjectByName("MapGenerator")->GetComponentsInChildren<HexTile>())
    {
        MapHexTile *mapHexTile = tile->GetMapHexTile();
        if (mapHexTile->type == Generation::MapHexTile::HexTileType::PointOfInterest)
        {
            if (tile->percentage == 0.0f || ConsiderInfluenced)
            {
                RoadMapPoints.insert(tile);
            }
        }
    }

    // Usuwanie na losowej pozycji w RoadMapPoints dopoki nie pozostanie zadana ilosc
    auto itr = RoadMapPoints.begin();
    while (RoadMapPoints.size() > NumberOfPoints)
    {
        itr = RoadMapPoints.begin();
        std::advance(itr, Random::Range<int>(0, RoadMapPoints.size() - 1));
        RoadMapPoints.erase(itr);
    }
    // std::vector<int> indexes;
    // for (int i = RoadMapPoints.size() - 1; i > NumberOfPoints; --i) {
    //     indexes.push_back(Random::Range<int>(0, i));
    // }
    // std::sort(indexes.begin(), indexes.end());
    //
    // auto endItr = RoadMapPoints.end();
    //--endItr;
    // while (RoadMapPoints.size() > NumberOfPoints)
    //{
    //     for (int i = RoadMapPoints.size() - indexes.back(); i > 0; --i) {
    //         --endItr;
    //     }
    //     indexes.pop_back();
    //     RoadMapPoints.erase(endItr);
    // }
    ///

    for (HexTile *t : RoadMapPoints)
    {
        GameObject *m = Twin2Engine::Manager::SceneManager::CreateGameObject(Marker, t->GetTransform());

        m->GetTransform()->SetLocalPosition(glm::vec3(0.0f, height, 0.0f));
    }

    // textField.text = new StringBuilder()
    //     .Append("Points: 0/")
    //     .Append(RoadMapPoints.Count)
    //     .Append("\n")
    //     .Append("Multiplier: x1.0")
    //     .ToString();
}
void ConcertRoad::Finish()
{
}

YAML::Node ConcertRoad::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "ConcertRoad";
    node["marker"] = SceneManager::GetPrefabSaveIdx(Marker->GetId());

    return node;
}

bool ConcertRoad::Deserialize(const YAML::Node &node)
{
    if (!node["marker"] || !Component::Deserialize(node))
        return false;

    Marker = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["marker"].as<size_t>()));

    return true;
}

#if _DEBUG
void ConcertRoad::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("ConcertRoad##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str()))
    {
        if (Component::DrawInheritedFields())
            return;

        // TODO: Zrobic
    }
}
#endif