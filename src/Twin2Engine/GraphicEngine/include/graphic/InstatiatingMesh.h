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

    public:
        InstatiatingMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        virtual ~InstatiatingMesh();

        void Draw(unsigned int number);
    };
}

#endif // !_INSTATIATING_MESH_H_
