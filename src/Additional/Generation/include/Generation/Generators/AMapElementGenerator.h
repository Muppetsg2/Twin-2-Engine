#pragma once

#include <Tilemap/HexagonalTilemap.h>

#include <core/ScriptableObject.h>
#include <manager/PrefabManager.h>
#include <manager/SceneManager.h>


namespace Generation::Generators
{
	class AMapElementGenerator : public Twin2Engine::Core::ScriptableObject
	{
		//SCIPTABLE_OBJECT_BODY(AMapElementGenerator)

	public:
		AMapElementGenerator() = default;
		virtual ~AMapElementGenerator() { }
		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) = 0;
		virtual void Clean() { }
	};
}