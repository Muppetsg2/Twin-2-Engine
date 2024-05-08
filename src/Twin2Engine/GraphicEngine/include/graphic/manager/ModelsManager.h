#pragma once

#include <graphic/InstantiatingModel.h>
#include <graphic/ModelData.h>
#include <graphic/Vertex.h>
#include <vector>
#include <utility>

#define ASSIMP_LOADING true
#define TINYGLTF_LOADING false

namespace Twin2Engine::Manager
{
	class SceneManager;
	class PrefabManager;

	class ModelsManager
	{
		friend class GraphicEngine::InstantiatingModel;
		friend class SceneManager;
		friend class PrefabManager;

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
		static void LoadPlane(GraphicEngine::ModelData* modelData);
		static void LoadSphere(GraphicEngine::ModelData* modelData);
		static void LoadTorus(GraphicEngine::ModelData* modelData);
		static void LoadCone(GraphicEngine::ModelData* modelData);
		static void LoadPiramid(GraphicEngine::ModelData* modelData);
		static void LoadTetrahedron(GraphicEngine::ModelData* modelData);
		static void LoadCylinder(GraphicEngine::ModelData* modelData);
		static void LoadHexagon(GraphicEngine::ModelData* modelData);

		static GraphicEngine::ModelData* LoadModelData(const std::string& modelPath);
		static void UnloadModel(const std::string& path);
		static void UnloadModel(size_t managerId);
		static void UnloadCube();
		static void UnloadPlane();
		static void UnloadSphere();
		static void UnloadTorus();
		static void UnloadCone();
		static void UnloadPiramid();
		static void UnloadTetrahedron();
		static void UnloadCylinder();
		static void UnloadHexagon();

		static std::pair<glm::vec3, glm::vec3> CalcTangentBitangent(std::vector<GraphicEngine::Vertex> vertices, unsigned int i1, unsigned int i2, unsigned int i3);
		static void GenerateCircle(std::vector<GraphicEngine::Vertex> vertices, std::vector<unsigned int> indices, unsigned int segments, float y = 0.f, unsigned int cullFace = GL_CCW);

	public:
		static GraphicEngine::InstatiatingModel LoadModel(const std::string& modelPath);
		static GraphicEngine::InstatiatingModel GetModel(size_t managerId);
		static GraphicEngine::InstatiatingModel GetCube();
		static GraphicEngine::InstatiatingModel GetPlane();
		static GraphicEngine::InstatiatingModel GetSphere();
		static GraphicEngine::InstatiatingModel GetTorus();
		static GraphicEngine::InstatiatingModel GetCone();
		static GraphicEngine::InstatiatingModel GetPiramid();
		static GraphicEngine::InstatiatingModel GetTetrahedron();
		static GraphicEngine::InstatiatingModel GetCylinder();
		static GraphicEngine::InstatiatingModel GetHexagon();

		static GraphicEngine::InstatiatingModel CreateModel(const std::string& modelName, std::vector<GraphicEngine::Vertex> vertices, std::vector<unsigned int> indices);
		//static void FreeModel(InstatiatingModel*& model);

		static YAML::Node Serialize();

		static constexpr const char* CUBE_PATH = "{Cube}";
		static constexpr const char* PLANE_PATH = "{Plane}";
		static constexpr const char* SPHERE_PATH = "{Sphere}";
		static constexpr const char* TORUS_PATH = "{Torus}";
		static constexpr const char* CONE_PATH = "{Cone}";
		static constexpr const char* PIRAMID_PATH = "{Piramid}";
		static constexpr const char* TETRAHEDRON_PATH = "{Tetrahedron}";
		static constexpr const char* CYLINDER_PATH = "{Cylinder}";
		static constexpr const char* HEXAGON_PATH = "{Hexagon}";
	};
}