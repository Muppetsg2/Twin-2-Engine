#include <RadioStation/RadioStationPlayingController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

RadioStationPlayingController* RadioStationPlayingController::instance = nullptr;


void RadioStationPlayingController::Initialize()
{
    //_buttonDo = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingDo"));
    //_buttonRe = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingRe"));
    //_buttonMi = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingMi"));
    //_buttonFa = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingFa"));

    _buttonDoHandleId = _buttonDo->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::Do); };
    _buttonReHandleId = _buttonRe->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::Re); };
    _buttonMiHandleId = _buttonMi->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::Mi); };
    _buttonFaHandleId = _buttonFa->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::Fa); };
}

void RadioStationPlayingController::OnDestroy()
{
    _buttonDo->GetOnClickEvent() -= _buttonDoHandleId;
    _buttonRe->GetOnClickEvent() -= _buttonReHandleId;
    _buttonMi->GetOnClickEvent() -= _buttonMiHandleId;
    _buttonFa->GetOnClickEvent() -= _buttonFaHandleId;
}

void RadioStationPlayingController::Play(RadioStation* radioStation, Playable* playable)
{
    // Gdy wywo³uje Player
    if (dynamic_cast<Player*>(playable)) 
    {
        _radioStation = radioStation;
        _playable = playable;
        GenerateNotes();
        GetGameObject()->SetActive(true);
    }
    // Gdy wywo³uje Enemy
    else
    {
        // TODO: Okreœlanie wyniku dla enemy
        radioStation->StartTakingOver(playable, Random::Range(0, 4) / 4.0f);
    }
}

RadioStationPlayingController* RadioStationPlayingController::Instance()
{
    return instance;
}

void RadioStationPlayingController::GenerateNotes()
{
    size_t notesToGenerate = _generatedNotesNumber;
    float widthStep = _notesAreaWidth / (notesToGenerate - 1);
    float heightStep = _notesAreaHeight / 3.0f;

    vec3 beginOffset(-0.5f * _notesAreaWidth, -0.5f * _notesAreaHeight, 0.0f);

    _generatedNotes.reserve(notesToGenerate);
    _notesGameObjects.reserve(notesToGenerate);
    _notesImages.reserve(notesToGenerate);

    NoteType note;
    GameObject* generated = nullptr;
    Image* image = nullptr;

    for (size_t index = 0ull; index < notesToGenerate; ++index)
    {
        note = (NoteType)Random::Range(static_cast<unsigned>(NoteType::Do), static_cast<unsigned>(NoteType::Fa));
        generated = SceneManager::CreateGameObject(GetTransform());
        image = generated->AddComponent<Image>();

        generated->GetTransform()->SetLocalPosition(beginOffset + vec3(index * widthStep, (unsigned)note * heightStep, 0.0f));
        image->SetSprite(_notesSpritesIds[static_cast<size_t>(note)]);

        _generatedNotes.push_back(note);
        _notesGameObjects.push_back(generated);
        _notesImages.push_back(image);
    }
}

void RadioStationPlayingController::PlayNote(NoteType note)
{
    if (note == _generatedNotes[_currentNote])
    {
        _notesImages[_currentNote]->SetColor(_correctColor);
        _correctCounter++;
    }
    else
    {
        _notesImages[_currentNote]->SetColor(_wrongColor);
    }

    ++_currentNote;

    if (_currentNote == _generatedNotes.size())
    {
        // Czyszczenie obiektów nut
        _generatedNotes.clear();

        size_t size = _notesGameObjects.size();
        for (size_t index = 0ull; index < size; ++index)
        {
            SceneManager::DestroyGameObject(_notesGameObjects[index]);
        }
        _notesGameObjects.clear();
        _notesImages.clear();

        _radioStation->StartTakingOver(_playable, _correctCounter / (float)size);

        GetGameObject()->SetActive(false);
    }
}

YAML::Node RadioStationPlayingController::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "RadioStationPlayingController";
    node["generatedNotesNumber"] = _generatedNotesNumber;
    node["correctColor"] = _correctColor;
    node["wrongColor"] = _wrongColor;
    node["notesSpritesIds"] = _notesSpritesIds;

    node["buttonDo"] = _buttonDo->GetId();
    node["buttonRe"] = _buttonRe->GetId();
    node["buttonMi"] = _buttonMi->GetId();
    node["buttonFa"] = _buttonFa->GetId();

    return YAML::Node();
}

bool RadioStationPlayingController::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) return false;

    _generatedNotesNumber = node["generatedNotesNumber"].as<int>();
    _correctColor = node["correctColor"].as<vec4>();
    _wrongColor = node["wrongColor"].as<vec4>();
    _notesSpritesIds = node["notesSpritesIds"].as<vector<size_t>>();


    _buttonDo = dynamic_cast<Button*>(SceneManager::GetGameObjectWithId(node["buttonDo"].as<size_t>()));
    _buttonRe = dynamic_cast<Button*>(SceneManager::GetGameObjectWithId(node["buttonRe"].as<size_t>()));
    _buttonMi = dynamic_cast<Button*>(SceneManager::GetGameObjectWithId(node["buttonMi"].as<size_t>()));
    _buttonFa = dynamic_cast<Button*>(SceneManager::GetGameObjectWithId(node["buttonFa"].as<size_t>()));

    return true;
}

#if _DEBUG

bool RadioStationPlayingController::DrawInheritedFields()
{
    if (Component::DrawInheritedFields()) return true;

    ImGui::InputInt("GeneratedNotesNumber", &_generatedNotesNumber);

    // TODO: Zrobiæ okreœlanie correct color, wrong color, notesSprites oraz okreœlanie buttonów

    return false;
}

void RadioStationPlayingController::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("RadioStationPlayingController##Component").append(id);

    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}

#endif