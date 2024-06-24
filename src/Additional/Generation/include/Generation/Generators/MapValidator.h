#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/VectorShuffler.h>


namespace Generation::Generators
{
    class MapValidator : public AMapElementGenerator
    {
        SCRIPTABLE_OBJECT_BODY(MapValidator)

    public:

        bool destroyWaterTile = true;
        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;
        virtual void Clear() override;

        SO_SERIALIZE()
        SO_DESERIALIZE()

#if _DEBUG
        virtual void DrawEditor() override
        {
            string id = to_string(GetId());

            ImGui::Text("Name: ");
            ImGui::SameLine();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
            ImGui::Text(Twin2Engine::Manager::ScriptableObjectManager::GetName(GetId()).c_str());
            ImGui::PopFont();

            ScriptableObject::DrawInheritedFields();
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(MapValidator, Generation::Generators)