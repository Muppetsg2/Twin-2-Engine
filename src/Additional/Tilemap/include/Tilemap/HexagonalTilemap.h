#pragma once

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
		unsigned int _width = 1;
		unsigned int _height = 1;
		float _distanceBetweenTiles = 1.f;
		float _edgeLength = 1.f / glm::sqrt(3.f);
		glm::ivec2 _leftBottomPosition = glm::ivec2(0);
		glm::ivec2 _rightTopPosition = glm::ivec2(0);
		glm::ivec2 _toCenter = glm::ivec2(0);

		bool _initialized = false;

	public:

		void Resize(glm::ivec2 leftBottomPosition, glm::ivec2 rightTopPosition);
		void Clear();

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

		void SetDistanceBetweenTiles(float distanceBetweenTiles);
		float GetDistanceBetweenTiles() const;
		void SetEdgeLength(float edgeLength);
		float GetEdgeLength() const;

		glm::vec2 ConvertToRealPosition(const glm::ivec2& position) const;
		glm::ivec2 ConvertToTilemapPosition(const glm::vec2& position) const;

		static glm::ivec2 GetPositionInDirection(const glm::ivec2& position, unsigned int direction);

		virtual void Initialize() override;
		virtual void OnDestroy() override;
		
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}
#include <Tilemap/HexagonalTile.h>

#include <Generation/MapHexTile.h>