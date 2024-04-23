#ifndef _MAP_ELEMENT_GENERATOR_H_
#define _MAP_ELEMENT_GENERATOR_H_

#include <Tilemap/HexagonalTilemap.h>

namespace Generation::Generators
{
	class AMapElementGenerator : public Twin2Engine::Core::ScriptableObject
	{
		//SCIPTABLE_OBJECT_BODY(AMapElementGenerator)

	public:
		AMapElementGenerator() = default;
		virtual ~AMapElementGenerator() { } 
		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) = 0;
	};
}

#endif // !_MAP_ELEMENT_GENERATOR_H_