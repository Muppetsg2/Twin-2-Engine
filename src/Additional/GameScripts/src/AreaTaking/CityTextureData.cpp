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

	_neutralMaterialPath = node["neutralMaterialName"].as<string>();
	_blueMaterialPath = node["blueMaterialName"].as<string>();
	_redMaterialPath = node["redMaterialName"].as<string>();
	_greenMaterialPath = node["greenMaterialName"].as<string>();
	_purpleMaterialPath = node["purpleMaterialName"].as<string>();
	_yellowMaterialPath = node["yellowMaterialName"].as<string>();
	_cyanMaterialPath = node["cyanMaterialName"].as<string>();
	_pinkMaterialPath = node["pinkMaterialName"].as<string>();

	_neutralMaterial = MaterialsManager::GetMaterial(_neutralMaterialPath);
	_blueMaterial = MaterialsManager::GetMaterial(_blueMaterialPath);
	_cyanMaterial = MaterialsManager::GetMaterial(_cyanMaterialPath);
	_greenMaterial = MaterialsManager::GetMaterial(_greenMaterialPath);
	_yellowMaterial = MaterialsManager::GetMaterial(_yellowMaterialPath);
	_redMaterial = MaterialsManager::GetMaterial(_redMaterialPath);
	_pinkMaterial = MaterialsManager::GetMaterial(_pinkMaterialPath);
	_purpleMaterial = MaterialsManager::GetMaterial(_purpleMaterialPath);

	return true;
}

Material* CityTextureData::GetMaterial(TILE_COLOR color)
{
	switch (color) {
	case TILE_COLOR::NEUTRAL:
		if (!_neutralMaterial)
		{
			_neutralMaterial = MaterialsManager::GetMaterial(_neutralMaterialPath);
		}
		return _neutralMaterial;

	case TILE_COLOR::BLUE:
		if (!_blueMaterial)
		{
			_blueMaterial = MaterialsManager::GetMaterial(_blueMaterialPath);
		}
		return _blueMaterial;

	case TILE_COLOR::RED:
		if (!_redMaterial)
		{
			_redMaterial = MaterialsManager::GetMaterial(_redMaterialPath);
		}
		return _redMaterial;

	case TILE_COLOR::GREEN:
		if (!_greenMaterial)
		{
			_greenMaterial = MaterialsManager::GetMaterial(_greenMaterialPath);
		}
		return _greenMaterial;

	case TILE_COLOR::PURPLE:
		if (!_purpleMaterial)
		{
			_purpleMaterial = MaterialsManager::GetMaterial(_purpleMaterialPath);
		}
		return _purpleMaterial;

	case TILE_COLOR::YELLOW:
		if (!_yellowMaterial)
		{
			_yellowMaterial = MaterialsManager::GetMaterial(_yellowMaterialPath);
		}
		return _yellowMaterial;

	case TILE_COLOR::CYAN:
		if (!_cyanMaterial)
		{
			_cyanMaterial = MaterialsManager::GetMaterial(_cyanMaterialPath);
		}
		return _cyanMaterial;

	case TILE_COLOR::PINK:
		if (!_pinkMaterial)
		{
			_pinkMaterial = MaterialsManager::GetMaterial(_pinkMaterialPath);
		}
		return _pinkMaterial;

	default:
		return nullptr;
	}
	return nullptr;
}