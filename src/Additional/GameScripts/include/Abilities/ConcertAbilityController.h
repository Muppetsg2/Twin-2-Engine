#pragma once

#include <core/Component.h>

#include <Playable.h>
#include <AreaTaking/GetMoneyFromTiles.h>

class ConcertAbilityController : public Twin2Engine::Core::Component {
private:

    MoneyFunctionData* moneyFunction;

    float currTimerTime = 0.0f;
    float currCooldown = 0.0f;
    float usedMoneyRequired = 0.0f;

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

public:
    // Component Parameters
    float lastingTime = 10.0f;
    float moneyRequired = 10.0f;
    float cooldownTime = 5.0f;
    float takingOverSpeed = 50.0f;

    void Initialize();
    void OnDestroy();
    void Update();
    bool Use();

    float GetCost() const;
    float GetAbilityRemainingTime() const;
    float GetCooldownRemainingTime() const;

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
    virtual bool DrawInheritedFields() override;
public:
    virtual void DrawEditor() override;
#endif
};