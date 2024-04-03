#include "InstatiatingMesh.h"
#include <InstatiatingMesh.h>

using namespace Twin2Engine::GraphicEngine;


//Twin2Engine::GraphicEngine::InstatiatingMesh::InstatiatingMesh(Mesh* mesh, GLuint instanceDataUBO)
//{
//    this->mesh = mesh;
//    SetInstanceDataSSBO(instanceDataUBO);
//}

Twin2Engine::GraphicEngine::InstatiatingMesh::InstatiatingMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    _vertices = vertices;
    _indices = indices;

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

    // pozycje wierzcho³ków
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // wektory normalne wierzcho³ków
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // wspó³rzedne tekstury wierzcho³ków
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Twin2Engine::GraphicEngine::InstatiatingMesh::SetInstanceDataSSBO(GLuint instanceDataSSBO)
{
    _instanceDataSSBO = instanceDataSSBO;
    //glBindVertexArray(mesh->_VAO);
    glBindVertexArray(_VAO);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instanceDataSSBO);

    glBindVertexArray(0);
}

void Twin2Engine::GraphicEngine::InstatiatingMesh::SetMaterialIndexSSBO(GLuint materialIndexSSBO)
{
    _materialIndexSSBO = materialIndexSSBO;
    //glBindVertexArray(mesh->_VAO);
    glBindVertexArray(_VAO);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materialIndexSSBO);

    glBindVertexArray(0);
}

void Twin2Engine::GraphicEngine::InstatiatingMesh::SetMaterialInputUBO(GLuint materialInputUBO)
{
    _materialInputUBO = materialInputUBO;
    //glBindVertexArray(mesh->_VAO);
    glBindVertexArray(_VAO);

    glBindBufferBase(GL_UNIFORM_BUFFER, 2, materialInputUBO);

    glBindVertexArray(0);

}


void Twin2Engine::GraphicEngine::InstatiatingMesh::Draw(Shader* shader, unsigned int number)
{
    // narysuj siatkê
    //glBindVertexArray(mesh->_VAO);
    glBindVertexArray(_VAO);

    //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, amount);
    //glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, number);
    glDrawElementsInstanced(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0, number);
    glBindVertexArray(0);
}