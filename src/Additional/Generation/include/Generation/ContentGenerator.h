#ifndef _CONTENT_GENERATOR_H_
#define _CONTENT_GENERATOR_H_

#include <Generation/Generators/AMapElementGenerator.h>

#include <core/Component.h>

namespace Generation
{
	class ContentGenerator : public Twin2Engine::Core::Component
	{
    public:
        std::list<Generators::AMapElementGenerator*> mapElementGenerators;
        void GenerateContent(Tilemap::HexagonalTilemap* targetTilemap);
	};
}

#endif // !_CONTENT_GENERATOR_H_
