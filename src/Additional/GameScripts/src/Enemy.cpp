#include <Enemy.h>
#include <EnemyMovement.h>
#include <string>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Generation;
using namespace glm;
using namespace std;

void Enemy::Initialize()
{
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
    _movement = GetGameObject()->GetComponent<EnemyMovement>();
    list<HexTile*> tempList = _tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
    _tiles.insert(_tiles.begin(), tempList.cbegin(), tempList.cend());

    _movement->OnFindPathError += [&](GameObject* gameObject, HexTile* tile) {
            PerformMovement();
        };
    _movement->OnFinishMoving += [&](GameObject* gameObject, HexTile* tile) {
        PerformMovement();
        };

}


void Enemy::OnEnable()
{
    SPDLOG_INFO("ENEMY OnEneable");
    PerformMovement();
}

void Enemy::OnDestroy()
{

}

void Enemy::Update()
{

}



void Enemy::PerformMovement()
{
    vec3 globalPosition = GetTransform()->GetGlobalPosition();
    globalPosition.y = 0.0f;

    vec3 tilePosition;

    vector<HexTile*> possible;
    //possible.reserve((1 + _movement->maxSteps) * _movement->maxSteps / 2 * 6);
    possible.reserve((1 + _movement->maxSteps) * _movement->maxSteps * 3);

    list<HexTile*> tempList = _tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
    _tiles.clear();
    _tiles.insert(_tiles.begin(), tempList.cbegin(), tempList.cend());

    size_t size = _tiles.size();
    float maxRadius = (_movement->maxSteps + 0.25) * _tilemap->GetDistanceBetweenTiles();

    for (size_t index = 0ull; index < size; ++index)
    {
        MapHexTile::HexTileType type = _tiles[index]->GetMapHexTile()->type;
        if (type != MapHexTile::HexTileType::Mountain && type != MapHexTile::HexTileType::None)
        {
            tilePosition = _tiles[index]->GetTransform()->GetGlobalPosition();
            tilePosition.y = 0.0f;
            float distance = glm::distance(globalPosition, tilePosition);
            if (distance <= maxRadius)
            {
                possible.push_back(_tiles[index]);
            }
        }
    }

    SPDLOG_INFO("ENEMY Possible Size: {}", possible.size());
    HexTile* result = possible[Random::Range(0ull, possible.size() - 1ull)];

    _movement->SetDestination(result);
}

YAML::Node Enemy::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "Enemy";
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool Enemy::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

#if _DEBUG
void Enemy::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Enemy##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        //glm::vec3 v = light->direction;
        //ImGui::DragFloat3(string("Direction##").append(id).c_str(), glm::value_ptr(v), .1f, -1.f, 1.f);
        //if (v != light->direction) {
        //    SetDirection(v);
        //
        //float p = light->power;
        //ImGui::DragFloat(string("Power##").append(id).c_str(), &p);
        //if (p != light->power) {
        //    SetPower(p);
        //}
    }
}
#endif