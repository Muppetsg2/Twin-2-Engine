#include <RadioStation/RadioStationPlayingController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

RadioStationPlayingController* RadioStationPlayingController::instance = nullptr;


void RadioStationPlayingController::Initialize()
{
    if (!instance)
    {
        instance = this;
    }
    //_buttonDo = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingDo"));
    //_buttonRe = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingRe"));
    //_buttonMi = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingMi"));
    //_buttonFa = dynamic_cast<Button*>(SceneManager::FindObjectByName("ButtonPlayingFa"));
    _windowImage = GetGameObject()->GetComponent<Image>();

    _buttonDoHandleId = _buttonDo->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::Do); });
    _buttonReHandleId = _buttonRe->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::Re); });
    _buttonMiHandleId = _buttonMi->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::Mi); });
    _buttonFaHandleId = _buttonFa->GetOnClickEvent().AddCallback([&]() { PlayNote(NoteType::Fa); });
}

void RadioStationPlayingController::Update()
{
    if (Input::IsKeyPressed(KEY::S))
    {
        PlayNote(NoteType::Do);
    }
    if (Input::IsKeyPressed(KEY::A))
    {
        PlayNote(NoteType::Re);
    }
    if (Input::IsKeyPressed(KEY::D))
    {
        PlayNote(NoteType::Mi);
    }
    if (Input::IsKeyPressed(KEY::W))
    {
        PlayNote(NoteType::Fa);
    }

    if (_timeLimitTimer > 0.0f)
    {
        _timeLimitTimer -= Time::GetDeltaTime();
        _remainingTimeTextTimer->SetText(to_wstring(static_cast<unsigned>(glm::round(_timeLimitTimer))));
        if (_timeLimitTimer <= 0.0f)
        {
            EndPlaying();
        }
    }
    else if (_resultShowingTimer > 0.0f)
    {
        _resultShowingTimer -= Time::GetDeltaTime();
        if (_resultShowingTimer <= 0.0f)
        {
            _resultShowingTimer = 0.0f;

            _radioStation->StartTakingOver(_playable, _lastScore);
            GameManager::instance->minigameActive = false;
            _resultImage->GetGameObject()->SetActive(false);
            GetGameObject()->SetActive(false);

            OnEventPlayerFinishedPlaying((Player*)_playable, _radioStation);
        }
    }
}

void RadioStationPlayingController::OnDestroy()
{
    if (instance == this)
    {
        instance = nullptr;
    }

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
        _currentNote = 0;

        // CallingEvents
        //OnEventPlayableStartsPlaying(playable, radioStation);
        OnEventPlayerStartedPlaying((Player*) playable, radioStation);

        GenerateNotes();
        GetGameObject()->SetActive(true);
        _windowImage->SetEnable(true);
        _buttonDo->GetGameObject()->SetActive(true);
        _buttonRe->GetGameObject()->SetActive(true);
        _buttonMi->GetGameObject()->SetActive(true);
        _buttonFa->GetGameObject()->SetActive(true);
        _remainingTimeTextTimer->GetGameObject()->SetActive(true);
        _timeLimitTimer = _timeLimit;
    }
    // Gdy wywo³uje Enemy
    else
    {
        // TODO: Okreœlanie wyniku dla enemy
        
        // CallingEvents
        //OnEventPlayableStartsPlaying(playable, radioStation);

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

    vec3 beginOffset(-0.5f * _notesAreaWidth, 0.0f, 0.0f);

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

        generated->GetTransform()->SetLocalPosition(beginOffset + vec3(index * widthStep, ((unsigned)note) * heightStep, 0.0f));
        image->SetSprite(_notesSpritesIds[static_cast<size_t>(note)]);
        image->SetLayer(3);


        _generatedNotes.push_back(note);
        _notesGameObjects.push_back(generated);
        _notesImages.push_back(image);
    }
}

void RadioStationPlayingController::PlayNote(NoteType note)
{
    // Warunek sprawdzaj¹cy czy nie dosz³o do zagrania po zakoñczeniu gry
    if (!_generatedNotes.size())
        return;

    if (note == _generatedNotes[_currentNote])
    {
        GameManager::instance->minigameActive = true;
        _notesImages[_currentNote]->SetColor(_correctColor);
        ++_correctCounter;
    }
    else
    {
        _notesImages[_currentNote]->SetColor(_wrongColor);
    }

    ++_currentNote;

    if (_currentNote == _generatedNotes.size())
    {
        EndPlaying();
    }
}

void RadioStationPlayingController::EndPlaying()
{
    //Zerowanie timera aby w updatcie siê wy³¹czy³
    _timeLimitTimer = 0.0f;

    // Czyszczenie obiektów nut
    _generatedNotes.clear();

    size_t size = _notesGameObjects.size();
    for (size_t index = 0ull; index < size; ++index)
    {
        SceneManager::DestroyGameObject(_notesGameObjects[index]);
    }
    _notesGameObjects.clear();
    _notesImages.clear();


    _windowImage->SetEnable(false);
    _buttonDo->GetGameObject()->SetActive(false);
    _buttonRe->GetGameObject()->SetActive(false);
    _buttonMi->GetGameObject()->SetActive(false);
    _buttonFa->GetGameObject()->SetActive(false);
    _remainingTimeTextTimer->GetGameObject()->SetActive(false);

    _lastScore = _correctCounter / (float)size;

    ShowResult();
}

void RadioStationPlayingController::ShowResult()
{
    size_t size = _resultsThresholds.size();
    for (size_t index = 0ull; index < size; ++index)
    {
        if (_lastScore < _resultsThresholds[index])
        {
            _resultImage->SetSprite(_resultsImagesIds[index]);
            break;
        }
    }

    _resultShowingTimer = _resultShowingTime;
    _resultImage->GetGameObject()->SetActive(true);
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
    node["correctColor"] = _correctColor;
    node["wrongColor"] = _wrongColor;

    vector<size_t> savingNotesSpritesIds;
    savingNotesSpritesIds.reserve(_notesSpritesIds.size());
    for (size_t index = 0ull; index < _notesSpritesIds.size(); ++index)
    {
        savingNotesSpritesIds.push_back(SceneManager::GetSpriteSaveIdx(_notesSpritesIds[index]));
    }
    node["notesSpritesIds"] = savingNotesSpritesIds;

    savingNotesSpritesIds.resize(_resultsImagesIds.size());
    for (size_t index = 0ull; index < _resultsImagesIds.size(); ++index)
    {
        savingNotesSpritesIds[index] = SceneManager::GetSpriteSaveIdx(_resultsImagesIds[index]);
    }
    node["resultsSpritesIds"] = _resultsImagesIds;
    node["resultsThresholds"] = _resultsThresholds;

    node["buttonDo"] = _buttonDo->GetId();
    node["buttonRe"] = _buttonRe->GetId();
    node["buttonMi"] = _buttonMi->GetId();
    node["buttonFa"] = _buttonFa->GetId();
    node["resultImage"] = _resultImage->GetId();
    node["remainingTimeTextTimer"] = _remainingTimeTextTimer->GetId();

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
    _correctColor = node["correctColor"].as<vec4>();
    _wrongColor = node["wrongColor"].as<vec4>();

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