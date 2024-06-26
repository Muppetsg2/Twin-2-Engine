#include <PlayerMovement.h>
#include <physic/CollisionManager.h>
#include <physic/Ray.h>
#include <core/CameraComponent.h>
#include <core/Input.h>
#include <core/Time.h>
#include <cmath>
#include <string>
#include <UIScripts/MinigameManager.h>

// PATH FINDING
#include <AstarPathfinding/AStarPath.h>
#include <AstarPathfinding/AStarPathfinder.h>

using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;

using namespace Generation;
using namespace AStar;

void PlayerMovement::Initialize() {
	//seeker = GetGameObject()->GetComponent<Seeker>();
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
    _audioComponent = GetGameObject()->GetComponents<AudioComponent>().back();

    _player = GetGameObject()->GetComponent<Player>();
    //OnFinishMoving.AddCallback([](GameObject* playerGO, HexTile* destHexTile) {
    //    Playable* occupyingEntity = destHexTile->occupyingEntity;
    //    if (occupyingEntity != nullptr) {
    //        MinigameManager::GetLastInstance()->StartMinigame(playerGO->GetComponent<Player>(), occupyingEntity);
    //    }
    //    });
}

void PlayerMovement::OnDestroy() {
    //seeker = GetGameObject()->GetComponent<Seeker>();
    if (_path) {
        delete _path;
        _path = nullptr;
    }
    _info.WaitForFinding();
}

void PlayerMovement::Update() {
    Ray ray = CameraComponent::GetMainCamera()->GetScreenPointRay(Input::GetCursorPos());// Camera.main.ScreenPointToRay(Input.mousePosition);
    RaycastHit raycastHit;

    Transform* transform = GetTransform();
    glm::vec3 position = transform->GetGlobalPosition();

    if (!reachEnd) {
        position = transform->GetGlobalPosition();

        float dist = glm::distance(position, _waypoint);

        Tilemap::HexagonalTile* tile = _tilemap->GetTile(_tilemap->ConvertToTilemapPosition(vec2(_waypoint.x, _waypoint.z)));

        if (dist <= nextWaypointDistance) {
            if (_path->IsOnEnd())
            {
                reachEnd = true;
                SPDLOG_INFO("On end of path");

                _audioComponent->Stop();
                OnFinishMoving(GetGameObject(), destinatedTile);
            }
            else
            {
                position = _waypoint;
                transform->SetGlobalPosition(position);
                _waypoint = _path->Next();
                _waypoint.y += _heightOverSurface;

                vec3 direction = _waypoint - position;
                direction.y = 0.0f;
                float angle = glm::degrees(glm::acos(direction.x / glm::length(direction)));
                if (direction.z > 0.0f)
                {
                    angle = -angle;
                }
                GetTransform()->SetGlobalRotation(glm::vec3(0.0f, angle + 90, 0.0f));
            }
        }
        else {
            //transform->SetGlobalPosition(glm::vec3(glm::mix(position, tempWaypointPos, 0.5f)) + vec3(0.0f, 0.5f, 0.0f));
            float walk_dist = Time::GetDeltaTime() * speed;
            transform->Translate(glm::normalize(_waypoint - position) * walk_dist);
        }
    }
    else if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive)
    {
        _mutexCheckingPath.lock();

        if (_showedPathEnabled)
        {
            _showedPathEnabled = false;


            _pointedTile = _checkedTile;
            _playerDestinationMarker->GetTransform()->SetGlobalPosition(
                _pointedTile->GetTransform()->GetGlobalPosition() + vec3(0.0f, _destinationMarkerHeightOverSurface, 0.0f));
            _playerDestinationMarker->SetActive(true);
            _playerWrongDestinationMarker->SetActive(false);


            AStar::AStarPathfindingNode* pathPoint;
            if (_showedPathTiles.size())
            {
                size_t showedPathTilesSize = _showedPathTiles.size();
                for (size_t index = 0ull; index < showedPathTilesSize; ++index)
                {
                    _showedPathTiles[index]->DisableAffected();
                }
                _showedPathTiles.clear();
            }
            _showedPathTiles.reserve(_showedPath->Count());
            size_t size = _showedPath->Count();
            //SPDLOG_INFO("Path Count: {}", _showedPath->Count());

            //pathPoint = _showedPath->Next();
            for (size_t pathIndex = 0ull; pathIndex < size; ++pathIndex)
            {
                pathPoint = _showedPath->Next();
                GameObject* tileObj = pathPoint->GetGameObject();
                HexTile* hexTile = tileObj->GetComponent<HexTile>();
                if (hexTile)
                {
                    hexTile->EnableAffected();
                    _showedPathTiles.push_back(hexTile);
                }
            }
            delete _showedPath;
            _showedPath = nullptr;
            //_showedPathTiles.shrink_to_fit();
        }
        else if (_showedPathDisabled)
        {
            _showedPathDisabled = false;

            _pointedTile = nullptr;

            _playerDestinationMarker->GetTransform()->SetGlobalPosition(
                _checkedTile->GetTransform()->GetGlobalPosition() + vec3(0.0f, _destinationMarkerHeightOverSurface, 0.0f));
            _playerDestinationMarker->SetActive(true);
            _playerWrongDestinationMarker->SetActive(true);

            if (_showedPathTiles.size())
            {
                size_t showedPathTilesSize = _showedPathTiles.size();
                for (size_t index = 0ull; index < showedPathTilesSize; ++index)
                {
                    _showedPathTiles[index]->DisableAffected();
                }
                _showedPathTiles.clear();
            }
        }

        _mutexCheckingPath.unlock();

        if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        {
            //SPDLOG_INFO("COL_ID: {}", raycastHit.collider->colliderId);
            HexTile* hexTile = raycastHit.collider->GetGameObject()->GetComponent<HexTile>();

            if (hexTile != _player->CurrTile)
            {
                if (hexTile != _pointedTile)
                {
                    if (hexTile)
                    {
                        MapHexTile* mapHexTile = hexTile->GetMapHexTile();

                        if (
                            mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain 
                            && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation 
                                && hexTile->currCooldown > 0.0f) 
                            )
                            //&& !hexTile->isFighting)
                        {
                            CheckDestination(hexTile);
                            //_pointedTile = hexTile;
                            //_playerDestinationMarker->GetTransform()->SetGlobalPosition(
                            //                _pointedTile->GetTransform()->GetGlobalPosition() + vec3(0.0f, _destinationMarkerHeightOverSurface, 0.0f));
                            //_playerDestinationMarker->SetActive(true);
                        }
                        else
                        {
                            _pointedTile = nullptr;
                            _playerDestinationMarker->SetActive(false);
                            _playerWrongDestinationMarker->SetActive(false);
                            if (_showedPathTiles.size())
                            {
                                size_t showedPathTilesSize = _showedPathTiles.size();
                                for (size_t index = 0ull; index < showedPathTilesSize; ++index)
                                {
                                    _showedPathTiles[index]->DisableAffected();
                                }
                                _showedPathTiles.clear();
                            }
                        }
                    }
                    else
                    {
                        _pointedTile = nullptr;
                        _playerDestinationMarker->SetActive(false);
                        _playerWrongDestinationMarker->SetActive(false);
                        if (_showedPathTiles.size())
                        {
                            size_t showedPathTilesSize = _showedPathTiles.size();
                            for (size_t index = 0ull; index < showedPathTilesSize; ++index)
                            {
                                _showedPathTiles[index]->DisableAffected();
                            }
                            _showedPathTiles.clear();
                        }
                    }
                }
            }
            else
            {
                _pointedTile = nullptr;
                _playerDestinationMarker->SetActive(false);
                _playerWrongDestinationMarker->SetActive(false);
                if (_showedPathTiles.size())
                {
                    size_t showedPathTilesSize = _showedPathTiles.size();
                    for (size_t index = 0ull; index < showedPathTilesSize; ++index)
                    {
                        _showedPathTiles[index]->DisableAffected();
                    }
                    _showedPathTiles.clear();
                }
            }

            if (Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::LEFT))
            {
                if (_pointedTile)
                {
                    SetDestination(_pointedTile);
                    _playerDestinationMarker->SetActive(false);
                    _playerWrongDestinationMarker->SetActive(false);
                    if (_showedPathTiles.size())
                    {
                        size_t showedPathTilesSize = _showedPathTiles.size();
                        for (size_t index = 0ull; index < showedPathTilesSize; ++index)
                        {
                            _showedPathTiles[index]->DisableAffected();
                        }
                        _showedPathTiles.clear();
                    }
                }
                else if (_playerWrongDestinationMarker->GetActive())
                {
                    _audioComponent->SetAudio(_soundWrongDestination);
                    _audioComponent->Play();
                }
            }
        }
        else
        {
            _pointedTile = nullptr;
            _playerDestinationMarker->SetActive(false);
            _playerWrongDestinationMarker->SetActive(false);
            if (_showedPathTiles.size())
            {
                size_t showedPathTilesSize = _showedPathTiles.size();
                for (size_t index = 0ull; index < showedPathTilesSize; ++index)
                {
                    _showedPathTiles[index]->DisableAffected();
                }
                _showedPathTiles.clear();
            }
        }
    }
}   


void PlayerMovement::OnPathComplete(const AStarPath& p) {
    _mutexPath.lock();

    if (_path) {
        delete _path;
        _path = nullptr;
    }

    _path = new AStarPath(p);

    destination = tempDest;
    destinatedTile = tempDestTile;
    _waypoint = _path->Next();
    _waypoint.y += _heightOverSurface;

    reachEnd = false;

    OnStartMoving.Invoke(GetGameObject(), destinatedTile);

    _audioComponent->SetAudio(_engineSound);
    _audioComponent->Play();

    tempDestTile = nullptr;

    _mutexPath.unlock();
}

void PlayerMovement::OnPathFailure() {
    _mutexPath.lock();
    EndMoveAction();
    OnFindPathError(GetGameObject(), tempDestTile);
    _mutexPath.unlock();
}

void PlayerMovement::EndMoveAction() {
    reachEnd = true;
    //alreadyChecked = false;
    if (_path) {
        delete _path;
        _path = nullptr;
    }
}

void PlayerMovement::MoveAndSetDestination(HexTile* dest) {
    if (reachEnd && !GameManager::instance->minigameActive)
    {
        MapHexTile* mapHexTile = dest->GetMapHexTile();
        if (mapHexTile->type != Generation::MapHexTile::HexTileType::Mountain 
            && !(mapHexTile->type == Generation::MapHexTile::HexTileType::RadioStation 
                && dest->currCooldown > 0.0f) 
            )
            //&& !dest->isFighting)
        {
            SetDestination(dest);
        }
    }
}

void PlayerMovement::StartUp(HexTile* startUpTile)
{
    vec3 startUpPosition = startUpTile->GetTransform()->GetGlobalPosition();
    GetTransform()->SetGlobalPosition(startUpPosition);
    //destinatedTile = dest;
    //destination = destPos;
    GameManager::instance->gameStarted = true;
    OnStartMoving.Invoke(GetGameObject(), startUpTile);
    //Player* player = GetGameObject()->GetComponent<Player>();
    //player->CurrTile = dest;
    //dest->StartTakingOver(player);
    OnFinishMoving(GetGameObject(), startUpTile);
}

bool PlayerMovement::InCircle(glm::vec3 point) {
    glm::vec3 position = GetTransform()->GetGlobalPosition();
    return (point.x - position.x) * (point.x - position.x) + (point.z - position.z) * (point.z - position.z) < radius * radius;
}

void PlayerMovement::SetDestination(HexTile* dest) {
    glm::vec3 destPos = dest->GetTransform()->GetGlobalPosition() + glm::vec3(0.0f, _heightOverSurface, 0.0f);
    
    //if (InCircle(destPos))
    //{
    if (!_path || _path->IsOnEnd())
    {
        tempDestTile = dest;

        //if (_info)
        //{
        //    delete _info;
        //}
        //
        //_info = new AStarPathfindingInfo(AStarPathfinder::FindPath(GetTransform()->GetGlobalPosition(), dest->GetTransform()->GetGlobalPosition(), maxSteps,
        //    [&](const AStarPath& path) { OnPathComplete(path); }, [&]() { OnPathFailure(); }));
        _info = AStarPathfinder::FindPath(GetTransform()->GetGlobalPosition(), dest->GetTransform()->GetGlobalPosition(), maxSteps,
                [&](const AStarPath& path) { OnPathComplete(path); }, [&]() { OnPathFailure(); });

    }
    //}
}

void PlayerMovement::CheckDestination(HexTile* dest)
{
    //SPDLOG_INFO("CheckingDestination");
    if (!_checkingInfo.IsSearching())
    {
        _checkedTile = dest;
        _checkingInfo = AStarPathfinder::FindNodePath(GetTransform()->GetGlobalPosition(), dest->GetTransform()->GetGlobalPosition(), maxSteps,
            [&](const AStarNodePath& path) { OnCheckPathComplete(path); }, [&]() { OnCheckPathFailure(); });
    }
}

void PlayerMovement::OnCheckPathComplete(const AStar::AStarNodePath& p)
{
    _mutexCheckingPath.lock();

    _showedPath = new AStar::AStarNodePath(p);
    _showedPathEnabled = true;
    
    _mutexCheckingPath.unlock();
}

void PlayerMovement::OnCheckPathFailure()
{
    _mutexCheckingPath.lock();
    _showedPathDisabled = true;
    _mutexCheckingPath.unlock();
}

void PlayerMovement::DrawCircle(int steps, float radius) {
    //circleRenderer->positionCount = steps;

    for (int i = 0; i < steps; ++i)
    {
        float p = (float)i / steps;
        float radian = p * 2 * M_PI;

        float x = glm::cos(radian) * radius;
        float z = glm::sin(radian) * radius;

        glm::vec3 position = GetTransform()->GetGlobalPosition();

        //circleRenderer->SetPosition(i, glm::vec3(position.x + x, position.y, position.z + z));
    }
}

void PlayerMovement::DrawLine(glm::vec3 startPos, glm::vec3 endPos) {
    //lineRenderer->positionCount = 2;
    //lineRenderer->SetPosition(0, startPos);
    //lineRenderer->SetPosition(1, endPos);
}

YAML::Node PlayerMovement::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "PlayerMovement";
    node["speed"] = speed;
    node["maxSteps"] = maxSteps;
    node["nextWaypointDistance"] = nextWaypointDistance;
    node["heightOverSurface"] = _heightOverSurface;

    //if (_prefabPlayerDestinationMarker != nullptr)
    //{
    //    node["prefabPlayerDestinationMarker"] = SceneManager::GetPrefabSaveIdx(_prefabPlayerDestinationMarker->GetId());
    //}
    node["destinationMarkerHeightOverSurface"] = _destinationMarkerHeightOverSurface;

    node["engineSound"] = SceneManager::GetAudioSaveIdx(_engineSound);

    return node;
}

bool PlayerMovement::Deserialize(const YAML::Node& node)
{
    if (!node["speed"] || !node["maxSteps"] || !node["nextWaypointDistance"] ||
        !node["heightOverSurface"] || !node["engineSound"] || !Component::Deserialize(node))
        return false;

    speed = node["speed"].as<float>();
    maxSteps = node["maxSteps"].as<size_t>();
    nextWaypointDistance = node["nextWaypointDistance"].as<float>();
    _heightOverSurface = node["heightOverSurface"].as<float>();

    if (node["prefabPlayerDestinationMarker"])
    {
        //_prefabPlayerDestinationMarker = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["prefabPlayerDestinationMarker"].as<size_t>()));
        _playerDestinationMarker = SceneManager::CreateGameObject(
            PrefabManager::GetPrefab(SceneManager::GetPrefab(node["prefabPlayerDestinationMarker"].as<size_t>())));
        _playerWrongDestinationMarker = _playerDestinationMarker->GetTransform()->GetChildAt(0ull)->GetGameObject();

        //_playerDestinationMarker = destinationMarker->GetTransform();
    }
    _destinationMarkerHeightOverSurface = node["destinationMarkerHeightOverSurface"].as<float>();

    // TODO: Nie zaczytaly sie sprite audio prefabs
    _engineSound = SceneManager::GetAudio(node["engineSound"].as<size_t>());
    _soundWrongDestination = SceneManager::GetAudio(node["wrongDestinationSound"].as<size_t>());

    return true;
}

#if _DEBUG
void PlayerMovement::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Player Movement##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}
#endif