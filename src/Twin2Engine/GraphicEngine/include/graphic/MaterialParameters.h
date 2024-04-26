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
		class MaterialParameters
		{
			friend class Manager::MaterialsManager;

			std::vector<char> _materialData;
			std::map<size_t, unsigned int> _variablesValuesOffsets;

			STD140Struct _variables;

			std::map<size_t, char> _textureMappings;
			std::vector<GLuint> _textures;

			static std::hash<std::string> hasher;

			MaterialParameters();
			template<class T>
			std::enable_if_t<is_in_v<T, bool, int, unsigned int, float, double, 
							glm::vec2, glm::vec3, glm::vec4,
							glm::ivec2, glm::ivec3, glm::ivec4>>
			Add(const std::string& variableName, const T& value) {
				_variables.Add(variableName, value);
			}

			void AddTexture2D(const std::string& textureName, unsigned int textureId);

		public:
			//MaterialParameters(const std::vector<std::string>& variableNames, const std::vector<unsigned int>& parametersSizes, const std::vector<std::string>& textureParametersNames);

			template<class T>
			typename std::enable_if_t<is_in_v<T, bool, int, unsigned int, float, double,
									glm::vec2, glm::vec3, glm::vec4,
									glm::ivec2, glm::ivec3, glm::ivec4>, bool>
			Set(const std::string& variableName, const T& value) {
				return _variables.Set(variableName, value);
			}


			template<class T>
			typename std::enable_if_t<is_in_v<T, bool, int, unsigned int, float, double,
									glm::vec2, glm::vec3, glm::vec4,
									glm::ivec2, glm::ivec3, glm::ivec4>, T>
			Get(const std::string& variableName) {
				return _variables.Get<T>(variableName);
			}


			void SetTexture2D(const std::string& textureName, unsigned int textureId);
			void UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded);

			std::vector<char> GetData() const;
		};
	}
}

#endif