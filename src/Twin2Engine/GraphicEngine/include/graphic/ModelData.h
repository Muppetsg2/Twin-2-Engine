#pragma once

#include <graphic/InstantiatingMesh.h>

namespace Twin2Engine::GraphicEngine
{
    struct ModelData
    {
        size_t id;
        std::vector<InstantiatingMesh*> meshes;
    };

    //bool operator<(const ModelData& data1, const ModelData& data2)
    //{
    //    return data1.modelHash < data2.modelHash;
    //}
}