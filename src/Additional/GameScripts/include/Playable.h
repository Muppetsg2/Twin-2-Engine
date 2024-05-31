#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Random.h>

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

    class AlbumHexTileTakingOverRecord {
    public:
        std::pair<HexTile*, float> Data;
    };

    std::list<std::pair<HexTile*, float>> albumTakingOverTiles;

    int colorIdx = 0;
    glm::vec4 Color;
    float TakeOverSpeed;

    PatronData* patron;

    float albumTime;
    float albumCooldown;
    float currAlbumTime;
    float currAlbumCooldown;
    bool isAlbumActive = false;
    float albumRequiredMoney = 10.0f;

    float albumTakingOverTimeMin = 0.0f;
    float albumTakingOverTimeMax = 0.0f;
    std::vector<MinMaxPair> albumsIncreasingMinMaxIntervals;
    float takingIntoAttentionFactor = 0.5f;
    std::vector<float> albumsIncreasingIntervals;
    std::vector<float> albumsIncreasingIntervalsCounter;

    float fansTime;
    float fansCooldown;
    float currFansTime;
    float currFansCooldown;
    float fansRadius = 1.0f;
    bool isFansActive = false;
    float fansRequiredMoney = 10.0f;

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
    void InitPrices();
    void UpdatePrices();
    void CreateIndicator();
    HexTile* GetAlbumTile();
    void AlbumUpdate();
    void AlbumStoppingTakingOverUpdate();
    void AlbumFunc();
    void FansControlDraw();
    void FansFunc();
    void FansEndFunc();
    void FansExit();
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
