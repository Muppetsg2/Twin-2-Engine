#ifndef _MODEL_DATA_H_
#define _MODEL_DATA_H_

#include <graphic/InstatiatingMesh.h>

namespace Twin2Engine::GraphicEngine
{
    struct ModelData
    {
        size_t id;
        std::vector<InstatiatingMesh*> meshes;
    };

    //bool operator<(const ModelData& data1, const ModelData& data2)
    //{
    //    return data1.modelHash < data2.modelHash;
    //}
}

#endif