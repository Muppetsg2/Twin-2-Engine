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

        virtual void OnDestroy() override;

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;

        virtual void DrawEditor() override;
	};
}