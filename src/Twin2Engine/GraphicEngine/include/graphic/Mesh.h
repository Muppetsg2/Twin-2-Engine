#ifndef MESH_H
#define MESH_H

#include <graphic/Shader.h>
using Twin2Engine::GraphicEngine::Shader;

using std::vector;
using std::string;

namespace Twin2Engine::Manager {
    class ModelsManager;
}

namespace Twin2Engine::GraphicEngine
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Texture {
        unsigned int id;
        string type;
        string path;
    };

    class Mesh {

        friend class InstatiatingMesh;
        friend class Manager::ModelsManager;

    public:
        /*  dane klasy Mesh  */
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        /*  Funkcje  */
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        virtual ~Mesh() = default;

        void Draw(Shader* shader);

    private:
        /*  Dane renderowania  */
        unsigned int VAO, VBO, EBO;
        /*  Funkcje    */
        void SetupMesh();
    };
}
#include <graphic/InstatiatingMesh.h>

#endif