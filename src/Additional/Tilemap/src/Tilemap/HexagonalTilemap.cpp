﻿#include <Tilemap/HexagonalTilemap.h>

#include <tools/YamlConverters.h>

constexpr const float SQRT_3 = 1.7320508075688772935274463415059f;

using namespace Tilemap;

void HexagonalTilemap::Resize(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition)
{
	//if (!_initialized) {
	//	_leftBottomPosition = leftBottomPosition;
	//	_rightTopPosition = rightTopPosition;
	//	return;
	//}

	HexagonalTile*** oldTilemap = _tilemap;
	unsigned int oldWidth = _width;
	unsigned int oldHeight = _height;
	glm::ivec2 oldToCenter = _toCenter;
	glm::ivec2 oldLeftBottomPosition = _leftBottomPosition;
	glm::ivec2 oldRightTopPosition = _rightTopPosition;

	// Creating new tilemap
	if (leftBottomPosition.x <= rightTopPosition.x)
	{
		_leftBottomPosition.x = leftBottomPosition.x;
		_rightTopPosition.x = rightTopPosition.x;
	}
	else
	{
		_leftBottomPosition.x = rightTopPosition.x;
		_rightTopPosition.x = leftBottomPosition.x;
	}
	if (leftBottomPosition.y <= rightTopPosition.y)
	{
		_leftBottomPosition.y = leftBottomPosition.y;
		_rightTopPosition.y = rightTopPosition.y;
	}
	else
	{
		_leftBottomPosition.y = rightTopPosition.y;
		_rightTopPosition.y = leftBottomPosition.y;
	}

	_toCenter = -_leftBottomPosition;

	_width = _rightTopPosition.x - _leftBottomPosition.x + 1;
	_height = _rightTopPosition.y - _leftBottomPosition.y + 1;

	_tilemap = new HexagonalTile * *[_width];
	for (int i = 0; i < _width; ++i)
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
		int bytesToCopy = copyHeight * sizeof(HexagonalTile);
		glm::ivec2 srcBegin = beginPos + oldToCenter;
		glm::ivec2 dstBegin = beginPos + _toCenter;
		//for (int x = 0; x < copyWidth; x++)
		//{
		//	std::memcpy(_tilemap[dstBegin.x + x], oldTilemap[srcBegin.x + x], bytesToCopy);
		//}
		//SPDLOG_INFO("COPING MEM W {} H {}", copyWidth, copyHeight);
		for (int x = 0; x < copyWidth; ++x)
		{
			for (int y = 0; y < copyHeight; ++y)
			{
				//SPDLOG_INFO("NT [{}, {}]: , OT: [{}, {}]  W {} H {}", dstBegin.x + x, dstBegin.y + y, srcBegin.x + x, srcBegin.y + y, copyWidth, copyHeight);
				_tilemap[dstBegin.x + x][dstBegin.y + y] = oldTilemap[srcBegin.x + x][srcBegin.y + y];
				oldTilemap[srcBegin.x + x][srcBegin.y + y] = nullptr;
			}
		}
	}

	for (int i = 0; i < _width; ++i)
	{
		for (int j = 0; j < _height; ++j)
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
	for (int i = 0; i < oldWidth; ++i)
	{
		for (int j = 0; j < oldHeight; ++j)
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
	for (int i = 0; i < _width; ++i)
	{
		for (int j = 0; j < _height; ++j)
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

	SPDLOG_WARN("Setting on position: [{}, {}]", position.x, position.y);
	if (position.x >= _leftBottomPosition.x && position.x <= _rightTopPosition.x && position.y >= _leftBottomPosition.y && position.y <= _rightTopPosition.y)
	{
		//_tilemap[position.x + _toCenter.x][position.y + _toCenter.y].SetGameObject(gameObject);
		if (_tilemap[position.x + _toCenter.x][position.y + _toCenter.y]->GetGameObject() != nullptr)
		{
			Twin2Engine::Manager::SceneManager::DestroyGameObject(_tilemap[position.x + _toCenter.x][position.y + _toCenter.y]->GetGameObject());
		}
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
		else if (position.x > _rightTopPosition.x)
		{
			newRightTopPosition.x = position.x;
		}
		if (position.y < _leftBottomPosition.y)
		{
			newLeftBottomPosition.y = position.y;
		}
		else if (position.y > _rightTopPosition.y)
		{
			newRightTopPosition.y = position.y;
		}
		SPDLOG_WARN("Resizing position: [{}, {}]", position.x, position.y);
		Resize(newLeftBottomPosition, newRightTopPosition);

		if (_tilemap[position.x + _toCenter.x][position.y + _toCenter.y]->GetGameObject() != nullptr)
		{
			Twin2Engine::Manager::SceneManager::DestroyGameObject(_tilemap[position.x + _toCenter.x][position.y + _toCenter.y]->GetGameObject());
		}
		_tilemap[position.x + _toCenter.x][position.y + _toCenter.y]->SetGameObject(instantiatedGameObject);
	}

	//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3(position.x * _distanceBetweenTiles, 0.0f, position.y * _distanceBetweenTiles));
	//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3((position.x * _distanceBetweenTiles + (position.y % 2) * 0.5f * _distanceBetweenTiles) * 1.5f, 0.0f, position.y * _distanceBetweenTiles * 0.25f * SQRT_3));


	instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3(position.x * _edgeLength * 1.5f, 0.0f, (position.y + (abs(position.x) % 2) * 0.5f) * _distanceBetweenTiles));
	//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3((position.x + (abs(position.y) % 2) * 0.5f) * _edgeLength * 4.0f, 0.0f, position.y * 0.5f * _distanceBetweenTiles));

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
		if (position.x % 2)
		{
			return position + HexagonalTile::adjacentDirectionsOddX[direction];
		}
		else
		{
			return position + HexagonalTile::adjacentDirectionsEvenX[direction];
		}
	}
	return position;
}

void HexagonalTilemap::RemoveTile(const glm::ivec2& position)
{
	Twin2Engine::Manager::SceneManager::DestroyGameObject(_tilemap[position.x][position.y]->_gameObject);
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
			instantiatedGameObject = Twin2Engine::Manager::SceneManager::CreateGameObject(prefab, this->GetGameObject()->GetTransform());

			currentTile->SetGameObject(instantiatedGameObject);
			//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3((currentPos.x * _distanceBetweenTiles + (abs(currentPos.y) % 2) * 0.5f * _distanceBetweenTiles) * 1.5f, 0.0f, currentPos.y * _distanceBetweenTiles * 0.25f * SQRT_3));

			instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3(currentPos.x * _edgeLength * 1.5f, 0.0f, (currentPos.y + (abs(currentPos.x) % 2) * 0.5f) * _distanceBetweenTiles));
			//instantiatedGameObject->GetTransform()->SetLocalPosition(glm::vec3((currentPos.x + (abs(currentPos.y) % 2) * 0.5f) * _edgeLength * 4.0f, 0.0f, currentPos.y * 0.5f * _distanceBetweenTiles));

			//const glm::ivec2* directions = (currentPos.y % 2) ? adjacentDirectionsOddY : adjacentDirectionsEvenY;
			const glm::ivec2* directions = (currentPos.x % 2) ? HexagonalTile::adjacentDirectionsOddX : HexagonalTile::adjacentDirectionsEvenX;

			for (int i = 0; i < 6; ++i)
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

void HexagonalTilemap::SetDistanceBetweenTiles(float distanceBetweenTiles)
{
	_distanceBetweenTiles = distanceBetweenTiles;

	_edgeLength = _distanceBetweenTiles / SQRT_3;
}

float HexagonalTilemap::GetDistanceBetweenTiles() const
{
	return _distanceBetweenTiles;
}

void HexagonalTilemap::SetEdgeLength(float edgeLength)
{
	_edgeLength = edgeLength;

	_distanceBetweenTiles = _edgeLength * SQRT_3;
}

float HexagonalTilemap::GetEdgeLength() const
{
	return _edgeLength;
}

glm::vec2 HexagonalTilemap::ConvertToRealPosition(const glm::ivec2& position) const
{
	return glm::vec2(position.x * _edgeLength * 1.5f, (position.y + (abs(position.x) % 2) * 0.5f) * _distanceBetweenTiles);
	//return glm::vec2((position.x + (abs(position.y) % 2) * 0.5f) * _edgeLength * 4.0f, position.y * 0.5f * _distanceBetweenTiles);
}

glm::ivec2 HexagonalTilemap::ConvertToTilemapPosition(const glm::vec2& position) const
{
	float fx = position.x / (_edgeLength * 1.5f);
	int x = glm::round(fx);

	float factor = position.y / _distanceBetweenTiles * 2;
	factor -= glm::floor(factor);
	factor = glm::abs(factor) * 0.5f;
	if (x % 2)
	{
		if (fx > (x + 0.25f + factor))
		{
			++x;
		}
		else if (fx <= (x - 0.25f - factor))
		{
			--x;
		}
	}
	else
	{
		if (fx > (x + 0.75f - factor))
		{
			++x;
		}
		else if (fx <= (x - 0.75f + factor))
		{
			--x;
		}
	}

	int y = glm::round(((position.y - 0.5f * (abs(x) % 2)) / _distanceBetweenTiles));

	return glm::ivec2(x, y);

}

YAML::Node HexagonalTilemap::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "HexagonalTilemap";
	/*
	node["leftBottomPosition"] = _leftBottomPosition;
	node["rightTopPosition"] = _rightTopPosition;
	*/
	node["edgeLength"] = _edgeLength;

	// TODO: Zrobic serializacje zawartoci tilemapy w formie par pozycja, gameObject
	//GameObject* processed = nullptr;
	//
	//YAML::Node listNode = YAML::Node(YAML::NodeType::Sequence);
	//YAML::Node pairNode;
	//
	//for (unsigned int i = 0; i < _width; ++i)
	//{
	//	for (unsigned int j = 0; j < _height; ++j)
	//	{
	//		processed = _tilemap[i][j]->GetGameObject();
	//
	//		if (processed)
	//		{
	//			pairNode["position"] = glm::ivec2(i, j);
	//			pairNode["id"] = processed->Id();
	//			listNode.push_back(pairNode);
	//		}
	//	}
	//}
	//node["tilemapGameObjects"] = listNode;

	return node;
}

bool HexagonalTilemap::Deserialize(const YAML::Node& node) {
	if (/*
		!node["leftBottomPosition"] || !node["rightTopPosition"] || 
		*/
		!node["edgeLength"] ||
		!Component::Deserialize(node)) return false;

	/*
	_leftBottomPosition = node["leftBottomPosition"].as<glm::ivec2>();
	_rightTopPosition = node["rightTopPosition"].as<glm::ivec2>();
	*/
	_edgeLength = node["edgeLength"].as<float>();
	_distanceBetweenTiles = _edgeLength * SQRT_3;

	/*
	if (_leftBottomPosition.x > _rightTopPosition.x)
	{
		int temp = _leftBottomPosition.x;
		_leftBottomPosition.x = _rightTopPosition.x;
		_rightTopPosition.x = temp;
	}
	if (_leftBottomPosition.y > _rightTopPosition.y)
	{
		int temp = _leftBottomPosition.y;
		_leftBottomPosition.y = _rightTopPosition.y;
		_rightTopPosition.y = temp;
	}

	_width = _rightTopPosition.x - _leftBottomPosition.x + 1;
	_height = _rightTopPosition.y - _leftBottomPosition.y + 1;
	*/


	_tilemap = new HexagonalTile * *[_width];

	for (unsigned int i = 0; i < _width; ++i)
	{
		_tilemap[i] = new HexagonalTile * [_height];

		for (unsigned int j = 0; j < _height; ++j)
		{
			_tilemap[i][j] = new HexagonalTile();
			_tilemap[i][j]->SetTilemap(this);
			_tilemap[i][j]->SetPosition(glm::ivec2(i + _leftBottomPosition.x, j + _leftBottomPosition.y));
		}
	}

	// TODO: Zrobic deserializacjê zawartoci tilemapy w formie par pozycja, gameObject
	//glm::ivec2 position;
	//size_t id;
	//for (const YAML::Node& seqNode : node["tilemapGameObjects"])
	//{
	//	position = seqNode["position"].as<glm::ivec2>();
	//	id = seqNode["id"].as<size_t>();
	//	// TODO: Okreliæ mapowanie zapisanych id gamoeobject w tilemapie na obecne id gameobject w secenie.
	//	//_tilemap[position.x][position.y]->SetGameObject(Twin2Engine::Manager::SceneManager::GetGameObjectWithId(id));
	//}


	//Resize(node["leftBottomPosition"].as<glm::ivec2>(), node["rightTopPosition"].as<glm::ivec2>()); Raczej do usuniecia

	return true;
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

		ImGui::Text("LeftBottomCorner: ");
		ImGui::SameLine();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("%d\t%d", _leftBottomPosition.x, _leftBottomPosition.y);
		ImGui::PopFont();
		ImGui::Text("RightTopCorner: ");
		ImGui::SameLine();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("%d\t%d", _rightTopPosition.x, _rightTopPosition.y);
		ImGui::PopFont();
	}
}
#endif

void HexagonalTilemap::Initialize()
{
	//_tilemap = new HexagonalTile * *[1];
	//_tilemap[0] = new HexagonalTile * [1];
	//_tilemap[0][0] = new HexagonalTile();
	//_tilemap[0][0]->SetTilemap(this);
	//_tilemap[0][0]->SetPosition(glm::ivec2(0, 0));
	//
	_initialized = true;


	//Resize(_leftBottomPosition, _rightTopPosition);
}

void HexagonalTilemap::OnDestroy()
{
	for (int i = 0; i < _width; ++i)
	{
		for (int j = 0; j < _height; ++j)
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