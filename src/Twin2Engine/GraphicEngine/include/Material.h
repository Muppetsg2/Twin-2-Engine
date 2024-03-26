#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <Shader.h>

#include <MaterialsManager.h>

#include <unordered_map>

namespace Twin2Engine::GraphicEngine
{
	class MaterialValues
	{
		std::unordered_map<size_t, std::vector<char>> _variablesValuesMappings;

	public:
		MaterialValues(const std::vector<string>& variableNames)
		{
			std::hash<string> hasher;

			for (int i = 0; i < variableNames.size(); i++)
			{
				_variablesValuesMappings[hasher(variableNames[i])];
			}

		}

		template<class T>
		bool Set(const std::string& variableName, T value)
		{
			std::hash<string> hasher;
			size_t hashed = hasher(variableName);

			//auto it = _variablesValuesMappings.find(hashed);

			//if (it != _variablesValuesMappings.end())
			if (_variablesValuesMappings.contains(hashed))
			{
				const char* ptr = reinterpret_cast<const char*>(&value);
				std::vector<char> result(ptr, ptr + sizeof(value));
				//_variablesValuesMappings[hashed] = *(reinterpret_cast<std::vector<char>*>(&value));
				_variablesValuesMappings[hashed] = result;
				return true;
			}
			return false;
		}
		
		std::vector<char> GetData() const
		{
			size_t totalSize = 0;
			for (const auto& pair : _variablesValuesMappings) {
				totalSize += pair.second.size();
			}

			// Create the flattened vector with the appropriate size
			std::vector<char> flattenedVector(totalSize);

			// Copy the vectors from the map into the flattened vector
			size_t offset = 0;
			for (const auto& pair : _variablesValuesMappings) {
				const auto& vector = pair.second;
				std::copy(vector.begin(), vector.end(), flattenedVector.begin() + offset);
				offset += vector.size();
			}

			return flattenedVector;
		}
	};

	struct MaterialData
	{
		size_t id;
		int useNumber;
		Shader* shader;
		MaterialValues* materialParameters;
	};

	class Material
	{
		friend class MaterialsManager;
		//size_t id;


		//Shader* _shader;
		MaterialData* _materialData;

		//Material(Shader* shader);
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
		//bool operator<(const Material& other);

		Shader* GetShader() const;
		unsigned int GetId() const;
		MaterialValues* GetMaterialParameters() const;
	};

	bool operator<(const Material& material1, const Material& material2);

}

#endif


