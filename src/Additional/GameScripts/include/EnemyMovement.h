#pragma once

#pragma once


#include <core/Component.h>
#include <core/Transform.h>
#include <core/GameObject.h>

#include <manager/SceneManager.h>

#include <Generation/MapHexTile.h>
#include <AreaTaking/HexTile.h>
#include <tools/EventHandler.h>


using namespace Twin2Engine::Core;
using namespace Twin2Engine::Tools;

class Path;
class LineRenderer;
class Seeker : public Component {
};

namespace AStar
{
	class AStarPath;
	class AStarPathfinder;
}

class EnemyMovement : public Component {
private:
	HexTile* destinatedTile = nullptr;

	glm::vec3 tempDest;
	HexTile* tempDestTile;

	AStar::AStarPath* _path = nullptr;
	Tilemap::HexagonalTilemap* _tilemap = nullptr;
	//int currWaypoint = 0;
	bool reachEnd = true;
	glm::vec3 _waypoint;

	bool InCircle(glm::vec3 point);
	void SetDestination(HexTile* dest);

public:
	//Moving

	glm::vec3 destination;
	float speed = 0.7f;
	int maxSteps = 5;
	float nextWaypointDistance = 0.001f;


	//Circle

	LineRenderer* circleRenderer;
	float radius = 3.0f;
	int steps = 20;

	//Line

	LineRenderer* lineRenderer;

	//Seeker* seeker;
	//Path* path;
	EventHandler<GameObject*, HexTile*> OnFindPathError;
	EventHandler<GameObject*, HexTile*> OnStartMoving;
	EventHandler<GameObject*, HexTile*> OnFinishMoving;


	virtual void Initialize() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	void OnPathComplete(const AStar::AStarPath& p);
	void OnPathFailure();
	void EndMoveAction();

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};