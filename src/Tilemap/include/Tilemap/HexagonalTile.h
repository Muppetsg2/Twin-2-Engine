#ifndef _HEXAGONAL_TILE_H_
#define _HEXAGONAL_TILE_H_

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

		inline HexagonalTilemap* GetTilemap() const;
		inline glm::ivec2 GetPosition() const;

		Twin2Engine::Core::GameObject* GetGameObject() const;
		void SetGameObject(Twin2Engine::Core::GameObject* gameObject);

		inline HexagonalTile** GetAdjacentTiles() const;
		inline Twin2Engine::Core::GameObject** GetAdjacentGameObjects() const;
	};
}

#endif