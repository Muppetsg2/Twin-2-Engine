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
		static std::map<size_t, GraphicEngine::ModelData*> loadedModels;

		static std::map<size_t, std::string> modelsPaths;

#if ASSIMP_LOADING
		static inline void LoadModelAssimp(const std::string& modelPath, GraphicEngine::ModelData* modelData);
		static inline void ExtractMeshAssimp(const aiMesh* mesh, std::vector<GraphicEngine::Vertex>& vertices, std::vector<unsigned int>& indices);
#elif TINYGLTF_LOADING
		static inline void LoadModelGLTF(const std::string& modelPath, ModelData* modelData);
		static inline void ExtractMeshGLTF(const tinygltf::Mesh& mesh, const tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
#endif

		static void LoadCube(GraphicEngine::ModelData* modelData);
		static void LoadSphere(GraphicEngine::ModelData* modelData);
		static void LoadPlane(GraphicEngine::ModelData* modelData);
		static void LoadPiramid(GraphicEngine::ModelData* modelData);

		static GraphicEngine::ModelData* LoadModelData(const std::string& modelPath);
		static void UnloadModel(const std::string& path);
		static void UnloadModel(size_t managerId);
		static void UnloadCube();
		static void UnloadSphere();
		static void UnloadPlane();
		static void UnloadPiramid();

		static std::pair<glm::vec3, glm::vec3> CalcTangentBitangent(std::vector<GraphicEngine::Vertex> vertices, unsigned int i1, unsigned int i2, unsigned int i3);

	public:
		static GraphicEngine::InstatiatingModel LoadModel(const std::string& modelPath);
		static GraphicEngine::InstatiatingModel GetModel(size_t managerId);
		static GraphicEngine::InstatiatingModel GetCube();
		static GraphicEngine::InstatiatingModel GetSphere();
		static GraphicEngine::InstatiatingModel GetPlane();
		static GraphicEngine::InstatiatingModel GetPiramid();

		static GraphicEngine::InstatiatingModel CreateModel(const std::string& modelName, std::vector<GraphicEngine::Vertex> vertices, std::vector<unsigned int> indices);
		//static void FreeModel(InstatiatingModel*& model);

		static constexpr const char* CUBE_PATH = "{Cube}";
		static constexpr const char* SPHERE_PATH = "{Sphere}";
		static constexpr const char* PLANE_PATH = "{Plane}";
		static constexpr const char* PIRAMID_PATH = "{Piramid}";
	};
}