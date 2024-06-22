#include <AreaTaking/HexTileTextureData.h>
#include <graphic/manager/MaterialsManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace std;

SCRIPTABLE_OBJECT_SOURCE_CODE_NN(HexTileTextureData, "HexTileTextureData")

void HexTileTextureData::Clear() {

	/*
	_blueMaterialsNames.clear();
	_redMaterialsNames.clear();
	_greenMaterialsNames.clear();
	_purpleMaterialsNames.clear();
	_yellowMaterialsNames.clear();
	_cyanMaterialsNames.clear();
	_pinkMaterialsNames.clear();
	*/

	_neutralMaterial = nullptr;
	_neutralBorderMaterial = nullptr;
	_blueMaterials.clear();
	_blueBorderMaterial = nullptr;
	_redMaterials.clear();
	_redBorderMaterial = nullptr;
	_greenMaterials.clear();
	_greenBorderMaterial = nullptr;
	_purpleMaterials.clear();
	_purpleBorderMaterial = nullptr;
	_yellowMaterials.clear();
	_yellowBorderMaterial = nullptr;
	_cyanMaterials.clear();
	_cyanBorderMaterial = nullptr;
	_pinkMaterials.clear();
	_pinkBorderMaterial = nullptr;

	/*
	for (size_t i = 0; i < _materials.size(); ++i)
	{
		_materials[i].clear();
		_materialNames[i].clear();
	}

	_materials.clear();
	_materialNames.clear();
	*/
}

void HexTileTextureData::Serialize(YAML::Node& node) const
{
	ScriptableObject::Serialize(node);
	node["__SO_RegisteredName__"] = _registeredName;

	node["neutralMaterialName"] = MaterialsManager::GetMaterialPath(_neutralMaterial);
	node["neutralBorderMaterialName"] = MaterialsManager::GetMaterialPath(_neutralBorderMaterial);

	node["blueMaterialsNames"] = vector<string>();
	for (const auto& mat : _blueMaterials) {
		node["blueMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["blueRetreatingMaterialsNames"] = vector<string>();
	for (const auto& mat : _blueRetreatingMaterials) {
		node["blueRetreatingMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["blueBorderMaterialName"] = MaterialsManager::GetMaterialPath(_blueBorderMaterial);

	node["redRetreatingMaterialsNames"] = vector<string>();
	for (const auto& mat : _redRetreatingMaterials) {
		node["redRetreatingMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["redMaterialsNames"] = vector<string>();
	for (const auto& mat : _redMaterials) {
		node["redMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["redBorderMaterialName"] = MaterialsManager::GetMaterialPath(_redBorderMaterial);

	node["greenMaterialsNames"] = vector<string>();
	for (const auto& mat : _greenMaterials) {
		node["greenMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["greenRetreatingMaterialsNames"] = vector<string>();
	for (const auto& mat : _greenRetreatingMaterials) {
		node["greenRetreatingMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["greenBorderMaterialName"] = MaterialsManager::GetMaterialPath(_greenBorderMaterial);

	node["purpleMaterialsNames"] = vector<string>();
	for (const auto& mat : _purpleMaterials) {
		node["purpleMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["purpleRetreatingMaterialsNames"] = vector<string>();
	for (const auto& mat : _purpleRetreatingMaterials) {
		node["purpleRetreatingMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["purpleBorderMaterialName"] = MaterialsManager::GetMaterialPath(_purpleBorderMaterial);

	node["yellowMaterialsNames"] = vector<string>();
	for (const auto& mat : _yellowMaterials) {
		node["yellowMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["yellowRetreatingMaterialsNames"] = vector<string>();
	for (const auto& mat : _yellowRetreatingMaterials) {
		node["yellowRetreatingMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["yellowBorderMaterialName"] = MaterialsManager::GetMaterialPath(_yellowBorderMaterial);

	node["cyanMaterialsNames"] = vector<string>();
	for (const auto& mat : _cyanMaterials) {
		node["cyanMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["cyanRetreatingMaterialsNames"] = vector<string>();
	for (const auto& mat : _cyanRetreatingMaterials) {
		node["cyanRetreatingMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["cyanBorderMaterialName"] = MaterialsManager::GetMaterialPath(_cyanBorderMaterial);

	node["pinkMaterialsNames"] = vector<string>();
	for (const auto& mat : _pinkMaterials) {
		node["pinkMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["pinkRetreatingMaterialsNames"] = vector<string>();
	for (const auto& mat : _pinkRetreatingMaterials) {
		node["pinkRetreatingMaterialsNames"].push_back(MaterialsManager::GetMaterialPath(mat));
	}
	node["pinkBorderMaterialName"] = MaterialsManager::GetMaterialPath(_pinkBorderMaterial);
}

bool HexTileTextureData::Deserialize(const YAML::Node& node)
{
	if (!node["neutralMaterialName"] || !node["neutralBorderMaterialName"] || !node["blueMaterialsNames"] || !node["blueRetreatingMaterialsNames"] ||
		!node["blueBorderMaterialName"] || !node["redMaterialsNames"] || !node["redRetreatingMaterialsNames"] || !node["redBorderMaterialName"] ||
		!node["greenMaterialsNames"] || !node["greenRetreatingMaterialsNames"] || !node["greenBorderMaterialName"] || !node["purpleMaterialsNames"] ||
		!node["purpleRetreatingMaterialsNames"] || !node["purpleBorderMaterialName"] || !node["yellowMaterialsNames"] || !node["yellowRetreatingMaterialsNames"] || 
		!node["yellowBorderMaterialName"] || !node["cyanMaterialsNames"] || !node["cyanRetreatingMaterialsNames"] || !node["cyanBorderMaterialName"] || 
		!node["pinkRetreatingMaterialsNames"] || !node["pinkMaterialsNames"] || !node["pinkBorderMaterialName"] || !ScriptableObject::Deserialize(node)) return false;

	_neutralMaterial = MaterialsManager::GetMaterial(node["neutralMaterialName"].as<string>());
	_neutralBorderMaterial = MaterialsManager::GetMaterial(node["neutralBorderMaterialName"].as<string>());
	_blueBorderMaterial = MaterialsManager::GetMaterial(node["blueBorderMaterialName"].as<string>());
	_redBorderMaterial = MaterialsManager::GetMaterial(node["redBorderMaterialName"].as<string>());
	_greenBorderMaterial = MaterialsManager::GetMaterial(node["greenBorderMaterialName"].as<string>());
	_purpleBorderMaterial = MaterialsManager::GetMaterial(node["purpleBorderMaterialName"].as<string>());
	_yellowBorderMaterial = MaterialsManager::GetMaterial(node["yellowBorderMaterialName"].as<string>());
	_cyanBorderMaterial = MaterialsManager::GetMaterial(node["cyanBorderMaterialName"].as<string>());
	_pinkBorderMaterial = MaterialsManager::GetMaterial(node["pinkBorderMaterialName"].as<string>());

	string s;
	_blueMaterials.reserve(node["blueMaterialsNames"].size());
	for (const auto& mat : node["blueMaterialsNames"]) {
		s = mat.as<string>();
		_blueMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_redMaterials.reserve(node["redMaterialsNames"].size());
	for (const auto& mat : node["redMaterialsNames"]) {
		s = mat.as<string>();
		_redMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_greenMaterials.reserve(node["greenMaterialsNames"].size());
	for (const auto& mat : node["greenMaterialsNames"]) {
		s = mat.as<string>();
		_greenMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_purpleMaterials.reserve(node["purpleMaterialsNames"].size());
	for (const auto& mat : node["purpleMaterialsNames"]) {
		s = mat.as<string>();
		_purpleMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_yellowMaterials.reserve(node["yellowMaterialsNames"].size());
	for (const auto& mat : node["yellowMaterialsNames"]) {
		s = mat.as<string>();
		_yellowMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_cyanMaterials.reserve(node["cyanMaterialsNames"].size());
	for (const auto& mat : node["cyanMaterialsNames"]) {
		s = mat.as<string>();
		_cyanMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_pinkMaterials.reserve(node["pinkMaterialsNames"].size());
	for (const auto& mat : node["pinkMaterialsNames"]) {
		s = mat.as<string>();
		_pinkMaterials.emplace_back(MaterialsManager::GetMaterial(s));
	}
	
	// Retrieving RetreatingMaterials

	_blueRetreatingMaterials.reserve(node["blueRetreatingMaterialsNames"].size());
	for (const auto& mat : node["blueRetreatingMaterialsNames"]) {
		s = mat.as<string>();
		_blueRetreatingMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_redRetreatingMaterials.reserve(node["redRetreatingMaterialsNames"].size());
	for (const auto& mat : node["redRetreatingMaterialsNames"]) {
		s = mat.as<string>();
		_redRetreatingMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_greenRetreatingMaterials.reserve(node["greenRetreatingMaterialsNames"].size());
	for (const auto& mat : node["greenRetreatingMaterialsNames"]) {
		s = mat.as<string>();
		_greenRetreatingMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_purpleRetreatingMaterials.reserve(node["purpleRetreatingMaterialsNames"].size());
	for (const auto& mat : node["purpleRetreatingMaterialsNames"]) {
		s = mat.as<string>();
		_purpleRetreatingMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_yellowRetreatingMaterials.reserve(node["yellowRetreatingMaterialsNames"].size());
	for (const auto& mat : node["yellowRetreatingMaterialsNames"]) {
		s = mat.as<string>();
		_yellowRetreatingMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_cyanRetreatingMaterials.reserve(node["cyanRetreatingMaterialsNames"].size());
	for (const auto& mat : node["cyanRetreatingMaterialsNames"]) {
		s = mat.as<string>();
		_cyanRetreatingMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	_pinkRetreatingMaterials.reserve(node["pinkRetreatingMaterialsNames"].size());
	for (const auto& mat : node["pinkRetreatingMaterialsNames"]) {
		s = mat.as<string>();
		_pinkRetreatingMaterials.push_back(MaterialsManager::GetMaterial(s));
	}

	return true;
}

Material* HexTileTextureData::GetMaterial(TILE_COLOR color, size_t stage)
{
	if (stage == 0 || color == TILE_COLOR::NEUTRAL) return _neutralMaterial;

	switch (color) {
		case TILE_COLOR::BLUE: {
			if (stage - 1 >= _blueMaterials.size()) return nullptr;
			return _blueMaterials[stage - 1];
		}
		case TILE_COLOR::RED: {
			if (stage - 1 >= _redMaterials.size()) return nullptr;
			return _redMaterials[stage - 1];
		}
		case TILE_COLOR::GREEN: {
			if (stage - 1 >= _greenMaterials.size()) return nullptr;
			return _greenMaterials[stage - 1];
		}
		case TILE_COLOR::PURPLE: {
			if (stage - 1 >= _purpleMaterials.size()) return nullptr;
			return _purpleMaterials[stage - 1];
		}
		case TILE_COLOR::YELLOW: {
			if (stage - 1 >= _yellowMaterials.size()) return nullptr;
			return _yellowMaterials[stage - 1];
		}
		case TILE_COLOR::CYAN: {
			if (stage - 1 >= _cyanMaterials.size()) return nullptr;
			return _cyanMaterials[stage - 1];
		}
		case TILE_COLOR::PINK: {
			if (stage - 1 >= _pinkMaterials.size()) return nullptr;
			return _pinkMaterials[stage - 1];
		}
		default: {
			return nullptr;
		}
	}
	return nullptr;
}

Material* HexTileTextureData::GetRetreatingMaterial(TILE_COLOR color, size_t stage)
{
	if (stage == 0 || color == TILE_COLOR::NEUTRAL) return _neutralMaterial;

	switch (color) {
	case TILE_COLOR::BLUE: {
		if (stage - 1 >= _blueRetreatingMaterials.size()) return nullptr;
		return _blueRetreatingMaterials[stage - 1];
	}
	case TILE_COLOR::RED: {
		if (stage - 1 >= _redRetreatingMaterials.size()) return nullptr;
		return _redRetreatingMaterials[stage - 1];
	}
	case TILE_COLOR::GREEN: {
		if (stage - 1 >= _greenRetreatingMaterials.size()) return nullptr;
		return _greenRetreatingMaterials[stage - 1];
	}
	case TILE_COLOR::PURPLE: {
		if (stage - 1 >= _purpleRetreatingMaterials.size()) return nullptr;
		return _purpleRetreatingMaterials[stage - 1];
	}
	case TILE_COLOR::YELLOW: {
		if (stage - 1 >= _yellowRetreatingMaterials.size()) return nullptr;
		return _yellowRetreatingMaterials[stage - 1];
	}
	case TILE_COLOR::CYAN: {
		if (stage - 1 >= _cyanRetreatingMaterials.size()) return nullptr;
		return _cyanRetreatingMaterials[stage - 1];
	}
	case TILE_COLOR::PINK: {
		if (stage - 1 >= _pinkRetreatingMaterials.size()) return nullptr;
		return _pinkRetreatingMaterials[stage - 1];
	}
	default: {
		return nullptr;
	}
	}
	return nullptr;
}

Twin2Engine::Graphic::Material* HexTileTextureData::GetBorderMaterial(TILE_COLOR color)
{
	switch (color) {
	case TILE_COLOR::NEUTRAL:
		return _neutralBorderMaterial;
	case TILE_COLOR::BLUE:
		return _blueBorderMaterial;
	case TILE_COLOR::RED:
		return _redBorderMaterial;
	case TILE_COLOR::GREEN:
		return _greenBorderMaterial;
	case TILE_COLOR::PURPLE:
		return _purpleBorderMaterial;
	case TILE_COLOR::YELLOW:
		return _yellowBorderMaterial;
	case TILE_COLOR::CYAN:
		return _cyanBorderMaterial;
	case TILE_COLOR::PINK:
		return _pinkBorderMaterial;
	default:
		return nullptr;
	}
	return nullptr;
}

/*
SO_SERIALIZATION_BEGIN(HexTileTextureData, ScriptableObject)
SO_SERIALIZE_FIELD(_materialNames)
SO_SERIALIZATION_END()
*/

/*
SO_DESERIALIZATION_BEGIN(HexTileTextureData, ScriptableObject)
//SO_DESERIALIZE_FIELD_F_T_R("_materialNames", _materials, DeserializationHelperMaterialNamesToMaterials, vector<vector<string>>)
SO_DESERIALIZE_T_R("_materialNames", DeserializationHelperMaterialNamesToMaterials, vector<vector<string>>)
SO_DESERIALIZATION_END()
*/

/*
bool HexTileTextureData::Deserialize(const YAML::Node& node)
{
	if (!ScriptableObject::Deserialize(node)) return false;

	if (!node["_materialNames"]) return false;

	for (auto i : node["_materialNames"]) {

		vector<string> names;
		for (auto z : i) {
			names.push_back(z.as<string>());
		}

		_materialNames.push_back(names);
	}

	DeserializationHelperMaterialNamesToMaterials();

	//if (node["_materialNames"]) DeserializationHelperMaterialNamesToMaterials(node["_materialNames"].as<vector<vector<string>>>());
	//else return false;

	return true;
}
*/

/*
vector<vector<Material*>> HexTileTextureData::DeserializationHelperMaterialNamesToMaterials(vector<vector<string>> materialNames)
{
	_materialNames = std::move(materialNames);
	
	vector<vector<Material*>> ret = vector<vector<Material*>>();

	for (size_t i = 0; i < _materialNames.size(); ++i)
	{
		vector<Material*> materials;

		for (size_t j = 0; j < _materialNames[i].size(); ++j)
		{
			materials.push_back(MaterialsManager::GetMaterial(_materialNames[i][j]));
		}

		ret.push_back(std::move(materials));
	}

	return ret;
}

void HexTileTextureData::DeserializationHelperMaterialNamesToMaterials(vector<vector<string>> materialNames)
{
	_materialNames = std::move(materialNames);

	for (size_t i = 0; i < _materialNames.size(); ++i)
	{
		vector<Material*> materials;

		for (size_t j = 0; j < _materialNames[i].size(); ++j)
		{
			materials.push_back(MaterialsManager::GetMaterial(_materialNames[i][j]));
		}

		_materials.push_back(std::move(materials));
		materials.clear();
	}

	materialNames.clear();
}
*/

/*
void HexTileTextureData::DeserializationHelperMaterialNamesToMaterials()
{
	for (size_t i = 0; i < _materialNames.size(); ++i)
	{
		vector<Material*> materials;

		for (size_t j = 0; j < _materialNames[i].size(); ++j)
		{
			materials.push_back(MaterialsManager::GetMaterial(_materialNames[i][j]));
		}

		_materials.push_back(std::move(materials));
		materials.clear();
	}
}
*/