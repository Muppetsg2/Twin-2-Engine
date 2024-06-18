#pragma once

#include <RadioStation/RadioStation.h>
#include <Playable.h>
#include <Player.h>

#include <core/Component.h>
#include <core/Time.h>
#include <core/Input.h>

#include <graphic/Texture2D.h>

#include <ui/Button.h>
#include <ui/Image.h>

// TODO: Repair

class RadioStationPlayingController : public Twin2Engine::Core::Component
{
private:
    // Notes Data
    ENUM_CLASS(NoteType, UP, RIGHT, DOWN, LEFT)
    std::vector<size_t> _notesSpritesIds;

    static RadioStationPlayingController* _instance;
    bool _gameStarted = false;

    //std::vector<size_t> _resultsImagesIds;
    //std::vector<float> _resultsThresholds;

    Image* _notesArea;
    GameObject* _resultText;
    Text* _remainingTimeText;

    // Notes
    size_t _currentNote = 0;
    glm::vec4 _correctNoteColor = glm::vec4(1.f);
    glm::vec4 _wrongNoteColor = glm::vec4(glm::vec3(0.f), 1.f);
    int _correctCounter = 0;
    float _score = 0.0f;


    // Generation
    float _notesAreaWidth = 100.0f;
    float _notesAreaHeight = 100.0f;
    int _generatedNotesNumber = 4;
    std::vector<NoteType> _generatedNotes;
    //std::vector<Twin2Engine::Core::GameObject*> _notesGameObjects;
    std::vector<Twin2Engine::UI::Image*> _notesImages;

    // Time
    float _timeLimit = 5.0f;
    float _timeLimitCounter = 0.0f;
    float _resultShowingTime = 5.0f;
    float _resultShowingCounter = 0.0f;

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
    void EndPlaying();

public:
    static RadioStationPlayingController* Instance();

    Twin2Engine::Tools::EventHandler<Player*, RadioStation*> OnEventPlayerStartedPlaying;
    Twin2Engine::Tools::EventHandler<Player*, RadioStation*> OnEventPlayerFinishedPlaying;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void OnDestroy() override;

    void Play(RadioStation* radioStation, Playable* playable);

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
    virtual void DrawEditor() override;
#endif
};