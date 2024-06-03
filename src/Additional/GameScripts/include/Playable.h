#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Random.h>
#include <physic/CollisionManager.h>

#include <Patrons/PatronData.h>

//#include <AreaTaking/GetMoneyFromTiles.h>

#include <MoneyFunctionData.h>
//using namespace Twin2Engine::Core;

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
    float albumCooldown = 5.0f; // parameter
    float currAlbumTime;
    float currAlbumCooldown;
    bool isAlbumActive = false;
    float albumRequiredMoney = 10.0f; // parameter

    float albumTakingOverTimeMin = 5.0f; // parameter
    float albumTakingOverTimeMax = 7.0f; // parameter
    std::vector<MinMaxPair> albumsIncreasingMinMaxIntervals{ {.min = 3.0f, .max = 4.0f }, {.min = 3.0f, .max = 4.0f } }; // parameter
    float takingIntoAttentionFactor = 0.5f; // parameter // Okreœla z jakiej czêœci tile o najmniejszych wp³ywach bêdzie losowany do oddzia³ywania przez album
    std::vector<float> albumsIncreasingIntervals;
    std::vector<float> albumsIncreasingIntervalsCounter;
#pragma endregion

#pragma region FansMeetingAbility
    float fansTime = 10.0f; // parameter
    float fansCooldown = 10.0f; // parameter
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

    //MoneyFunctionData* functionData;
    //MoneyGainFromTiles* money;

    virtual void Initialize() override;
    virtual void Update() override;


    void InitPrices();
    void UpdatePrices();
    void CreateIndicator();

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
    void FansEnd();
    void FansExit();
#pragma endregion

    void RadioStationFunc(float value, float time);
    void RadioStationEndFunc(float value);
    void CheckIfDead(Playable* playable);

    //virtual void OnDead() = 0;

    virtual void LostPaperRockScissors(Playable* playable) = 0;
    virtual void WonPaperRockScissors(Playable* playable) = 0;
    virtual void LostFansControl(Playable* playable) = 0;
    virtual void WonFansControl(Playable* playable) = 0;
    virtual void StartPaperRockScissors(Playable* playable) = 0;
    virtual void StartFansControl(Playable* playable) = 0;

public:
    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
    virtual bool DrawInheritedFields();
public:
    virtual void DrawEditor() override;
#endif
};
