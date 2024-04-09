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

		for (int j = 0; j < _height; j++)
		{
			_tilemap[i][j].SetTilemap(this);
			_tilemap[i][j].SetPosition(glm::ivec2(i + _leftBottomPosition.x, j + _leftBottomPosition.y));
		}
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
	//glm::ivec2 oldToCenter = _toCenter;
	glm::ivec2 oldLeftBottomPosition = _leftBottomPosition;
	glm::ivec2 oldRightTopPosition = _rightTopPosition;

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

		for (int j = 0; j < _height; j++)
		{
			_tilemap[i][j].SetTilemap(this);
			_tilemap[i][j].SetPosition(glm::ivec2(i + _leftBottomPosition.x, j + _leftBottomPosition.y));
		}
	}

	// Coping old tilemap
	glm::ivec2 srcBegin = _leftBottomPosition - oldLeftBottomPosition;
	glm::ivec2 dstBegin = srcBegin;
	if (srcBegin.x < 0)
	{
		srcBegin.x = 0;
	}
	if (srcBegin.y < 0)
	{
		srcBegin.y = 0;
	}

	if (dstBegin.x > 0)
	{
		dstBegin.x = 0;
	}
	if (dstBegin.y > 0)
	{
		dstBegin.y = 0;
	}
	dstBegin *= -1;

	int minRTX = (oldRightTopPosition.x < _rightTopPosition.x) ? oldRightTopPosition.x : _rightTopPosition.x;
	int minRTY = (oldRightTopPosition.y < _rightTopPosition.y) ? oldRightTopPosition.y : _rightTopPosition.y;

	int copyWidth = minRTX - srcBegin.x;
	int bytesToCopy = (minRTY - srcBegin.y) * sizeof(HexagonalTile);

	for (int x = 0; x < copyWidth; x++)
	{
		std::memmove(_tilemap[dstBegin.x + x], oldTilemap[srcBegin.x + x], bytesToCopy);
	}


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


void HexagonalTilemap::SetTile(const glm::ivec2& position, Twin2Engine::Core::GameObject* gameObject)
{
	if (position.x > _leftBottomPosition.x && position.x < _rightTopPosition.x && position.y > _leftBottomPosition.y && position.y < _rightTopPosition.y)
	{
		_tilemap[position.x + _toCenter.x][position.y + _toCenter.y].SetGameObject(gameObject);
	}
	else
	{
		glm::vec2 newLeftBottomPosition(_leftBottomPosition);
		glm::vec2 newRigthTopPosition(_rightTopPosition);

		if (position.x < _leftBottomPosition.x)
		{
			newLeftBottomPosition.x = position.x;
		}
		if (position.y < _leftBottomPosition.y)
		{
			newLeftBottomPosition.y = position.y;
		}
		if (position.x > _rightTopPosition.x)
		{
			newRigthTopPosition.x = position.x;
		}
		if (position.y > _rightTopPosition.y)
		{
			newRigthTopPosition.y = position.y;
		}

		Resize(newLeftBottomPosition, newRigthTopPosition);

		_tilemap[position.x + _toCenter.x][position.y + _toCenter.y].SetGameObject(gameObject);
	}

	gameObject->GetTransform()->SetLocalPosition(glm::vec3(position.x * _distanceBetweenTiles, position.y * _distanceBetweenTiles, 0.0f));
}


inline HexagonalTile* HexagonalTilemap::GetTile(const glm::ivec2& position) const
{
	return &_tilemap[position.x + _toCenter.x][position.y + _toCenter.y];
}

void HexagonalTilemap::RemoveTile(const glm::ivec2& position)
{

}

void HexagonalTilemap::Fill(const glm::ivec2& position, Twin2Engine::Core::GameObject* gameObject)
{
	// Check if the given position is within the tilemap bounds
	if (position.x < _leftBottomPosition.x || position.x > _rightTopPosition.x ||
		position.y < _leftBottomPosition.y || position.y > _rightTopPosition.y) 
	{
		std::cerr << "Error: Position is out of bounds.\n";
		return;
	}

	// Perform a breadth-first search to fill empty tiles within the bounds
	std::queue<glm::ivec2> queue;
	std::unordered_set<glm::ivec2, std::hash<glm::ivec2>> visited;

	// Add the initial position to the queue
	queue.push(position);
	//visited.insert(position);

	// Define the neighboring directions for hexagonal tiles
	std::vector<glm::ivec2> directions = {
		glm::ivec2(1, 0), glm::ivec2(-1, 0), glm::ivec2(0, 1),
		glm::ivec2(0, -1), glm::ivec2(-1, -1), glm::ivec2(1, 1)
	};

	while (!queue.empty()) 
	{
		glm::ivec2 currentPos = queue.front();
		queue.pop();

		// Get the tile at the current position
		HexagonalTile* currentTile = GetTile(currentPos);

		// Fill the tile if it's empty
		if (currentTile && !currentTile->GetGameObject())
		{
			currentTile->SetGameObject(gameObject);

			// Add neighboring positions to the queue if they're within bounds and not visited yet
			for (const auto& dir : directions) 
			{
				glm::ivec2 neighborPos = currentPos + dir;
				if (neighborPos.x >= _leftBottomPosition.x && neighborPos.x <= _rightTopPosition.x &&
					neighborPos.y >= _leftBottomPosition.y && neighborPos.y <= _rightTopPosition.y &&
					GetTile(neighborPos)->GetGameObject() == nullptr)
				//	visited.find(neighborPos) == visited.end()) 
				{
					queue.push(neighborPos);

					// Kopiowanie gameobjectu
					Twin2Engine::Core::GameObject* instantiatedGameObject = Twin2Engine::Core::GameObject::Instatiate(gameObject);
					GetTile(neighborPos)->SetGameObject(instantiatedGameObject);
					instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3(neighborPos.x * _distanceBetweenTiles, neighborPos.y * _distanceBetweenTiles, 0.0f));
					SPDLOG_ERROR("Nie zaimplementowano kopiowania gameobjectu");
					//visited.insert(neighborPos);
				}
			}
		}
	}
}

inline void HexagonalTilemap::SetDistanceBetweenTiles(float distanceBetweenTiles)
{
	_distanceBetweenTiles = distanceBetweenTiles;

	_edgeLength = _distanceBetweenTiles / glm::sqrt(3.f);
}

inline float HexagonalTilemap::GetDistanceBetweenTiles() const
{
	return _distanceBetweenTiles;
}

inline void HexagonalTilemap::SetDistanceBetweenTiles(float edgeLength)
{
	_edgeLength = edgeLength;

	_distanceBetweenTiles = 0.5f * _edgeLength * glm::sqrt(3.f);
}

inline float HexagonalTilemap::GetDistanceBetweenTiles() const
{
	return _edgeLength;
}