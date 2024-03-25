#ifndef _MODELS_MANAGER_H_
#define _MODELS_MANAGER_H_

#include <Model.h>
#include <InstatiatingModel.h>
#include <ModelData.h>

#include <glad/glad.h>

#include <list>
#include <string>

namespace GraphicEngine
{
	class InstatiatingModel;
	struct ModelData;

	class ModelsManager
	{
		friend class InstatiatingModel;
		//friend struct ModelData;

		

		static GLenum binaryFormat;

		static std::hash<std::string> stringHash;
		static std::list<ModelData*> loadedModels;

		static ModelData* LoadModel(const std::string& modelPath);
		static void UnloadModel(ModelData* modelData);


	public:
		static void Init();
		static void End();

		static InstatiatingModel GetModel(const std::string& modelPath);

		static InstatiatingModel CreateModel(const std::string& modelName, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
		//static void FreeModel(InstatiatingModel*& model);

		
	};
}


#endif