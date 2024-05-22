#pragma once

#include <Clouds/Cloud.h>
#include <graphic/Shader.h>

class CloudController {
	private:
		static CloudController* instance;
		Twin2Engine::Graphic::Shader* CloudDepthShader;
		GLuint depthmapFBO;
		GLuint depthmap;

	public:
		std::unordered_set<Cloud*> clouds;
		static const int CLOUD_DEPTH_MAP_ID;

		CloudController();
		~CloudController();

		static CloudController* Instance();
		static void DeleteInstance();

		void RenderCloudBackDepthMap();
};