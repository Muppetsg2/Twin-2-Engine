#pragma once

#include <RadioStation/RadioStation.h>
#include <Playable.h>
#include <Player.h>

#include <core/Component.h>
#include <core/Time.h>

#include <physic/CollisionManager.h>

class RadioStationPlayingController;

class RadioStation : public Twin2Engine::Core::Component
{
    friend class RadioStationPlayingController;
    
    std::list<HexTile*> _takenTiles;
    
    Playable* _takenPlayable;

    float _takingOverTime = 0.0f;
    float _takingOverTimer = 0.0f;

    float _cooldown = 0.0f;
    float _cooldownTimer = 0.0f;


public:
    float takingRadius = 3.0f;

    virtual void Initialize() override;
    virtual void Update() override;


    void Play(Playable* playable);
    void StartTakingOver(Playable* playable, float score);

    float GetRemainingCooldownTime() const;


    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
    virtual bool DrawInheritedFields() override;
public:
    virtual void DrawEditor() override;
#endif
};