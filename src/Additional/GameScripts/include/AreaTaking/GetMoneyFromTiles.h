#pragma once
#include <core/Component.h>

//#include <Playable.h>

#include <GameManager.h>
#include <AreaTaking/HexTile.h>
#include <Generation/MapHexTile.h>

#include <GameTimer.h>

//#include "MoneyPanelController.h"
class Playable;
//class HexTile;

class MoneyGainFromTiles : public Twin2Engine::Core::Component {
public:
    float gainingInterval = 1.0f;
    float moneyBaseFactor = 1.0f;
    float money = 0.0f;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void OnDestroy() override;
    bool SpendMoney(float amount);
    bool CheckCanSpendMoney(float amount);

private:
    Playable* playable;
    size_t eventIdOnDayTicked;

    float GetTileTypeFactor(Generation::MapHexTile::HexTileType tileType);
    float GainMoneyFromTile(HexTile* tile);
    void UpdateMoney(bool player = true);

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
public:
    virtual void DrawEditor();
#endif
};