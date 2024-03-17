#ifndef INSTATIATING_MESH
#define INSTATIATING_MESH

#include "Mesh.h"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::vector;
//using glm::mat4;

class InstatiatingMesh
{
	Mesh* mesh;
    unsigned int instancesTransformsVBO;
    unsigned int amount;

	void setupMesh(const vector<glm::mat4>& transforms)
	{
        //unsigned int instancesTransformsVBO;
        glGenBuffers(1, &instancesTransformsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instancesTransformsVBO);
        glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), &transforms[0], GL_DYNAMIC_DRAW);

        //for (unsigned int i = 0; i < transforms.size(); i++)
        {
            unsigned int VAO = mesh->VAO;
            glBindVertexArray(VAO);
            // Atrybuty wierzcho³ków
            GLsizei vec4Size = sizeof(glm::vec4);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }
	}

public:
	InstatiatingMesh(Mesh* mesh, const vector<glm::mat4>& transforms)
	{
		this->mesh = mesh;
        amount = transforms.size();

        setupMesh(transforms);
	}


    void SetNewTransforms(const vector<glm::mat4>& transforms)
    {
        amount = transforms.size();
        glBindBuffer(GL_ARRAY_BUFFER, instancesTransformsVBO);
        glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), &transforms[0], GL_DYNAMIC_DRAW);

        //for (unsigned int i = 0; i < transforms.size(); i++)
        //{
        //    unsigned int VAO = mesh->VAO;
        //    glBindVertexArray(VAO);
        //    // Atrybuty wierzcho³ków
        //    GLsizei vec4Size = sizeof(glm::vec4);
        //    glEnableVertexAttribArray(3);
        //    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        //    glEnableVertexAttribArray(4);
        //    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
        //    glEnableVertexAttribArray(5);
        //    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        //    glEnableVertexAttribArray(6);
        //    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
        //
        //    glVertexAttribDivisor(3, 1);
        //    glVertexAttribDivisor(4, 1);
        //    glVertexAttribDivisor(5, 1);
        //    glVertexAttribDivisor(6, 1);
        //
        //    glBindVertexArray(0);
        //}
    }


    void Draw(Shader* shader)
    {
        //printf("Drawing mesh %d\n", mesh->VAO);
        //printf("Drawing mesh %d\n", mesh->vertices.size());
        //printf("Drawing amount %d\n", amount);
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        //printf("Textures size %d\n", mesh->textures.size());
        for (unsigned int i = 0; i < mesh->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // aktywuj odpowiedni¹ jednostkê teksturowania przed powi¹zaniem
            // pobierz numer tekstury (N dla diffuse_textureN)
            string number;
            string name = mesh->textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            //printf("Textures id %d\n", mesh->textures[i].id);
            //printf("Textures name %s\n", (name + number).c_str());
            //printf("Textures path %s\n", (mesh->textures[i].path).c_str());
            //shader.setFloat(("material." + name + number).c_str(), i);
            //shader->setFloat(("material." + name + number).c_str(), i);
            shader->setInt((name + number).c_str(), GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        // narysuj siatkê
        glBindVertexArray(mesh->VAO);
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, amount);
        glBindVertexArray(0);
    }
};


#endif // !INSTATIATING_MESH
