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

    ChangeState(&_initState);
}


void Enemy::OnEnable()
{
    //PerformMovement();
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


    if (CurrTile->ownerEntity == playable) {
        CurrTile->ResetTile();
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
    _tilemap = map;
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