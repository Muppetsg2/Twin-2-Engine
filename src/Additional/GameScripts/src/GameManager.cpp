#include <GameManager.h>
#include <manager/SceneManager.h>
#include <manager/PrefabManager.h>
#include <graphic/manager/MaterialsManager.h>
#include <core/Random.h>

#include <UIScripts/AreaTakenGraph.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::UI;

using namespace std;

GameManager *GameManager::instance = nullptr;
size_t GameManager::_colorsNum = 7;

void GameManager::Initialize()
{
    if (instance == nullptr)
    {
        instance = this;
        GameObject* temp = SceneManager::FindObjectByName("DateText");

        static constexpr int startMonth = 1;
        static constexpr int startYear = 2020;
        static constexpr int startDay = 1;

        if (temp != nullptr) {
            _dateText = temp->GetComponent<Text>();
            _dateEventHandleId = GameTimer::Instance()->OnDateTicked += [&](int day, int month, int year) {
                _dateText->SetText(
                    to_wstring(startYear + year)
                    .append(L".")
                    .append(startMonth + month < 10 ? L"0" + std::to_wstring(startMonth + month) : to_wstring(startMonth + month))
                    .append(L".")
                    .append(startDay + day - 1 < 10 ? L"0" + to_wstring(startDay + day - 1) : to_wstring(startDay + day - 1))
                );
            };

            _dateText->SetText(
                to_wstring(startYear)
                .append(L".")
                .append(startMonth < 10 ? L"0" + std::to_wstring(startMonth) : to_wstring(startMonth))
                .append(L".")
                .append(startDay < 10 ? L"0" + to_wstring(startDay) : to_wstring(startDay))
            );
        }

        _freePatronsData = _patronsData;
        // GeneratePlayer();

        if (_mapGenerator == nullptr)
        {
            GameObject *mg = SceneManager::FindObjectByType<Generation::MapGenerator>();

            if (mg != nullptr)
            {
                _mapGenerator = mg->GetComponent<Generation::MapGenerator>();

                _mapGenerationEventId = (_mapGenerator->OnMapGenerationEvent += [&](Generation::MapGenerator *generator) -> void
                                         {
                                             UpdateTiles();

                                             for (auto e : entities)
                                             {
                                                 e->SetTileMap(_mapGenerator->tilemap);
                                             }
                                         });

                if (!_mapGenerator->IsMapGenerated())
                    _mapGenerator->Generate();
            }
        }

        while (_carMaterials.size() < _colorsNum)
        {
            _carMaterials.push_back(nullptr);
        }

        _audioComponent = GetGameObject()->GetComponent<AudioComponent>();

        _textChooseStartingPosition = SceneManager::FindObjectByName("ChooseStartingPosition");
    }
    else
    {
        Twin2Engine::Manager::SceneManager::DestroyGameObject(this->GetGameObject());
    }

    //particleGenerator = new ParticleGenerator("origin/ParticleShader", "res/textures/particle.png", 10, 1.0f, 0.0f, 3.0f, 1.5f, 0.1f, 0.1f, 0.5f);
    //particleGenerator->startPosition = glm::vec4(0.0f, 4.0f, 0.0f, 1.0f);
    //particleGenerator->active = true;

}

void GameManager::OnDestroy() {
    //delete particleGenerator;
    if (this == instance)
    {
        instance = nullptr;
        Twin2Engine::Manager::ScriptableObjectManager::Unload("res/scriptableobjects/HexTileTextureData.so");
    }

    if (GameTimer::Instance() != nullptr) {
        if (_dateEventHandleId != -1) {
            GameTimer::Instance()->OnDateTicked -= _dateEventHandleId;
            _dateEventHandleId = -1;
        }

        /*
        if (_dayEventHandleId != -1) {
            GameTimer::Instance()->OnDayTicked -= _dayEventHandleId;
            _dayEventHandleId = -1;
        }

        if (_monthEventHandleId != -1) {
            GameTimer::Instance()->OnMonthTicked -= _monthEventHandleId;
            _monthEventHandleId = -1;
        }

        if (_yearEventHandleId != -1) {
            GameTimer::Instance()->OnYearTicked -= _yearEventHandleId;
            _yearEventHandleId = -1;
        }
        */
    }

    if (_mapGenerationEventId != -1 && _mapGenerator != nullptr)
    {
        _mapGenerator->OnMapGenerationEvent -= _mapGenerationEventId;
        _mapGenerationEventId = -1;
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

void GameManager::Update()
{

    if (_mapGenerator == nullptr)
    {
        GameObject *mg = SceneManager::FindObjectByType<Generation::MapGenerator>();

        if (mg != nullptr)
        {
            _mapGenerator = mg->GetComponent<Generation::MapGenerator>();
            if (!mg->GetComponent<Generation::MapGenerator>()->IsMapGenerated())
            {
                mg->GetComponent<Generation::MapGenerator>()->Generate();
            }
        }
    }

    if (_settingBackgroundMusicVolume)
    {
        _audioComponent->SetVolume((1.0f - _turningBackgroundMusicFactor) * _audioComponent->GetVolume() + _turningBackgroundMusicFactor * _targetVolume);
        if (glm::abs(_targetVolume - _audioComponent->GetVolume()) < 0.01)
        {
            _audioComponent->SetVolume(_targetVolume);
            _settingBackgroundMusicVolume = false;
        }
    }


    //if (gameStartUp && Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::LEFT))
    if (gameStartUp)
    {
        Ray ray = CameraComponent::GetMainCamera()->GetScreenPointRay(Input::GetCursorPos());// Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit raycastHit;

        //if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        //{
        //    HexTile* hexTile = raycastHit.collider->GetGameObject()->GetComponent<HexTile>();
        //    MapHexTile* mapHexTile = hexTile->GetMapHexTile();
        //    
        //    if (mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation && hexTile->currCooldown > 0.0f) && !hexTile->isFighting)
        //    {
        //        _player->StartPlayer(hexTile);
        //        gameStartUp = false;
        //    }
        //    
        //}
        if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        {
            SPDLOG_INFO("COL_ID: {}", raycastHit.collider->colliderId);
            HexTile* hexTile = raycastHit.collider->GetGameObject()->GetComponent<HexTile>();

            if (hexTile != _player->CurrTile && hexTile != _player->move->_pointedTile)
            {
                if (hexTile)
                {
                    MapHexTile* mapHexTile = hexTile->GetMapHexTile();

                    if (mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation && hexTile->currCooldown > 0.0f) && !hexTile->isFighting)
                    {
                        _player->move->_pointedTile = hexTile;
                        _player->move->_playerDestinationMarker->GetTransform()->SetGlobalPosition(
                            _player->move->_pointedTile->GetTransform()->GetGlobalPosition() + vec3(0.0f, _player->move->_destinationMarkerHeightOverSurface, 0.0f));
                        _player->move->_playerDestinationMarker->SetActive(true);
                        _player->move->_playerWrongDestinationMarker->SetActive(false);
                    }
                    else
                    {
                        _player->move->_pointedTile = nullptr;
                        _player->move->_playerDestinationMarker->SetActive(false);
                    }
                }
                else
                {
                    _player->move->_pointedTile = nullptr;
                    _player->move->_playerDestinationMarker->SetActive(false);
                }
            }

            if (_player->move->_pointedTile && Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::LEFT))
            {
                if (_player->move->_pointedTile->GetMapHexTile()->type == MapHexTile::HexTileType::PointOfInterest)
                {
                    _textChooseStartingPosition->SetActive(false);
                    DeleteGodRaysInCities();

                    for (GameObject* goTile : _shadowedTiles)
                    {
                        SceneManager::DestroyGameObject(goTile);
                    }
                    _shadowedTiles.clear();

                    _player->OwnTiles.clear();

                    _player->StartPlayer(_player->move->_pointedTile);
                    gameStartUp = false;
                    _player->move->_pointedTile = nullptr;
                    _player->move->_playerDestinationMarker->SetActive(false);
                }
                else
                {
                    _player->_audioComponent->SetAudio(_player->move->_soundWrongDestination);
                    _player->_audioComponent->Play();
                }
            }
        }
        else
        {
            _player->move->_pointedTile = nullptr;
            _player->move->_playerDestinationMarker->SetActive(false);
        }
    }

    if (startPhase3) {
        RestartMapPhase3();
    }
    if (startPhase2) {
        RestartMapPhase2();
    }
    if (Input::IsKeyDown(KEY::LEFT_CONTROL) && Input::IsKeyPressed(KEY::U)) {
        RestartMapPhase1();
    }
}

void GameManager::UpdateEnemies(int colorIdx)
{
}

void GameManager::UpdateTiles()
{
    list<HexTile *> temp = _mapGenerator->GetGameObject()->GetComponentsInChildren<HexTile>();
    Tiles.clear();
    Tiles.insert(Tiles.begin(), temp.begin(), temp.end());
}

GameObject* GameManager::GeneratePlayer()
{
    GameObject* player = Twin2Engine::Manager::SceneManager::CreateGameObject(prefabPlayer);
    Player* p = player->GetComponent<Player>();
    _player = p;

    p->GetTransform()->SetGlobalPosition(vec3(0.0f, -5.0f, 0.0f));
    // p->patron = playersPatron;
    p->SetPatron(playersPatron);

    int chosenColor = 0;
    uint8_t color = (uint8_t) playersPatron->GetColor();
    while (color /= 2) chosenColor++;

    int chosen = chosenColor;// Random::Range(0ull, _freeColors.size() - 1ull);
    // int chosen = 0;
    p->colorIdx = _freeColors[chosen];
    _freeColors.erase(_freeColors.begin() + chosen);
    // p->colorIdx = chosen;

    _freePatronsData.erase(find(_freePatronsData.begin(), _freePatronsData.end(), playersPatron));

    p->GetGameObject()->GetComponent<MeshRenderer>()->SetMaterial(0ull, _carMaterials[p->colorIdx]);

    _player->move->_playerDestinationMarker->GetComponent<MeshRenderer>()->AddMaterial(
        _player->GetGameObject()->GetComponent<MeshRenderer>()->GetMaterial(0ull));

    //_player->move->_playerWrongDestinationMarker->GetComponent<MeshRenderer>()->AddMaterial(
    //        _player->GetGameObject()->GetComponent<MeshRenderer>()->GetMaterial(0ull));

    entities.push_back(p);

    switch (p->patron->GetPatronMusic())
    {
    case PatronMusic::ROCK:
        _audioComponent->SetAudio(_rockBackgroundMusics[0ull]);
        _audioComponent->Play();

        break;

    case PatronMusic::ELECTRONIC:
        _audioComponent->SetAudio(_electricBackgroundMusics[0ull]);
        _audioComponent->Play();

        break;

    case PatronMusic::POP:
        _audioComponent->SetAudio(_popBackgroundMusics[0ull]);
        _audioComponent->Play();

        break;

    case PatronMusic::HEAVY_METAL:
        _audioComponent->SetAudio(_heavyMetalBackgroundMusics[0ull]);
        _audioComponent->Play();

        break;

    case PatronMusic::CLASSIC:
        _audioComponent->SetAudio(_classicBackgroundMusics[0ull]);
        _audioComponent->Play();

        break;

    case PatronMusic::DISCO:
        _audioComponent->SetAudio(_discoBackgroundMusics[0ull]);
        _audioComponent->Play();

        break;

    default:

        break;
    }

    return player;
}

GameObject* GameManager::GenerateEnemy()
{
    // if (freeColors.size() == 0)
    //{
    //     return nullptr;
    // }

    GameObject* enemy = Twin2Engine::Manager::SceneManager::CreateGameObject(enemyPrefab);
    // GameObject* enemy = Instantiate(enemyPrefab, new Vector3(), Quaternion.identity, gameObject.transform);

    Enemy* e = enemy->GetComponent<Enemy>();

    unsigned chosenPatron = Random::Range<unsigned>(0u, _freePatronsData.size() - 1ull);
    // e->patron = _freePatronsData[chosenPatron];
    e->SetPatron(_freePatronsData[chosenPatron]);
    _freePatronsData.erase(find(_freePatronsData.begin(), _freePatronsData.end(), e->patron));

    int chosenColor = 0;
    uint8_t color = (uint8_t)e->patron->GetColor();
    while (color /= 2) chosenColor++;

    int chosen = chosenColor; // Random::Range(0ull, _freeColors.size() - 1ull);
    // int chosen = 1;
    //e->colorIdx = _freeColors[chosen];
    e->colorIdx = chosenColor;
    for (size_t index = 0ull; index < _freeColors.size(); ++index)
    {
        if (_freeColors[index] == chosenColor)
        {
            _freeColors.erase(_freeColors.begin() + index);
        }
    }
    // e->colorIdx = chosen;
    e->GetGameObject()->GetComponent<MeshRenderer>()->SetMaterial(0ull, _carMaterials[e->colorIdx]);

    e->GetTransform()->SetGlobalPosition(vec3(0.0f, -5.0f, 0.0f));

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

    // int idx = freeColors[Random::Range<int>(0, (freeColors.size() - 1))];
    // freeColors.erase(std::find(freeColors.begin(), freeColors.end(), idx));

    // e->colorIdx = idx;

    float minutes = 10.0f; // (GameTimer::Instance != nullptr ? GameTimer::Instance->TotalSeconds : 0.0f) / 60.0f;

    e->TakeOverSpeed = Random::Range(15.0f, 25.0f) + minutes * 2.0f;

    // e->patron = patrons[Random::Range<int>(0, 5)];

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
    // e->functionData = functionData;

    e->upgradeChance = glm::clamp(Random::Range(0.0f, 0.5f) + minutes * 0.1f, 0.0f, 0.5f);

    entities.push_back(e);
    return enemy;
}

void GameManager::StartMinigame()
{
    minigameActive = true;
    // miniGameManager->minigameTime = minigameTime;
    // miniGameManager->Restart();
    miniGameCanvas->SetActive(true);
}

void GameManager::EndMinigame()
{
    minigameActive = false;
    miniGameCanvas->SetActive(false);
}

void GameManager::GameOver()
{
    gameOver = true;
    GameTimer::Instance()->SaveIfHighest();
    // gameOverUI.SetActive(true);
    // UIGameOverPanelController::Instance->OpenPanel();
}

void GameManager::EnemyDied(Enemy* enemy)
{
    size_t i = 0;
    for (auto& entity : entities) {
        if (entity == enemy) {
            entities.erase(entities.begin() + i);
            break;
        }
        ++i;
    }

    SceneManager::DestroyGameObject(enemy->GetGameObject());

    // ONLY PLAYER
    if (entities.size() == 1) {
        RestartMap();
    }
}

void GameManager::RestartMap()
{
    // TODO: KEEP PLAYER WITH 50% of $
    // TODO: GENERATE NEW MAP
    // TODO: ADD ONE MORE ENEMY
    RestartMapPhase1();
}


void GameManager::SpawnGodRaysInCities() {
    Prefab* gorRayP = PrefabManager::GetPrefab("res/prefabs/GodRay.prefab");
    for (auto& tile : SceneManager::FindObjectByName("MapGenerator")->GetComponentsInChildren<HexTile>())
    {
        MapHexTile* mapHexTile = tile->GetMapHexTile();
        if (mapHexTile->type == Generation::MapHexTile::HexTileType::PointOfInterest)
        {
            godRays.insert(SceneManager::CreateGameObject(gorRayP, tile->GetTransform()));
        }
    }
}

void GameManager::DeleteGodRaysInCities() {
    GameObject* gr = nullptr;

    while (godRays.size() > 0) {
        gr = *godRays.begin();
        godRays.erase(gr);
        SceneManager::DestroyGameObject(gr);
    }
}


void GameManager::StartGame()
{
    GeneratePlayer();

    _textChooseStartingPosition->SetActive(true);
    SpawnGodRaysInCities();

    Prefab* prefabShadowingHexPlane = PrefabManager::GetPrefab(_prefabPathShadowingPlane);
    list<MapHexTile*> tiles = _mapGenerator->GetGameObject()->GetComponentsInChildren<MapHexTile>();
    for (MapHexTile* tile : tiles)
    {
        if (tile->type != MapHexTile::HexTileType::Mountain && tile->type != MapHexTile::HexTileType::PointOfInterest)
        {
            GameObject* instanced = SceneManager::CreateGameObject(prefabShadowingHexPlane, tile->GetTransform());
            _shadowedTiles.push_back(instanced);
        }
    }

    for (unsigned i = 0u; i < _enemiesNumber; ++i)
    {
        GenerateEnemy();
    }

    for (auto e : entities)
    {
        e->SetTileMap(_mapGenerator->tilemap);
    }

    GameTimer::Instance()->StartTimer();
    gameStartUp = true;
}

void GameManager::RestartMapPhase1() {
    gameStarted = false;
    _player->ResetOnNewMap();

    int key = 0;
    GameObject* go = nullptr;


    //for (size_t index = 0ull; index < entities.size(); ++index)
    //{
    //    if (entities[index] != _player)
    //    {
    //        entities[index]->CurrTile = nullptr;
    //        SceneManager::DestroyGameObject(entities[index]->GetGameObject());
    //    }
    //}
    //entities.clear();
    //entities.push_back(_player);


    _freePatronsData.clear();
    for (auto& p : _patronsData) {
        _freePatronsData.push_back(p);
    }
    _freePatronsData.erase(find(_freePatronsData.begin(), _freePatronsData.end(), _player->patron));
    // HexagonalTilemap* tileMap = mapGeneratorGO->GetComponent<HexagonalTilemap>();
    //Generation::MapGenerator* mapGenerator = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Generation::MapGenerator>();
    startPhase2 = true;
    //mapGenerator->Clear();
}

void GameManager::RestartMapPhase2() {
    AreaTakenGraph::Instance()->Reset();

    _mapGenerator->Clear();

    startPhase2 = false;
    startPhase3 = true;
}

void GameManager::RestartMapPhase3() {
    _mapGenerator->Clear();
    _mapGenerator->Generate();

    if (_enemiesNumber < 5) {
        ++_enemiesNumber;
    }

    _freeColors = { 0, 1, 2, 3, 4, 5, 6 };
    _freeColors.erase(find(_freeColors.begin(), _freeColors.end(), _player->colorIdx));

    for (unsigned i = 0u; i < _enemiesNumber; ++i)
    {
        GenerateEnemy();
    }

    for (auto e : entities)
    {
        e->SetTileMap(_mapGenerator->tilemap);
    }

    GameTimer::Instance()->ResetTimer();
    GameTimer::Instance()->StartTimer();
    gameStartUp = true;

    

    gameStarted = false;
    startPhase3 = false;
    _textChooseStartingPosition->SetActive(true);
    SpawnGodRaysInCities();

    Prefab* prefabShadowingHexPlane = PrefabManager::GetPrefab(_prefabPathShadowingPlane);
    list<MapHexTile*> tiles = _mapGenerator->GetGameObject()->GetComponentsInChildren<MapHexTile>();
    for (MapHexTile* tile : tiles)
    {
        if (tile->type != MapHexTile::HexTileType::Mountain && tile->type != MapHexTile::HexTileType::PointOfInterest)
        {
            GameObject* instanced = SceneManager::CreateGameObject(prefabShadowingHexPlane, tile->GetTransform());
            _shadowedTiles.push_back(instanced);
        }
    }
}

Player* GameManager::GetPlayer() const
{
    return _player;
}

void GameManager::SetBackgrounMusicVolumeSmooth(float volume, float factor)
{
    _targetVolume = volume;
    _turningBackgroundMusicFactor = factor;
    _settingBackgroundMusicVolume = true;
}

YAML::Node GameManager::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "GameManager";

    if (enemyPrefab != nullptr)
    {
        node["enemyPrefab"] = SceneManager::GetPrefabSaveIdx(enemyPrefab->GetId());
    }
    if (prefabPlayer != nullptr)
    {
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


    node["RockBackgroundMusics"] = vector<size_t>();
    for (size_t index = 0ull; index < _rockBackgroundMusics.size(); ++index)
    {
        node["RockBackgroundMusics"].push_back(SceneManager::GetAudioSaveIdx(_rockBackgroundMusics[index]));
    }

    node["ElectricBackgroundMusics"] = vector<size_t>();
    for (size_t index = 0ull; index < _electricBackgroundMusics.size(); ++index)
    {
        node["ElectricBackgroundMusics"].push_back(SceneManager::GetAudioSaveIdx(_electricBackgroundMusics[index]));
    }

    node["PopBackgroundMusics"] = vector<size_t>();
    for (size_t index = 0ull; index < _popBackgroundMusics.size(); ++index)
    {
        node["PopBackgroundMusics"].push_back(SceneManager::GetAudioSaveIdx(_popBackgroundMusics[index]));
    }

    node["HeavyMetalBackgroundMusics"] = vector<size_t>();
    for (size_t index = 0ull; index < _heavyMetalBackgroundMusics.size(); ++index)
    {
        node["HeavyMetalBackgroundMusics"].push_back(SceneManager::GetAudioSaveIdx(_heavyMetalBackgroundMusics[index]));
    }

    node["ClassicBackgroundMusics"] = vector<size_t>();
    for (size_t index = 0ull; index < _classicBackgroundMusics.size(); ++index)
    {
        node["ClassicBackgroundMusics"].push_back(SceneManager::GetAudioSaveIdx(_classicBackgroundMusics[index]));
    }

    node["DiscoBackgroundMusics"] = vector<size_t>();
    for (size_t index = 0ull; index < _discoBackgroundMusics.size(); ++index)
    {
        node["DiscoBackgroundMusics"].push_back(SceneManager::GetAudioSaveIdx(_discoBackgroundMusics[index]));
    }

    //node["RockBackgroundMusics"] = _rockBackgroundMusics;
    //node["ElectricBackgroundMusics"] = _electricBackgroundMusics;
    //node["PopBackgroundMusics"] = _popBackgroundMusics;
    //node["HeavyMetalBackgroundMusics"] = _heavyMetalBackgroundMusics;
    //node["JazzBackgroundMusics"] = _jazzBackgroundMusics;
    //node["DiscoBackgroundMusics"] = _discoBackgroundMusics;

    return node;
}

bool GameManager::Deserialize(const YAML::Node &node)
{
    if (!node["enemyPrefab"] || !node["prefabPlayer"] || !node["carMaterials"] || !node["patronsData"] || 
        !node["RockBackgroundMusics"] || !node["ElectricBackgroundMusics"] || !node["PopBackgroundMusics"] || 
        !node["HeavyMetalBackgroundMusics"] || !node["ClassicBackgroundMusics"] || !node["DiscoBackgroundMusics"] || 
        !Component::Deserialize(node))
        return false;

    size_t size = node["carMaterials"].size();

    if (size == 0)
        return false;

    if (node["enemyPrefab"])
    {
        enemyPrefab = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["enemyPrefab"].as<size_t>()));
    }
    if (node["prefabPlayer"])
    {
        prefabPlayer = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["prefabPlayer"].as<size_t>()));
    }

    if (_carMaterials.size() == 0)
    {

        if (size == _colorsNum)
        {
            _carMaterials.reserve(size);
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
            }
        }
        else if (size < _colorsNum && size > 0ull)
        {
            _carMaterials.reserve(_colorsNum);
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
            }

            for (size_t index = 0ull; index < _colorsNum - size; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>())));
            }
        }
    }
    else if (_carMaterials.size() == _colorsNum)
    {

        if (size == _colorsNum)
        {
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }
        }
        else if (size < _colorsNum && size > 0ull)
        {
            for (size_t index = 0ull; index < size; ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }

            for (size_t index = size; index < _colorsNum; ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>()));
            }
        }
    }
    else if (_carMaterials.size() < _colorsNum && _carMaterials.size() > 0ull)
    {

        if (size == _colorsNum)
        {
            for (size_t index = 0ull; index < _carMaterials.size(); ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }

            for (size_t index = _carMaterials.size(); index < size; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
            }
        }
        else if (size < _colorsNum && size > 0ull)
        {

            size_t min = _carMaterials.size() < size ? _carMaterials.size() : size;

            for (size_t index = 0ull; index < min; ++index)
            {
                _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>()));
            }

            if (size > _carMaterials.size())
            {
                for (size_t index = _carMaterials.size(); index < size; ++index)
                {
                    _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][index].as<size_t>())));
                }
            }
            else if (_carMaterials.size() > size)
            {
                for (size_t index = size; index < _carMaterials.size(); ++index)
                {
                    _carMaterials[index] = MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>()));
                }
            }

            size_t toAdd = _colorsNum - (size > _carMaterials.size() ? size : _carMaterials.size());

            for (size_t index = 0ull; index < toAdd; ++index)
            {
                _carMaterials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(node["carMaterials"][size - 1].as<size_t>())));
            }
        }
    }

    _rockBackgroundMusics = node["RockBackgroundMusics"].as<vector<size_t>>();
    for (size_t index = 0ull; index < _rockBackgroundMusics.size(); ++index)
    {
        _rockBackgroundMusics[index] = SceneManager::GetAudio(_rockBackgroundMusics[index]);
    }
    _electricBackgroundMusics = node["ElectricBackgroundMusics"].as<vector<size_t>>();
    for (size_t index = 0ull; index < _electricBackgroundMusics.size(); ++index)
    {
        _electricBackgroundMusics[index] = SceneManager::GetAudio(_electricBackgroundMusics[index]);
    }
    _popBackgroundMusics = node["PopBackgroundMusics"].as<vector<size_t>>();
    for (size_t index = 0ull; index < _popBackgroundMusics.size(); ++index)
    {
        _popBackgroundMusics[index] = SceneManager::GetAudio(_popBackgroundMusics[index]);
    }
    _heavyMetalBackgroundMusics = node["HeavyMetalBackgroundMusics"].as<vector<size_t>>();
    for (size_t index = 0ull; index < _heavyMetalBackgroundMusics.size(); ++index)
    {
        _heavyMetalBackgroundMusics[index] = SceneManager::GetAudio(_heavyMetalBackgroundMusics[index]);
    }
    _classicBackgroundMusics = node["ClassicBackgroundMusics"].as<vector<size_t>>();
    for (size_t index = 0ull; index < _classicBackgroundMusics.size(); ++index)
    {
        _classicBackgroundMusics[index] = SceneManager::GetAudio(_classicBackgroundMusics[index]);
    }
    _discoBackgroundMusics = node["DiscoBackgroundMusics"].as<vector<size_t>>();
    for (size_t index = 0ull; index < _discoBackgroundMusics.size(); ++index)
    {
        _discoBackgroundMusics[index] = SceneManager::GetAudio(_discoBackgroundMusics[index]);
    }

    size = node["patronsData"].size();
    _patronsData.reserve(size);
    for (size_t index = 0ull; index < size; ++index)
    {
        _patronsData.push_back((PatronData *)ScriptableObjectManager::Load(node["patronsData"][index].as<string>()));
    }

    return true;
}

#if _DEBUG
void GameManager::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Game Manager##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str()))
    {

        if (Component::DrawInheritedFields())
            return;

        // TODO: ZROBIC
        /*
        Twin2Engine::UI::Text* _dayText;
        Twin2Engine::UI::Text* _monthText;
        Twin2Engine::UI::Text* _yearText;
        */

        ImGui::TextColored(ImVec4(0.5f, 1.f, 1.f, 1.f), "If You Want To Reload Game. Press CTRL+U");

        std::map<size_t, string> prefabNames = Twin2Engine::Manager::PrefabManager::GetAllPrefabsNames();

        prefabNames.insert(std::pair(0, "None"));

        if (enemyPrefab != nullptr)
        {
            if (!prefabNames.contains(enemyPrefab->GetId()))
            {
                enemyPrefab = nullptr;
            }
        }

        size_t prefabId = enemyPrefab != nullptr ? enemyPrefab->GetId() : 0;

        if (ImGui::BeginCombo(string("Enemy Prefab##SO").append(id).c_str(), prefabNames[prefabId].c_str()))
        {

            bool clicked = false;
            size_t choosed = prefabId;
            for (auto &item : prefabNames)
            {

                if (ImGui::Selectable(string(item.second).append("##").append(id).c_str(), item.first == prefabId))
                {

                    if (clicked)
                        continue;

                    choosed = item.first;
                    clicked = true;
                }
            }

            if (clicked)
            {
                if (choosed != 0)
                {
                    enemyPrefab = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                }
                else
                {
                    enemyPrefab = nullptr;
                }
            }

            ImGui::EndCombo();
        }

        if (prefabPlayer != nullptr)
        {
            if (!prefabNames.contains(prefabPlayer->GetId()))
            {
                prefabPlayer = nullptr;
            }
        }

        prefabId = prefabPlayer != nullptr ? prefabPlayer->GetId() : 0;

        if (ImGui::BeginCombo(string("Player Prefab##SO").append(id).c_str(), prefabNames[prefabId].c_str()))
        {

            bool clicked = false;
            size_t choosed = prefabId;
            for (auto &item : prefabNames)
            {

                if (ImGui::Selectable(string(item.second).append("##").append(id).c_str(), item.first == prefabId))
                {

                    if (clicked)
                        continue;

                    choosed = item.first;
                    clicked = true;
                }
            }

            if (clicked)
            {
                if (choosed != 0)
                {
                    prefabPlayer = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                }
                else
                {
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

        mat_temp.clear();

        vector<string> names = vector<string>();
        vector<size_t> ids = vector<size_t>();
        names.resize(mats.size());
        ids.resize(mats.size());

        std::sort(mats.begin(), mats.end(), [&](std::pair<size_t, string> const &left, std::pair<size_t, string> const &right) -> bool
                  { return left.second.compare(right.second) < 0; });

        std::transform(mats.begin(), mats.end(), names.begin(), [](std::pair<size_t, string> const& i) -> string {
            return i.second + "##" + std::to_string(i.first);
        });

        std::transform(mats.begin(), mats.end(), ids.begin(), [](std::pair<size_t, string> const &i) -> size_t
                       { return i.first; });

        mats.clear();

        ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool node_open = ImGui::TreeNodeEx(string("Car Materials##").append(id).c_str(), node_flag);

        if (node_open)
        {
            for (size_t i = 0; i < _carMaterials.size(); ++i)
            {
                Material *item = _carMaterials[i];
                int choosed = -1;

                if (item != nullptr)
                    choosed = std::find(ids.begin(), ids.end(), item->GetId()) - ids.begin();

                if (ImGui::ComboWithFilter(string("##Car Materials").append(id).append(std::to_string(i)).c_str(), &choosed, names, 20))
                {
                    if (choosed != -1)
                    {
                        _carMaterials[i] = MaterialsManager::GetMaterial(ids[choosed]);
                    }
                }
            }
            ImGui::TreePop();
        }

        names.clear();
        ids.clear();
    }
}
#endif