#ifndef _HEXAGONAL_TILE_H_
#define _HEXAGONAL_TILE_H_

#include <Tilemap/HexagonalTilemap.h>

namespace Tilemap
{
	class HexagonalTile
	{
		friend class HexagonalTilemap;

		Twin2Engine::Core::GameObject* _gameObject;

		HexagonalTilemap* _tilemap;

		glm::ivec2 _position;


		inline void SetTilemap(HexagonalTilemap* tilemap);
		inline void SetPosition(const glm::ivec2& position);

	public:
		HexagonalTile();
		HexagonalTile(HexagonalTilemap* tilemap, const glm::ivec2& position);

		inline HexagonalTilemap* GetTilemap() const;
		inline glm::ivec2 GetPosition() const;

		inline Twin2Engine::Core::GameObject* GetGameObject() const;
		inline void SetGameObject(Twin2Engine::Core::GameObject* gameObject);
	};
}

#endif