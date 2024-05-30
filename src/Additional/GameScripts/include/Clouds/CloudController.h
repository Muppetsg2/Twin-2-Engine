#pragma once

#include <Clouds/Cloud.h>
#include <graphic/Shader.h>
#include <graphic/Texture2D.h>
#include <graphic/InstantiatingMesh.h>

class CloudController {
	private:
		static CloudController* instance;
		std::unordered_map<Twin2Engine::Graphic::InstantiatingMesh*, std::vector<Transform*>> depthQueue;
		Twin2Engine::Graphic::Shader* CloudDepthShader;
		Twin2Engine::Graphic::Shader* CloudShader;
		Twin2Engine::Graphic::Texture2D* NoiseTexture;
		GLuint noiseTexture3d;
		GLuint depthmapFBO;
		GLuint depthmap;
		GLuint lightdepthmap;
		GLuint dmap;

	public:
		//std::unordered_set<Twin2Engine::Core::MeshRenderer*> cloudsMeshRenderers;
		static const int CLOUD_DEPTH_MAP_ID;

		static float ABSORPTION;
		static float DENSITY_FAC;
		static float NUMBER_OF_STEPS;
		static float CLOUD_LIGHT_STEPS;
		static float CLOUD_LIGHT_MULTIPLIER;
		static float CLOUD_EXPOSURE;
		static float POS_MULT;
		static float APLPHA_TRESHOLD;
		static float DENSITY_TRESHOLD;
		static glm::vec3  NOISE_D_VEL_3D;

		CloudController();
		~CloudController();

		void RegisterCloud(Cloud* cloud);
		void UnregisterCloud(Cloud* cloud);

		static CloudController* Instance();
		static void DeleteInstance();

		void RenderCloudBackDepthMap();

		void UpdateCloudShaderUniforms();
};