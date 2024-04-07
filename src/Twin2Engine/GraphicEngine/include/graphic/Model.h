#ifndef MODEL_H
#define MODEL_H

#include <graphic/Mesh.h>

using namespace std;

namespace Twin2Engine
{
    namespace Manager {
        class ModelsManager;
    }

    namespace GraphicEngine {
        class InstatiatingModel;

        class Model
        {
            friend class InstatiatingModel;
            friend class Manager::ModelsManager;
        public:
            /*  Funkcje   */
            Model(const char* path);
            Model(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
            virtual ~Model() = default;
            void Draw(Shader* shader);
        private:
            /*  Dane modelu  */
            vector<Mesh> meshes;
            vector<Texture> textures_loaded;
            string directory;
            /*  Funkcje   */
            void loadModel(string path);

            void processNode(aiNode* node, const aiScene* scene);

            Mesh processMesh(aiMesh* mesh, const aiScene* scene);

            vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                string typeName);

            unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);

        };
    }
}
#endif