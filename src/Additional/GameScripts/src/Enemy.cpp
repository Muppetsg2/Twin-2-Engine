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

void Enemy::UpdateTiles(MapGenerator* gen)
{
    _tiles.clear();
    if (gen != nullptr) {
        list<HexTile*> tempList = gen->GetGameObject()->GetComponentsInChildren<HexTile>();
        _tiles.insert(_tiles.begin(), tempList.cbegin(), tempList.cend());
    }
}

void Enemy::ChangeState(State<Enemy*>* newState) {
    _nextState = newState;
}

void Enemy::SetMoveDestination(HexTile* tile)
{
    if (CurrTile && CurrTile != tile)
    {
        CurrTile->StopTakingOver(this);
    }
    _movement->SetDestination(tile);
}

void Enemy::Initialize()
{
    Playable::Initialize();
    _movement = GetGameObject()->GetComponent<EnemyMovement>();

    if (_tilemap != nullptr) {
        _onMapGenerationEventId = (_tilemap->GetGameObject()->GetComponent<MapGenerator>()->OnMapGenerationEvent += [&](MapGenerator* gen) -> void { UpdateTiles(gen); });
    }

    ChangeState(&_initState);
}


void Enemy::OnEnable()
{
    //PerformMovement();
}

void Enemy::OnDestroy()
{
    if (_onMapGenerationEventId != -1) {
        if (_tilemap != nullptr) {
            if (_tilemap->GetGameObject() != nullptr) {
                _tilemap->GetGameObject()->GetComponent<MapGenerator>()->OnMapGenerationEvent -= _onMapGenerationEventId;
                _onMapGenerationEventId = -1;
            }
        }
    }
}

void Enemy::Update()
{
    UpdatePrices();
    if (_nextState != nullptr) {
        State<Enemy*>* oldState = _nextState;
        _stateMachine.ChangeState(this, _nextState);
        if (oldState == _nextState) {
            _nextState = nullptr;
        }
    }

    _currThinkingTime -= Time::GetDeltaTime();
    if (_currThinkingTime <= 0.f) {
        _stateMachine.Update(this);
        _currThinkingTime = _timeToThink;
    }
}

void Enemy::LostPaperRockScissors(Playable* playable)
{
    CurrTile->StopTakingOver(this);
    ChangeState(&_movingState);

    //GameObject* tiles[6];
    //CurrTile->GetMapHexTile()->tile->GetAdjacentGameObjects(tiles);
    //for (int i = 0; i < 6; ++i) {
    //    if (tiles[i] != nullptr) {
    //        _movement->reachEnd = true;
    //        //_movement->MoveAndSetDestination(tiles[i]->GetComponent<HexTile>());
    //        SetMoveDestination(tiles[i]->GetComponent<HexTile>());
    //        break;
    //    }
    //}
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
    GameManager::instance->minigameActive = false;
    //bool startTakingOver = CurrTile->occupyingEntity == playable;
    playable->LostPaperRockScissors(this);


    if (CurrTile->takenEntity == playable) {
    //if (CurrTile->takenEntity != this) {
        CurrTile->ResetTile();

        //FinishedMovement(CurrTile);
    }

    playable->CheckIfDead(this);
    CurrTile->StartTakingOver(this);
    ChangeState(&_takingOverState);

    //if (startTakingOver) {
    //    CurrTile->StartTakingOver(this);
    //}

    // TakeOver
    //enemyStrategy.WonPaperRockScisors(this);
}

void Enemy::LostFansControl(Playable* playable)
{
}

void Enemy::WonFansControl(Playable* playable)
{
}

//void Enemy::StartPaperRockScissors(Playable* playable)
//{
//}

void Enemy::StartFansControl(Playable* playable)
{
}

float Enemy::GetMaxRadius() const {
    return (_movement->maxSteps + 0.25) * _tilemap->GetDistanceBetweenTiles();
}

void Enemy::SetTileMap(Tilemap::HexagonalTilemap* map)
{
    if (_tilemap != map) {
        if (_onMapGenerationEventId != -1) {
            _tilemap->GetGameObject()->GetComponent<MapGenerator>()->OnMapGenerationEvent -= _onMapGenerationEventId;
            _onMapGenerationEventId = -1;
        }

        _tilemap = map;
        _tiles.clear();
        if (_tilemap != nullptr) {
            _onMapGenerationEventId = (_tilemap->GetGameObject()->GetComponent<MapGenerator>()->OnMapGenerationEvent += [&](MapGenerator* gen) -> void { UpdateTiles(gen); });

            list<HexTile*> tempList = _tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
            _tiles.insert(_tiles.begin(), tempList.cbegin(), tempList.cend());
        }
    }
}

void Enemy::OnDead()
{
}

YAML::Node Enemy::Serialize() const
{
    YAML::Node node = Playable::Serialize();
    node["type"] = "Enemy";

    return node;
}

bool Enemy::Deserialize(const YAML::Node& node)
{
    if (!Playable::Deserialize(node))
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