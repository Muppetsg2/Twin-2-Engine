#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/CitiesManager.h>
#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class CitiesGenerator : public AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(CitiesGenerator)

	public:
		//Twin2Engine::Core::GameObject* prefabCity;
		Twin2Engine::Core::Prefab* prefabCity = nullptr;

		bool byRegions = true;
		float density = 1.0f;

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

            std::map<size_t, string> prefabNames = Twin2Engine::Manager::PrefabManager::GetAllPrefabsNames();

            prefabNames.insert(std::pair(0, "None"));

            if (prefabCity != nullptr) {
                if (!prefabNames.contains(prefabCity->GetId())) {
                    prefabCity = nullptr;
                }
            }

            size_t prefabId = prefabCity != nullptr ? prefabCity->GetId() : 0;

            if (ImGui::BeginCombo(string("prefabCity##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

                bool clicked = false;
                size_t choosed = prefabId;
                for (auto& item : prefabNames) {

                    if (ImGui::Selectable(string(item.second).append("##").append(id).c_str(), item.first == prefabId)) {

                        if (clicked) continue;

                        choosed = item.first;
                        clicked = true;
                    }
                }

                if (clicked) {
                    if (choosed != 0) {
                        prefabCity = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                    }
                    else {
                        prefabCity = nullptr;
                    }
                }

                ImGui::EndCombo();
            }

			ImGui::Checkbox(string("byRegions##SO").append(id).c_str(), &byRegions);
			ImGui::SliderFloat(string("Density##SO").append(id).c_str(), &density, 0.0f, 1.0f);

			ScriptableObject::DrawInheritedFields();
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(CitiesGenerator, Generation::Generators)