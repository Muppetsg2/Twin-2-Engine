#pragma once

#include <graphic/InstatiatingModel.h>
#include <graphic/ModelData.h>
#include <graphic/Vertex.h>
#include <vector>
#include <utility>

#define ASSIMP_LOADING true
#define TINYGLTF_LOADING false

namespace Twin2Engine::Manager
{
	class SceneManager;

	class ModelsManager
	{
		friend class GraphicEngine::InstatiatingModel;
		friend class SceneManager;

		static GLenum binaryFormat;

		static std::hash<std::string> stringHash;
		static std::list<GraphicEngine::ModelData*> loadedModels;

		static GraphicEngine::ModelData* _cubeModel;
		static GraphicEngine::ModelData* _sphereModel;
		static GraphicEngine::ModelData* _planeModel;
		static GraphicEngine::ModelData* _piramidModel;

#if ASSIMP_LOADING
		static inline void LoadModelAssimp(const std::string& modelPath, GraphicEngine::ModelData* modelData);
		static inline void ExtractMeshAssimp(const aiMesh* mesh, std::vector<GraphicEngine::Vertex>& vertices, std::vector<unsigned int>& indices);
#elif TINYGLTF_LOADING
		static inline void LoadModelGLTF(const std::string& modelPath, ModelData* modelData);
		static inline void ExtractMeshGLTF(const tinygltf::Mesh& mesh, const tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
#endif

		static GraphicEngine::ModelData* LoadModel(const std::string& modelPath);
		static void UnloadModel(GraphicEngine::ModelData* modelData);

		static std::pair<glm::vec3, glm::vec3> CalcTangentBitangent(std::vector<GraphicEngine::Vertex> vertices, unsigned int i1, unsigned int i2, unsigned int i3);

	public:
		static void Init();
		static void End();

		static GraphicEngine::InstatiatingModel GetModel(const std::string& modelPath);
		static GraphicEngine::InstatiatingModel GetCube();
		static GraphicEngine::InstatiatingModel GetSphere();
		static GraphicEngine::InstatiatingModel GetPlane();
		static GraphicEngine::InstatiatingModel GetPiramid();

		static GraphicEngine::InstatiatingModel CreateModel(const std::string& modelName, std::vector<GraphicEngine::Vertex> vertices, std::vector<unsigned int> indices);
		//static void FreeModel(InstatiatingModel*& model);


	};
}