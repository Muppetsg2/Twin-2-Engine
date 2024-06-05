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
    Playable::Initialize();
    _movement = GetGameObject()->GetComponent<EnemyMovement>();
    list<HexTile*> tempList = _tilemap->GetGameObject()->GetComponentsInChildren<HexTile>();
    _tiles.insert(_tiles.begin(), tempList.cbegin(), tempList.cend());
    ChangeState(&_initState);
}


void Enemy::OnEnable()
{
    //PerformMovement();
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