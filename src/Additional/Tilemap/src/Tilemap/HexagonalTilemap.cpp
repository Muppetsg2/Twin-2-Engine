#include <Tilemap/HexagonalTilemap.h>

#include <tools/YamlConverters.h>

constexpr const float SQRT_3 = 1.7320508075688772935274463415059f;

using namespace Tilemap;

/*
HexagonalTilemap::HexagonalTilemap()
{
	_leftBottomPosition = glm::ivec2(0, 0);
	_rightTopPosition = glm::ivec2(0, 0);
	_toCenter = glm::ivec2(0, 0);

	_width = 1;
	_height = 1;
	_distanceBetweenTiles = 1.0f;
	_edgeLength = _distanceBetweenTiles / glm::sqrt(3.f);

	_tilemap = new HexagonalTile** [1];
	_tilemap[0] = new HexagonalTile*[1];
	_tilemap[0][0] = new HexagonalTile();
	_tilemap[0][0]->SetTilemap(this);
	_tilemap[0][0]->SetPosition(glm::ivec2(0, 0));
}
*/
//HexagonalTilemap::HexagonalTilemap(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition, float length, bool isDistanceBetweenTiles)
//{
//	if (isDistanceBetweenTiles)
//	{
//		_distanceBetweenTiles = length;
//		_edgeLength = length / glm::sqrt(3.f);
//	}
//	else
//	{
//		_edgeLength = length;
//		_distanceBetweenTiles = 0.5f * length * glm::sqrt(3.f);
//	}
//
//	// Creating new tilemap
//	if (leftBottomPosition.x < rightTopPosition.x)
//	{
//		_leftBottomPosition.x = leftBottomPosition.x;
//		_rightTopPosition.x = rightTopPosition.x;
//	}
//	if (leftBottomPosition.y < rightTopPosition.y)
//	{
//		_leftBottomPosition.y = leftBottomPosition.y;
//		_rightTopPosition.y = rightTopPosition.y;
//	}
//
//	_toCenter = -_leftBottomPosition;
//
//	_width = _rightTopPosition.x - _leftBottomPosition.x + 1;
//	_height = _rightTopPosition.y - _leftBottomPosition.x + 1;
//
//	_tilemap = new HexagonalTile * [_width];
//
//	for (int i = 0; i < _width; i++)
//	{
//		_tilemap[i] = new HexagonalTile[_height];
//
//		for (int j = 0; j < _height; j++)
//		{
//			_tilemap[i][j].SetTilemap(this);
//			_tilemap[i][j].SetPosition(glm::ivec2(i + _leftBottomPosition.x, j + _leftBottomPosition.y));
//		}
//	}
//}

/*
HexagonalTilemap::~HexagonalTilemap()
{
	for (int i = 0; i < _width; i++)
	{
		delete[] _tilemap[i];
	}

	delete[] _tilemap;

	_leftBottomPosition = glm::ivec2(0, 0);
	_rightTopPosition = glm::ivec2(0, 0);
	_toCenter = glm::ivec2(0, 0);

	_width = 1;
	_height = 1;
	_distanceBetweenTiles = 1.0f;
	_edgeLength = _distanceBetweenTiles / glm::sqrt(3.f);

	_tilemap = new HexagonalTile * *[1];
	_tilemap[0] = new HexagonalTile * [1];
	_tilemap[0][0] = new HexagonalTile();
	_tilemap[0][0]->SetTilemap(this);
	_tilemap[0][0]->SetPosition(glm::ivec2(0, 0));
}
*/

void HexagonalTilemap::Resize(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition)
{
	if (!_initialized) {
		_leftBottomPosition = leftBottomPosition;
		_rightTopPosition = rightTopPosition;
		return;
	}

	HexagonalTile*** oldTilemap = _tilemap;
	unsigned int oldWidth = _width;
	unsigned int oldHeight = _height;
	glm::ivec2 oldToCenter = _toCenter;
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

	_tilemap = new HexagonalTile**[_width];
	for (int i = 0; i < _width; i++)
	{
		_tilemap[i] = new HexagonalTile * [_height](nullptr);
	}

	// Coping old tilemap
	if (oldLeftBottomPosition.x <= _rightTopPosition.x &&
		oldLeftBottomPosition.y <= _rightTopPosition.y &&
		oldRightTopPosition.x >= _leftBottomPosition.x &&
		oldRightTopPosition.y >= _leftBottomPosition.y)
	{
		glm::ivec2 beginPos((_leftBottomPosition.x > oldLeftBottomPosition.x) ? _leftBottomPosition.x : oldLeftBottomPosition.x,
			(_leftBottomPosition.y > oldLeftBottomPosition.y) ? _leftBottomPosition.y : oldLeftBottomPosition.y);
		glm::ivec2 endPos((_rightTopPosition.x < oldRightTopPosition.x) ? _rightTopPosition.x : oldRightTopPosition.x,
			(_rightTopPosition.y < oldRightTopPosition.y) ? _rightTopPosition.y : oldRightTopPosition.y);

		int copyWidth = endPos.x - beginPos.x + 1;
		int copyHeight = endPos.y - beginPos.y + 1;
		int bytesToCopy = (endPos.y - beginPos.y + 1) * sizeof(HexagonalTile);
		glm::ivec2 srcBegin = beginPos + oldToCenter;
		glm::ivec2 dstBegin = beginPos + _toCenter;
		//for (int x = 0; x < copyWidth; x++)
		//{
		//	std::memcpy(_tilemap[dstBegin.x + x], oldTilemap[srcBegin.x + x], bytesToCopy);
		//}
		//SPDLOG_INFO("COPING MEM W {} H {}", copyWidth, copyHeight);
		for (int x = 0; x < copyWidth; x++)
		{
			for (int y = 0; y < copyHeight; y++)
			{
				//SPDLOG_INFO("NT [{}, {}]: , OT: [{}, {}]  W {} H {}", dstBegin.x + x, dstBegin.y + y, srcBegin.x + x, srcBegin.y + y, copyWidth, copyHeight);
				_tilemap[dstBegin.x + x][dstBegin.y + y] = oldTilemap[srcBegin.x + x][srcBegin.y + y];
				oldTilemap[srcBegin.x + x][srcBegin.y + y] = nullptr;
			}
		}
	}

	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			if (_tilemap[i][j] == nullptr)
			{
				_tilemap[i][j] = new HexagonalTile();
				_tilemap[i][j]->SetTilemap(this);
				_tilemap[i][j]->SetPosition(glm::ivec2(i + _leftBottomPosition.x, j + _leftBottomPosition.y));
			}
		}
	}

	// Deleting old tilemap
	for (int i = 0; i < oldWidth; i++)
	{
		for (int j = 0; j < oldHeight; j++)
		{
			if (oldTilemap[i][j] != nullptr)
			{
				delete oldTilemap[i][j];
			}
		}

		delete[] oldTilemap[i];
	}

	delete[] oldTilemap;
}


void HexagonalTilemap::Clear()
{
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			if (_tilemap[i][j] != nullptr)
			{
				delete _tilemap[i][j];
			}
		}

		delete[] _tilemap[i];
	}

	delete[] _tilemap;

	_leftBottomPosition = glm::ivec2(0, 0);
	_rightTopPosition = glm::ivec2(0, 0);
	_toCenter = glm::ivec2(0, 0);

	_width = 1;
	_height = 1;
	//_distanceBetweenTiles = 1.0f;
	//_edgeLength = _distanceBetweenTiles / glm::sqrt(3.f);

	_tilemap = new HexagonalTile * *[1];
	_tilemap[0] = new HexagonalTile * [1];
	_tilemap[0][0] = new HexagonalTile();
	_tilemap[0][0]->SetTilemap(this);
	_tilemap[0][0]->SetPosition(glm::ivec2(0, 0));
}


unsigned int HexagonalTilemap::GetWidth() const
{
	return _width;
}

unsigned int HexagonalTilemap::GetHeight() const
{
	return _height;
}

glm::ivec2 HexagonalTilemap::GetLeftBottomPosition() const
{
	return _leftBottomPosition;
}

glm::ivec2 HexagonalTilemap::GetRightTopPosition() const
{
	return _rightTopPosition;
}


//void HexagonalTilemap::SetTile(const glm::ivec2& position, Twin2Engine::Core::GameObject* gameObject)
void HexagonalTilemap::SetTile(const glm::ivec2& position, Twin2Engine::Core::Prefab* prefab)
{

	//Twin2Engine::Core::GameObject* instantiatedGameObject = Twin2Engine::Core::GameObject::Instantiate(prefab);
	Twin2Engine::Core::GameObject* instantiatedGameObject = Twin2Engine::Manager::SceneManager::CreateGameObject(prefab, this->GetGameObject()->GetTransform());

	if (position.x >= _leftBottomPosition.x && position.x <= _rightTopPosition.x && position.y >= _leftBottomPosition.y && position.y <= _rightTopPosition.y)
	{
		//_tilemap[position.x + _toCenter.x][position.y + _toCenter.y].SetGameObject(gameObject);
		_tilemap[position.x + _toCenter.x][position.y + _toCenter.y]->SetGameObject(instantiatedGameObject);
	}
	else
	{
		glm::vec2 newLeftBottomPosition(_leftBottomPosition);
		glm::vec2 newRightTopPosition(_rightTopPosition);

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
			newRightTopPosition.x = position.x;
		}
		if (position.y > _rightTopPosition.y)
		{
			newRightTopPosition.y = position.y;
		}

		Resize(newLeftBottomPosition, newRightTopPosition);

		_tilemap[position.x + _toCenter.x][position.y + _toCenter.y]->SetGameObject(instantiatedGameObject);
	}

	//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3(position.x * _distanceBetweenTiles, 0.0f, position.y * _distanceBetweenTiles));
	//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3((position.x * _distanceBetweenTiles + (position.y % 2) * 0.5f * _distanceBetweenTiles) * 1.5f, 0.0f, position.y * _distanceBetweenTiles * 0.25f * SQRT_3));
	instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3(position.x * _distanceBetweenTiles * 0.75f, 0.0f, (position.y + (abs(position.x) % 2) * 0.5f) * _distanceBetweenTiles * 0.5f * SQRT_3));
	//gameObject->GetTransform()->SetLocalPosition(glm::vec3(position.x * _distanceBetweenTiles, position.y * _distanceBetweenTiles, 0.0f));
}


inline HexagonalTile* HexagonalTilemap::GetTile(const glm::ivec2& position) const
{
	if (position.x < _leftBottomPosition.x || position.x > _rightTopPosition.x || position.y < _leftBottomPosition.y || position.y > _rightTopPosition.y)
	{
		return nullptr;
	}

	return _tilemap[position.x + _toCenter.x][position.y + _toCenter.y];
}

glm::ivec2 HexagonalTilemap::GetPositionInDirection(const glm::ivec2& position, unsigned int direction)
{
	if (direction < 6)
	{
		if (position.y % 2)
		{
			return position + HexagonalTile::adjacentDirectionsOddY[direction];
		}
		else
		{
			return position + HexagonalTile::adjacentDirectionsEvenY[direction];
		}
	}
	return position;
}

void HexagonalTilemap::RemoveTile(const glm::ivec2& position)
{

}

void HexagonalTilemap::Fill(const glm::ivec2& position, Twin2Engine::Core::Prefab* prefab)
{
	

	// Check if the given position is within the tilemap bounds
	if (position.x < _leftBottomPosition.x || position.x > _rightTopPosition.x ||
		position.y < _leftBottomPosition.y || position.y > _rightTopPosition.y)
	{
		//std::cerr << "Error: Position is out of bounds.\n";
		return;
	}

	// Perform a breadth-first search to fill empty tiles within the bounds
	std::queue<glm::ivec2> toFillTilesQueue;
	//std::unordered_set<glm::ivec2, std::hash<glm::ivec2>> visited;

	// Add the initial position to the queue
	toFillTilesQueue.push(position);
	//visited.insert(position);

	// Define the neighboring directions for hexagonal tiles
	
	const glm::ivec2 adjacentDirectionsEvenY[6] = {
		glm::ivec2(-1, 1), glm::ivec2(0, 1), glm::ivec2(1, 0),
		glm::ivec2(0, -1), glm::ivec2(-1, -1), glm::ivec2(-1, 0)
	};

	const glm::ivec2 adjacentDirectionsOddY[6] = {
		glm::ivec2(0, 1), glm::ivec2(1, 1), glm::ivec2(1, 0),
		glm::ivec2(1, -1), glm::ivec2(0, -1), glm::ivec2(-1, 0)
	};


	glm::ivec2 currentPos;
	HexagonalTile* currentTile = nullptr;
	Twin2Engine::Core::GameObject* instantiatedGameObject = nullptr;

	while (!toFillTilesQueue.empty())
	{
		currentPos = toFillTilesQueue.front();
		toFillTilesQueue.pop();

		currentTile = GetTile(currentPos);

		if (currentTile && !currentTile->GetGameObject())
		{
			// Kopiowanie gameobjectu
			//instantiatedGameObject = Twin2Engine::Core::GameObject::Instantiate(prefab, this->GetGameObject()->GetTransform());
			instantiatedGameObject = Twin2Engine::Manager::SceneManager::CreateGameObject(prefab, this->GetGameObject()->GetTransform());

			currentTile->SetGameObject(instantiatedGameObject);
			//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3((currentPos.x * _distanceBetweenTiles + (abs(currentPos.y) % 2) * 0.5f * _distanceBetweenTiles) * 1.5f, 0.0f, currentPos.y * _distanceBetweenTiles * 0.25f * SQRT_3));
			instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3(currentPos.x * _distanceBetweenTiles * 0.75f, 0.0f, (currentPos.y + (abs(currentPos.x) % 2) * 0.5f) * _distanceBetweenTiles * 0.5f * SQRT_3));

			const glm::ivec2* directions = (currentPos.y % 2) ? adjacentDirectionsOddY : adjacentDirectionsEvenY;

			//for (const auto& dir : directions)
			//for (const auto& dir : directions)
			for (int i = 0; i < 6; i++)
			{
				glm::ivec2 neighborPos = currentPos + directions[i];

				if (neighborPos.x >= _leftBottomPosition.x && neighborPos.x <= _rightTopPosition.x &&
					neighborPos.y >= _leftBottomPosition.y && neighborPos.y <= _rightTopPosition.y &&
					GetTile(neighborPos)->GetGameObject() == nullptr)
				{
					toFillTilesQueue.push(neighborPos);
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

inline void HexagonalTilemap::SetEdgeLength(float edgeLength)
{
	_edgeLength = edgeLength;

	_distanceBetweenTiles = 0.5f * _edgeLength * glm::sqrt(3.f);
}

inline float HexagonalTilemap::GetEdgeLength() const
{
	return _edgeLength;
}

glm::vec2 HexagonalTilemap::ConvertToRealPosition(const glm::ivec2& position) const
{
	return glm::vec2(position.x * _distanceBetweenTiles * 0.75f, (position.y + (abs(position.x) % 2) * 0.5f) * _distanceBetweenTiles * 0.5f * SQRT_3);
}

glm::vec2 HexagonalTilemap::ConvertToTilemapPosition(const glm::ivec2& position) const
{
	return glm::ivec2(position.x / (_distanceBetweenTiles * 0.75f), glm::floor((position.y / (_distanceBetweenTiles * 0.5f * SQRT_3))));
}

YAML::Node HexagonalTilemap::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "HexagonalTilemap";
	node["leftBottomPosition"] = _leftBottomPosition;
	node["rightTopPosition"] = _rightTopPosition;
	return node;
}

bool HexagonalTilemap::Deserialize(const YAML::Node& node) {
	if (!node["leftBottomPosition"] || !node["rightTopPosition"] ||
		!Component::Deserialize(node)) return false;

	Resize(node["leftBottomPosition"].as<glm::ivec2>(), node["rightTopPosition"].as<glm::ivec2>());

	return true;
}

void HexagonalTilemap::Initialize()
{
	_tilemap = new HexagonalTile * *[1];
	_tilemap[0] = new HexagonalTile * [1];
	_tilemap[0][0] = new HexagonalTile();
	_tilemap[0][0]->SetTilemap(this);
	_tilemap[0][0]->SetPosition(glm::ivec2(0, 0));

	_initialized = true;

	Resize(_leftBottomPosition, _rightTopPosition);
}

void HexagonalTilemap::OnDestroy()
{
	for (int i = 0; i < _width; i++)
	{
		for (int j = 0; j < _height; j++)
		{
			if (_tilemap[i][j] != nullptr)
			{
				delete _tilemap[i][j];
			}
		}

		delete[] _tilemap[i];
	}

	delete[] _tilemap;
}

#if _DEBUG
void HexagonalTilemap::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("Hexagonal Tilemap##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		Component::DrawInheritedFields();

		float dbt = _distanceBetweenTiles;
		ImGui::DragFloat(string("Distance Between Tiles##").append(id).c_str(), &dbt, 0.1f, 0.0f);

		if (dbt != _distanceBetweenTiles) {
			SetDistanceBetweenTiles(dbt);
		}
	}
}
#endif