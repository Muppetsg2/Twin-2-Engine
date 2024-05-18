#pragma once

#include <graphic/Vertex.h>

#define MESH_FRUSTUM_CULLING
#include <physic/BoundingVolume.h>

namespace Twin2Engine::Graphic
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
        InstantiatingMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
        virtual ~InstantiatingMesh();

#ifdef MESH_FRUSTUM_CULLING
        Physic::BoundingVolume* SphericalBV = nullptr;
        bool IsOnFrustum(Frustum& frustum, glm::mat4 model);
#endif // MESH_FRUSTUM_CULLING

        void Draw(unsigned int number);
    };
}