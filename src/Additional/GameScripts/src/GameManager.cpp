#include <GameManager.h>
#include <manager/SceneManager.h>


GameManager* GameManager::instance = nullptr;


void GameManager::Initialize() {
    if (instance == nullptr)
    {
        instance = this;
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

}

GameObject* GameManager::GenerateEnemy() {
    if (freeColors.Count == 0)
    {
        return nullptr;
    }

    GameObject* enemy = Twin2Engine::Manager::SceneManager::CreateGameObject(enemyPrefab, GetTransform());
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

    int idx = freeColors[UnityEngine.Random.Range(0, freeColors.Count)];
    freeColors.Remove(idx);
    e.colorIdx = idx;

    float minutes = (GameTimer::Instance != nullptr ? GameTimer::Instance->TotalSeconds : 0.0f) / 60.0f;

    e.TakeOverSpeed = UnityEngine.Random.Range(15f, 25f) + minutes * 2f;

    e.patron = patrons[UnityEngine.Random.Range(0, patrons.Length)];

    e.albumTime = UnityEngine.Random.Range(6f, 10f) + minutes * 2f;
    e.albumCooldown = UnityEngine.Random.Range(5f, 10f) - minutes * 2f;

    e.fansTime = UnityEngine.Random.Range(6f, 10f) + minutes * 2f;
    e.fansCooldown = UnityEngine.Random.Range(10f, 15f) - minutes * 2f;

    e.radioTime = UnityEngine.Random.Range(1f, 3f) + minutes;

    e.paperRockScisorsWinLuck = Mathf.Clamp(UnityEngine.Random.Range(.1f, .5f) + minutes * .1f, 0f, .5f);
    e.paperRockScisorsDrawLuck = Mathf.Clamp(UnityEngine.Random.Range(.1f, .5f) + minutes * .1f, 0f, .5f);
    e.radioWinNoteLuck = Mathf.Clamp(UnityEngine.Random.Range(0f, 1f) + minutes * .1f, 0f, 1f);
    e.albumUseLuck = Mathf.Clamp(UnityEngine.Random.Range(0f, 1f) + minutes * .1f, 0f, 1f);
    e.concertUseLuck = Mathf.Clamp(UnityEngine.Random.Range(0f, 1f) + minutes * .1f, 0f, 1f);
    e.fansUseLuck = Mathf.Clamp(UnityEngine.Random.Range(0f, 1f) + minutes * .1f, 0f, 1f);
    e.functionData = functionData;

    e.upgradeChance = Mathf.Clamp(UnityEngine.Random.Range(0f, .5f) + minutes * .1f, 0f, .5f);

    entities.Add(e);
    return enemy;
}


void GameManager::StartMinigame() {
    minigameActive = true;
    miniGameManager->minigameTime = minigameTime;
    miniGameManager->Restart();
    miniGameCanvas->SetActive(true);
}

void GameManager::EndMinigame() {
    minigameActive = false;
    miniGameCanvas->SetActive(false);
}

void GameManager::GameOver() {
    GameTimer::Instance->SaveIfHighest();
    //gameOverUI.SetActive(true);
    UIGameOverPanelController::Instance->OpenPanel();
}
