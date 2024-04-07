#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/UIRenderingManager.h>

#include <graphic/Window.h>

#include <core/CameraComponent.h>
//#include <Mesh.h>
//#include <Shader.h>
//#include <ShaderManager.h>
//
//#include <Material.h>
//#include <ModelsManager.h>
//#include <MeshRenderer.h>
//#include <MeshRenderingManager.h>
//#include <UIRenderingManager.h>
//
//#include <core/GameObject.h>
//#include <core/Transform.h>
//#include <core/Window.h>

//using Twin2Engine::Core::GameObject;
//using Twin2Engine::Core::Transform;
//using Twin2Engine::Core::Window;

//using std::vector;


namespace Twin2Engine::GraphicEngine
{
	class GraphicEngineManager
	{
			////Mesh* mesh;
			//InstatiatingMesh* mesh;
			//Shader* shader;
			//Material material;
			//Material material2;
			//Material wallMat;
			//Material roofMat;
			//InstatiatingModel modelMesh1;
			//InstatiatingModel modelMesh2;
			//GameObject* gameObject;
			//GameObject* gameObject2;
			//GameObject* gameObject3;

	private:
		GLuint _depthMapFBO;
		GLuint _depthMap;

	public:
		GraphicEngineManager();
		virtual ~GraphicEngineManager();

		//GraphicEngine()
		//{
		//	vector<Vertex> vertexes;
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 0.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 0.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(1.0f, 0.0f) });
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 1.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) });
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 1.0f, 0.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) });
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 0.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 0.0f) });
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 0.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(1.0f, 0.0f) });
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(1.0f, 1.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(1.0f, 1.0f) });
		//	vertexes.push_back(Vertex{ .Position = glm::vec3(0.0f, 1.0f, 1.0f), .Normal = glm::vec3(1.0f, 0.0f, 0.0f), .TexCoords = glm::vec2(0.0f, 1.0f) });
		//
		//	vector<unsigned int> indices;
		//
		//	indices.push_back(2);
		//	indices.push_back(1);
		//	indices.push_back(0);
		//	indices.push_back(3);
		//	indices.push_back(2);
		//	indices.push_back(0);
		//
		//	indices.push_back(0);
		//	indices.push_back(4);
		//	indices.push_back(3);
		//	indices.push_back(4);
		//	indices.push_back(7);
		//	indices.push_back(3);
		//
		//	indices.push_back(0);
		//	indices.push_back(1);
		//	indices.push_back(5);
		//	indices.push_back(0);
		//	indices.push_back(5);
		//	indices.push_back(4);
		//
		//	indices.push_back(4);
		//	indices.push_back(5);
		//	indices.push_back(6);
		//	indices.push_back(4);
		//	indices.push_back(6);
		//	indices.push_back(7);
		//
		//	indices.push_back(3);
		//	indices.push_back(7);
		//	indices.push_back(6);
		//	indices.push_back(3);
		//	indices.push_back(6);
		//	indices.push_back(2);
		//
		//	indices.push_back(1);
		//	indices.push_back(2);
		//	indices.push_back(6);
		//	indices.push_back(1);
		//	indices.push_back(6);
		//	indices.push_back(5);
		//	//indices.push_back(6);
		//	//indices.push_back(2);
		//	//indices.push_back(1);
		//	//indices.push_back(5);
		//	//indices.push_back(6);
		//	//indices.push_back(1);
		//
		//
		//	//vector<Texture> textures;
		//
		//	//mesh = new InstatiatingMesh(vertexes, indices);
		//
		//	ShaderManager::Init();
		//
		//
		//	//shader = ShaderManager::CreateShaderProgram("res/shaders/Basic.shpr");
		//	//shader = ShaderManager::CreateShaderProgram("origin/Basic", "shaders/normalVert.vert", "shaders/fargmentShader.frag");
		//	//shader = ShaderManager::GetShaderProgram("origin/Basic");
		//	
		//
		//	//modelMesh = ModelsManager::CreateModel("NewModel", vertexes, indices, textures);
		//	modelMesh1 = ModelsManager::CreateModel("NewModel", vertexes, indices);
		//	modelMesh2 = ModelsManager::GetModel("res/models/castle.obj");
		//
		//
		//	//shader = ShaderManager::CreateShaderProgram("origin/Textured", "shaders/textured.vert", "shaders/textured.frag");
		//	//shader = ShaderManager::CreateShaderProgram("origin/Textured");// , "shaders/textured.vert", "shaders/textured.frag");
		//	material = MaterialsManager::GetMaterial("textured");
		//
		//	material2 = MaterialsManager::GetMaterial("Basic2");
		//
		//	wallMat = MaterialsManager::GetMaterial("wallMat");
		//	roofMat = MaterialsManager::GetMaterial("roofMat");
		//
		//	gameObject = new GameObject();
		//	auto comp = gameObject->AddComponent<MeshRenderer>();
		//	comp->AddMaterial(material);
		//	comp->SetModel(modelMesh2);
		//
		//	gameObject2 = new GameObject();
		//	gameObject2->GetTransform()->Translate(glm::vec3(2, 1, 0));
		//	comp = gameObject2->AddComponent<MeshRenderer>();
		//	comp->AddMaterial(material);
		//	comp->SetModel(modelMesh1);
		//
		//
		//	gameObject3 = new GameObject();
		//	gameObject3->GetTransform()->Translate(glm::vec3(0, -2, 0));
		//	comp = gameObject3->AddComponent<MeshRenderer>();
		//	comp->AddMaterial(wallMat);
		//	comp->AddMaterial(roofMat);
		//	comp->SetModel(modelMesh2);
		//
		//	//std::cout << "Tutaj4\n";
		//	//instance = this;
		//}

		void Render();

		GLuint GetDepthMap() const;
	};
}

#endif