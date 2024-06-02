#include <Enemy.h>
#include <EnemyMovement.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Generation;
using namespace glm;
using namespace std;

TakingOverState Enemy::_takingOverState;
MovingState Enemy::_movingState;
FightingState Enemy::_fightingState;
RadioStationState Enemy::_radioStationState;

void Enemy::ChangeState(State<Enemy*>* newState) {
    _stateMachine.ChangeState(this, newState);
}

void Enemy::Initialize()
{
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
    _movement = GetGameObject()->GetComponent<EnemyMovement>();
    list<HexTile*> tempList = _tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
    tiles.insert(tiles.begin(), tempList.cbegin(), tempList.cend());

    _movement->OnFindPathError += [&](GameObject* gameObject, HexTile* tile) {
            PerformMovement();
            //FinishedMovement(tile);
        };
    _movement->OnFinishMoving += [&](GameObject* gameObject, HexTile* tile) {
        //PerformMovement();
        FinishedMovement(tile);
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
    if (isTakingArea)
    {
        takingAreaCounter += Time::GetDeltaTime();
        if (CurrTile->percentage >= targetPercentage)
        {
            takingAreaCounter = 0.0f;
            isTakingArea = false;
            PerformMovement();
        }
    }
    _stateMachine.Update(this);
}


void Enemy::FinishedMovement(HexTile* hexTile)
{
    if (CurrTile && CurrTile != hexTile)
    {
        CurrTile->StopTakingOver(this);
    }
    CurrTile = hexTile;
    isTakingArea = true;
    hexTile->StartTakingOver(this);
    targetPercentage = Random::Range(50.0f, 95.0f);
}

void Enemy::PerformMovement()
{
    vec3 globalPosition = GetTransform()->GetGlobalPosition();
    globalPosition.y = 0.0f;

    vec3 tilePosition;

    vector<HexTile*> possible;
    //possible.reserve((1 + _movement->maxSteps) / 2 * _movement->maxSteps * 6);
    possible.reserve((1 + _movement->maxSteps) * _movement->maxSteps * 3);

    list<HexTile*> tempList = _tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
    tiles.clear();
    tiles.insert(tiles.begin(), tempList.cbegin(), tempList.cend());

    size_t size = tiles.size();
    float maxRadius = GetMaxRadius();

    for (size_t index = 0ull; index < size; ++index)
    {
        MapHexTile::HexTileType type = tiles[index]->GetMapHexTile()->type;
        if (type != MapHexTile::HexTileType::Mountain && type != MapHexTile::HexTileType::None)
        {
            tilePosition = tiles[index]->GetTransform()->GetGlobalPosition();
            tilePosition.y = 0.0f;
            float distance = glm::distance(globalPosition, tilePosition);
            if (distance <= maxRadius)
            {
                possible.push_back(tiles[index]);
            }
        }
    }

    SPDLOG_INFO("ENEMY Possible Size: {}", possible.size());
    HexTile* result = possible[Random::Range(0ull, possible.size() - 1ull)];

    _movement->SetDestination(result);
}

void Enemy::LostPaperRockScissors(Playable* playable)
{
}

void Enemy::WonPaperRockScissors(Playable* playable)
{
}

void Enemy::LostFansControl(Playable* playable)
{
}

void Enemy::WonFansControl(Playable* playable)
{
}

void Enemy::StartPaperRockScissors(Playable* playable)
{
}

void Enemy::StartFansControl(Playable* playable)
{
}

float Enemy::GetMaxRadius() const {
    return (_movement->maxSteps + 0.25) * _tilemap->GetDistanceBetweenTiles();
}

void Enemy::OnDead()
{
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