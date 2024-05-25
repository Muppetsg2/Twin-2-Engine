#pragma once

#include <Generation/Generators/AMapElementGenerator.h>

#include <Generation/MapRegion.h>
#include <Generation/VectorShuffler.h>

#include <core/Random.h>

namespace Generation::Generators
{
	class RadioStationGeneratorSectorBased : public AMapElementGenerator
	{
		SCRIPTABLE_OBJECT_BODY(RadioStationGeneratorSectorBased)

	public:
		//Twin2Engine::Core::GameObject* prefabRadioStation;
		Twin2Engine::Core::Prefab* prefabRadioStation;
		float densityFactorPerSector = 1.0f;

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

			ImGui::InputFloat(string("densityFactorPerSector##SO").append(id).c_str(), &densityFactorPerSector);

			ScriptableObject::DrawInheritedFields();
		}
#endif
	};
}

SERIALIZABLE_SCRIPTABLE_OBJECT(RadioStationGeneratorSectorBased, Generation::Generators)