#pragma once

#include <manager/SceneManager.h>

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Random.h>
#include <physic/CollisionManager.h>

#include <Patrons/PatronData.h>

//#include <AreaTaking/GetMoneyFromTiles.h>

#include <MoneyFunctionData.h>
//using namespace Twin2Engine::Core;

// TILEMAP
#include <Tilemap/HexagonalTilemap.h>
class HexTile;

//class MoneyFunctionData {
//public:
//    float GetValue(int count, float startMoney) {
//        return std::exp((count - 1) / 10.0f) + startMoney - 1.0f;
//    }
//};

class Playable : public Twin2Engine::Core::Component {
protected:
    virtual void OnDead() = 0;

    Tilemap::HexagonalTilemap* _tilemap = nullptr;

public:
    struct MinMaxPair {
        float min;
        float max;
    };

    std::list<std::pair<HexTile*, float>> albumTakingOverTiles;

    int colorIdx = 0;
    glm::vec4 Color;
    float TakeOverSpeed = 10.0f;

    PatronData* patron;

#pragma region AlbumAbility
    float albumTime  = 10.0f; // parameter
private:
    float albumCooldown = 5.0f; // parameter
public:
    float usedAlbumCooldown = 5.0f; // parameter
    float currAlbumTime;
    float currAlbumCooldown;
    bool isAlbumActive = false;
    float albumRequiredMoney = 10.0f; // parameter

    float albumTakingOverTimeMin = 5.0f; // parameter
    float albumTakingOverTimeMax = 7.0f; // parameter
    std::vector<MinMaxPair> albumsIncreasingMinMaxIntervals{ {.min = 3.0f, .max = 4.0f }, {.min = 3.0f, .max = 4.0f } }; // parameter
    float takingIntoAttentionFactor = 0.5f; // parameter // Okresla z jakiej czesci tile o najmniejszych wplywach bedzie losowany do oddzialywania przez album
    std::vector<float> albumsIncreasingIntervals;
    std::vector<float> albumsIncreasingIntervalsCounter;
#pragma endregion

#pragma region FansMeetingAbility
    float fansTime = 10.0f; // parameter
private:
    float fansCooldown = 10.0f; // parameter
public:
    float usedFansCooldown = 10.0f; // parameter
    float currFansTime = 0.0f;
    float currFansCooldown = 0.0f;
    float fansRadius = 3.0f; // parameter
    bool isFansActive = false;
    float fansRequiredMoney = 10.0f; // parameter
    std::list<HexTile*> tempFansCollider;
#pragma endregion

    std::list<HexTile*> OwnTiles;
    HexTile* CurrTile;
    HexTile* tileBefore;

    float radioTime = 2.0f;

    Twin2Engine::Core::GameObject* Indicator;

    uint32_t albumUsed = 0;
    uint32_t fansUsed = 0;
    uint32_t concertUsed = 0;
    uint32_t enemiesKilled = 0;
    uint32_t islandsWon = 0;

    float fansStartMoney;
    float albumStartMoney;

    MoneyFunctionData* moneyFunction;
    //MoneyGainFromTiles* money;

#pragma region EVENTS
    // ALBUM EVENTS
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAlbumStarted;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAlbumFinished;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAlbumCooldownStarted;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAlbumCooldownFinished;

    // FANS MEETING EVENTS
    Twin2Engine::Tools::EventHandler<Playable*> OnEventFansMeetingStarted;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventFansMeetingFinished;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventFansMeetingCooldownStarted;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventFansMeetingCooldownFinished;

#pragma endregion

    virtual void Initialize() override;
    virtual void Update() override;


    void InitPrices();
    void UpdatePrices();
    void CreateIndicator();

    //void EndedPlayingRadioStation();

#pragma region AlbumAbility
    void UseAlbum();
    void EndUsingAlbum();
    HexTile* GetAlbumTile();
    void AlbumUpdate();
    void AlbumUpdateCounters();
    void AlbumStoppingTakingOverUpdate();
#pragma endregion

#pragma region FansMeetingAbility
    void FansControlDraw();
    void UseFans();
    void UpdateFans();
    void FansEnd();
    void FansExit();
#pragma endregion

    void RadioStationFunc(float value, float time);
    void RadioStationEndFunc(float value);
    void CheckIfDead(Playable* playable);

    //virtual void OnDead() = 0;

    Playable* fightingPlayable;

    virtual void LostPaperRockScissors(Playable* playable) = 0;
    virtual void WonPaperRockScissors(Playable* playable) = 0;
    virtual void LostFansControl(Playable* playable) = 0;
    virtual void WonFansControl(Playable* playable) = 0;
    //virtual void StartPaperRockScissors(Playable* playable) = 0;
    virtual void StartFansControl(Playable* playable) = 0;

    virtual float GetMaxRadius() const;
    
    void SetPatron(PatronData* patronData);

    float GlobalAvg() const;
    float LocalAvg() const;
    float FansRangeAvg() const;
    float FightPowerScore() const;
    std::vector<HexTile*> GetLocalTiles() const;
    std::vector<HexTile*> GetLocalTakenTiles() const;
    std::vector<HexTile*> GetInMoveRangeTiles() const;
    std::vector<HexTile*> GetFansRangeTiles() const;
    static std::vector<HexTile*> GetInRangeTiles(HexTile* centerTile, float range);

    virtual void SetTileMap(Tilemap::HexagonalTilemap* map);

public:
    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
    virtual void DrawEditor() override;
#endif
};

#include <AreaTaking/HexTile.h>