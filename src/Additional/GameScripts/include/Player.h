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
    friend class GameManager;
private:
    //static Mesh* hexMesh;
    Twin2Engine::Core::GameObject* _hexIndicator;
    ConcertAbilityController* _concertAbility;

    MoneyGainFromTiles* _money;
    Prefab* _starPrefab = nullptr;
    bool _lost;

    // AUDIO
    AudioComponent* _audioComponent = nullptr;
    size_t _onHoverClickAudio = 0;
    size_t _offHoverClickAudio = 0;
    size_t _abilitiesUseAudio = 0;
    size_t _abilitiesEndAudio = 0;
    size_t _cooldownEndAudio = 0;
    size_t _notEnoughResAudio = 0;

    size_t _spriteButtonStep1;
    size_t _spriteButtonStep2;
    //Coroutine* fansCorountine;

    float _buttonDeltaYMovement = 10;

    HexTile* _startMove = nullptr;
    HexTile* _finishMove = nullptr;


    Playable* _loosingFightPlayable = nullptr;

    // Player UI
    glm::vec4 _abilityInactiveColor = glm::vec4(0.2, 0.2, 0.2, 1.0f);

    // Album
    Twin2Engine::UI::Text* _albumText = nullptr;
    Twin2Engine::UI::Button* _albumButton = nullptr;
    Twin2Engine::Core::GameObject* _albumButtonObject = nullptr;
    Twin2Engine::Core::Transform* _albumKey = nullptr;
    Twin2Engine::UI::Image* _albumButtonFrameImage = nullptr;
    Twin2Engine::UI::Image* _albumCircleImage = nullptr;
    Twin2Engine::UI::Image* _albumImage = nullptr;
    int _albumButtonEventHandleId = -1;
    int _albumButtonDestroyedEventHandleId = -1;
    int _albumButtonHoveringEventHandleId = -1;
    bool _isHoveringAlbumButton = false;
    bool _isShowingAlbumPossible = false;

    // FansMeeting
    Twin2Engine::UI::Text* _fansMeetingText = nullptr;
    Twin2Engine::UI::Button* _fansMeetingButton = nullptr;
    Twin2Engine::Core::GameObject* _fansMeetingButtonObject = nullptr;
    Twin2Engine::Core::Transform* _fansMeetingKey = nullptr;
    Twin2Engine::UI::Image* _fansMeetingButtonFrameImage = nullptr;
    Twin2Engine::UI::Image* _fansMeetingCircleImage = nullptr;
    Twin2Engine::UI::Image* _fansImage = nullptr;
    int _fansMeetingButtonEventHandleId = -1;
    int _fansMeetingButtonHoveringEventHandleId = -1;
    int _fansMeetingButtonDestroyedEventHandleId = -1;
    bool _isHoveringFansMeetingButton = false;
    bool _isShowingFansMeetingAffectedTiles = false;

    bool _endFans = false;

    glm::vec3 _keyPosition = glm::vec3(-45.0f, 45.0f, 0.0f);

    //Concert
    Twin2Engine::UI::Text* _concertText = nullptr;
    Twin2Engine::UI::Button* _concertButton = nullptr;
    Twin2Engine::Core::GameObject* _concertButtonObject = nullptr;
    Twin2Engine::Core::Transform* _concertKey = nullptr;
    Twin2Engine::UI::Image* _concertButtonFrameImage = nullptr;
    Twin2Engine::UI::Image* _concertCircleImage = nullptr;
    Twin2Engine::UI::Image* _concertImage = nullptr;
    int _concertButtonEventHandleId = -1;
    int _concertButtonDestroyedEventHandleId = -1;
    int _concertButtonHoveringEventHandleId = -1;
    bool _isHoveringConcertButton = false;
    bool _isShowingConcertPossible = false;

    // Money
    Twin2Engine::UI::Text* _moneyText;
    Twin2Engine::UI::Text* _negativeMoneyText;

    glm::vec4 _enoughMoneyColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 _notEnoughMoneyColor = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
    float _negativeMoneyTextXOffset = 150.0f;
    float _negativeMoneyTextLetterWidth = 25.0f;


    glm::vec4 _abilityCooldownColor = glm::vec4(1.f, 0.f, 1.f, 0.275f);
    glm::vec4 _abilityActiveColor = glm::vec4(0.f, 1.f, 1.f, 0.275f);

    // FANS MEETING
    std::list<HexTile*> _affectedTiles;
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
    void ResetOnNewMap();
    //void MinigameEnd(void* sender);

    //virtual void StartPaperRockScissors(Playable* playable) override;
    virtual void WonPaperRockScissors(Playable* playable) override;
    virtual void LostPaperRockScissors(Playable* playable) override;

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