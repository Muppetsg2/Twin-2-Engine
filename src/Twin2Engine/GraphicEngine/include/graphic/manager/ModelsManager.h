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
		friend class Graphic::InstantiatingMesh;
		friend class SceneManager;
		friend class PrefabManager;

		static GLenum binaryFormat;

		static std::hash<std::string> _stringHash;
		static std::map<size_t, Graphic::ModelData*> _loadedModels;

		static std::map<size_t, std::string> _modelsPaths;

#if ASSIMP_LOADING
		static inline void LoadModelAssimp(const std::string& modelPath, Graphic::ModelData* modelData);
		static inline void ExtractMeshAssimp(const aiMesh* mesh, std::vector<Graphic::Vertex>& vertices, std::vector<unsigned int>& indices);
#elif TINYGLTF_LOADING
		static inline void LoadModelGLTF(const std::string& modelPath, ModelData* modelData);
		static inline void ExtractMeshGLTF(const tinygltf::Mesh& mesh, const tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
#endif

		static void LoadCube(Graphic::ModelData* modelData);
		static void LoadPlane(Graphic::ModelData* modelData);
		static void LoadSphere(Graphic::ModelData* modelData);
		static void LoadTorus(Graphic::ModelData* modelData);
		static void LoadCone(Graphic::ModelData* modelData);
		static void LoadPiramid(Graphic::ModelData* modelData);
		static void LoadTetrahedron(Graphic::ModelData* modelData);
		static void LoadCylinder(Graphic::ModelData* modelData);
		static void LoadHexagon(Graphic::ModelData* modelData);

		static Graphic::ModelData* LoadModelData(const std::string& modelPath);
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

		static std::pair<glm::vec3, glm::vec3> CalcTangentBitangent(std::vector<Graphic::Vertex>& vertices, unsigned int i1, unsigned int i2, unsigned int i3);
		static void GenerateCircle(std::vector<Graphic::Vertex>& vertices, std::vector<unsigned int>& indices, unsigned int segments, float y = 0.f, unsigned int cullFace = GL_CCW);

	public:
		static Graphic::InstantiatingModel LoadModel(const std::string& modelPath);
		static Graphic::InstantiatingModel GetModel(size_t managerId);
		static Graphic::InstantiatingModel GetCube();
		static Graphic::InstantiatingModel GetPlane();
		static Graphic::InstantiatingModel GetSphere();
		static Graphic::InstantiatingModel GetTorus();
		static Graphic::InstantiatingModel GetCone();
		static Graphic::InstantiatingModel GetPiramid();
		static Graphic::InstantiatingModel GetTetrahedron();
		static Graphic::InstantiatingModel GetCylinder();
		static Graphic::InstantiatingModel GetHexagon();

		static Graphic::InstantiatingModel CreateModel(const std::string & modelName, std::vector<Graphic::Vertex> vertices, std::vector<unsigned int> indices);
		//static void FreeModel(InstantiatingModel*& model);

		static YAML::Node Serialize();
		static void DrawEditor(bool* p_open);

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