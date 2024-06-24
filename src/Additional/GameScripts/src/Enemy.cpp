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
    if (newState == (&_fightingState)) {
        SPDLOG_INFO("Enemy wants to fight!");
    }
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


//void Enemy::OnDestroy()
//{
//    Playable::OnDestroy();
//    if (patron != nullptr) {
//        GameManager::instance->FreePatron(patron);
//    }
//}

void Enemy::Update()
{
    if (_loosingFightPlayable)
    {
        //Enemy* enemy = dynamic_cast<Enemy*>(playable);
        //if (enemy != nullptr)
        //{
        //    enemy->LostPaperRockScissors(this);
        //    CurrTile->isFighting = false;
        //}
        CurrTile->isFighting = false;
        GameManager::instance->minigameActive = false;

        fightingPlayable = nullptr;
        minigameChoice = MinigameRPS_Choice::NONE;

        _loosingFightPlayable->LostPaperRockScissors(this);

        if (CurrTile->ownerEntity == _loosingFightPlayable) {
            CurrTile->ResetTile();
        }

        //playable->CheckIfDead(this);
        CurrTile->StartTakingOver(this);

        _loosingFightPlayable = nullptr;
    }

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
    fightingPlayable = nullptr;
    minigameChoice = MinigameRPS_Choice::NONE;
    CurrTile->StopTakingOver(this);
}

void Enemy::WonPaperRockScissors(Playable* playable)
{
    _loosingFightPlayable = playable;
}

//void Enemy::StartPaperRockScissors(Playable* playable)
//{
//}

float Enemy::GetMaxRadius() const {
    return (_movement->maxSteps + 0.25) * _tilemap->GetDistanceBetweenTiles();
}

void Enemy::SetTileMap(Tilemap::HexagonalTilemap* map)
{
    _tilemap = map;
}

void Enemy::OnDead()
{
    GameManager::instance->EnemyDied(this);
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

        if (Playable::DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}
#endif