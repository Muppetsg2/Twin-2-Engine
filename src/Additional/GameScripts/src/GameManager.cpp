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