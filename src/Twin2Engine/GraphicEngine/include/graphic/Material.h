#pragma once

#include <graphic/Shader.h>
#include <graphic/MaterialParameters.h>

namespace Twin2Engine
{
	namespace Manager 
	{
		class MaterialsManager;
	}

	namespace Graphic {

		struct MaterialData
		{
			size_t id;
			Shader* shader;
			MaterialParameters* materialParameters;
		};

		class Material
		{
			friend class Manager::MaterialsManager;

			MaterialData* _materialData;

			Material(MaterialData* materialData);

		public:
			Material(const Material& other);
			Material(Material&& other);
			Material(std::nullptr_t);
			Material();

			~Material();

			Material& operator=(const Material& other);
			Material& operator=(Material&& other);
			Material& operator=(std::nullptr_t);
			bool operator==(std::nullptr_t);
			bool operator!=(std::nullptr_t);
			bool operator==(const Material& other);
			bool operator!=(const Material& other);

			Shader* GetShader() const;
			size_t GetId() const;
			MaterialParameters* GetMaterialParameters() const;

#if _DEBUG
			void DrawEditor();
#endif
		};

		bool operator<(const Material& material1, const Material& material2);
	}
	
}