#pragma once

#include <core/Component.h>

#include <Playable.h>
#include <AreaTaking/GetMoneyFromTiles.h>

class ConcertAbilityController : public Twin2Engine::Core::Component {
public:

    void Initialize();
    void Update();
    void Use();

private:
    // Component Parameters
    float lastingTime = 10.0f;
    float moneyRequired = 10.0f;
    float cooldownTime = 5.0f;
    float takingOverSpeed = 50.0f;

    MoneyFunctionData* moneyFunction;

    float currTimerTime = 0.0f;
    float currCooldown = 0.0f;
    float startMoneyRequired = 0.0f;

    // Cooldown
    void StartCooldown();

    // Components
    Playable* playable;
    MoneyGainFromTiles* money;

    // Script Fields
    bool canUse = true;
    float savedTakingOverSpeed = 0.0f;

    // Performing Concert
    void StartPerformingConcert();
    void StopPerformingConcert();

    float GetCost() const;

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
    virtual bool DrawInheritedFields() override;
public:
    virtual void DrawEditor() override;
#endif
};