#ifndef _MODELS_MANAGER_H_
#define _MODELS_MANAGER_H_

#include <graphic/InstatiatingModel.h>

namespace Twin2Engine::Manager
{
	class ModelsManager
	{
		friend class GraphicEngine::InstatiatingModel;

		static GLenum binaryFormat;

		static std::hash<std::string> stringHash;
		static std::list<GraphicEngine::ModelData*> loadedModels;

		static GraphicEngine::ModelData* _cubeModel;
		static GraphicEngine::ModelData* _sphereModel;
		static GraphicEngine::ModelData* _planeModel;
		static GraphicEngine::ModelData* _piramidModel;

		static GraphicEngine::ModelData* LoadModel(const std::string& modelPath);
		static void UnloadModel(GraphicEngine::ModelData* modelData);


	public:
		static void Init();
		static void End();

		static GraphicEngine::InstatiatingModel GetModel(const std::string& modelPath);
		static GraphicEngine::InstatiatingModel GetCube();
		static GraphicEngine::InstatiatingModel GetSphere();
		static GraphicEngine::InstatiatingModel GetPlane();
		static GraphicEngine::InstatiatingModel GetPiramid();

		static GraphicEngine::InstatiatingModel CreateModel(const std::string& modelName, vector<GraphicEngine::Vertex> vertices, vector<unsigned int> indices, vector<GraphicEngine::Texture> textures);
		//static void FreeModel(InstatiatingModel*& model);


	};
}


#endif