#pragma once

#include <core/Component.h>

#include <Playable.h>
#include <AreaTaking/GetMoneyFromTiles.h>

class ConcertAbilityController : public Twin2Engine::Core::Component {
private:

    GameObject* _cityLights = nullptr;
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
    //float savedTakingOverSpeed = 0.0f;
    float cooldownTime = 30.0f;

    // Performing Concert
    void StartPerformingConcert();
    void StopPerformingConcert();

public:
    // Events
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAbilityStarted;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAbilityFinished;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAbilityCooldownStarted;
    Twin2Engine::Tools::EventHandler<Playable*> OnEventAbilityCooldownFinished;

    // Component Parameters
    float lastingTime = 45.0f;
    float moneyRequired = 10.0f;
    float additionalTakingOverSpeed = 50.0f;

    void Initialize();
    void OnDestroy();
    void Update();
    bool Use();

    float GetCost() const;
    float GetAbilityRemainingTime() const;
    float GetCooldownRemainingTime() const;
    float GetCooldown() const;
    float GetAdditionalTakingOverSpeed() const;
    bool IsUsed() const;
    bool IsOnCooldown() const;

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
    virtual bool DrawInheritedFields() override;
public:
    virtual void DrawEditor() override;
#endif
};