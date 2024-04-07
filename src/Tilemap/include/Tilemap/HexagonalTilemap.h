#ifndef _HEXAGONAL_TILEMAP_H_
#define _HEXAGONAL_TILEMAP_H_

namespace Tilemap
{
	class HexagonalTile
	{

	};

	class HexagonalTilemap
	{
		HexagonalTile** _tilemap;
		unsigned int _width;
		unsigned int _height;
		float _distanceBetweenTiles;
		float _edgeLength;
		glm::ivec2 _toCenter;
		glm::ivec2 _leftBottomPosition;
		glm::ivec2 _rightTopPosition;


	public:
		HexagonalTilemap(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition, float length, bool isDistanceBetweenTiles);
		~HexagonalTilemap();

		void Resize(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition);

		inline unsigned int GetWidth() const;
		inline unsigned int GetHeight() const;
		inline glm::ivec2 GetLeftBottomPosition() const;
		inline glm::ivec2 GetRightTopPosition() const;

		void SetTile(glm::ivec4 position, Twin2Engine::Core::GameObject gameObject);
		void RemoveTile(glm::ivec4 position);
	};
}

#endif // !_HEXAGONAL_TILEMAP_H_