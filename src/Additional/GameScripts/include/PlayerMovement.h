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

class LineRenderer;
class Seeker : public Component {
};

namespace AStar
{
	class AStarPath;
	class AStarPathfinder;
	class AStarPathfindingInfo;
}

class Player;

class PlayerMovement : public Component {
	friend class Player;
	friend class GameManager;
	private:
		Player* _player;

		std::mutex _mutexPath;

		HexTile* _checkedTile = nullptr;
		Twin2Engine::Core::AudioComponent* _audioComponent = nullptr;
		size_t _engineSound = 0;
		size_t _soundWrongDestination = 0;


		std::mutex _mutexCheckingPath;
		bool _showedPathEnabled;
		bool _showedPathDisabled;

		AStar::AStarNodePath* _showedPath = nullptr;


		HexTile* destinatedTile = nullptr;


		HexTile* tempDestTile;
		glm::vec3 tempDest;

		AStar::AStarPathfindingInfo _info;
		AStar::AStarPathfindingInfo _checkingInfo;

		AStar::AStarPath* _path = nullptr;
		Tilemap::HexagonalTilemap* _tilemap = nullptr;
		//int currWaypoint = 0;
		glm::vec3 _waypoint;
		float _heightOverSurface = 0.0f;

		bool InCircle(glm::vec3 point);
		void SetDestination(HexTile* dest);
		void CheckDestination(HexTile* dest);
		void OnCheckPathComplete(const AStar::AStarNodePath& p);
		void OnCheckPathFailure();
		void DrawCircle(int steps, float radius);
		void DrawLine(glm::vec3 startPos, glm::vec3 endPos);
			
	public:
		std::vector<HexTile*> _showedPathTiles = std::vector<HexTile*>();
		Twin2Engine::Core::GameObject* _playerDestinationMarker = nullptr;
		Twin2Engine::Core::GameObject* _playerWrongDestinationMarker = nullptr;
		HexTile* _pointedTile = nullptr;
		float _destinationMarkerHeightOverSurface = 0.5f;
		//Moving

		bool reachEnd = true;
		glm::vec3 destination;
		float speed = 0.7f;
		size_t originMaxSteps = 5;
		size_t maxSteps = 5;
		float nextWaypointDistance = 0.05f;
		

		//Circle

		LineRenderer* circleRenderer;
		float radius = 3.0f;
		size_t steps = 20;

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
		void StartUp(HexTile* startUpTile);

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