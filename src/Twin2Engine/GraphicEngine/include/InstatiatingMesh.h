#ifndef _INSTATIATING_MESH_H_
#define _INSTATIATING_MESH_H_

//#include <Mesh.h>
#include <Shader.h>
#include <Vertex.h>

#include <vector>

#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

//using std::vector;

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

        //Mesh* mesh;
        //unsigned int instancesTransformsVBO;
        //unsigned int amount;


    public:
        //InstatiatingMesh(Mesh* mesh, GLuint instanceDataUBO);
        InstatiatingMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

        void SetInstanceDataSSBO(GLuint instanceDataSSBO);
        void SetMaterialIndexSSBO(GLuint materialIndexSSBO);
        void SetMaterialInputUBO(GLuint materialInputUBO);

        void Draw(Shader* shader, unsigned int number);
    };

}

#endif // !_INSTATIATING_MESH_H_
