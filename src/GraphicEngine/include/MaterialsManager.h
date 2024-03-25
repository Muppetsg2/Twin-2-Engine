#ifndef _MATERIALS_MANAGER_H_
#define _MATERIALS_MANAGER_H_

#include <Material.h>
#include <ShaderManager.h>

namespace GraphicEngine
{
	class Material;
	struct MaterialData;

	class MaterialsManager
	{
		friend class Material;
		//static 
		static std::hash<std::string> stringHash;
		static std::list<MaterialData*> loadedMaterials;

		static void UnloadMaterial(Material& material);

	public:

		static Material GetMaterial(const std::string& name);
		static Material CreateMaterial(const std::string& newMaterialName, const std::string& shaderName, const std::vector<string>& materialParameters);
	};
}

#endif