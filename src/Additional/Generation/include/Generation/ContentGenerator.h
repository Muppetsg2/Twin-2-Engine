#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <core/Component.h>

namespace Generation
{
	class ContentGenerator : public Twin2Engine::Core::Component
	{
    public:
        std::vector<Generators::AMapElementGenerator*> mapElementGenerators;
        void GenerateContent(Tilemap::HexagonalTilemap* targetTilemap);

        virtual YAML::Node Serialize() const override;

        virtual void DrawEditor() override;
	};
}