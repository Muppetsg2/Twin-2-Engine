#pragma once

#include <core/AudioComponent.h>
#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Prefab.h>
#include <graphic/Material.h>
#include <ui/Text.h>

#include <Generation/MapGenerator.h>

#include <AreaTaking/HexTile.h>
#include <Enemy.h>
#include <Player.h>
#include <GameTimer.h>

#include <ParticleSystemsController.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

class MiniGameManager;
class HexTile;
class PlayerInterface;
class Player;
class Enemy;

namespace Generation {
    class MapHexTile;
}

class GameManager : public Component {
private:

    static size_t _colorsNum;

    Twin2Engine::Core::AudioComponent* _audioComponent;

    // Musics
    std::vector<size_t> _rockBackgroundMusics;
    std::vector<size_t> _electricBackgroundMusics;
    std::vector<size_t> _popBackgroundMusics;
    std::vector<size_t> _heavyMetalBackgroundMusics;
    std::vector<size_t> _jazzBackgroundMusics;
    std::vector<size_t> _discoBackgroundMusics;


    std::vector<int> _freeColors{ 0, 1, 2, 3, 4, 5, 6 };
    std::vector<Twin2Engine::Graphic::Material*> _carMaterials;

    std::vector<PatronData*> _patronsData;
    std::vector<PatronData*> _freePatronsData;

    int _dateEventHandleId = -1;
    //size_t _dayEventHandleId;
    //size_t _monthEventHandleId;
    //size_t _yearEventHandleId;

    Twin2Engine::Core::GameObject* _textChooseStartingPosition;

    Twin2Engine::UI::Text* _dateText;
    //Twin2Engine::UI::Text* _dayText;
    //Twin2Engine::UI::Text* _monthText;
    //Twin2Engine::UI::Text* _yearText;

    unsigned int _enemiesNumber = 2u;
    int _mapGenerationEventId = -1;
    Generation::MapGenerator* _mapGenerator;
    
    //ParticleGenerator* particleGenerator;
    Player* _player;
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

    //HexTile* _pointedTile = nullptr;

    bool gameStartUp = false;
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
    virtual void OnDestroy() override;
    virtual void OnEnable() override;
    virtual void Update() override;

    void UpdateEnemies(int colorIdx);
    void UpdateTiles();
    GameObject* GeneratePlayer();
    GameObject* GenerateEnemy();

    void StartMinigame();
    void EndMinigame();
    void GameOver();
    void EnemyDied(Enemy* enemy);
    void RestartMap();

    void StartGame();
    Player* GetPlayer() const;

    virtual YAML::Node Serialize() const override;
    virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
    virtual void DrawEditor() override;
#endif
};