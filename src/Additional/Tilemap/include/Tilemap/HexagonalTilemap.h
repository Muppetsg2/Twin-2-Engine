#ifndef _HEXAGONAL_TILEMAP_H_
#define _HEXAGONAL_TILEMAP_H_

#include <core/GameObject.h>
#include <core/Component.h>
#include <core/Prefab.h>
#include <core/Transform.h>
#include <manager/SceneManager.h>


namespace Generation
{
	class MapHexTile;
}
using Generation::MapHexTile;

namespace Tilemap
{
	class HexagonalTile;

	class HexagonalTilemap : public Twin2Engine::Core::Component
	{
		HexagonalTile*** _tilemap;
		unsigned int _width;
		unsigned int _height;
		float _distanceBetweenTiles;
		float _edgeLength;
		glm::ivec2 _leftBottomPosition;
		glm::ivec2 _rightTopPosition;
		glm::ivec2 _toCenter;


	public:
		//HexagonalTilemap(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition, float length, bool isDistanceBetweenTiles);
		HexagonalTilemap();
		~HexagonalTilemap();

		void Resize(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition);

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		glm::ivec2 GetLeftBottomPosition() const;
		glm::ivec2 GetRightTopPosition() const;

		inline HexagonalTile* GetTile(const glm::ivec2& position) const;
		//void SetTile(const glm::ivec2& position, Twin2Engine::Core::GameObject* gameObject);
		void SetTile(const glm::ivec2& position, Twin2Engine::Core::Prefab* prefab);
		void RemoveTile(const glm::ivec2& position);

		//void Fill(const glm::ivec2& position, Twin2Engine::Core::GameObject* gameObject);
		void Fill(const glm::ivec2& position, Twin2Engine::Core::Prefab* prefab);

		inline void SetDistanceBetweenTiles(float distanceBetweenTiles);
		inline float GetDistanceBetweenTiles() const;
		inline void SetEdgeLength(float edgeLength);
		inline float GetEdgeLength() const;

		glm::vec2 ConvertToRealPosition(const glm::ivec2& position) const;
		glm::vec2 ConvertToTilemapPosition(const glm::ivec2& position) const;

		static glm::ivec2 GetPositionInDirection(const glm::ivec2& position, unsigned int direction);

		virtual YAML::Node Serialize() const override;
	};
}
#include <Tilemap/HexagonalTile.h>

#include <Generation/MapHexTile.h>

#endif // !_HEXAGONAL_TILEMAP_H_