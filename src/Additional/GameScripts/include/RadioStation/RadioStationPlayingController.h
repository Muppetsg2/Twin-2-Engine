#pragma once

#include <RadioStation/RadioStation.h>
#include <core/Component.h>
#include <Playable.h>

class RadioStationPlayingController : public Twin2Engine::Core::Component
{
private:
    // Notes Data
    ENUM_CLASS_BASE(NoteType, uint8_t, UP, RIGHT, DOWN, LEFT)
    std::vector<size_t> _notesSpritesIds;

    static RadioStationPlayingController* _instance;
    bool _gameStarted = false;
    bool _playClicked = false;

    Twin2Engine::UI::Image* _notesArea = nullptr;
    Twin2Engine::Core::GameObject* _resultText = nullptr;
    Twin2Engine::Core::GameObject* _playText = nullptr;
    Twin2Engine::UI::Text* _remainingTimeText = nullptr;

    // Notes
    size_t _currentNote = 0;
    glm::vec4 _correctNoteColor = glm::vec4(1.f);
    glm::vec4 _wrongNoteColor = glm::vec4(glm::vec3(0.f), 1.f);
    int _correctCounter = 0;
    float _score = 0.0f;

    Twin2Engine::Core::AudioComponent* _audioComponent = nullptr;

    // Notes Sounds
    size_t _soundDown = 0;
    size_t _soundLeft = 0;
    size_t _soundRight = 0;
    size_t _soundUp = 0;
    size_t _soundDownFalse = 0;
    size_t _soundLeftFalse = 0;
    size_t _soundRightFalse = 0;
    size_t _soundUpFalse = 0;

    // Generation
    float _notesAreaWidth = 100.0f;
    float _notesAreaHeight = 100.0f;
    int _generatedNotesNumber = 4;
    std::vector<NoteType> _generatedNotes;
    std::vector<Twin2Engine::UI::Image*> _notesImages;

    // Time
    float _timeLimit = 5.0f;
    float _timeLimitCounter = 0.0f;
    float _resultShowingTime = 5.0f;
    float _resultShowingCounter = 0.0f;
    float _timeChangeColorPercent = 20.f;

    // Reference To Start Objects
    RadioStation* _radioStation = nullptr;
    Playable* _playable = nullptr;

    // Buttons
    Twin2Engine::UI::Button* _buttonUp;
    Twin2Engine::UI::Button* _buttonRight;
    Twin2Engine::UI::Button* _buttonDown;
    Twin2Engine::UI::Button* _buttonLeft;

    // Buttons Events
    int _buttonUpHandleId = -1;
    int _buttonRightHandleId = -1;
    int _buttonDownHandleId = -1;
    int _buttonLeftHandleId = -1;

    void GenerateNotes();
    void PlayNote(NoteType note);
    void ShowResult();
    void HideResult();
    void EndPlaying();

public:
    static RadioStationPlayingController* Instance();

    Twin2Engine::Tools::EventHandler<Player*, RadioStation*> OnEventPlayerStartedPlaying;
    Twin2Engine::Tools::EventHandler<Player*, RadioStation*> OnEventPlayerFinishedPlaying;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void OnDestroy() override;

    void Play(RadioStation* radioStation, Playable* playable);
    void PlayClicked();

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
    virtual void DrawEditor() override;
#endif
};