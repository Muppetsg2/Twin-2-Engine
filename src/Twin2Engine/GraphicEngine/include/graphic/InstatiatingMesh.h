#ifndef INSTATIATING_MESH
#define INSTATIATING_MESH

#include <graphic/Mesh.h>

using std::vector;

namespace Twin2Engine
{
    namespace GraphicEngine
    {

        class InstatiatingMesh
        {
            Mesh* mesh;
            unsigned int instancesTransformsVBO;
            unsigned int amount;

            GLuint _instanceDataSSBO;
            GLuint _materialIndexSSBO;
            GLuint _materialInputUBO;

            void setupMesh(const vector<glm::mat4>& transforms);

        public:
            InstatiatingMesh(Mesh* mesh, GLuint instanceDataUBO);

            void SetInstanceDataSSBO(GLuint instanceDataSSBO);
            void SetMaterialIndexSSBO(GLuint materialIndexSSBO);
            void SetMaterialInputUBO(GLuint materialInputUBO);

            void Draw(Shader* shader, unsigned int number);
        };

    }
}

#endif // !INSTATIATING_MESH
