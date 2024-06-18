#pragma once

#include <core/GameObject.h>
#include <core/Prefab.h>
#include <core/Random.h>

#include <manager/SceneManager.h>

#include <Generation/CitiesManager.h>

#include <Generation/Generators/AMapElementGenerator.h>

#include <Humans/HumanMovement.h>
#include <Humans/Human.h>

namespace Humans
{
	class HumansGenerator : public Generation::Generators::AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(HumansGenerator)
	private:
		std::string prefabPath = "";
		Twin2Engine::Core::Prefab* prefabHuman;
		unsigned int number = 0;

	public:

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;

		virtual void Clear() override;

		SO_SERIALIZE();
		SO_DESERIALIZE();

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

            if (prefabHuman != nullptr) {
                if (!prefabNames.contains(prefabHuman->GetId())) {
                    prefabHuman = nullptr;
                }
            }

            size_t prefabId = prefabHuman != nullptr ? prefabHuman->GetId() : 0;

            if (ImGui::BeginCombo(string("prefabHuman##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

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
                        prefabHuman = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
                        prefabPath = Twin2Engine::Manager::PrefabManager::GetPrefabPath(prefabHuman);
                    }
                    else {
                        prefabHuman = nullptr;
                        prefabPath = "";
                    }
                }

                ImGui::EndCombo();
            }

            prefabNames.clear();

            ImGui::DragUInt(std::string("Density##").append(id).c_str(), &number, 1, 0, UINT_MAX);

            ScriptableObject::DrawInheritedFields();
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(HumansGenerator, Humans)