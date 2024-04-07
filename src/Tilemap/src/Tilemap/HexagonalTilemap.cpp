#include <Tilemap/HexagonalTilemap.h>

using namespace Tilemap;

HexagonalTilemap::HexagonalTilemap(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition, float length, bool isDistanceBetweenTiles)
{
	if (isDistanceBetweenTiles)
	{
		_distanceBetweenTiles = length;
		_edgeLength = length / glm::sqrt(3.f);
	}
	else
	{
		_edgeLength = length;
		_distanceBetweenTiles = 0.5f * length * glm::sqrt(3.f);
	}

	// Creating new tilemap
	if (leftBottomPosition.x < rightTopPosition.x)
	{
		_leftBottomPosition.x = leftBottomPosition.x;
		_rightTopPosition.x = rightTopPosition.x;
	}
	if (leftBottomPosition.y < rightTopPosition.y)
	{
		_leftBottomPosition.y = leftBottomPosition.y;
		_rightTopPosition.y = rightTopPosition.y;
	}

	_toCenter = -_leftBottomPosition;

	_width = _rightTopPosition.x - _leftBottomPosition.x + 1;
	_height = _rightTopPosition.y - _leftBottomPosition.x + 1;

	_tilemap = new HexagonalTile * [_width];

	for (int i = 0; i < _width; i++)
	{
		_tilemap[i] = new HexagonalTile[_height];
	}
}

HexagonalTilemap::~HexagonalTilemap()
{
	for (int i = 0; i < _width; i++)
	{
		delete[] _tilemap[i];
	}

	delete[] _tilemap;
}

void HexagonalTilemap::Resize(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition)
{
	HexagonalTile** oldTilemap = _tilemap;
	unsigned int oldWidth = _width;
	glm::ivec2 oldToCenter = _toCenter;

	// Creating new tilemap
	if (leftBottomPosition.x < rightTopPosition.x)
	{
		_leftBottomPosition.x = leftBottomPosition.x;
		_rightTopPosition.x = rightTopPosition.x;
	}
	if (leftBottomPosition.y < rightTopPosition.y)
	{
		_leftBottomPosition.y = leftBottomPosition.y;
		_rightTopPosition.y = rightTopPosition.y;
	}

	_toCenter = -_leftBottomPosition;

	_width = _rightTopPosition.x - _leftBottomPosition.x + 1;
	_height = _rightTopPosition.y - _leftBottomPosition.x + 1;

	_tilemap = new HexagonalTile * [_width];

	for (int i = 0; i < _width; i++)
	{
		_tilemap[i] = new HexagonalTile[_height];
	}

	// Coping old tilemap

	//for (int x = oldT)


	// Deleting old tilemap
	for (int i = 0; i < oldWidth; i++)
	{
		delete[] oldTilemap[i];
	}

	delete[] oldTilemap;
}


inline unsigned int HexagonalTilemap::GetWidth() const
{
	return _width;
}

inline unsigned int HexagonalTilemap::GetHeight() const
{
	return _height;
}

inline glm::ivec2 HexagonalTilemap::GetLeftBottomPosition() const
{
	return _leftBottomPosition;
}

inline glm::ivec2 HexagonalTilemap::GetRightTopPosition() const
{
	return _rightTopPosition;
}


void HexagonalTilemap::SetTile(glm::ivec4 position, Twin2Engine::Core::GameObject gameObject)
{

}

void HexagonalTilemap::RemoveTile(glm::ivec4 position)
{

}