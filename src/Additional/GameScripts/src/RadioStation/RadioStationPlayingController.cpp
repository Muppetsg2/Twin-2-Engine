#include <RadioStation/RadioStationPlayingController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

RadioStationPlayingController* RadioStationPlayingController::_instance = nullptr;


void RadioStationPlayingController::Initialize()
{
    if (!_instance)
    {
        _instance = this;
    }

    _buttonUpHandleId = _buttonUp->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::UP); });
    _buttonRightHandleId = _buttonRight->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::RIGHT); });
    _buttonDownHandleId = _buttonDown->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::DOWN); });
    _buttonLeftHandleId = _buttonLeft->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::LEFT); });
}

void RadioStationPlayingController::Update()
{
    if (_gameStarted) 
    {
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
            _remainingTimeText->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::vformat(
                "{:.2f}", 
                std::make_format_args(_timeLimitCounter)
            )));

            if (_timeLimitCounter <= 0.0f)
            {
                EndPlaying();
            }
        }
    }
    else {
        if (_resultShowingCounter > 0.0f)
        {
            _resultShowingCounter -= Time::GetDeltaTime();
            if (_resultShowingCounter <= 0.0f)
            {
                _resultShowingCounter = 0.0f;

                _radioStation->StartTakingOver(_playable, _score);
                _resultText->SetActive(false);
                GetGameObject()->SetActive(false);

                OnEventPlayerFinishedPlaying((Player*)_playable, _radioStation);
            }
        }
    }
}

void RadioStationPlayingController::OnDestroy()
{
    if (_instance == this)
    {
        _instance = nullptr;
    }

    _buttonUp->GetOnClickEvent() -= _buttonUpHandleId;
    _buttonRight->GetOnClickEvent() -= _buttonRightHandleId;
    _buttonDown->GetOnClickEvent() -= _buttonDownHandleId;
    _buttonLeft->GetOnClickEvent() -= _buttonLeftHandleId;
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

        GameManager::instance->minigameActive = true;
        OnEventPlayerStartedPlaying((Player*)playable, radioStation);

        GenerateNotes();
        GetGameObject()->SetActive(true);
        _buttonUp->SetInteractable(true);
        _buttonRight->SetInteractable(true);
        _buttonDown->SetInteractable(true);
        _buttonLeft->SetInteractable(true);
        _timeLimitCounter = _timeLimit;
        _remainingTimeText->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(std::vformat(
            "{:.2f}",
            std::make_format_args(_timeLimitCounter)
        )));
    }
}

RadioStationPlayingController* RadioStationPlayingController::Instance()
{
    return _instance;
}

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
    //_notesGameObjects.reserve(_generatedNotesNumber);
    _notesImages.reserve(_generatedNotesNumber);

    NoteType note;
    std::tuple<GameObject*, Image*> generated;

    for (size_t index = 0ull; index < _generatedNotesNumber; ++index)
    {
        note = (NoteType)Random::Range(static_cast<unsigned>(NoteType::UP), static_cast<unsigned>(NoteType::LEFT));
        generated = SceneManager::CreateGameObject<Image>(_notesArea == nullptr ? GetTransform() : _notesArea->GetTransform());

        std::get<0>(generated)->GetTransform()->SetLocalPosition(beginOffset + vec3(index * widthStep, glm::vec2(0.0f)));
        std::get<1>(generated)->SetSprite(_notesSpritesIds[static_cast<size_t>(note)]);
        //std::get<1>(generated)->SetLayer(1);


        _generatedNotes.push_back(note);
        //_notesGameObjects.push_back(generated);
        _notesImages.push_back(std::get<1>(generated));
    }
}

void RadioStationPlayingController::PlayNote(NoteType note)
{
    // Warunek sprawdzaj¹cy czy nie dosz³o do zagrania po zakoñczeniu gry
    if (!_generatedNotes.size() || !_gameStarted) return;

    if (note == _generatedNotes[_currentNote])
    {
        _notesImages[_currentNote]->SetColor(_correctNoteColor);
        ++_correctCounter;
    }
    else
    {
        _notesImages[_currentNote]->SetColor(_wrongNoteColor);
    }

    ++_currentNote;

    if (_currentNote == _generatedNotes.size())
    {
        EndPlaying();
    }
}

void RadioStationPlayingController::EndPlaying()
{
    // Making Timer Zero for Update
    _timeLimitCounter = 0.0f;
    _gameStarted = false;

    // Notes Clearing
    _generatedNotes.clear();

    size_t size = _notesImages.size();
    for (size_t index = 0; index < size; ++index)
    {
        SceneManager::DestroyGameObject(_notesImages[index]->GetGameObject());
    }
    _notesImages.clear();

    GetGameObject()->SetActive(false);
    _buttonUp->GetGameObject()->SetActive(false);
    _buttonRight->GetGameObject()->SetActive(false);
    _buttonDown->GetGameObject()->SetActive(false);
    _buttonLeft->GetGameObject()->SetActive(false);

    _score = (float)_correctCounter / (float)size;

    GameManager::instance->minigameActive = false;

    ShowResult();
}

void RadioStationPlayingController::ShowResult()
{
    _resultShowingCounter = _resultShowingTime;
    _resultText->SetActive(true);

    if (_resultText->GetComponent<Text>() != nullptr)
        _resultText->GetComponent<Text>()->SetText(
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
    node["correctNoteColor"] = _correctNoteColor;
    node["wrongNoteColor"] = _wrongNoteColor;

    node["notesSpritesIds"] = vector<size_t>();
    for (size_t index = 0; index < _notesSpritesIds.size(); ++index)
    {
        node["notesSpritesIds"].push_back(SceneManager::GetSpriteSaveIdx(_notesSpritesIds[index]));
    }

    node["resultObjectId"] = _resultText == nullptr ? 0 : _resultText->Id();
    node["remainingTimeTextTimer"] = _remainingTimeText == nullptr ? 0 : _remainingTimeText->GetId();
    node["notesAreaImageId"] = _notesArea == nullptr ? 0 : _notesArea->GetId();

    node["buttonUp"] = _buttonUp->GetId();
    node["buttonRight"] = _buttonRight->GetId();
    node["buttonDown"] = _buttonDown->GetId();
    node["buttonLeft"] = _buttonLeft->GetId();

    return node;
}

bool RadioStationPlayingController::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) return false;

    _generatedNotesNumber = node["generatedNotesNumber"].as<int>();
    _notesAreaWidth = node["notesAreaWidth"].as<float>();
    _notesAreaHeight = node["notesAreaHeight"].as<float>();
    _timeLimit = node["timeLimit"].as<float>();
    _resultShowingTime = node["resultShowingTime"].as<float>();
    _correctNoteColor = node["correctNoteColor"].as<vec4>();
    _wrongNoteColor = node["wrongNoteColor"].as<vec4>();

    _notesSpritesIds = std::vector<size_t>();
    for (size_t index = 0; index < node["notesSpritesIds"].size(); ++index)
    {
        _notesSpritesIds.push_back(SceneManager::GetSprite(node["notesSpritesIds"][index].as<size_t>()));
    }

    node["resultObjectId"] = _resultText == nullptr ? 0 : _resultText->Id();
    node["remainingTimeTextTimer"] = _remainingTimeText == nullptr ? 0 : _remainingTimeText->GetId();
    node["notesAreaImageId"] = _notesArea == nullptr ? 0 : _notesArea->GetId();

    node["buttonUp"] = _buttonUp->GetId();
    node["buttonRight"] = _buttonRight->GetId();
    node["buttonDown"] = _buttonDown->GetId();
    node["buttonLeft"] = _buttonLeft->GetId();

    _buttonDo = (Button*) SceneManager::GetComponentWithId(node["buttonDo"].as<size_t>());
    _buttonRe = (Button*) SceneManager::GetComponentWithId(node["buttonRe"].as<size_t>());
    _buttonMi = (Button*) SceneManager::GetComponentWithId(node["buttonMi"].as<size_t>());
    _buttonFa = (Button*) SceneManager::GetComponentWithId(node["buttonFa"].as<size_t>());
    _resultImage = (Image*) SceneManager::GetComponentWithId(node["resultImage"].as<size_t>());
    _remainingTimeTextTimer = (Text*) SceneManager::GetComponentWithId(node["remainingTimeTextTimer"].as<size_t>());

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
        ImGui::InputFloat(string("NotesAreaWidth##").append(id).c_str(), &_notesAreaWidth);
        ImGui::InputFloat(string("NotesAreaHeight##").append(id).c_str(), &_notesAreaHeight);
        ImGui::InputFloat(string("ResultShowingTime##").append(id).c_str(), &_resultShowingTime);

        ImGui::ColorEdit4(string("CorrectColor##").append(id).c_str(), glm::value_ptr(_correctColor));
        ImGui::ColorEdit4(string("WrongColor##").append(id).c_str(), glm::value_ptr(_wrongColor));

        ImGui::DragFloat(string("TimeLimit##").append(id).c_str(), &_timeLimit, 1.f, 0.f, FLT_MAX);        

        unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Text>();
        size_t choosed_1 = _remainingTimeTextTimer == nullptr ? 0 : _remainingTimeTextTimer->GetId();

        if (ImGui::BeginCombo(string("RemainingTimeTextTimer##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
                    _remainingTimeTextTimer = static_cast<Text*>(items[choosed_1]);
                }
            }

            ImGui::EndCombo();
        }

        items.clear();

        items = SceneManager::GetComponentsOfType<Image>();
        choosed_1 = _resultImage == nullptr ? 0 : _resultImage->GetId();

        if (ImGui::BeginCombo(string("ResultImage##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
                    _resultImage = static_cast<Image*>(items[choosed_1]);
                }
            }

            ImGui::EndCombo();
        }

        items.clear();
        items = SceneManager::GetComponentsOfType<Button>();
        choosed_1 = _buttonDo == nullptr ? 0 : _buttonDo->GetId();
        size_t choosed_2 = _buttonRe == nullptr ? 0 : _buttonRe->GetId();
        size_t choosed_3 = _buttonMi == nullptr ? 0 : _buttonMi->GetId();
        size_t choosed_4 = _buttonFa == nullptr ? 0 : _buttonFa->GetId();

        if (ImGui::BeginCombo(string("ButtonDo##").append(id).c_str(), choosed_1 == 0 ? "None" : items[choosed_1]->GetGameObject()->GetName().c_str())) {

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
                    _buttonDo = static_cast<Button*>(items[choosed_1]);
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(string("ButtonRe##").append(id).c_str(), choosed_2 == 0 ? "None" : items[choosed_2]->GetGameObject()->GetName().c_str())) {

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
                    _buttonRe = static_cast<Button*>(items[choosed_2]);
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(string("ButtonMi##").append(id).c_str(), choosed_3 == 0 ? "None" : items[choosed_3]->GetGameObject()->GetName().c_str())) {

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
                    _buttonMi = static_cast<Button*>(items[choosed_3]);
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(string("ButtonFa##").append(id).c_str(), choosed_4 == 0 ? "None" : items[choosed_4]->GetGameObject()->GetName().c_str())) {

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
                    _buttonFa = static_cast<Button*>(items[choosed_4]);
                }
            }

            ImGui::EndCombo();
        }

        items.clear();

        // TODO: Zrobiæ okreœlanie notesSprites oraz okreœlanie buttonów
        /*
        _notesSpritesIds = node["notesSpritesIds"].as<vector<size_t>>();
        for (size_t index = 0ull; index < _notesSpritesIds.size(); ++index)
        {
            _notesSpritesIds[index] = SceneManager::GetSprite(_notesSpritesIds[index]);
        }

        _resultsImagesIds = node["resultsSpritesIds"].as<vector<size_t>>();
        for (size_t index = 0ull; index < _resultsImagesIds.size(); ++index)
        {
            _resultsImagesIds[index] = SceneManager::GetSprite(_resultsImagesIds[index]);
        }
        _resultsThresholds = node["resultsThresholds"].as<vector<float>>();
        */
    }
}

#endif