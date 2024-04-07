#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <graphic/Shader.h>
#include <graphic/MaterialParameters.h>

namespace Twin2Engine
{
	namespace Manager {
		class MaterialsManager;
	}

	//class MaterialParameters
	//{
	//	std::unordered_map<size_t, std::vector<char>> _variablesValuesMappings;
	//
	//public:
	//	MaterialParameters(const std::vector<string>& variableNames)
	//	{
	//		std::hash<string> hasher;
	//
	//		for (int i = 0; i < variableNames.size(); i++)
	//		{
	//			_variablesValuesMappings[hasher(variableNames[i])];
	//		}
	//
	//	}
	//
	//	template<class T>
	//	bool Set(const std::string& variableName, T value)
	//	{
	//		std::hash<string> hasher;
	//		size_t hashed = hasher(variableName);
	//
	//		//auto it = _variablesValuesMappings.find(hashed);
	//
	//		//if (it != _variablesValuesMappings.end())
	//		if (_variablesValuesMappings.contains(hashed))
	//		{
	//			const char* ptr = reinterpret_cast<const char*>(&value);
	//			std::vector<char> result(ptr, ptr + sizeof(value));
	//			//_variablesValuesMappings[hashed] = *(reinterpret_cast<std::vector<char>*>(&value));
	//			_variablesValuesMappings[hashed] = result;
	//			return true;
	//		}
	//		return false;
	//	}
	//	
	//	std::vector<char> GetData() const
	//	{
	//		size_t totalSize = 0;
	//		for (const auto& pair : _variablesValuesMappings) {
	//			totalSize += pair.second.size();
	//		}
	//
	//		// Create the flattened vector with the appropriate size
	//		std::vector<char> flattenedVector(totalSize);
	//
	//		// Copy the vectors from the map into the flattened vector
	//		size_t offset = 0;
	//		for (const auto& pair : _variablesValuesMappings) {
	//			const auto& vector = pair.second;
	//			std::copy(vector.begin(), vector.end(), flattenedVector.begin() + offset);
	//			offset += vector.size();
	//		}
	//
	//		return flattenedVector;
	//	}
	//};

	namespace GraphicEngine {

		struct MaterialData
		{
			size_t id;
			int useNumber;
			Shader* shader;
			MaterialParameters* materialParameters;
		};

		class Material
		{
			friend class Manager::MaterialsManager;
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
			MaterialParameters* GetMaterialParameters() const;
		};

		bool operator<(const Material& material1, const Material& material2);
	}
}

#endif


