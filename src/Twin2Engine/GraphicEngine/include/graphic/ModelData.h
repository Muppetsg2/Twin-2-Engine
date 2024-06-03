#pragma once

#include <graphic/InstantiatingMesh.h>

namespace Twin2Engine::Graphic
{
    struct ModelData
    {
        size_t id;
        std::vector<InstantiatingMesh*> meshes;
    };
}
