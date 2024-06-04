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
InitState Enemy::_initState;

void Enemy::ChangeState(State<Enemy*>* newState) {
    _stateMachine.ChangeState(this, newState);
}

void Enemy::SetMoveDestination(HexTile* tile)
{
    _movement->SetDestination(tile);
}

void Enemy::Initialize()
{
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
    _movement = GetGameObject()->GetComponent<EnemyMovement>();
    list<HexTile*> tempList = _tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
    _tiles.insert(_tiles.begin(), tempList.cbegin(), tempList.cend());
    ChangeState(&_initState);
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
    _currThinkingTime -= Time::GetDeltaTime();
    if (_currThinkingTime <= 0.f) {
        _stateMachine.Update(this);
        _currThinkingTime = _timeToThink;
    }
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

void Enemy::LostPaperRockScissors(Playable* playable)
{
    CurrTile->StopTakingOver(this);

    GameObject* tiles[6];
    CurrTile->GetMapHexTile()->tile->GetAdjacentGameObjects(tiles);
    for (int i = 0; i < 6; ++i) {
        if (tiles[i] != nullptr) {
            _movement->reachEnd = true;
            //_movement->MoveAndSetDestination(tiles[i]->GetComponent<HexTile>());
            SetMoveDestination(tiles[i]->GetComponent<HexTile>());
            break;
        }
    }
    //PerformMovement();
}

void Enemy::WonPaperRockScissors(Playable* playable)
{
    //Enemy* enemy = dynamic_cast<Enemy*>(playable);
    //if (enemy != nullptr)
    //{
    //    enemy->LostPaperRockScissors(this);
    //    CurrTile->isFighting = false;
    //}
    CurrTile->isFighting = false;
    playable->LostPaperRockScissors(this);

    if (CurrTile->takenEntity == playable) {
    //if (CurrTile->takenEntity != this) {
        CurrTile->ResetTile();
        playable->CheckIfDead(this);

        //FinishedMovement(CurrTile);
        CurrTile->StartTakingOver(this);

    }

    // TakeOver
    //enemyStrategy.WonPaperRockScisors(this);
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

void Enemy::OnDead()
{
}

YAML::Node Enemy::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "Enemy";

    return node;
}

bool Enemy::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    return true;
}

#if _DEBUG
void Enemy::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Enemy##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}
#endif