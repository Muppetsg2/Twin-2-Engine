#ifndef _MODEL_DATA_H_
#define _MODEL_DATA_H_

#include <graphic/InstatiatingMesh.h>
#include <graphic/Model.h>

namespace Twin2Engine::GraphicEngine
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
        bool operator<(const ModelData& data2) const
        {
            return modelHash < data2.modelHash;
        }
    };

    //bool operator<(const ModelData& data1, const ModelData& data2)
    //{
    //    return data1.modelHash < data2.modelHash;
    //}
}

#endif