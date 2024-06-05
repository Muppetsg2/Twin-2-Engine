#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Prefab.h>
#include <graphic/Material.h>
#include <ui/Text.h>

#include <AreaTaking/HexTile.h>
#include <Enemy.h>
#include <Player.h>
#include <GameTimer.h>

//#include <pch.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

class MiniGameManager;
class HexTile;
class PlayerInterface;

namespace Generation {
    class MapHexTile;
}


//class ConcertRoad : public Component {
class GameManager : public Component {
private:

    std::vector<int> freeColors{ 0, 1, 2, 3, 4, 5, 6 };
    std::vector<Twin2Engine::Graphic::Material*> _carMaterials;



    Twin2Engine::UI::Text* _dayText;
    Twin2Engine::UI::Text* _monthText;
    Twin2Engine::UI::Text* _yearText;

public:
    static GameManager* instance;

    std::vector<Playable*> entities;

    std::vector<HexTile*> Tiles;

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

    Material IndicatorMaterial;
    glm::vec3 IndicatorColor;

    glm::vec3 colors[6];
    PatronData* patrons[6];


    virtual void Initialize() override;
    virtual void OnEnable() override;
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

#if _DEBUG
    virtual void DrawEditor() override;
#endif
};