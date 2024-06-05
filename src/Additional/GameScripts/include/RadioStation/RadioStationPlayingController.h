#pragma once

#include <RadioStation/RadioStation.h>
#include <Playable.h>
#include <Player.h>

#include <core/Component.h>
#include <core/Time.h>

#include <graphic/Texture2D.h>

#include <ui/Button.h>
#include <ui/Image.h>

class RadioStationPlayingController : public Twin2Engine::Core::Component
{
    ENUM_CLASS(NoteType, Do, Re, Mi, Fa)

    static RadioStationPlayingController* instance;

    std::vector<NoteType> _generatedNotes;
    std::vector<Twin2Engine::Core::GameObject*> _notesGameObjects;
    std::vector<Twin2Engine::UI::Image*> _notesImages;
    
    std::vector<size_t> _notesSpritesIds;

    glm::vec4 _correctColor;
    glm::vec4 _wrongColor;

    size_t _currentNote = 0ull;
    int _generatedNotesNumber = 4;
    int _correctCounter = 0;

    float _notesAreaWidth = 600.0f;
    float _notesAreaHeight = 200.0f;

    RadioStation* _radioStation = nullptr;
    Playable* _playable = nullptr;

    Twin2Engine::UI::Button* _buttonDo;
    Twin2Engine::UI::Button* _buttonRe;
    Twin2Engine::UI::Button* _buttonMi;
    Twin2Engine::UI::Button* _buttonFa;

    size_t _buttonDoHandleId = 0ull;
    size_t _buttonReHandleId = 0ull;
    size_t _buttonMiHandleId = 0ull;
    size_t _buttonFaHandleId = 0ull;

    void GenerateNotes();
    void PlayNote(NoteType note);

public:
    static RadioStationPlayingController* Instance();

    virtual void Initialize() override;
    virtual void OnDestroy() override;


    void Play(RadioStation* radioStation, Playable* playable);


    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
protected:
    virtual bool DrawInheritedFields() override;
public:
    virtual void DrawEditor() override;
#endif
};