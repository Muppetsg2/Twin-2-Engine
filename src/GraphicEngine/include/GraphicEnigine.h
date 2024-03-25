#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <vector>

#include <glm/glm.hpp>

#include <Mesh.h>
#include <Shader.h>
#include <ShaderManager.h>

#include <Material.h>
#include <ModelsManager.h>
#include <MeshRenderer.h>
#include <MeshRenderingManager.h>

#include <inc/GameObject.h>
#include <inc/Transform.h>

using Twin2EngineCore::GameObject;
using Twin2EngineCore::Transform;

using std::vector;

namespace GraphicEngine
{
	class GraphicEngine
	{
		Mesh* mesh;
		Shader* shader;
		Material material;
		InstatiatingModel modelMesh;
		GameObject* gameObject;
		
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
			////std::cout << "Tutaj" << std::endl;
			//shader = new Shader("C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\CompiledShaders\\origin\\Basic.shdr");
			//shader = new Shader("res/CompiledShaders/origin/Basic.shdr");
			ShaderManager::Init();
			//shader = ShaderManager::GetShaderProgram("res/CompiledShaders/origin/Basic.shdr");
			shader = ShaderManager::CreateShaderProgram("res/CompiledShaders/origin/Basic.shdr", "shaders/normalVert.vert", "shaders/fargmentShader.frag");
			//shader = new Shader(true, "C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\normalVert.vert",
			//shader = new Shader("C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\normalVert.vert",
			//					"C:\\Users\\matga\\Desktop\\Mateusz\\Studia\\Semestr_VI\\PSGK\\Engine\\Twin-2-Engine\\res\\shaders\\fargmentShader.frag");

			////std::cout << "Tutaj" << std::endl;
			shader->use();
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			shader->setVec4("uColor", (float*)(&color));
			color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
			shader->setVec4("uColor", (float*)(&color));

			modelMesh = ModelsManager::CreateModel("NewModel", vertexes, indices, textures);
			material = MaterialsManager::CreateMaterial("new", "res/CompiledShaders/origin/Basic.shdr");

			gameObject = new GameObject();
			auto comp = gameObject->AddComponent<MeshRenderer>();

			//std::cout << "Tutaj5\n";
			comp->AddMaterial(material);
			//std::cout << "Tutaj3\n";
			comp->SetModel(modelMesh);
			//std::cout << "Tutaj4\n";
			//instance = this;
		}


		//static GraphicEngine* GetInstance()
		//{
		//	return instance;
		//}

		void Render(glm::mat4& view, glm::mat4& projection)
		{
			shader->use();
			shader->setMat4("projection", projection);
			shader->setMat4("view", view);
			
			glm::mat4 model(1.0f);
			shader->setMat4("model", model);
			//std::cout << "Tutaj1\n";
			MeshRenderingManager::Render();
			//std::cout << "Tutaj2\n";
			//shader->setMat4("normalModel", model);
			//
			////mesh->Draw(shader);
			//modelMesh.Draw(shader);
			//
			//glm::mat4 newview(1.0f);
			//shader->setMat4("view", newview);
			//shader->setMat4("projection", newview);
			////mesh->Draw(shader);
			//modelMesh.Draw(shader);
		}
	};
}

//GraphicEngine::GraphicEngine::instance = nullptr;

#endif