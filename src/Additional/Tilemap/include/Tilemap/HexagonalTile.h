#pragma once

#include <core/GameObject.h>
#include <core/Transform.h>

#include <Tilemap/HexagonalTilemap.h>

namespace Tilemap
{
	class HexagonalTilemap;

	class HexagonalTile
	{
		friend class HexagonalTilemap;

		Twin2Engine::Core::GameObject* _gameObject;

		HexagonalTilemap* _tilemap;

		glm::ivec2 _position;


		static const glm::ivec2 adjacentDirectionsEvenY[6];
		static const glm::ivec2 adjacentDirectionsOddY[6];

		void SetTilemap(HexagonalTilemap* tilemap);
		void SetPosition(const glm::ivec2& position);

	public:
		HexagonalTile();
		HexagonalTile(HexagonalTilemap* tilemap, const glm::ivec2& position);
		~HexagonalTile();

		inline HexagonalTilemap* GetTilemap() const;
		inline glm::ivec2 GetPosition() const;

		Twin2Engine::Core::GameObject* GetGameObject() const;
		void SetGameObject(Twin2Engine::Core::GameObject* gameObject);

		inline void GetAdjacentTiles(HexagonalTile** out_AdjacentTiles) const;
		void GetAdjacentGameObjects(Twin2Engine::Core::GameObject** out_adjacentGameObjects) const;
	};
}