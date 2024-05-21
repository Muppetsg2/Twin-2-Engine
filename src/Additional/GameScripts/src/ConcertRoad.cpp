#include <ConcertRoad.h>
#include <GameManager.h>
#include <manager/SceneManager.h>
#include <algorithm>


ConcertRoad* ConcertRoad::instance = nullptr;


void ConcertRoad::Initialize() {
    if (instance == nullptr)
    {
        instance = this;
    }
    else
    {
        Twin2Engine::Manager::SceneManager::DestroyGameObject(this->GetGameObject());
    }
}

void ConcertRoad::Update() {

    while (entityPoints.size() < GameManager::instance->entities.size())
    {
        entityPoints.push_back(0);
    }

    while (entityMultiplier.size() < GameManager::instance->entities.size())
    {
        entityMultiplier.push_back(1.0f);
    }

    while (entityPoints.size() > GameManager::instance->entities.size())
    {
        entityPoints.erase(entityPoints.begin()); //RemoveAt(0);
    }

    while (entityMultiplier.size() > GameManager::instance->entities.size())
    {
        entityMultiplier.erase(entityMultiplier.begin());
    }

    for (int i = 0; i < GameManager::instance->entities.size(); ++i)
    {
        entityPoints[i] = 0;
        entityMultiplier[i] = 1.0f;
    }

    //int LevelOfQuality = 255;

    for(auto tile : RoadMapPoints)
    {
        //if (tile->takenEntity != nullptr)
        //{
        //    auto posItr = std::find(GameManager::instance->entities.begin(), GameManager::instance->entities.end(), tile->takenEntity);
        //    int pos = std::distance(GameManager::instance->entities.begin(), posItr);
        //    entityPoints[pos] += 1;
        //    //entityPoints[GameManager::instance->entities.IndexOf(tile->takenEntity)] += 1;
        //
        //    for (int i = 3; i >= 0; --i)
        //    {
        //        if (tile->percentage >= mulPercentage[i])
        //        {
        //            float mul = (i != 3 ? mulPercentage[i + 1] : 100.0f) / 100.0f;
        //            entityMultiplier[pos] += ((maxMultiplier - minMultiplier) / NumberOfPoints) * mul;
        //            break;
        //        }
        //    }
        //}

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

    //textField.text = "";
    //textField.text = new StringBuilder()
    //    .Append("Points: ")
    //    //.Append(entityPoints[GameManager.instance.entities.FindIndex(0, (x) => { return x.GetType().Equals(typeof(Player)); })])
    //    .Append($"{((RoadMapPoints.Count > 0) ? (entityPoints[GameManager.instance.entities.FindIndex(0, (x) => { return x.GetType().Equals(typeof(Player)); })]) : (0)):N0}")
    //    .Append("/")
    //    .Append(RoadMapPoints.Count)
    //    .Append("\n")
    //    .Append("Multiplier: x")
    //    .Append($"{((RoadMapPoints.Count > 0) ? (entityMultiplier[GameManager.instance.entities.FindIndex(0, (x) => { return x.GetType().Equals(typeof(Player)); })]):(0)):N1}")
    //    .ToString();
}

void ConcertRoad::Use() {
    //NumberOfPoints = min(NumberOfPoints, CitiesManager::Instance.GetAllCities().Count);
    Begin();
}

void ConcertRoad::Begin() {
    Twin2Engine::Manager::SceneManager::FindObjectByName("MapGenerator")->GetComponentsInChildren<MapHexTile>();
    std::unordered_set<Generation::MapHexTile*> tiles;// (Twin2Engine::Manager::SceneManager::FindObjectByName("MapGenerator")->GetComponentsInChildren<MapHexTile>().);

    RoadMapPoints.clear();
    entityPoints.clear();
    entityMultiplier.clear();

    for (MapHexTile* t : tiles)
    {
        //if (t->Type == TileType.PointOfInterest)
        //{
        //    if (t.percentage == 0.0f || ConsiderInfluenced)
        //    {
        //        RoadMapPoints.Add(t);
        //    }
        //}
    }

    //System.Random rnd = new();

    while (RoadMapPoints.size() > NumberOfPoints)
    {
        //RoadMapPoints.RemoveAt(rnd.Next(RoadMapPoints.Count));
    }

    for(MapHexTile* t : RoadMapPoints)
    {
        //GameObject m = Instantiate(Marker, t.transform);
        //m.transform.localPosition = new(0f, height, 0f);
    }

    //textField.text = new StringBuilder()
    //    .Append("Points: 0/")
    //    .Append(RoadMapPoints.Count)
    //    .Append("\n")
    //    .Append("Multiplier: x1.0")
    //    .ToString();
}
void ConcertRoad::Finish() {

}