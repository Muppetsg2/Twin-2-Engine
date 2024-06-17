#pragma once 

#include <core/GameObject.h>

#include <Playable.h>
#include <PlayerMovement.h>
#include <GameManager.h>
#include <Patrons/PatronData.h>
#include <RadioStation/RadioStation.h>

#include <UIScripts/PopularityGainingBonusBarController.h>

#include <ui/Button.h>
#include <ui/Image.h>
#include <ui/Text.h>

#include <core/Input.h>
#include <core/AudioComponent.h>

//using namespace Twin2Engine::Core;

//class Playable;
class PlayerMovement;
class ConcertAbilityController;
class MoneyGainFromTiles;

class Player : public Playable {
private:
    //static Mesh* hexMesh;
    Twin2Engine::Core::GameObject* hexIndicator;
    ConcertAbilityController* concertAbility;

    MoneyGainFromTiles* money;
    Prefab* _starPrefab = nullptr;
    bool lost;

    AudioComponent* audioComponent = nullptr;

    size_t _spriteButtonStep1;
    size_t _spriteButtonStep2;
    //Coroutine* fansCorountine;

    float _buttonDeltaYMovement = 10;

    // Player UI
    // Album
    Twin2Engine::UI::Text* albumText;
    Twin2Engine::UI::Button* albumButton;
    Twin2Engine::Core::GameObject* albumButtonObject;
    Twin2Engine::UI::Image* albumButtonFrameImage;
    Twin2Engine::UI::Image* albumCircleImage;
    int albumButtonEventHandleId = -1;
    int albumButtonDestroyedEventHandleId = -1;
    int albumButtonHoveringEventHandleId = -1;
    bool isHoveringAlbumButton = false;
    bool isShowingAlbumPossible = false;

    // FansMeeting
    Twin2Engine::UI::Text* fansMeetingText;
    Twin2Engine::UI::Button* fansMeetingButton;
    Twin2Engine::Core::GameObject* fansMeetingButtonObject;
    Twin2Engine::UI::Image* fansMeetingButtonFrameImage;
    Twin2Engine::UI::Image* fansMeetingCircleImage;
    int fansMeetingButtonEventHandleId = -1;
    int fansMeetingButtonHoveringEventHandleId = -1;
    int fansMeetingButtonDestroyedEventHandleId = -1;
    bool isHoveringFansMeetingButton = false;
    bool isShowingFansMeetingAffectedTiles = false;

    //Concert
    Twin2Engine::UI::Text* concertText;
    Twin2Engine::UI::Button* concertButton;
    Twin2Engine::Core::GameObject* concertButtonObject;
    Twin2Engine::UI::Image* concertButtonFrameImage;
    Twin2Engine::UI::Image* concertCircleImage; 
    int concertButtonEventHandleId = -1;
    int concertButtonDestroyedEventHandleId = -1;
    int concertButtonHoveringEventHandleId = -1;
    bool isHoveringConcertButton = false;
    bool isShowingConcertPossible = false;

    // Money
    Twin2Engine::UI::Text* moneyText;


    glm::vec4 _abilityCooldownColor = glm::vec4(1.f, 0.f, 1.f, 0.275f);
    glm::vec4 _abilityActiveColor = glm::vec4(0.f, 1.f, 1.f, 0.275f);

    // FANS MEETING
    std::list<HexTile*> affectedTiles;
    void ShowAffectedTiles();
    void HideAffectedTiles();

public:
    PlayerMovement* move;
    Playable* fightingPlayable = nullptr;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void OnDestroy() override;
    void StartPlayer(HexTile* startUpTile);

    void AlbumCall();
    void FansMeetingCall();
    void ConcertCall();
    void StartMove(HexTile* tile);
    void FinishMove(HexTile* tile);
    void MinigameEnd();
    //void MinigameEnd(void* sender);
    //void StartPaperRockScisors(Playable* playable);
    //void WonPaperRockScisors(Playable* playable);
    //void LostPaperRockScisors(Playable* playable);

    virtual void LostPaperRockScissors(Playable* playable) override;
    virtual void WonPaperRockScissors(Playable* playable) override;
    //virtual void StartPaperRockScissors(Playable* playable) override;

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