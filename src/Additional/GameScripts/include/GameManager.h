#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Prefab.h>
#include <vector>
#include <graphic/Material.h>
#include <Generation/MapHexTile.h>
#include <Enemy.h>
#include <Player.h>
//#include <pch.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

class MiniGameManager;
class PlayerInterface;

namespace Generation {
    class MapHexTile;
}

//class ConcertRoad : public Component {
class GameManager : public Component {
private:
    std::vector<int> freeColors;
public:
    static GameManager* instance;

    std::vector<Playable*> entities;

    std::vector<Generation::MapHexTile*> Tiles;

    Prefab* enemyPrefab = nullptr;

    GameObject* gameOverUI = nullptr;

    bool minigameActive = false;
    float minigameTime = 3.0f;
    GameObject* miniGameCanvas;
    MiniGameManager* miniGameManager;

    PlayerInterface* playerInterface;
    PatronData* playersPatron;

    bool gameStarted = false;
    bool gameOver = false;

    bool changeTile = false;

    Prefab* prefabPlayer = nullptr;
    bool createPlayer = true;

    Material* IndicatorMaterial;
    glm::vec3 IndicatorColor;

    glm::vec3 colors[6];
    PatronData* patrons[6];


    virtual void Initialize() override;
    virtual void Update() override;

    void UpdateEnemies(int colorIdx);
    void UpdateTiles();
    GameObject* GeneratePlayer();
    GameObject* GenerateEnemy();

    void StartMinigame();
    void EndMinigame();
    void GameOver();

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;
    virtual void DrawEditor() override;
};