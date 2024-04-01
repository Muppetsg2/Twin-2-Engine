#ifndef MODEL_H
#define MODEL_H

#include <graphic/Mesh.h>

#include <graphic/ModelsManager.h>

using namespace std;

// Mo�na u�y� TextureManagera skoro ju� go zrobi�em (Patryk)
//#include "stb_image.h"
//#include <stb_image.h>

namespace Twin2Engine::GraphicEngine
{
    class InstatiatingModel;
    class ModelsManager;


    class Model
    {
        friend class InstatiatingModel;
        friend class ModelsManager;
    public:
        /*  Funkcje   */
        Model(const char* path);
        Model(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
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

#include "InstatiatingModel.h"

#endif