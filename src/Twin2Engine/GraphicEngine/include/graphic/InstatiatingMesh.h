#ifndef _INSTATIATING_MESH_H_
#define _INSTATIATING_MESH_H_

#include <graphic/Vertex.h>

//namespace Twin2Engine::Manager {
//    class ModelsManager;
//}

namespace Twin2Engine::GraphicEngine
{
    class InstatiatingMesh
    {
        /*  dane klasy Mesh  */
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;

        GLuint _VAO;
        GLuint _VBO;
        GLuint _EBO;

        GLuint _instanceDataSSBO;
        GLuint _materialIndexSSBO;
        GLuint _materialInputUBO;

    public:
        InstatiatingMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        virtual ~InstatiatingMesh();

        void SetInstanceDataSSBO(GLuint instanceDataSSBO);
        void SetMaterialIndexSSBO(GLuint materialIndexSSBO);
        void SetMaterialInputUBO(GLuint materialInputUBO);

        void Draw(unsigned int number);
    };
}

#endif // !_INSTATIATING_MESH_H_
