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
	_materialNames = materialNames;


	size_t sizeOuter = materialNames.size();
	size_t sizeInner = 0ull;

	_materials.reserve(sizeOuter);

	for (size_t i = 0ull; i < sizeOuter; ++i)
	{
		sizeInner = materialNames[i].size();

		vector<Material*> materials;
		materials.reserve(sizeInner);
		_materials.push_back(materials);

		for (size_t j = 0ull; j < sizeInner; ++j)
		{
			_materials[i].push_back(MaterialsManager::GetMaterial(materialNames[i][j]));
		}
	}

	return _materials;
}