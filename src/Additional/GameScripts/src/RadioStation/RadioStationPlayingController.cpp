#include <RadioStation/RadioStationPlayingController.h>
#include <ui/Button.h>
#include <ui/Image.h>
#include <core/Time.h>
#include <core/Input.h>
#include <Player.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

RadioStationPlayingController* RadioStationPlayingController::_instance = nullptr;

void RadioStationPlayingController::GenerateNotes()
{
    if (_notesArea != nullptr) {
        _notesAreaWidth = _notesArea->GetWidth();
        _notesAreaHeight = _notesArea->GetHeight();
    }

    float widthStep = _notesAreaWidth / (_generatedNotesNumber - 1);
    float heightStep = _notesAreaHeight / 3.0f;

    vec3 beginOffset(-0.5f * _notesAreaWidth, glm::vec2(0.0f));

    _generatedNotes.reserve(_generatedNotesNumber);
    _notesImages.reserve(_generatedNotesNumber);

    NoteType note;
    std::tuple<GameObject*, Image*> generated;

    for (size_t index = 0ull; index < _generatedNotesNumber; ++index)
    {
        note = (NoteType)Random::Range(static_cast<unsigned int>(NoteType::UP), static_cast<unsigned int>(NoteType::LEFT));
        generated = SceneManager::CreateGameObject<Image>(_notesArea == nullptr ? GetTransform() : _notesArea->GetTransform());

        std::get<0>(generated)->GetTransform()->SetLocalPosition(beginOffset + vec3(index * widthStep, glm::vec2(0.0f)));
        std::get<1>(generated)->SetSprite(_notesSpritesIds[static_cast<size_t>(note)]);
        std::get<1>(generated)->SetLayer(1);

        _generatedNotes.push_back(note);
        _notesImages.push_back(std::get<1>(generated));
    }
}

void RadioStationPlayingController::PlayNote(NoteType note)
{
    // Warunek sprawdzaj¹cy czy nie dosz³o do zagrania po zakoñczeniu gry
    if (!_generatedNotes.size() || !_gameStarted || !_playClicked) return;

    if (note == _generatedNotes[_currentNote])
    {
        _notesImages[_currentNote]->SetColor(_correctNoteColor);
        ++_correctCounter;

        switch (note)
        {
        case NoteType::DOWN:
            _audioComponent->SetAudio(_soundDown);
            break;

        case NoteType::LEFT:
            _audioComponent->SetAudio(_soundLeft);
            break;

        case NoteType::RIGHT:
            _audioComponent->SetAudio(_soundRight);
            break;

        case NoteType::UP:
            _audioComponent->SetAudio(_soundUp);
            break;
        }
        _audioComponent->Play();
    }
    else
    {
        _notesImages[_currentNote]->SetColor(_wrongNoteColor);

        switch (note)
        {
        case NoteType::DOWN:
            _audioComponent->SetAudio(_soundDownFalse);
            break;

        case NoteType::LEFT:
            _audioComponent->SetAudio(_soundLeftFalse);
            break;

        case NoteType::RIGHT:
            _audioComponent->SetAudio(_soundRightFalse);
            break;

        case NoteType::UP:
            _audioComponent->SetAudio(_soundUpFalse);
            break;
        }
        _audioComponent->Play();
    }

    ++_currentNote;

    if (_currentNote == _generatedNotes.size())
    {
        EndPlaying();
    }
}

void RadioStationPlayingController::ShowResult()
{
    _resultShowingCounter = _resultShowingTime;
    if (_resultText != nullptr) {
        _resultText->SetActive(true);

        Text* t = _resultText->GetComponent<Text>();
        if (t != nullptr) {
            t->SetText(
                std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes
                (
                    std::to_string(_correctCounter)
                    .append("/")
                    .append
                    (
                        std::to_string(_generatedNotesNumber)
                    )
                )
            );

            t->SetColor(_correctCounter == 0 ? _wrongNoteColor : _correctNoteColor);
        }
    }
}

void RadioStationPlayingController::HideResult()
{
    _gameStarted = false;
    _playClicked = false;

    // Notes Clearing
    _generatedNotes.clear();

    for (size_t index = 0; index < _notesImages.size(); ++index)
    {
        SceneManager::DestroyGameObject(_notesImages[index]->GetGameObject());
    }
    _notesImages.clear();

    GameManager::instance->minigameActive = false;
}

void RadioStationPlayingController::EndPlaying()
{
    // Making Timer Zero for Update
    _timeLimitCounter = 0.0f;

    _playText->SetActive(false);
    _buttonUp->SetInteractable(false);
    _buttonRight->SetInteractable(false);
    _buttonDown->SetInteractable(false);
    _buttonLeft->SetInteractable(false);

    _score = (float)_correctCounter / (float)_notesImages.size();

    GameManager::instance->SetBackgrounMusicVolumeSmooth(0.5f, 0.1f);

    ShowResult();
}

RadioStationPlayingController* RadioStationPlayingController::Instance()
{
    return _instance;
}

void RadioStationPlayingController::Initialize()
{
    if (!_instance)
    {
        _instance = this;
    }

    while (_notesSpritesIds.size() < size<NoteType>()) {
        _notesSpritesIds.push_back(SceneManager::GetSprite(0));
    }

    if (_buttonUpHandleId == -1 && _buttonUp != nullptr) _buttonUpHandleId = _buttonUp->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::UP); };
    if (_buttonRightHandleId == -1 && _buttonRight != nullptr) _buttonRightHandleId = _buttonRight->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::RIGHT); };
    if (_buttonDownHandleId == -1 && _buttonDown != nullptr) _buttonDownHandleId = _buttonDown->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::DOWN); };
    if (_buttonLeftHandleId == -1 && _buttonLeft != nullptr) _buttonLeftHandleId = _buttonLeft->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::LEFT); };

    GetGameObject()->SetActive(false);
    if (_resultText != nullptr) _resultText->SetActive(false);

    _audioComponent = GetGameObject()->GetComponent<AudioComponent>();

    //_soundDown = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin_D4_1_mezzo-forte_arco-normal.mp3");
    //_soundLeft = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin_E4_1_mezzo-forte_arco-normal.mp3");
    //_soundRight = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin_F4_1_mezzo-forte_arco-normal.mp3");
    //_soundUp = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin_G4_1_mezzo-forte_arco-normal.mp3");
    //_soundDownFalse = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin-false-sounds.mp3");
    //_soundLeftFalse = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin-false-sounds.mp3");
    //_soundRightFalse = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin-false-sounds.mp3");
    //_soundUpFalse = AudioManager::LoadAudio("res/music/RadioStation/Violin/violin-false-sounds.mp3");
}

void RadioStationPlayingController::Update()
{
    if (_gameStarted && _playClicked)
    {
        if (_timeLimitCounter <= 0.f || _currentNote == _generatedNotes.size()) {

            if (_resultShowingCounter > 0.0f)
            {
                _resultShowingCounter -= Time::GetDeltaTime();
                if (_resultShowingCounter <= 0.0f)
                {
                    _resultShowingCounter = 0.0f;

                    _radioStation->StartTakingOver(_playable, _score);
                    if (_resultText != nullptr) _resultText->SetActive(false);
                    GetGameObject()->SetActive(false);

                    HideResult();

                    OnEventPlayerFinishedPlaying((Player*)_playable, _radioStation);
                }
            }
        }
        else {

            if (_buttonUp != nullptr && !_buttonUp->IsInteractable()) _buttonUp->SetInteractable(true);
            if (_buttonRight != nullptr && !_buttonRight->IsInteractable()) _buttonRight->SetInteractable(true);
            if (_buttonDown != nullptr && !_buttonDown->IsInteractable()) _buttonDown->SetInteractable(true);
            if (_buttonLeft != nullptr && !_buttonLeft->IsInteractable()) _buttonLeft->SetInteractable(true);

            if (Input::IsKeyPressed(KEY::W))
            {
                PlayNote(NoteType::UP);
            }
            if (Input::IsKeyPressed(KEY::D))
            {
                PlayNote(NoteType::RIGHT);
            }
            if (Input::IsKeyPressed(KEY::S))
            {
                PlayNote(NoteType::DOWN);
            }
            if (Input::IsKeyPressed(KEY::A))
            {
                PlayNote(NoteType::LEFT);
            }

            if (_timeLimitCounter > 0.0f)
            {
                _timeLimitCounter -= Time::GetDeltaTime();

                _timeLimitCounter = _timeLimitCounter < 0.f ? 0.f : _timeLimitCounter;

                if (_remainingTimeText != nullptr) {

                    _remainingTimeText->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::vformat(
                        "{:.2f}s",
                        std::make_format_args(_timeLimitCounter)
                    )));

                    if (_timeLimitCounter <= _timeChangeColorPercent / 100.f * _timeLimit) {
                        _remainingTimeText->SetColor(glm::vec4(1.f, glm::vec2(0.141176477f), 1.f));
                    }
                }

                if (_timeLimitCounter <= 0.0f)
                {
                    EndPlaying();
                }
            }
        }
    }
    else if (_gameStarted && !_playClicked) {
        if (Input::IsMouseButtonPressed(MOUSE_BUTTON::LEFT))
        {
            PlayClicked();
        }
    }
}

void RadioStationPlayingController::OnDestroy()
{
    if (_instance == this)
    {
        _instance = nullptr;
    }

    if (_buttonUpHandleId != -1) {
        _buttonUp->GetOnClickEvent() -= _buttonUpHandleId;
        _buttonUpHandleId = -1;
    }
    if (_buttonRightHandleId != -1) {
        _buttonRight->GetOnClickEvent() -= _buttonRightHandleId;
        _buttonRightHandleId = -1;
    }
    if (_buttonDownHandleId != -1) {
        _buttonDown->GetOnClickEvent() -= _buttonDownHandleId;
        _buttonDownHandleId = -1;
    }
    if (_buttonLeftHandleId != -1) {
        _buttonLeft->GetOnClickEvent() -= _buttonLeftHandleId;
        _buttonLeftHandleId = -1;
    }
}

void RadioStationPlayingController::Play(RadioStation* radioStation, Playable* playable)
{
    // Player
    if (dynamic_cast<Player*>(playable) != nullptr)
    {
        _radioStation = radioStation;
        _playable = playable;
        _currentNote = 0;
        _gameStarted = true;
        _playClicked = false;
        _correctCounter = 0;
        _score = 0.f;

        GameManager::instance->minigameActive = true;
        GameManager::instance->SetBackgrounMusicVolumeSmooth(0.1f, 0.1f);

        OnEventPlayerStartedPlaying((Player*)playable, radioStation);

        GetGameObject()->SetActive(true);
        _playText->SetActive(true);
        _buttonUp->SetInteractable(false);
        _buttonRight->SetInteractable(false);
        _buttonDown->SetInteractable(false);
        _buttonLeft->SetInteractable(false);
        _timeLimitCounter = _timeLimit;
        
        _soundDown = AudioManager::LoadAudio(playable->patron->GetSoundDown());
        _soundLeft = AudioManager::LoadAudio(playable->patron->GetSoundLeft());
        _soundRight = AudioManager::LoadAudio(playable->patron->GetSoundRight());
        _soundUp = AudioManager::LoadAudio(playable->patron->GetSoundUp());
        _soundDownFalse = AudioManager::LoadAudio(playable->patron->GetSoundDownFalse());
        _soundLeftFalse = AudioManager::LoadAudio(playable->patron->GetSoundLeftFalse());
        _soundRightFalse = AudioManager::LoadAudio(playable->patron->GetSoundRightFalse());
        _soundUpFalse = AudioManager::LoadAudio(playable->patron->GetSoundUpFalse());

        if (_remainingTimeText != nullptr) {
            _remainingTimeText->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::vformat(
                "{:.2f}s",
                std::make_format_args(_timeLimitCounter)
            )));

            _remainingTimeText->SetColor(glm::vec4(1.f));
        }
    }
}

void RadioStationPlayingController::PlayClicked()
{
    GenerateNotes();
    _playClicked = true;
    _playText->SetActive(false);
}

YAML::Node RadioStationPlayingController::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "RadioStationPlayingController";
    node["generatedNotesNumber"] = _generatedNotesNumber;
    node["notesAreaWidth"] = _notesAreaWidth;
    node["notesAreaHeight"] = _notesAreaHeight;
    node["timeLimit"] = _timeLimit;
    node["resultShowingTime"] = _resultShowingTime;
    node["timeChangeColorPercent"] = _timeChangeColorPercent;
    node["correctNoteColor"] = _correctNoteColor;
    node["wrongNoteColor"] = _wrongNoteColor;

    node["notesSpritesIds"] = vector<size_t>();
    for (size_t index = 0; index < _notesSpritesIds.size(); ++index)
    {
        node["notesSpritesIds"].push_back(SceneManager::GetSpriteSaveIdx(_notesSpritesIds[index]));
    }

    node["playTextObjectId"] = _playText == nullptr ? 0 : _playText->Id();
    node["resultTextObjectId"] = _resultText == nullptr ? 0 : _resultText->Id();
    node["remainingTimeTextId"] = _remainingTimeText == nullptr ? 0 : _remainingTimeText->GetId();
    node["notesAreaImageId"] = _notesArea == nullptr ? 0 : _notesArea->GetId();

    node["buttonUp"] = _buttonUp->GetId();
    node["buttonRight"] = _buttonRight->GetId();
    node["buttonDown"] = _buttonDown->GetId();
    node["buttonLeft"] = _buttonLeft->GetId();

    return node;
}

bool RadioStationPlayingController::Deserialize(const YAML::Node& node)
{
    if (!node["generatedNotesNumber"] || !node["notesAreaWidth"] || !node["notesAreaHeight"] || 
        !node["timeLimit"] || !node["resultShowingTime"] || !node["correctNoteColor"] ||
        !node["wrongNoteColor"] || !node["notesSpritesIds"] || !node["playTextObjectId"] || !node["resultTextObjectId"] ||
        !node["remainingTimeTextId"] || !node["notesAreaImageId"] || !node["timeChangeColorPercent"] ||
        !node["buttonUp"] || !node["buttonRight"] || !node["buttonDown"] || !node["buttonLeft"] ||
        !Component::Deserialize(node)) return false;

    _generatedNotesNumber = node["generatedNotesNumber"].as<int>();
    _notesAreaWidth = node["notesAreaWidth"].as<float>();
    _notesAreaHeight = node["notesAreaHeight"].as<float>();
    _timeLimit = node["timeLimit"].as<float>();
    _resultShowingTime = node["resultShowingTime"].as<float>();
    _timeChangeColorPercent = node["timeChangeColorPercent"].as<float>();
    _correctNoteColor = node["correctNoteColor"].as<vec4>();
    _wrongNoteColor = node["wrongNoteColor"].as<vec4>();

    _notesSpritesIds = std::vector<size_t>();
    for (size_t index = 0; index < node["notesSpritesIds"].size(); ++index)
    {
        _notesSpritesIds.push_back(SceneManager::GetSprite(node["notesSpritesIds"][index].as<size_t>()));
    }

    while (_notesSpritesIds.size() < size<NoteType>()) {
        if (node["notesSpritesIds"].size() != 0) {
            _notesSpritesIds.push_back(SceneManager::GetSprite(node["notesSpritesIds"][node["notesSpritesIds"].size() - 1].as<size_t>()));
        }
        else {
            _notesSpritesIds.push_back(SceneManager::GetSprite(0));
        }
    }

    size_t id = node["playTextObjectId"].as<size_t>();
    _playText = id == 0 ? nullptr : SceneManager::GetGameObjectWithId(id);
    id = node["resultTextObjectId"].as<size_t>();
    _resultText = id == 0 ? nullptr : SceneManager::GetGameObjectWithId(id);
    id = node["remainingTimeTextId"].as<size_t>();
    _remainingTimeText = id == 0 ? nullptr : (Text*)SceneManager::GetComponentWithId(id);
    id = node["notesAreaImageId"].as<size_t>();
    _notesArea = id == 0 ? nullptr : (Image*)SceneManager::GetComponentWithId(id);

    _buttonUp = (Button*)SceneManager::GetComponentWithId(node["buttonUp"].as<size_t>());
    _buttonRight = (Button*)SceneManager::GetComponentWithId(node["buttonRight"].as<size_t>());
    _buttonDown = (Button*)SceneManager::GetComponentWithId(node["buttonDown"].as<size_t>());
    _buttonLeft = (Button*)SceneManager::GetComponentWithId(node["buttonLeft"].as<size_t>());

    return true;
}

#if _DEBUG

void RadioStationPlayingController::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("RadioStationPlayingController##Component").append(id);

    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        ImGui::InputInt(string("GeneratedNotesNumber##").append(id).c_str(), &_generatedNotesNumber);
        ImGui::InputFloat(string("ResultShowingTime##").append(id).c_str(), &_resultShowingTime);

        ImGui::DragFloat(string("TimeChangeColorPercent##").append(id).c_str(), &_timeChangeColorPercent, 0.1f, 0.0f, 100.f);

        ImGui::ColorEdit4(string("CorrectColor##").append(id).c_str(), glm::value_ptr(_correctNoteColor));
        ImGui::ColorEdit4(string("WrongColor##").append(id).c_str(), glm::value_ptr(_wrongNoteColor));

        ImGui::DragFloat(string("TimeLimit##").append(id).c_str(), &_timeLimit, 1.f, 0.f, FLT_MAX);        

        unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Text>();
        size_t choosed_1 = _remainingTimeText == nullptr ? 0 : _remainingTimeText->GetId();

        if (ImGui::BeginCombo(string("RemainingTimeText##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

            bool clicked = false;
            for (auto& item : items) {

                if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

                    if (clicked) continue;

                    choosed_1 = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed_1 != 0) {
                    _remainingTimeText = static_cast<Text*>(items[choosed_1]);
                }
            }

            ImGui::EndCombo();
        }

        items.clear();
        items = SceneManager::GetComponentsOfType<Button>();
        choosed_1 = _buttonUp == nullptr ? 0 : _buttonUp->GetId();
        size_t choosed_2 = _buttonRight == nullptr ? 0 : _buttonRight->GetId();
        size_t choosed_3 = _buttonDown == nullptr ? 0 : _buttonDown->GetId();
        size_t choosed_4 = _buttonLeft == nullptr ? 0 : _buttonLeft->GetId();

        if (ImGui::BeginCombo(string("ButtonUp##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

            bool clicked = false;
            for (auto& item : items) {

                if (item.second->GetId() == choosed_2 || item.second->GetId() == choosed_3 || item.second->GetId() == choosed_4) continue;

                if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

                    if (clicked) continue;

                    choosed_1 = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed_1 != 0) {

                    if (_buttonUp != nullptr) {
                        if (_buttonUpHandleId != -1) {
                            _buttonUp->GetOnClickEvent() -= _buttonUpHandleId;
                            _buttonUpHandleId = -1;
                        }
                    }

                    _buttonUp = static_cast<Button*>(items[choosed_1]);

                    if (_buttonUp != nullptr) {
                        if (_buttonUpHandleId == -1) _buttonUpHandleId = _buttonUp->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::UP); };
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(string("ButtonRight##").append(id).c_str(), choosed_2 == 0 ? "None" : items[choosed_2]->GetGameObject()->GetName().c_str())) {

            bool clicked = false;
            for (auto& item : items) {

                if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_3 || item.second->GetId() == choosed_4) continue;

                if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_2)) {

                    if (clicked) continue;

                    choosed_2 = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed_2 != 0) {

                    if (_buttonRight != nullptr) {
                        if (_buttonRightHandleId != -1) {
                            _buttonRight->GetOnClickEvent() -= _buttonRightHandleId;
                            _buttonRightHandleId = -1;
                        }
                    }

                    _buttonRight = static_cast<Button*>(items[choosed_2]);

                    if (_buttonRight != nullptr) {
                        if (_buttonRightHandleId == -1) _buttonRightHandleId = _buttonRight->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::RIGHT); };
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(string("ButtonDown##").append(id).c_str(), choosed_3 == 0 ? "None" : items[choosed_3]->GetGameObject()->GetName().c_str())) {

            bool clicked = false;
            for (auto& item : items) {

                if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_2 || item.second->GetId() == choosed_4) continue;

                if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_3)) {

                    if (clicked) continue;

                    choosed_3 = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed_3 != 0) {

                    if (_buttonDown != nullptr) {
                        if (_buttonDownHandleId != -1) {
                            _buttonDown->GetOnClickEvent() -= _buttonDownHandleId;
                            _buttonDownHandleId = -1;
                        }
                    }

                    _buttonDown = static_cast<Button*>(items[choosed_3]);

                    if (_buttonDown != nullptr) {
                        if (_buttonDownHandleId == -1) _buttonDownHandleId = _buttonDown->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::DOWN); };
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(string("ButtonLeft##").append(id).c_str(), choosed_4 == 0 ? "None" : items[choosed_4]->GetGameObject()->GetName().c_str())) {

            bool clicked = false;
            for (auto& item : items) {

                if (item.second->GetId() == choosed_1 || item.second->GetId() == choosed_2 || item.second->GetId() == choosed_3) continue;

                if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_4)) {

                    if (clicked) continue;

                    choosed_4 = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed_4 != 0) {

                    if (_buttonLeft != nullptr) {
                        if (_buttonLeftHandleId != -1) {
                            _buttonLeft->GetOnClickEvent() -= _buttonLeftHandleId;
                            _buttonLeftHandleId = -1;
                        }
                    }

                    _buttonLeft = static_cast<Button*>(items[choosed_4]);

                    if (_buttonLeft != nullptr) {
                        if (_buttonLeftHandleId == -1) _buttonLeftHandleId = _buttonLeft->GetOnClickEvent() += [&]() -> void { PlayNote(NoteType::LEFT); };
                    }
                }
            }

            ImGui::EndCombo();
        }

        items.clear();

        std::vector<GameObject*> objs = SceneManager::GetAllGameObjects();
        objs.erase(objs.begin());
        objs.insert(objs.begin(), nullptr);
        choosed_1 = std::find(objs.begin(), objs.end(), _playText) - objs.begin();

        if (ImGui::BeginCombo(string("PlayTextObject##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

            bool clicked = false;
            for (size_t i = 0; i < objs.size(); ++i) {

                if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

                    if (clicked) continue;

                    choosed_1 = i;
                    clicked = true;
                }
            }

            if (clicked) {
                _playText = objs[choosed_1];
            }

            ImGui::EndCombo();
        }

        choosed_1 = std::find(objs.begin(), objs.end(), _resultText) - objs.begin();

        if (ImGui::BeginCombo(string("ResultTextObject##").append(id).c_str(), choosed_1 == 0 ? "None" : objs[choosed_1]->GetName().c_str())) {

            bool clicked = false;
            for (size_t i = 0; i < objs.size(); ++i) {

                if (ImGui::Selectable(std::string(i == 0 ? "None" : objs[i]->GetName().c_str()).append("##").append(id).append(std::to_string(i)).c_str(), i == choosed_1)) {

                    if (clicked) continue;

                    choosed_1 = i;
                    clicked = true;
                }
            }

            if (clicked) {
                _resultText = objs[choosed_1];
            }

            ImGui::EndCombo();
        }

        objs.clear();

        items = SceneManager::GetComponentsOfType<Image>();
        choosed_1 = _notesArea == nullptr ? 0 : _notesArea->GetId();

        if (ImGui::BeginCombo(string("NotesArea##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

            bool clicked = false;
            for (auto& item : items) {

                if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_1)) {

                    if (clicked) continue;

                    choosed_1 = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed_1 != 0) {
                    _notesArea = static_cast<Image*>(items[choosed_1]);
                }
            }

            ImGui::EndCombo();
        }

        items.clear();

        ImGui::BeginDisabled(_notesArea != nullptr);
        {
            ImGui::InputFloat(string("NotesAreaWidth##").append(id).c_str(), &_notesAreaWidth);
            ImGui::InputFloat(string("NotesAreaHeight##").append(id).c_str(), &_notesAreaHeight);
        }
        ImGui::EndDisabled();

        std::map<size_t, string> spriteNames = SpriteManager::GetAllSpritesNames();

        vector<string> names = vector<string>();
        names.resize(spriteNames.size());

        std::transform(spriteNames.begin(), spriteNames.end(), names.begin(), [](std::pair<size_t, string> const& i) -> string {
            return i.second;
            });

        spriteNames.clear();

        std::sort(names.begin(), names.end(), [&](string const& left, string const& right) -> bool {
            return left.compare(right) < 0;
            });

        names.insert(names.begin(), "None##Nothing");

        for (size_t i = 0; i < _notesSpritesIds.size(); ++i)
        {
            std::string n = SpriteManager::GetSpriteName(_notesSpritesIds[i]);

            if (n == "") {
                _notesSpritesIds[i] = 0;
            }

            size_t choosed = _notesSpritesIds[i] == 0 ? 0 : std::find(names.begin(), names.end(), n) - names.begin();

            if (ImGui::ComboWithFilter(string("Note ").append(to_string((NoteType)(uint8_t)i)).append("##").append(id).c_str(), &choosed, names, 20)) {
                if (choosed != 0) {
                    _notesSpritesIds[i] = SpriteManager::GetSpriteId(names[choosed]);
                }
                else {
                    _notesSpritesIds[i] = 0;
                }
            }
        }
        
        names.clear();
    }
}

#endif