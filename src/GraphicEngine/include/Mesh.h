#ifndef MESH_H
#define MESH_H


#include "Shader.h"

#include <vector>
#include <string>

#include <glad/glad.h>  // Initialize with gladLoadGL()

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions

using std::vector;
using std::string;

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

public:
    /*  dane klasy Mesh  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    /*  Funkcje  */
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }

    void Draw(Shader shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        //printf("Textures size %d\n", textures.size());
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // aktywuj odpowiedni¹ jednostkê teksturowania przed powi¹zaniem
            // pobierz numer tekstury (N dla diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            //printf("Textures id %d\n", textures[i].id);
            //printf("Textures name %s\n", (name + number).c_str());
            //shader.setFloat(("material." + name + number).c_str(), i);
            shader.setInt((name + number).c_str(), i);
            //shader.setInt((name + number).c_str(), textures[i].id);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        // narysuj siatkê
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
private:
    /*  Dane renderowania  */
    unsigned int VAO, VBO, EBO;
    /*  Funkcje    */
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            &indices[0], GL_STATIC_DRAW);

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
};

#include "InstatiatingMesh.h"

#endif