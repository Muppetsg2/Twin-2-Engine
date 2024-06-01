#ifndef MONEYGAINFROMTILES_H
#define MONEYGAINFROMTILES_H

#include <core/Component.h>

//#include <Playable.h>

#include <GameManager.h>
#include <AreaTaking/HexTile.h>
#include <Generation/MapHexTile.h>
//#include "MoneyPanelController.h"
class Playable;
//class HexTile;

class MoneyGainFromTiles : public Twin2Engine::Core::Component {
public:
    float gainingInterval = 1.0f;
    float moneyBaseFactor = 1.0f;
    float money = 0;

    virtual void Initialize() override;
    virtual void OnEnable() override;
    virtual void Update() override;
    bool SpendMoney(float amount);

private:
    Playable* playable;

    float GetTileTypeFactor(Generation::MapHexTile::HexTileType tileType);
    float GainMoneyFromTile(HexTile* tile);
    void UpdateMoney(bool player = true);

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
    virtual bool DrawInheritedFields();
public:
    virtual void DrawEditor();
#endif
};

#endif // MONEYGAINFROMTILES_H
