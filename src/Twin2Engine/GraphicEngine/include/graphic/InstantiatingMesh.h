#pragma once

#include <graphic/Vertex.h>

#define MESH_FRUSTUM_CULLING
#include <BoundingVolume.h>

//namespace Twin2Engine::Manager {
//    class ModelsManager;
//}

namespace Twin2Engine::GraphicEngine
{
    class InstantiatingMesh
    {
        /*  dane klasy Mesh  */
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;

        GLuint _VAO;
        GLuint _VBO;
        GLuint _EBO;

    public:
        InstantiatingMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        virtual ~InstantiatingMesh();

#ifdef MESH_FRUSTUM_CULLING
        CollisionSystem::BoundingVolume* sphericalBV = nullptr;
        bool IsOnFrustum(Frustum& frustum, glm::mat4 model);
#endif // MESH_FRUSTUM_CULLING

        void Draw(unsigned int number);
    };
}