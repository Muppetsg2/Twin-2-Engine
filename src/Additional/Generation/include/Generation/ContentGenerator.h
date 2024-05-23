#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <core/Component.h>

namespace Generation
{
	class ContentGenerator : public Twin2Engine::Core::Component
	{
        //Tilemap::HexagonalTilemap* _tilemap = nullptr;
    public:
        std::vector<Generators::AMapElementGenerator*> mapElementGenerators;
        void GenerateContent(Tilemap::HexagonalTilemap* targetTilemap);

        virtual void Initialize() override;
        virtual void OnDestroy() override;

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
        virtual void DrawEditor() override;
#endif
	};
}