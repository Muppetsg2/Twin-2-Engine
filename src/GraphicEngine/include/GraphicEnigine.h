#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <vector>

#include <glm/glm.hpp>

#include <Mesh.h>
#include <Shader2.h>

using std::vector;

namespace GraphicEngine
{
	class GraphicEngine
	{
		Mesh* mesh;
		Shader* shader;
		Shader2* shader2;
		//static GraphicEngine* instance;

	public:
		GraphicEngine()
		{
			vector<Vertex> vertexes;
			vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 0.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
			vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 0.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
			vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 1.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
			vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 1.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
			vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 0.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
			vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 0.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
			vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 1.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
			vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 1.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });

			vector<unsigned int> indices;

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(0);
			indices.push_back(2);
			indices.push_back(3);

			indices.push_back(0);
			indices.push_back(4);
			indices.push_back(3);
			indices.push_back(4);
			indices.push_back(7);
			indices.push_back(3);

			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(5);
			indices.push_back(0);
			indices.push_back(5);
			indices.push_back(4);

			indices.push_back(4);
			indices.push_back(5);
			indices.push_back(6);
			indices.push_back(4);
			indices.push_back(6);
			indices.push_back(7);

			indices.push_back(3);
			indices.push_back(7);
			indices.push_back(6);
			indices.push_back(3);
			indices.push_back(6);
			indices.push_back(2);

			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(6);
			indices.push_back(1);
			indices.push_back(6);
			indices.push_back(5);


			vector<Texture> textures;

			mesh = new Mesh(vertexes, indices, textures);
			std::cout << "Tutaj" << std::endl;
			//shader = new Shader("C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\Shaders\\origin\\Basic.shdr");
			//shader = new Shader(true, "C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\normalVert.vert",
			shader = new Shader("C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\normalVert.vert",
								"C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\fargmentShader.frag");

			//shader2 = new Shader2("C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\normalVert.vert",
			//	"C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\fargmentShader.frag");
			std::cout << "Tutaj" << std::endl;

			//instance = this;
		}


		//static GraphicEngine* GetInstance()
		//{
		//	return instance;
		//}

		void Render(glm::mat4& view, glm::mat4& projection)
		{
			shader->use();
			shader->setMat4("view", view);
			shader->setMat4("projection", projection);
			
			glm::mat4 model(1.0f);
			shader->setMat4("model", model);
			//shader->setMat4("normalModel", model);
			
			mesh->Draw(*shader);
		}
	};
}

//GraphicEngine::GraphicEngine::instance = nullptr;

#endif