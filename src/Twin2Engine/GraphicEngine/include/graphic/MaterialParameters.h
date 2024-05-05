#ifndef _MATERIAL_PARAMETERS_H_
#define _MATERIAL_PARAMETERS_H_

#include <core/YamlConverters.h>
#include <graphic/STD140Struct.h>

namespace Twin2Engine
 {
	namespace Manager {
		class MaterialsManager;
	}

	namespace GraphicEngine {
		class MaterialParametersBuilder;

		class MaterialParameters
		{
			friend class Manager::MaterialsManager;
			friend class MaterialParametersBuilder;

			static std::hash<std::string> hasher;

			GLuint _materialParametersDataUBO;

			STD140Struct _parameters;

			std::map<size_t, char> _textureMappings;
			std::vector<GLuint> _textures;


			MaterialParameters();
			MaterialParameters(const STD140Struct& parameters, const std::map<size_t, char>& textureMappings, const std::vector<GLuint>& textures);

		public:
			template<class T>
			typename std::enable_if_t<is_in_v<T, bool, int, unsigned int, float, double,
									glm::vec2, glm::vec3, glm::vec4,
									glm::ivec2, glm::ivec3, glm::ivec4>, bool>
			Set(const std::string& variableName, const T& value) {
				return _parameters.Set(variableName, value);
			}


			~MaterialParameters();

			template<class T>
			typename std::enable_if_t<is_in_v<T, bool, int, unsigned int, float, double,
									glm::vec2, glm::vec3, glm::vec4,
									glm::ivec2, glm::ivec3, glm::ivec4>, T>
			Get(const std::string& variableName) {
				return _parameters.Get<T>(variableName);
			}


			void SetTexture2D(const std::string& textureName, unsigned int textureId);
			void UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded);

			GLuint GetDataUBO() const;
			const char* GetData() const;
			size_t GetSize() const;
		};
	}
}

#endif