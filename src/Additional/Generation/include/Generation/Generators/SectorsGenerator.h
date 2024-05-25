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
		//Twin2Engine::Core::GameObject* prefabSector = nullptr;
		Twin2Engine::Core::Prefab* prefabSector = nullptr;
		int minTilesPerSector = 7;
		int maxTilesPerSector = 7;

		float accuracyFactor = 1.0f;

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

			ImGui::InputInt(string("minTilesPerSector##SO").append(id).c_str(), &minTilesPerSector);
			ImGui::InputInt(string("maxTilesPerSector##SO").append(id).c_str(), &maxTilesPerSector);
			ImGui::InputFloat(string("accuracyFactor##SO").append(id).c_str(), &accuracyFactor);

			ScriptableObject::DrawInheritedFields();
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(SectorsGenerator, Generation::Generators)