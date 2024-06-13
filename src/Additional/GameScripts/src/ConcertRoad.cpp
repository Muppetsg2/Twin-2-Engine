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
    //entityPoints.clear();
    //entityMultiplier.clear();

    //while (entityPoints.size() < GameManager::instance->entities.size())
    //{
    //    entityPoints.push_back(0);
    //}
    //
    //while (entityMultiplier.size() < GameManager::instance->entities.size())
    //{
    //    entityMultiplier.push_back(1.0f);
    //}
    bool isPlayer = false;
    float newPossibleBonus = 0.0f;
    int stage = 0;

    for (ConcertRoadPoint& point : RoadMapPoints)
    {
        isPlayer = point.owningPlayable == GameManager::instance->GetPlayer();

        if (point.hexTile->ownerEntity != point.owningPlayable)
        {
            if (point.owningPlayable)
            {
                point.owningPlayable->TakeOverSpeed -= point.addedBonus;
                if (isPlayer)
                    PopularityGainingBonusBarController::Instance()->RemoveCurrentBonus(point.addedBonus);
            }

            point.owningPlayable = point.hexTile->ownerEntity;
            isPlayer = point.owningPlayable == GameManager::instance->GetPlayer();
        }

        stage = point.hexTile->GetStage();
        if (point.hexTile->ownerEntity != nullptr)
        {
            //auto posItr = std::find(GameManager::instance->entities.begin(), GameManager::instance->entities.end(), tile->ownerEntity);
            //int pos = std::distance(GameManager::instance->entities.begin(), posItr);
            //entityPoints[pos] += 1;
            // entityPoints[GameManager::instance->entities.IndexOf(tile->takenEntity)] += 1;

            //newBonus = minBonus + bonusesPerStage[stage] * (maxBonus - minBonus);
            if (bonusesPerStage[stage] != point.addedBonus)
            {
                point.owningPlayable->TakeOverSpeed -= point.addedBonus;

                if (isPlayer)
                {
                    PopularityGainingBonusBarController::Instance()->RemoveCurrentBonus(point.addedBonus);
                }

                point.addedBonus = bonusesPerStage[stage];
                point.owningPlayable->TakeOverSpeed += point.addedBonus;

                if (isPlayer)
                {
                    PopularityGainingBonusBarController::Instance()->AddCurrentBonus(point.addedBonus);
                }
            }
        }
        if (stage < 3 && (point.hexTile->occupyingEntity == GameManager::instance->GetPlayer()))
        {
            //newPossibleBonus = bonusesPerStage[stage + 1] - point.addedBonus;
            newPossibleBonus = bonusesPerStage[stage + 1] - bonusesPerStage[stage];
            if (newPossibleBonus != point.possibleBonus)
            {
                PopularityGainingBonusBarController::Instance()->RemovePossibleBonus(point.possibleBonus);
                point.possibleBonus = newPossibleBonus;
                PopularityGainingBonusBarController::Instance()->AddPossibleBonus(point.possibleBonus);
            }
        }
        else
        {
            if (0.0f != point.possibleBonus)
            {
                PopularityGainingBonusBarController::Instance()->RemovePossibleBonus(point.possibleBonus);
                point.possibleBonus = 0.0f;
            }
        }
    }
}

void ConcertRoad::OnDestroy()
{
    Finish();
    RoadMapPoints.clear();
    //entityPoints.clear();
    //entityMultiplier.clear();
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
    //entityPoints.clear();
    //entityMultiplier.clear();

    for (auto &tile : Twin2Engine::Manager::SceneManager::FindObjectByName("MapGenerator")->GetComponentsInChildren<HexTile>())
    {
        MapHexTile *mapHexTile = tile->GetMapHexTile();
        if (mapHexTile->type == Generation::MapHexTile::HexTileType::PointOfInterest)
        {
            if (tile->percentage == 0.0f || ConsiderInfluenced)
            {
                RoadMapPoints.emplace_back(tile, nullptr, 0.0f);
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

    for (ConcertRoadPoint& t : RoadMapPoints)
    {
        GameObject *m = Twin2Engine::Manager::SceneManager::CreateGameObject(_marker, t.hexTile->GetTransform());

        m->GetTransform()->SetLocalPosition(glm::vec3(0.0f, height, 0.0f));
    }
}
void ConcertRoad::Finish()
{
}

YAML::Node ConcertRoad::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "ConcertRoad";
    node["marker"] = SceneManager::GetPrefabSaveIdx(_marker->GetId());

    return node;
}

bool ConcertRoad::Deserialize(const YAML::Node &node)
{
    if (!node["marker"] || !Component::Deserialize(node))
        return false;

    _marker = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["marker"].as<size_t>()));

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