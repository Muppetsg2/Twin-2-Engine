#include <Playable.h>

#include <AreaTaking/HexTile.h>
#include <Generation/MapHexTile.h>

using namespace std;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Physic;

void Playable::Initialize()
{
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
    //if (patron->patronBonus == PatronBonus::ABILITIES_COOLDOWN) {
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
#pragma region AlbumAbility

HexTile* Playable::GetAlbumTile() {
    std::vector<HexTile*> tiles;
    tiles.reserve(OwnTiles.size() - albumTakingOverTiles.size());

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

    int index = Random::Range(0ull, (size_t)std::floor(takingIntoAttentionFactor * tiles.size()));
    return tiles[index];
}

void Playable::AlbumUpdate() {
    if (isAlbumActive) {
        AlbumUpdateCounters();
        currAlbumTime -= Time::GetDeltaTime();
        if (currAlbumTime < 0.0f) {
            currAlbumCooldown = albumCooldown;
            //currAlbumTime = 0.0f;
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
    currAlbumTime = albumTime;

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

#pragma region FansMeetingAbility

void Playable::FansControlDraw()
{
}

void Playable::UseFans() {
    fansUsed++;
    float usedRadius = fansRadius;

    currFansTime = fansTime;
    tileBefore = CurrTile;

    //if (patron->patronBonus == PatronBonus::ABILITIES_RANGE) {
    //    usedRadius += patron->GetBonus();
    //}

    vector<ColliderComponent*> colliders;
    CollisionManager::Instance()->OverlapSphere(CurrTile->GetGameObject()->GetTransform()->GetGlobalPosition(), usedRadius, colliders);

    vec3 usedGlobalPosition = GetTransform()->GetGlobalPosition();
    usedGlobalPosition.y = 0.0f;

    for (auto col : colliders) {
        if (col)
        {
            auto tile = col->GetGameObject()->GetComponent<HexTile>();

            if (tile && tile != CurrTile) {

                vec3 tileUsedGlobalPosition = tile->GetTransform()->GetGlobalPosition();
                tileUsedGlobalPosition.y = 0.0f;

                float mul = glm::distance(usedGlobalPosition, tileUsedGlobalPosition);
                mul = mul > 1.0f ? 1.0f - std::floor(mul) / usedRadius : 1.0f;

                if (mul > 0.0f) {
                    tile->StartRemotelyTakingOver(this, mul);
                    tempFansCollider.push_back(tile);
                }
            }
        }
    }

    isFansActive = true;
}

void Playable::FansEnd() {
    SPDLOG_INFO("Ending Fans");

    for (auto tile : tempFansCollider) {
        if (tile && tile != CurrTile) {
            tile->StopTakingOver(this);
        }
    }
    
    isFansActive = false;

}

void Playable::FansExit() {
    SPDLOG_INFO("Exit Fans");
    currFansCooldown = fansCooldown;
    currFansTime = 0.0f;
    // Assuming fansCoroutine is a thread or similar, use appropriate stopping mechanism
    FansEnd();
}

#pragma endregion

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

float Playable::GetMaxRadius() const {
#if TRACY_PROFILER
    ZoneScoped;
#endif

    return 0.f;
}

float Playable::GlobalAvg() const
{
#if TRACY_PROFILER
    ZoneScoped;
#endif

    float res = 0.f;
    for (auto& tile : OwnTiles) {
        res += tile->percentage;
    }
    // AllTakenTilesPercent / TakenTilesCount
    return res / OwnTiles.size();
}

float Playable::LocalAvg() const
{
#if TRACY_PROFILER
    ZoneScoped;
#endif

    // TODO: POLICZY� ILE JEST TILIE OBOK CURR TILE-a
    size_t neightboursCount = 6;
    float res = 0.f;

    vec3 currTilePos = CurrTile->GetTransform()->GetGlobalPosition();
    for (auto& tile : OwnTiles) {
        if (glm::distance(currTilePos, tile->GetTransform()->GetGlobalPosition()) <= _tilemap->GetDistanceBetweenTiles()) {
            res += tile->percentage;
        }
    }
    // AllTakenTilesNextToCurrentTilePercent / TakenTilesNextToCurrentTileCount
    return res / neightboursCount;
}

YAML::Node Playable::Serialize() const
{
    return Component::Serialize();
}

bool Playable::Deserialize(const YAML::Node& node)
{
    return false;
}
#if _DEBUG
void Playable::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("Playable##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {
        if (Component::DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}
#endif