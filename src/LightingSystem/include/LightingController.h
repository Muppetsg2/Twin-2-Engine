#pragma once

#include "Light.h"
#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <unordered_set>
#include <graphic/Shader.h>
#include <core/EventHandler.h>

namespace LightingSystem {
	class LightingController {
		private:
			static LightingController* instance;

			LightingController();
			~LightingController();

			struct Lights {											//1104
				unsigned int numberOfPointLights = 0;				//0		4
				unsigned int numberOfSpotLights = 0;				//4		4
				unsigned int numberOfDirLights = 0;					//8		4
				unsigned int padding;								//12	4 - padding
				PointLight pointLights[8];							//16	48 * 8 = 384
				SpotLight spotLights[8];							//400	64 * 8 = 512
				DirectionalLight directionalLights[4];				//912	112. * 4 = 192
			};

			struct LightingData {													//32
				alignas(16) glm::vec3 AmbientLight = glm::vec3(0.0f, 0.0f, 0.0f);	//0		16
				glm::vec3 ViewerPosition = glm::vec3(0.0f, 0.0f, 0.0f);				//16	12
				float HighlightParam = 2.0f;										//28	4
			};
			//Lights lights;

		public:
			static float DLShadowCastingRange;
			Twin2Engine::Core::EventHandler<> ViewerTransformChanged;

			static const int SHADOW_WIDTH;
			static const int SHADOW_HEIGHT;

			GLuint LightsBuffer;
			GLuint LightingDataBuffer;

			std::unordered_set<PointLight*> pointLights;
			std::unordered_set<SpotLight*> spotLights;
			std::unordered_set<DirectionalLight*> dirLights;

			static LightingController* Instance();
			static bool IsInstantiated();
			static void UnloadAll();

			void UpdateLightsBuffer();
			//Nale¿y u¿ywaæ po dodaniu lub usuniêciu nowego point light-a
			void UpdatePointLights();
			//Nale¿y u¿ywaæ po dodaniu lub usuniêciu nowego spot light-a
			void UpdateSpotLights();
			//Nale¿y u¿ywaæ po dodaniu lub usuniêciu nowego directional light-a
			void UpdateDirLights();
			//U¿ycie tej metody w celu modyfikacji wartoœci pozycji, po dodaniu lub usuniêciu Ÿród³a œwiat³a bez wywo³ania UpdatePointLights mo¿e przynieœæ nieprawid³owywynik
			void UpdatePLPosition(PointLight* pointLight);
			//U¿ycie tej metody w celu modyfikacji wartoœci pozycji, po dodaniu lub usuniêciu Ÿród³a œwiat³a bez wywo³ania UpdateSpotLights mo¿e przynieœæ nieprawid³owywynik
			void UpdateSLTransform(SpotLight* spotLight);
			//U¿ycie tej metody w celu modyfikacji wartoœci pozycji, po dodaniu lub usuniêciu Ÿród³a œwiat³a bez wywo³ania UpdateDirLights mo¿e przynieœæ nieprawid³owywynik
			void UpdateDLTransform(DirectionalLight* dirLight);
			//U¿ycie tej metody w celu modyfikacji Ÿród³a œwiat³a, po dodaniu lub usuniêciu Ÿród³a œwiat³a bez wywo³ania UpdatePointLights mo¿e przynieœæ nieprawid³owywynik
			void UpdatePL(PointLight* pointLight);
			//U¿ycie tej metody w celu modyfikacji Ÿród³a œwiat³a, po dodaniu lub usuniêciu Ÿród³a œwiat³a bez wywo³ania UpdateSpotLights mo¿e przynieœæ nieprawid³owywynik
			void UpdateSL(SpotLight* spotLight);
			//U¿ycie tej metody w celu modyfikacji Ÿród³a œwiat³a, po dodaniu lub usuniêciu Ÿród³a œwiat³a bez wywo³ania UpdateDirLights mo¿e przynieœæ nieprawid³owywynik
			void UpdateDL(DirectionalLight* dirLight);

			void BindLightBuffors(Twin2Engine::GraphicEngine::Shader* shader);
			void UpdateShadowMapsTab(Twin2Engine::GraphicEngine::Shader* shader);

			static glm::vec3 RecalculateDirLightSpaceMatrix(DirectionalLight* light); //, const glm::mat4& viewProjectionInverse
			void RenderShadowMaps();

			void SetAmbientLight(glm::vec3 ambientLightColor);
			void SetViewerPosition(glm::vec3& viewerPosition);
			void SetHighlightParam(float highlightParam);
			//void SetGamma(float gamma);

			/*/
			void RegisterPointLight(PointLight* pointLight);
			void RegisterSpotLight(SpotLight* spotLight);
			void RegisterDirLight(DirectionalLight* dirLight);

			void UnregisterPointLight(PointLight* pointLight);
			void UnregisterSpotLight(SpotLight* spotLight);
			void UnregisterDirLight(DirectionalLight* dirLight);/**/
	};
}