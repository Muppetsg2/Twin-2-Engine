#pragma once

#include <graphic/Vertex.h>

#define MESH_FRUSTUM_CULLING
#include <physic/BoundingVolume.h>

namespace Twin2Engine::Graphic
{
    class InstatiatingMesh
    {
        /*  dane klasy Mesh  */
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;

        GLuint _VAO;
        GLuint _VBO;
        GLuint _EBO;

    public:
        InstatiatingMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        virtual ~InstatiatingMesh();

#ifdef MESH_FRUSTUM_CULLING
        Physic::BoundingVolume* sphericalBV = nullptr;
        bool IsOnFrustum(Frustum& frustum, glm::mat4 model);
#endif // MESH_FRUSTUM_CULLING

        void Draw(unsigned int number);
    };
}