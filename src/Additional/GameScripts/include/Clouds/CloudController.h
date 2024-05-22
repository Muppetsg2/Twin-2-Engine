#pragma once

#include <Clouds/Cloud.h>
#include <graphic/Shader.h>
#include <graphic/InstantiatingMesh.h>

class CloudController {
	private:
		static CloudController* instance;
		std::unordered_map<Twin2Engine::Graphic::InstantiatingMesh*, std::vector<glm::mat4>> depthQueue;
		Twin2Engine::Graphic::Shader* CloudDepthShader;
		Twin2Engine::Graphic::Shader* CloudShader;
		GLuint depthmapFBO;
		GLuint depthmap;
		GLuint dmap;

	public:
		//std::unordered_set<Twin2Engine::Core::MeshRenderer*> cloudsMeshRenderers;
		static const int CLOUD_DEPTH_MAP_ID;

		CloudController();
		~CloudController();

		void RegisterCloud(Cloud* cloud);
		void UnregisterCloud(Cloud* cloud);

		static CloudController* Instance();
		static void DeleteInstance();

		void RenderCloudBackDepthMap();
};