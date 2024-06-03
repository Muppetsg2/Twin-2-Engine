#include <Playable.h>

#include <AreaTaking/HexTile.h>
#include <Generation/MapHexTile.h>

using namespace std;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

void Playable::Initialize()
{
    //if (patron->patronBonus == PatronBonus::AbilitiesCooldown) {
    //    albumCooldown *= patron->GetBonus() / 100.0f;
    //    fansCooldown *= patron->GetBonus() / 100.0f;
    //}
}

void Playable::Update()
{
}


void Playable::InitPrices() {
    fansStartMoney = fansRequiredMoney;
    albumStartMoney = albumRequiredMoney;
    //money = GetGameObject()->GetComponent<MoneyGainFromTiles>();
}

void Playable::UpdatePrices() {
    if (!OwnTiles.empty()) {
        //fansRequiredMoney = functionData->GetValue(OwnTiles.size() - 1, fansStartMoney);
        //albumRequiredMoney = functionData->GetValue(OwnTiles.size() - 1, albumStartMoney);
    }
}

void Playable::CreateIndicator() {
    //Indicator = Instantiate(GameManager::instance().sterowiec, Vector3(0, 0, 0), Quaternion::identity, gameObject.transform);
    //Indicator->GetComponent<MeshRenderer>()->material.color = GetColor();
    //Indicator->SetActive(false);
}

HexTile* Playable::GetAlbumTile() {
    std::vector<HexTile*> tiles;
    tiles.reserve(OwnTiles.size() - albumTakingOverTiles.size());

    SPDLOG_INFO("OwnTiles size: {}", OwnTiles.size());
    for (auto tile : OwnTiles) {
        //if (tile->isAlbumActive && std::find_if(albumTakingOverTiles.cbegin(), albumTakingOverTiles.cend(), [tile](const std::pair<HexTile*, float>& record) { return record.first == tile; }) == albumTakingOverTiles.end()) {
        if (std::find_if(albumTakingOverTiles.cbegin(), albumTakingOverTiles.cend(), [tile](const std::pair<HexTile*, float>& record) { return record.first == tile; }) == albumTakingOverTiles.end()) {
            tiles.push_back(tile);
        }
    }

    if (tiles.empty()) {
        return nullptr;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tiles.begin(), tiles.end(), g);

    std::sort(tiles.begin(), tiles.end(), [](HexTile* tile1, HexTile* tile2) {
        return tile1->percentage < tile2->percentage;
        });

    int index = Random::Range(0ull, (size_t) std::floor(takingIntoAttentionFactor * tiles.size()));
    return tiles[index];
}

void Playable::FansControlDraw()
{
}

#pragma region AlbumAbility

void Playable::AlbumUpdate() {
    if (isAlbumActive) {
        AlbumUpdateCounters();
        currAlbumTime -= Time::GetDeltaTime();
        if (currAlbumTime < 0.0f) {
            currAlbumCooldown = albumCooldown;
            currAlbumTime = 0.0f;
            EndUsingAlbum();
        }
    }
    AlbumStoppingTakingOverUpdate();
}

void Playable::AlbumUpdateCounters() {
    for (size_t i = 0; i < albumsIncreasingIntervalsCounter.size(); ++i) {
        albumsIncreasingIntervalsCounter[i] += Time::GetDeltaTime();
        if (albumsIncreasingIntervalsCounter[i] >= albumsIncreasingIntervals[i]) {
            albumsIncreasingIntervalsCounter[i] -= albumsIncreasingIntervals[i];
            albumsIncreasingIntervals[i] = Random::Range(albumsIncreasingMinMaxIntervals[i].min, albumsIncreasingMinMaxIntervals[i].max);

            HexTile* tile = GetAlbumTile();
            if (tile) {
                albumTakingOverTiles.emplace_back(tile, Random::Range(albumTakingOverTimeMin, albumTakingOverTimeMax));
                tile->StartRemotelyTakingOver(this, 10.0f);
            }
        }
    }
}

void Playable::AlbumStoppingTakingOverUpdate() {
    list<pair<HexTile*, float>> toRemove;
    for (auto& record : albumTakingOverTiles) {
        record.second -= Time::GetDeltaTime();
        if (record.second <= 0.0f) {
            record.first->StopTakingOver(this);
            toRemove.push_back(record);
        }
    }

    for (auto& record : toRemove) {
        albumTakingOverTiles.remove(record);
    }
}

void Playable::UseAlbum() {

    albumUsed++;
    albumsIncreasingIntervals.resize(albumsIncreasingMinMaxIntervals.size());
    albumsIncreasingIntervalsCounter.resize(albumsIncreasingMinMaxIntervals.size());

    SPDLOG_INFO("Album int size: {}", albumsIncreasingMinMaxIntervals.size());
    for (size_t i = 0ull; i < albumsIncreasingMinMaxIntervals.size(); ++i) {
        albumsIncreasingIntervals[i] = Random::Range(albumsIncreasingMinMaxIntervals[i].min, albumsIncreasingMinMaxIntervals[i].min);
        HexTile* tile = GetAlbumTile();
        if (tile) {
            albumTakingOverTiles.emplace_back(tile, Random::Range(albumTakingOverTimeMin, albumTakingOverTimeMax));
            tile->StartRemotelyTakingOver(this, 10.0f);
        }
    }

    for (auto tile : OwnTiles) {
        if (tile->occupyingEntity != this) {
            tile->isAlbumActive = true;
        }
    }
    isAlbumActive = true;
}

void Playable::EndUsingAlbum() {

    isAlbumActive = false;

    for (auto tile : OwnTiles) {
        if (tile->occupyingEntity != this) {
            tile->isAlbumActive = false;
        }
    }
}

#pragma endregion

void Playable::FansFunc() {
    fansUsed++;
    float usedRadius = fansRadius;

    if (patron->patronBonus == PatronBonus::AbilitiesRange) {
        usedRadius += patron->GetBonus();
    }

    vector<ColliderComponent*> colliders;
    CollisionManager::Instance()->OverlapSphere(CurrTile->GetGameObject()->GetTransform()->GetGlobalPosition(), usedRadius, colliders);

    for (auto col : colliders) {
        if (auto tile = col->GetGameObject()->GetComponent<HexTile>(); tile && tile != CurrTile) {
            float mul = glm::distance(GetTransform()->GetGlobalPosition(), tile->GetTransform()->GetGlobalPosition());
            mul = mul > 1.0f ? 1.0f - std::floor(mul) / usedRadius : 1.0f;
    
            if (mul > 0.0f) {
                tile->StartRemotelyTakingOver(this, mul);
            }
        }
    }

    isFansActive = true;
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(fansTime)));

    while (currFansTime > 0.0f) {
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(currFansTime)));
    }

    FansEndFunc();
    //co_return;
}
void Playable::FansEndFunc() {
    //for (auto obj : tempFansCollider) {
    //    if (auto tile = obj->GetComponent<HexTile>(); tile && tile != CurrTile) {
    //        tile->StopTakingOver(this);
    //    }
    //}
    //
    //isFansActive = false;
    //
    //isFansActive = false;

}

void Playable::FansExit() {
    currFansCooldown = fansCooldown;
    currFansTime = 0.0f;
    // Assuming fansCoroutine is a thread or similar, use appropriate stopping mechanism
    FansEndFunc();
}

void Playable::RadioStationFunc(float value, float time) {
    //tempRadioColliders = Physics::OverlapSphere(CurrTile->gameObject.transform.position, value);

    //for (auto obj : tempRadioColliders) {
    //    if (auto tile = obj->GetComponent<HexTile>(); tile && tile != CurrTile) {
    //        tile->StartRemotlyTakingOver(this);
    //    }
    //}

    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(time)));
    RadioStationEndFunc(value);
    //co_return;
}

void Playable::RadioStationEndFunc(float value) {
    //for (auto obj : tempRadioColliders) {
    //    if (auto tile = obj->GetComponent<HexTile>(); tile && tile != CurrTile) {
    //        tile->StopTakingOver(this);
    //    }
    //}
}

void Playable::CheckIfDead(Playable* playable) {
    if (OwnTiles.empty()) {
        ++playable->enemiesKilled;
        OnDead();
    }
}

YAML::Node Playable::Serialize() const
{
    return YAML::Node();
}

bool Playable::Deserialize(const YAML::Node& node)
{
    return false;
}
#if _DEBUG
bool Playable::DrawInheritedFields()
{
    return true;
}

void Playable::DrawEditor()
{
}
#endif