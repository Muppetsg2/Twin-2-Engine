#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <core/Component.h>

namespace Generation
{
	class ContentGenerator : public Twin2Engine::Core::Component
	{
        //Tilemap::HexagonalTilemap* _tilemap = nullptr;
        bool _reset = false;

    public:
        std::vector<Generators::AMapElementGenerator*> mapElementGenerators;
        void GenerateContent(Tilemap::HexagonalTilemap* targetTilemap);
        void ClearContent();
        void Reset();

        virtual void Initialize() override;
        virtual void OnDestroy() override;

        virtual YAML::Node Serialize() const override;
        virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
        virtual void DrawEditor() override;
#endif
	};
}