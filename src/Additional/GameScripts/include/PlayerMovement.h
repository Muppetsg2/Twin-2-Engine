#pragma once

#include <core/Component.h>
#include <core/Transform.h>
#include <core/GameObject.h>

#include <manager/SceneManager.h>

#include <Generation/MapHexTile.h>
#include <AreaTaking/HexTile.h>
#include <tools/EventHandler.h>

#include <AstarPathfinding/AStarPathfinder.h>
#include <GameManager.h>

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
	class AStarPathfindingInfo;
}

class PlayerMovement : public Component {
	private:
		HexTile* destinatedTile = nullptr;

		glm::vec3 tempDest;
		HexTile* tempDestTile;

		AStar::AStarPathfindingInfo _info;

		AStar::AStarPath* _path = nullptr;
		Tilemap::HexagonalTilemap* _tilemap = nullptr;
		//int currWaypoint = 0;
		glm::vec3 _waypoint;
		float _heightOverSurface = 0.0f;

		bool InCircle(glm::vec3 point);
		void SetDestination(HexTile* dest);
		void DrawCircle(int steps, float radius);
		void DrawLine(glm::vec3 startPos, glm::vec3 endPos);
			
	public:
		//Moving

		bool reachEnd = true;
		glm::vec3 destination;
		float speed = 0.7f;
		int maxSteps = 5;
		float nextWaypointDistance = 0.05f;
		

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
		void MoveAndSetDestination(HexTile* dest);

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		virtual void DrawEditor() override;
#endif
};