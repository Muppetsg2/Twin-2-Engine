#ifndef INSTATIATING_MESH
#define INSTATIATING_MESH

#include <Mesh.h>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::vector;

namespace GraphicEngine
{

    class InstatiatingMesh
    {
        Mesh* mesh;
        unsigned int instancesTransformsVBO;
        unsigned int amount;

        GLuint _instanceDataUBO;
        GLuint _materialInputUBO;

        void setupMesh(const vector<glm::mat4>& transforms);

    public:
        InstatiatingMesh(Mesh* mesh, GLuint instanceDataUBO);

        void SetInstanceDataUBO(GLuint instanceDataUBO);
        void SetMaterialInputUBO(GLuint materialInputUBO);

        void Draw(Shader* shader, unsigned int number);
    };

}

#endif // !INSTATIATING_MESH
