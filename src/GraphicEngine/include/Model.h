#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"

#include <ModelsManager.h>

#include <vector>
#include <iostream>

using namespace std;


#include "stb_image.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GraphicEngine
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