#pragma once

#include <core/Component.h>
#include <core/Transform.h>
#include <core/GameObject.h>
#include <Generation/MapHexTile.h>
#include <tools/EventHandler.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Tools;

class Path;
class LineRenderer;
class Seeker : public Component {
};

class PlayerMovement : public Component {
	private:
		Generation::MapHexTile* destinatedTile = nullptr;

		glm::vec3 tempDest;
		Generation::MapHexTile* tempDestTile;

		bool InCircle(glm::vec3 point);
		void SetDestination(Generation::MapHexTile* dest);
		void DrawCircle(int steps, float radius);
		void DrawLine(glm::vec3 startPos, glm::vec3 endPos);
		void OnDrawGizmos();
			
	public:
		//Moving

		glm::vec3 destination;
		float speed = 20.0f;
		int maxSteps = 5;
		float nextWaypointDistance = 0.001f;
		

		//Circle

		LineRenderer* circleRenderer;
		float radius = 2.0f;
		int steps = 20;

		//Line

		LineRenderer* lineRenderer;

		Seeker* seeker;
		Path* path;
		int currWaypoint = 0;
		bool reachEnd = true;
		EventHandler<GameObject*, Generation::MapHexTile*> OnFindPathError;
		EventHandler<GameObject*, Generation::MapHexTile*> OnStartMoving;
		EventHandler<GameObject*, Generation::MapHexTile*> OnFinishMoving;


		virtual void Initialize() override;
		virtual void Update() override;

		void OnPathComplete(Path* p);
		void EndMoveAction();
		void MoveAndSetDestination(Generation::MapHexTile* dest);
};