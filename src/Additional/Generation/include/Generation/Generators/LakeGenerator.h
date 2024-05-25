#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/MapSector.h>
#include <Generation/VectorShuffler.h>


namespace Generation::Generators
{
    class LakeGenerator : public AMapElementGenerator
    {
        SCRIPTABLE_OBJECT_BODY(LakeGenerator)

    public:
        int numberOfLakes = 0;
        float waterLevel = 0.0f;

        bool destroyWaterTile = true;
        virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

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

            ImGui::Checkbox(string("destroyWaterTile##SO").append(id).c_str(), &destroyWaterTile);
            ImGui::InputInt(string("numberOfLakes##SO").append(id).c_str(), &numberOfLakes);
            ImGui::InputFloat(string("waterLevel##SO").append(id).c_str(), &waterLevel);

            ScriptableObject::DrawInheritedFields();
        }
#endif
    };
}

SERIALIZABLE_SCRIPTABLE_OBJECT(LakeGenerator, Generation::Generators)