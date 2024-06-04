#include <Playable.h>

#include <AreaTaking/HexTile.h>
#include <Generation/MapHexTile.h>

using namespace std;
using namespace Twin2Engine::Core;

void Playable::Initialize()
{
    if (patron->patronBonus == PatronBonus::ABILITIES_COOLDOWN) {
        albumCooldown *= patron->GetBonus() / 100.0f;
        fansCooldown *= patron->GetBonus() / 100.0f;
    }
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
    for (auto tile : OwnTiles) {
        if (tile->isAlbumActive && std::find_if(albumTakingOverTiles.cbegin(), albumTakingOverTiles.cend(), [tile](const std::pair<HexTile*, float>& record) { return record.first == tile; }) == albumTakingOverTiles.end()) {
            tiles.push_back(tile);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(tiles.begin(), tiles.end(), g);

    std::sort(tiles.begin(), tiles.end(), [](HexTile* tile1, HexTile* tile2) {
        return tile1->percentage < tile2->percentage;
        });

    if (tiles.empty()) {
        return nullptr;
    }

    int index = std::rand() % static_cast<int>(std::ceil(takingIntoAttentionFactor * tiles.size()));
    return tiles[index];
}

void Playable::FansControlDraw()
{
}


void Playable::AlbumUpdate() {
    for (size_t i = 0; i < albumsIncreasingIntervalsCounter.size(); ++i) {
        albumsIncreasingIntervalsCounter[i] += 1.0f; // Assuming this is called every second
        if (albumsIncreasingIntervalsCounter[i] >= albumsIncreasingIntervals[i]) {
            albumsIncreasingIntervalsCounter[i] -= albumsIncreasingIntervals[i];
            albumsIncreasingIntervals[i] = Random::Range(albumsIncreasingMinMaxIntervals[i].min, albumsIncreasingMinMaxIntervals[i].max);
            
            HexTile* tile = GetAlbumTile();
            if (tile != nullptr) {
                albumTakingOverTiles.emplace_back(tile, Random::Range(albumTakingOverTimeMin, albumTakingOverTimeMax));
                tile->StartRemotelyTakingOver(this);
            }
        }
    }
    for (size_t i = 0; i < albumsIncreasingIntervalsCounter.size(); ++i) {
        albumsIncreasingIntervalsCounter[i] += Time::GetDeltaTime();
        if (albumsIncreasingIntervalsCounter[i] >= albumsIncreasingIntervals[i]) {
            albumsIncreasingIntervalsCounter[i] -= albumsIncreasingIntervals[i];
            albumsIncreasingIntervals[i] = Random::Range(albumsIncreasingMinMaxIntervals[i].min, albumsIncreasingMinMaxIntervals[i].max);

            HexTile* tile = GetAlbumTile();
            if (tile) {
                albumTakingOverTiles.emplace_back(tile, Random::Range(albumTakingOverTimeMin, albumTakingOverTimeMax));
                tile->StartRemotelyTakingOver(this);
            }
        }
    }
}

void Playable::AlbumStoppingTakingOverUpdate() {
    list<pair<HexTile*, float>> toRemove;
    for (auto& record : albumTakingOverTiles) {
        record.second += Time::GetDeltaTime();
        if (record.second >= record.first->percentage) {
            record.first->StopTakingOver(this);
            toRemove.push_back(record);
        }
    }

    for (auto& record : toRemove) {
        albumTakingOverTiles.remove(record);
    }
}

void Playable::AlbumFunc() {

    albumUsed++;
    albumsIncreasingIntervals.resize(albumsIncreasingMinMaxIntervals.size());
    albumsIncreasingIntervalsCounter.resize(albumsIncreasingMinMaxIntervals.size());

    for (size_t i = 0; i < albumsIncreasingMinMaxIntervals.size(); ++i) {
        albumsIncreasingIntervals[i] = std::rand() % static_cast<int>(albumsIncreasingMinMaxIntervals[i].max - albumsIncreasingMinMaxIntervals[i].min) + albumsIncreasingMinMaxIntervals[i].min;
    }

    for (auto tile : OwnTiles) {
        if (tile->occupyingEntity != this) {
            tile->isAlbumActive = true;
        }
    }

    isAlbumActive = true;
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(albumTime)));

    while (currAlbumTime > 0.0f) {
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(currAlbumTime)));
    }

    for (auto tile : OwnTiles) {
        if (tile->occupyingEntity != this) {
            tile->isAlbumActive = false;
        }
    }

    isAlbumActive = false;
    return;
}

void Playable::FansFunc() {
    fansUsed++;
    float usedRadius = fansRadius;

    if (patron->patronBonus == PatronBonus::ABILITIES_RANGE) {
        usedRadius += patron->GetBonus();
    }

    //auto tempFansCollider = Physics::OverlapSphere(CurrTile->GetGameObject()->GetTransform()->GetGlobalPosition(), usedRadius);

    //for (auto obj : tempFansCollider) {
    //    if (auto tile = obj->GetComponent<HexTile>(); tile && tile != CurrTile) {
    //        float mul = glm::distance(GetTransform()->GetGlobalPosition(), tile->GetTransform()->GetGlobalPosition());
    //        mul = mul > 1.0f ? 1.0f - std::floor(mul) / usedRadius : 1.0f;
    //
    //        if (mul > 0.0f) {
    //            tile->StartRemotlyTakingOver(this, mul);
    //        }
    //    }
    //}

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