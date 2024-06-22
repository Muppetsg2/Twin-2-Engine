#include <AreaTaking/CityTextureData.h>
#include <graphic/manager/MaterialsManager.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace std;

SCRIPTABLE_OBJECT_SOURCE_CODE_NN(CityTextureData, "CityTextureData")

void CityTextureData::Clear() {

	_neutralMaterial = nullptr;
	_blueMaterial = nullptr;
	_redMaterial = nullptr;
	_greenMaterial = nullptr;
	_purpleMaterial = nullptr;
	_yellowMaterial = nullptr;
	_cyanMaterial = nullptr;
	_pinkMaterial = nullptr;
}

void CityTextureData::Serialize(YAML::Node& node) const
{
	ScriptableObject::Serialize(node);
	node["__SO_RegisteredName__"] = _registeredName;

	node["neutralMaterialName"] = MaterialsManager::GetMaterialPath(_neutralMaterial);

	node["blueMaterialName"] = MaterialsManager::GetMaterialPath(_blueMaterial);

	node["cyanMaterialName"] = MaterialsManager::GetMaterialPath(_cyanMaterial);

	node["greenMaterialName"] = MaterialsManager::GetMaterialPath(_greenMaterial);

	node["yellowMaterialName"] = MaterialsManager::GetMaterialPath(_yellowMaterial);

	node["redMaterialName"] = MaterialsManager::GetMaterialPath(_redMaterial);

	node["pinkMaterialName"] = MaterialsManager::GetMaterialPath(_pinkMaterial);

	node["purpleMaterialName"] = MaterialsManager::GetMaterialPath(_purpleMaterial);
}

bool CityTextureData::Deserialize(const YAML::Node& node)
{
	if (!node["neutralMaterialName"] || !node["blueMaterialName"] || !node["cyanMaterialName"] ||
		!node["greenMaterialName"] || !node["yellowMaterialName"] || !node["redMaterialName"] || 
		!node["pinkMaterialName"] || !node["purpleMaterialName"] || !ScriptableObject::Deserialize(node)) return false;

	_neutralMaterial = MaterialsManager::GetMaterial(node["neutralMaterialName"].as<string>());
	_blueMaterial = MaterialsManager::GetMaterial(node["blueMaterialName"].as<string>());
	_cyanMaterial = MaterialsManager::GetMaterial(node["cyanMaterialName"].as<string>());
	_greenMaterial = MaterialsManager::GetMaterial(node["greenMaterialName"].as<string>());
	_yellowMaterial = MaterialsManager::GetMaterial(node["yellowMaterialName"].as<string>());
	_redMaterial = MaterialsManager::GetMaterial(node["redMaterialName"].as<string>());
	_pinkMaterial = MaterialsManager::GetMaterial(node["pinkMaterialName"].as<string>());
	_purpleMaterial = MaterialsManager::GetMaterial(node["purpleMaterialName"].as<string>());

	return true;
}

Material* CityTextureData::GetMaterial(TILE_COLOR color)
{
	switch (color) {
	case TILE_COLOR::NEUTRAL:
		return _neutralMaterial;
	case TILE_COLOR::BLUE:
		return _blueMaterial;
	case TILE_COLOR::RED:
		return _redMaterial;
	case TILE_COLOR::GREEN:
		return _greenMaterial;
	case TILE_COLOR::PURPLE:
		return _purpleMaterial;
	case TILE_COLOR::YELLOW:
		return _yellowMaterial;
	case TILE_COLOR::CYAN:
		return _cyanMaterial;
	case TILE_COLOR::PINK:
		return _pinkMaterial;
	default:
		return nullptr;
	}
	return nullptr;
}