#ifndef _MODEL_DATA_H_
#define _MODEL_DATA_H_

#include <InstatiatingMesh.h>
#include <Model.h>
#include <glad/glad.h>

#include <vector>

namespace GraphicEngine
{
    class Model;
    class InstatiatingMesh;

    struct ModelData
    {
        size_t modelHash;
        int useNumber;
        Model* model;
        GLuint ssboId;
        vector<InstatiatingMesh> meshes;

    };

    bool operator<(const ModelData& data1, const ModelData& data2)
    {
        return data1.modelHash < data2.modelHash;
    }
}

#endif