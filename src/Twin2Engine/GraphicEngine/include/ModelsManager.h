#ifndef _MODELS_MANAGER_H_
#define _MODELS_MANAGER_H_

//#include <Model.h>
#include <InstatiatingModel.h>
#include <ModelData.h>
#include <Vertex.h>

//#include <glad/glad.h>



#include <list>
#include <string>

using std::vector;

#define ASSIMP_LOADING true
#define TINYGLTF_LOADING false

#if ASSIMP_LOADING

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#elif TINYGLTF_LOADING

#include <tiny_gltf.h>

#endif

namespace Twin2Engine::GraphicEngine
{
	class InstatiatingModel;
	struct ModelData;

	class ModelsManager
	{
		friend class InstatiatingModel;
		//friend struct ModelData;

		

		//static GLenum binaryFormat;

		static std::hash<std::string> stringHash;
		static std::list<ModelData*> loadedModels;

#if ASSIMP_LOADING
		static inline void LoadModelAssimp(const std::string& modelPath, ModelData* modelData);
		static inline void ExtractMeshAssimp(const aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
#elif TINYGLTF_LOADING
		static inline void LoadModelGLTF(const std::string& modelPath, ModelData* modelData);
		static inline void ExtractMeshGLTF(const tinygltf::Mesh& mesh, const tinygltf::Model& model, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
#endif

		static ModelData* LoadModel(const std::string& modelPath);
		static void UnloadModel(ModelData* modelData);


	public:
		static void Init();
		static void End();

		static InstatiatingModel GetModel(const std::string& modelPath);

		static InstatiatingModel CreateModel(const std::string& modelName, vector<Vertex> vertices, vector<unsigned int> indices); //, vector<Texture> textures
		//static void FreeModel(InstatiatingModel*& model);

		
	};
}


#endif