#ifndef _MODEL_DATA_H_
#define _MODEL_DATA_H_

#include <graphic/InstatiatingMesh.h>

namespace Twin2Engine::Graphic
{
    struct ModelData
    {
        size_t id;
        std::vector<InstatiatingMesh*> meshes;
    };
}

#endif