#ifndef MESH_H
#define MESH_H


#include "Shader.h"
#include <Vertex.h>

using Twin2Engine::GraphicEngine::Shader;


#include <vector>
#include <string>

#include <glad/glad.h>  // Initialize with gladLoadGL()

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

using std::vector;
using std::string;


namespace Twin2Engine::GraphicEngine
{

    struct Texture {
        unsigned int id;
        string type;
        string path;
    };

    class Mesh {

        friend class InstatiatingMesh;

    public:
        /*  dane klasy Mesh  */
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        /*  Funkcje  */
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

        void Draw(Shader* shader);

    private:
        /*  Dane renderowania  */
        unsigned int VAO, VBO, EBO;
        /*  Funkcje    */
        void SetupMesh();
    };
}
#include "InstatiatingMesh.h"

#endif