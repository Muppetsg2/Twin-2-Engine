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

            if (mg != nullptr)
                _mapGenerator = mg->GetComponent<Generation::MapGenerator>();
            if (!mg->GetComponent<Generation::MapGenerator>()->IsMapGenerated())
                mg->GetComponent<Generation::MapGenerator>()->Generate();
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

        //GameTimer::Instance()->OnDayTicked -= _dayEventHandleId;
        //GameTimer::Instance()->OnMonthTicked -= _monthEventHandleId;
        //GameTimer::Instance()->OnYearTicked -= _yearEventHandleId;
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

    int chosen = Random::Range(0ull, freeColors.size() - 1ull);
    //int chosen = 0;
    p->colorIdx = freeColors[chosen];
    freeColors.erase(freeColors.begin() + chosen);
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

    int chosen = Random::Range(0ull, freeColors.size() - 1ull);
    //int chosen = 1;
    e->colorIdx = freeColors[chosen];
    freeColors.erase(freeColors.begin() + chosen);
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
        node["enemyPrefab"] = PrefabManager::GetPrefabPath(enemyPrefab);
    }
    if (prefabPlayer != nullptr) {
        node["prefabPlayer"] = PrefabManager::GetPrefabPath(prefabPlayer);
    }

    vector<string> carMaterialsStrings;
    carMaterialsStrings.reserve(_carMaterials.size());

    const size_t carMaterialsSize = _carMaterials.size();
    for (size_t index = 0ull; index < carMaterialsSize; ++index)
    {
        carMaterialsStrings.push_back(MaterialsManager::GetMaterialName(_carMaterials[index]->GetId()));
    }
    node["carMaterials"] = carMaterialsStrings;

    return node;
}

bool GameManager::Deserialize(const YAML::Node& node)
{
    if (!node["carMaterials"] || !Component::Deserialize(node))
        return false;

    if (node["enemyPrefab"]) {
        enemyPrefab = PrefabManager::LoadPrefab(node["enemyPrefab"].as<string>());
    }
    if (node["prefabPlayer"]) {
        prefabPlayer = PrefabManager::LoadPrefab(node["prefabPlayer"].as<string>());
    }

    size_t size = node["carMaterials"].size();
    _carMaterials.resize(size);
    for (size_t index = 0ull; index < size; ++index)
    {
        _carMaterials[index] = MaterialsManager::GetMaterial(node["carMaterials"][index].as<string>());
    }

    size = node["patronsData"].size();
    _patronsData.resize(size);
    for (size_t index = 0ull; index < size; ++index)
    {
        _patronsData[index] = (PatronData*)ScriptableObjectManager::Load(node["patronsData"][index].as<string>());
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

        // TODO: Zrobic
    }
}
#endif