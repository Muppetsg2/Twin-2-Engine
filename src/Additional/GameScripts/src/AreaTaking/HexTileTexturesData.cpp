#include <AreaTaking/HexTileTexturesData.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace std;

SCRIPTABLE_OBJECT_SOURCE_CODE_NN(HexTileTextureData, "HexTileTextureData")

SO_SERIALIZATION_BEGIN(HexTileTextureData, ScriptableObject)
SO_SERIALIZE_FIELD(_materialNames)
SO_SERIALIZATION_END()

SO_DESERIALIZATION_BEGIN(HexTileTextureData, ScriptableObject)
//SO_DESERIALIZE_FIELD(flatness)
SO_DESERIALIZE_FIELD_F_T_R("_materialNames", _materials, DeserializationHelperMaterialNamesToMaterials, vector<vector<string>>)

SO_DESERIALIZATION_END()


vector<vector<Material*>> HexTileTextureData::DeserializationHelperMaterialNamesToMaterials(vector<vector<string>> materialNames)
{
	_materialNames = std::move(materialNames);
	_materials.clear();

	for (size_t i = 0; i < materialNames.size(); ++i)
	{
		vector<Material*> materials;

		for (size_t j = 0; j < materialNames[i].size(); ++j)
		{
			materials.push_back(MaterialsManager::GetMaterial(materialNames[i][j]));
		}

		_materials.push_back(std::move(materials));
	}

	return _materials;
}