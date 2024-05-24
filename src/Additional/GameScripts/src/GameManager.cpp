#include <GameManager.h>
#include <manager/SceneManager.h>
#include <manager/PrefabManager.h>
#include <core/Random.h>
#include <string>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

GameManager* GameManager::instance = nullptr;


void GameManager::Initialize() {
    if (instance == nullptr)
    {
        instance = this;
        prefabPlayer = PrefabManager::GetPrefab("res/prefabs/Player.prefab");
        enemyPrefab = PrefabManager::GetPrefab("res/prefabs/Enemy.prefab");
        
        GeneratePlayer();
    }
    else
    {
        Twin2Engine::Manager::SceneManager::DestroyGameObject(this->GetGameObject());
    }
}

void GameManager::Update() {

}

void GameManager::UpdateEnemies(int colorIdx) {
}

void GameManager::UpdateTiles() {

}


GameObject* GameManager::GeneratePlayer() {
    GameObject* player = Twin2Engine::Manager::SceneManager::CreateGameObject(prefabPlayer);
    Player* p = player->GetComponent<Player>();

    p->patron = playersPatron;

    entities.push_back(p);

    return player;
}

GameObject* GameManager::GenerateEnemy() {
    if (freeColors.size() == 0)
    {
        return nullptr;
    }

    GameObject* enemy = Twin2Engine::Manager::SceneManager::CreateGameObject(enemyPrefab);
    //GameObject* enemy = Instantiate(enemyPrefab, new Vector3(), Quaternion.identity, gameObject.transform);

    Enemy* e = enemy->GetComponent<Enemy>();

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

    int idx = freeColors[Random::Range<int>(0, (freeColors.size() - 1))];
    freeColors.erase(std::find(freeColors.begin(), freeColors.end(), idx));

    e->colorIdx = idx;

    float minutes = 10.0f; // (GameTimer::Instance != nullptr ? GameTimer::Instance->TotalSeconds : 0.0f) / 60.0f;

    e->TakeOverSpeed = Random::Range(15.0f, 25.0f) + minutes * 2.0f;

    e->patron = patrons[Random::Range<int>(0, 5)];

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

YAML::Node GameManager::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "GameManager";
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool GameManager::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

#if _DEBUG
void GameManager::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("GameManager##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        Component::DrawInheritedFields();

        //glm::vec3 v = light->direction;
        //ImGui::DragFloat3(string("Direction##").append(id).c_str(), glm::value_ptr(v), .1f, -1.f, 1.f);
        //if (v != light->direction) {
        //    SetDirection(v);
        //
        //float p = light->power;
        //ImGui::DragFloat(string("Power##").append(id).c_str(), &p);
        //if (p != light->power) {
        //    SetPower(p);
        //}
    }
}
#endif