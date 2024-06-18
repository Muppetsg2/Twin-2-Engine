#pragma once

#include <Generation/Generators/AMapElementGenerator.h>
#include <Generation/MapSector.h>
#include <Generation/VectorShuffler.h>

#include <core/GameObject.h>
#include <manager/SceneManager.h>

namespace Generation::Generators
{
	class SectorsGenerator : public AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(SectorsGenerator)
	private:

		MapSector* CreateSector(Tilemap::HexagonalTilemap* tilemap, glm::ivec2 position);

	public:
		std::string prefabPath = "";
		Twin2Engine::Core::Prefab* prefabSector = nullptr;
		int minTilesPerSector = 7;
		int maxTilesPerSector = 7;

		float accuracyFactor = 1.0f;

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

			if (prefabSector != nullptr) {
				if (!prefabNames.contains(prefabSector->GetId())) {
					prefabSector = nullptr;
				}
			}

			size_t prefabId = prefabSector != nullptr ? prefabSector->GetId() : 0;

			if (ImGui::BeginCombo(string("prefabSector##SO").append(id).c_str(), prefabNames[prefabId].c_str())) {

				bool clicked = false;
				size_t choosed = prefabId;
				for (auto& item : prefabNames) {

					if (ImGui::Selectable(item.second.append("##").append(id).c_str(), item.first == prefabId)) {

						if (clicked) continue;

						choosed = item.first;
						clicked = true;
					}
				}

				if (clicked) {
					if (choosed != 0) {
						prefabSector = Twin2Engine::Manager::PrefabManager::GetPrefab(choosed);
						prefabPath = Twin2Engine::Manager::PrefabManager::GetPrefabPath(prefabSector);
					}
					else {
						prefabSector = nullptr;
						prefabPath = "";
					}
				}

				ImGui::EndCombo();
			}

			prefabNames.clear();

			ImGui::InputInt(string("minTilesPerSector##SO").append(id).c_str(), &minTilesPerSector);
			ImGui::InputInt(string("maxTilesPerSector##SO").append(id).c_str(), &maxTilesPerSector);
			ImGui::InputFloat(string("accuracyFactor##SO").append(id).c_str(), &accuracyFactor);

			ScriptableObject::DrawInheritedFields();
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(SectorsGenerator, Generation::Generators)