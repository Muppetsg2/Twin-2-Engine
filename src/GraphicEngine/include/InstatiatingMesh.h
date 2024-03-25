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

        GLuint _ssboId;
        GLuint _uboId;

        void setupMesh(const vector<glm::mat4>& transforms);

    public:
        InstatiatingMesh(Mesh* mesh, GLuint ssboId);

        void SetSSBO(GLuint ssboId);
        void SetUBO(GLuint uboId);

        void Draw(Shader* shader, unsigned int number);
    };

}

#endif // !INSTATIATING_MESH
