#pragma once

// CORE
#include <core/Component.h>
#include <core/Transform.h>
#include <core/GameObject.h>
#include <manager/SceneManager.h>
#include <tools/EventHandler.h>

#include <GameManager.h>

// MAP
#include <Generation/MapHexTile.h>
#include <AreaTaking/HexTile.h>

// ENEMY
//#include <Enemy.h>

// PATH FINDING
#include <AstarPathfinding/AStarPath.h>
#include <AstarPathfinding/AStarPathfinder.h>

//using namespace Twin2Engine::Core;
//using namespace Twin2Engine::Tools;

namespace AStar
{
	class AStarPath;
	class AStarPathfinder;
}
class Enemy;

class EnemyMovement : public Twin2Engine::Core::Component {
private:
	friend class Enemy;
	HexTile* destinatedTile = nullptr;

	std::mutex _mutexPath;

	glm::vec3 tempDest;
	HexTile* tempDestTile;

	AStar::AStarPathfindingInfo _info;
	AStar::AStarPath* _path = nullptr;
	Tilemap::HexagonalTilemap* _tilemap = nullptr;
	float _heightOverSurface = 0.1f;
	//int currWaypoint = 0;
	bool reachEnd = true;
	glm::vec3 _waypoint;

	bool InCircle(glm::vec3 point);
	void SetDestination(HexTile* dest);
	void OnPathComplete(const AStar::AStarPath& p);
	void OnPathFailure();
	void EndMoveAction();

public:
	//Moving

	glm::vec3 destination;
	float speed = 0.7f;
	size_t maxSteps = 5;
	float nextWaypointDistance = 0.05f;


	float radius = 3.0f;
	size_t steps = 20;


	Twin2Engine::Tools::EventHandler<Twin2Engine::Core::GameObject*, HexTile*> OnFindPathError;
	Twin2Engine::Tools::EventHandler<Twin2Engine::Core::GameObject*, HexTile*> OnStartMoving;
	Twin2Engine::Tools::EventHandler<Twin2Engine::Core::GameObject*, HexTile*> OnFinishMoving;


	virtual void Initialize() override;
	virtual void Update() override;
	virtual void OnDestroy() override;


	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};