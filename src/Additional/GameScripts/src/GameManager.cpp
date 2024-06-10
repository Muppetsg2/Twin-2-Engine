#include <GameManager.h>
#include <manager/SceneManager.h>
#include <manager/PrefabManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <core/Random.h>
#include <string>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::UI;

using namespace std;

GameManager* GameManager::instance = nullptr;
size_t GameManager::_colorsNum = 7;


void GameManager::Initialize() {
    if (instance == nullptr)
    {
        instance = this;
        //prefabPlayer = PrefabManager::GetPrefab("res/prefabs/Player.prefab");
        //enemyPrefab = PrefabManager::GetPrefab("res/prefabs/Enemy.prefab");
        _dayText = SceneManager::FindObjectByName("DayText")->GetComponent<Text>();
        _monthText = SceneManager::FindObjectByName("MonthText")->GetComponent<Text>();
        _yearText = SceneManager::FindObjectByName("YearText")->GetComponent<Text>();
        
        _dayEventHandleId = GameTimer::Instance()->OnDayTicked += [&](int day) {
            _dayText->SetText(wstring(L"Day ").append(to_wstring(day)));
            };
        _monthEventHandleId = GameTimer::Instance()->OnMonthTicked += [&](int month) {
            _monthText->SetText(wstring(L"Month ").append(to_wstring(month)));
            };
        _yearEventHandleId = GameTimer::Instance()->OnYearTicked += [&](int year) {
            _yearText->SetText(wstring(L"Year ").append(to_wstring(year)));
            };

        _freePatronsData = _patronsData;
        //GeneratePlayer();

        if (_mapGenerator == nullptr) {
            GameObject* mg = SceneManager::FindObjectByType<Generation::MapGenerator>();

            if (mg != nullptr) {
                _mapGenerator = mg->GetComponent<Generation::MapGenerator>();

                if (!_mapGenerator->IsMapGenerated())
                    _mapGenerator->Generate();
            }
        }

        while (_carMaterials.size() < _colorsNum) {
            _carMaterials.push_back(nullptr);
        }
    }
    else
    {
        Twin2Engine::Manager::SceneManager::DestroyGameObject(this->GetGameObject());
    }
}

void GameManager::OnDestroy() {
    if (this == instance)
    {
        instance = nullptr;
    }

    if (GameTimer::Instance() != nullptr) {
        GameTimer::Instance()->OnDayTicked -= _dayEventHandleId;
        GameTimer::Instance()->OnMonthTicked -= _monthEventHandleId;
        GameTimer::Instance()->OnYearTicked -= _yearEventHandleId;
    }
}

void GameManager::OnEnable() {
    SPDLOG_INFO("Creating enenmy");
    //GenerateEnemy();
    //GenerateEnemy();
    //GenerateEnemy();
    //GenerateEnemy();
    //GenerateEnemy();
}

void GameManager::Update() {

    if (_mapGenerator == nullptr) {
        GameObject* mg = SceneManager::FindObjectByType<Generation::MapGenerator>();

        if (mg != nullptr) {
            _mapGenerator = mg->GetComponent<Generation::MapGenerator>();
            if (!mg->GetComponent<Generation::MapGenerator>()->IsMapGenerated())
            {
                mg->GetComponent<Generation::MapGenerator>()->Generate();
            }

            for (auto e : entities) {
                e->SetTileMap(_mapGenerator->tilemap);
            }
        }
    }
}

void GameManager::UpdateEnemies(int colorIdx) {
}

void GameManager::UpdateTiles() {

}


GameObject* GameManager::GeneratePlayer() {
    GameObject* player = Twin2Engine::Manager::SceneManager::CreateGameObject(prefabPlayer);
    Player* p = player->GetComponent<Player>();

    int chosen = Random::Range(0ull, _freeColors.size() - 1ull);
    //int chosen = 0;
    p->colorIdx = _freeColors[chosen];
    _freeColors.erase(_freeColors.begin() + chosen);
    //p->colorIdx = chosen;

    //p->patron = playersPatron;
    p->SetPatron(playersPatron);

    _freePatronsData.erase(find(_freePatronsData.begin(), _freePatronsData.end(), playersPatron));

    p->GetGameObject()->GetComponent<MeshRenderer>()->SetMaterial(0ull, _carMaterials[p->colorIdx]);

    entities.push_back(p);

    return player;
}

GameObject* GameManager::GenerateEnemy() {
    //if (freeColors.size() == 0)
    //{
    //    return nullptr;
    //}

    GameObject* enemy = Twin2Engine::Manager::SceneManager::CreateGameObject(enemyPrefab);
    //GameObject* enemy = Instantiate(enemyPrefab, new Vector3(), Quaternion.identity, gameObject.transform);

    Enemy* e = enemy->GetComponent<Enemy>();

    int chosen = Random::Range(0ull, _freeColors.size() - 1ull);
    //int chosen = 1;
    e->colorIdx = _freeColors[chosen];
    _freeColors.erase(_freeColors.begin() + chosen);
    //e->colorIdx = chosen;
    e->GetGameObject()->GetComponent<MeshRenderer>()->SetMaterial(0ull, _carMaterials[e->colorIdx]);

    unsigned chosenPatron = Random::Range<unsigned>(0u, _freePatronsData.size() - 1ull);
    //e->patron = _freePatronsData[chosenPatron];
    e->SetPatron(_freePatronsData[chosenPatron]);
    _freePatronsData.erase(find(_freePatronsData.begin(), _freePatronsData.end(), e->patron));
    /*float h = Random.Range(0f, 1f);
    float s = Random.Range(.7f, 1f);
    float v = 1f;
    bool good = false;
    while (!good)
    {
        good = true;
        foreach (Playable p in entities)
        {
            Color.RGBToHSV(p.Color, out float tempH, out float tempS, out float tempV);
            if (Mathf.Abs(tempH - h) < .005f)
            {
                good = false;
                h = Random.Range(0f, 1f);
                continue;
            }
            if (Mathf.Abs(tempS - s) < .005f)
            {
                good = false;
                s = Random.Range(.7f, 1f);
                continue;
            }
        }
    }
    e.Color = Color.HSVToRGB(h, s, v);*/

    //int idx = freeColors[Random::Range<int>(0, (freeColors.size() - 1))];
    //freeColors.erase(std::find(freeColors.begin(), freeColors.end(), idx));

    //e->colorIdx = idx;

    float minutes = 10.0f; // (GameTimer::Instance != nullptr ? GameTimer::Instance->TotalSeconds : 0.0f) / 60.0f;

    e->TakeOverSpeed = Random::Range(15.0f, 25.0f) + minutes * 2.0f;

    //e->patron = patrons[Random::Range<int>(0, 5)];

    e->albumTime = Random::Range(6.0f, 10.0f) + minutes * 2.0f;
    e->albumCooldown = Random::Range(5.0f, 10.0f) - minutes * 2.0f;

    e->fansTime = Random::Range(6.0f, 10.0f) + minutes * 2.0f;
    e->fansCooldown = Random::Range(10.0f, 15.0f) - minutes * 2.0f;

    e->radioTime = Random::Range(1.0f, 3.0f) + minutes;

    e->paperRockScisorsWinLuck = glm::clamp(Random::Range(.1f, .5f) + minutes * .1f, 0.0f, .5f);
    e->paperRockScisorsDrawLuck = glm::clamp(Random::Range(.1f, .5f) + minutes * .1f, 0.0f, .5f);
    e->radioWinNoteLuck = glm::clamp(Random::Range(.00f, 1.0f) + minutes * .1f, 0.0f, 1.0f);
    e->albumUseLuck = glm::clamp(Random::Range(0.0f, 1.0f) + minutes * .1f, 0.0f, 1.0f);
    e->concertUseLuck = glm::clamp(Random::Range(0.0f, 1.0f) + minutes * .1f, 0.0f, 1.0f);
    e->fansUseLuck = glm::clamp(Random::Range(0.0f, 1.0f) + minutes * .1f, 0.0f, 1.0f);
    //e->functionData = functionData;

    e->upgradeChance = glm::clamp(Random::Range(0.0f, 0.5f) + minutes * 0.1f, 0.0f, 0.5f);

    entities.push_back(e);
    return enemy;
}


void GameManager::StartMinigame() {
    minigameActive = true;
    //miniGameManager->minigameTime = minigameTime;
    //miniGameManager->Restart();
    miniGameCanvas->SetActive(true);
}

void GameManager::EndMinigame() {
    minigameActive = false;
    miniGameCanvas->SetActive(false);
}

void GameManager::GameOver() {
    //GameTimer::Instance->SaveIfHighest();
    //gameOverUI.SetActive(true);
    //UIGameOverPanelController::Instance->OpenPanel();
}

void GameManager::StartGame()
{
    GeneratePlayer();

    for (unsigned i = 0u; i < _enemiesNumber; ++i)
    {
        GenerateEnemy();
    }

    GameTimer::Instance()->StartTimer();
}

YAML::Node GameManager::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "GameManager";

    if (enemyPrefab != nullptr) {
        node["enemyPrefab"] = SceneManager::GetPrefabSaveIdx(enemyPrefab->GetId());
    }
    if (prefabPlayer != nullptr) {
        node["prefabPlayer"] = SceneManager::GetPrefabSaveIdx(prefabPlayer->GetId());
    }

    node["carMaterials"] = vector<size_t>();
    for (size_t index = 0ull; index < _carMaterials.size(); ++index)
    {
        node["carMaterials"].push_back(SceneManager::GetMaterialSaveIdx(_carMaterials[index]->GetId()));
    }

    node["patronsData"] = vector<string>();
    for (size_t index = 0ull; index < _patronsData.size(); ++index)
    {
        node["patronsData"].push_back(ScriptableObjectManager::GetPath(_patronsData[index]->GetId()));
    }

    return node;
}

bool GameManager::Deserialize(const YAML::Node& node)
{
    if (!node["carMaterials"] || !node["enemyPrefab"] || !node["prefabPlayer"] || !node["patronsData"] || !Component::Deserialize(node))
        return false;

    size_t size = node["carMaterials"].size();

    if (size == 0) return false;

    if (node["enemyPrefab"]) {
        enemyPrefab = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["enemyPrefab"].as<size_t>()));
    }
    if (node["prefabPlayer"]) {
        prefabPlayer = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["prefabPlayer"].as<size_t>()));
    }

    if (_carMaterials.size() == 0) {

        if (size == _colorsNum) {
            _carMaterials.reserve(size);
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
            }
        }
        else if (size < _colorsNum && size > 0ull) {
            _carMaterials.reserve(_colorsNum);
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
            }

            for (size_t index = 0ull; index < _colorsNum - size; ++index) {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>())));
            }
        }
    }
    else if (_carMaterials.size() == _colorsNum) {

        if (size == _colorsNum) {
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }
        }
        else if (size < _colorsNum && size > 0ull) {
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }

            for (size_t index = size; index < _colorsNum; ++index) {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>()));
            }
        }
    }
    else if (_carMaterials.size() < _colorsNum && _carMaterials.size() > 0ull) {

        if (size == _colorsNum) {
            for (size_t index = 0ull; index < _carMaterials.size(); ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }

            for (size_t index = _carMaterials.size(); index < size; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
            }
        }
        else if (size < _colorsNum && size > 0ull) {

            size_t min = _carMaterials.size() < size ? _carMaterials.size() : size;

            for (size_t index = 0ull; index < min; ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }

            if (size > _carMaterials.size()) {
                for (size_t index = _carMaterials.size(); index < size; ++index)
                {
                    _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
                }
            }
            else if (_carMaterials.size() > size) {
                for (size_t index = size; index < _carMaterials.size(); ++index)
                {
                    _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>()));
                }
            }

            size_t toAdd = _colorsNum - (size > _carMaterials.size() ? size : _carMaterials.size());

            for (size_t index = 0ull; index < toAdd; ++index) {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>())));
            }
        }
    }

    size = node["patronsData"].size();
    _patronsData.reserve(size);
    for (size_t index = 0ull; index < size; ++index)
    {
        _patronsData.push_back((PatronData*)ScriptableObjectManager::Load(node["patronsData"][index].as<string>()));
    }

    return true;
}

#if _DEBUG
void GameManager::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Game Manager##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        // TODO: ZROBIC
        /*
        Twin2Engine::UI::Text* _dayText;
        Twin2Engine::UI::Text* _monthText;
        Twin2Engine::UI::Text* _yearText;
        */

        std::map<size_t, string> prefabNames = Twin2Engine::Manager::PrefabManager::GetAllPrefabsNames();

        prefabNames.insert(std::pair(0, "None"));

        if (enemyPrefab != nullptr) {
            if (!prefabNames.contains(enemyPrefab->GetId())) {
                enemyPrefab = nullptr;
            }
        }

        size_t prefabId = enemyPrefab != nullptr ? enemyPrefab->GetId() : 0;

        if (ImGui::BeginCombo(string("Enemy Prefab##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

            bool clicked = false;
            size_t choosed = prefabId;
            for (auto& item : prefabNames) {

                if (ImGui::Selectable(string(item.second).append("##").append(id).c_str(), item.first == prefabId)) {

                    if (clicked) continue;

                    choosed = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed != 0) {
                    enemyPrefab = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                }
                else {
                    enemyPrefab = nullptr;
                }
            }

            ImGui::EndCombo();
        }

        if (prefabPlayer != nullptr) {
            if (!prefabNames.contains(prefabPlayer->GetId())) {
                prefabPlayer = nullptr;
            }
        }

        prefabId = prefabPlayer != nullptr ? prefabPlayer->GetId() : 0;

        if (ImGui::BeginCombo(string("Player Prefab##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

            bool clicked = false;
            size_t choosed = prefabId;
            for (auto& item : prefabNames) {

                if (ImGui::Selectable(string(item.second).append("##").append(id).c_str(), item.first == prefabId)) {

                    if (clicked) continue;

                    choosed = item.first;
                    clicked = true;
                }
            }

            if (clicked) {
                if (choosed != 0) {
                    prefabPlayer = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                }
                else {
                    prefabPlayer = nullptr;
                }
            }

            ImGui::EndCombo();
        }

        ImGui::Text("Enemies Number: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text(std::to_string(_enemiesNumber).c_str());
        ImGui::PopFont();

        map<size_t, string> mat_temp = MaterialsManager::GetAllMaterialsNames();

        vector<std::pair<size_t, string>> mats = vector<std::pair<size_t, string>>(mat_temp.begin(), mat_temp.end());

        vector<string> names = vector<string>();
        vector<size_t> ids = vector<size_t>();
        names.resize(mats.size());
        ids.resize(mats.size());

        std::sort(mats.begin(), mats.end(), [&](std::pair<size_t, string> const& left, std::pair<size_t, string> const& right) -> bool {
            return left.second.compare(right.second) < 0;
        });

        std::transform(mats.begin(), mats.end(), names.begin(), [](std::pair<size_t, string> const& i) -> string {
            return i.second;
        });

        std::transform(mats.begin(), mats.end(), ids.begin(), [](std::pair<size_t, string> const& i) -> size_t {
            return i.first;
        });

        mats.clear();

        ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool node_open = ImGui::TreeNodeEx(string("Car Materials##").append(id).c_str(), node_flag);

        if (node_open) {
            for (size_t i = 0; i < _carMaterials.size(); ++i) {
                Material* item = _carMaterials[i];
                int choosed = -1;

                if (item != nullptr) choosed = std::find(ids.begin(), ids.end(), item->GetId()) - ids.begin();

                if (ImGui::ComboWithFilter(string("##Car Materials").append(id).append(std::to_string(i)).c_str(), &choosed, names, 20)) {
                    if (choosed != -1) {
                        _carMaterials[i] = MaterialsManager::GetMaterial(ids[choosed]);
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}
#endif