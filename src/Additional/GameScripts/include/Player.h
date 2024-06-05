#pragma once 

#include <core/GameObject.h>

#include <Playable.h>
#include <PlayerMovement.h>
#include <GameManager.h>
#include <Patrons/PatronData.h>
#include <RadioStation/RadioStation.h>


#include <ui/Button.h>
#include <ui/Text.h>

#include <core/Input.h>

//using namespace Twin2Engine::Core;

//class Playable;
class PlayerMovement;
class ConcertAbilityController;
class MoneyGainFromTiles;

class Player : public Playable {
private:
    //static Mesh* hexMesh;
    Twin2Engine::Core::GameObject* hexIndicator;
    Tilemap::HexagonalTilemap* _tilemap = nullptr;
    ConcertAbilityController* concertAbility;

    MoneyGainFromTiles* money;

    PlayerMovement* move;
    bool lost;

    Playable* fightingPlayable = nullptr;
    //Coroutine* fansCorountine;


    // Player UI
    // Album
    Twin2Engine::UI::Text* albumText;
    Twin2Engine::UI::Button* albumButton;
    Twin2Engine::Core::GameObject* albumButtonObject;
    size_t albumButtonEventHandleId;
    size_t albumButtonDestroyedEventHandleId;
    // FansMeeting
    Twin2Engine::UI::Text* fansMeetingText;
    Twin2Engine::UI::Button* fansMeetingButton;
    Twin2Engine::Core::GameObject* fansMeetingButtonObject;
    size_t fansMeetingButtonEventHandleId;
    size_t fansMeetingButtonDestroyedEventHandleId;
    //Concert
    Twin2Engine::UI::Text* concertText;
    Twin2Engine::UI::Button* concertButton;
    Twin2Engine::Core::GameObject* concertButtonObject;
    size_t concertButtonEventHandleId;
    size_t concertButtonDestroyedEventHandleId;
    // Money
    Twin2Engine::UI::Text* moneyText;


public:

    virtual void Initialize() override;
    virtual void Update() override;

    void AlbumCall();
    void FansMeetingCall();
    void ConcertCall();
    void FansControlDraw();
    void StartMove(HexTile* tile);
    void FinishMove(HexTile* tile);
    void MinigameEnd();
    //void MinigameEnd(void* sender);
    //void StartPaperRockScisors(Playable* playable);
    //void WonPaperRockScisors(Playable* playable);
    //void LostPaperRockScisors(Playable* playable);
    //void WonFansControl(Playable* playable);
    //void StartFansControl(Playable* playable);
    //void LostFansControl(Playable* playable);

    virtual void LostPaperRockScissors(Playable* playable) override;
    virtual void WonPaperRockScissors(Playable* playable) override;
    virtual void LostFansControl(Playable* playable) override;
    virtual void WonFansControl(Playable* playable) override;
    virtual void StartPaperRockScissors(Playable* playable) override;
    virtual void StartFansControl(Playable* playable) override;

    float GetMaxRadius() const override;

protected:
    virtual void OnDead() override;


public:
	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};