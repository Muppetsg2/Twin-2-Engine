#pragma once

#include "Light.h"
#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <unordered_set>
#include <graphic/Shader.h>
#include <core/EventHandler.h>

constexpr const unsigned int MAX_POINT_LIGHTS = 8;
constexpr const unsigned int MAX_SPOT_LIGHTS = 8;
constexpr const unsigned int MAX_DIRECTIONAL_LIGHTS = 4;

namespace LightingSystem {
	class LightingController {
		private:
			static LightingController* instance;

			LightingController();
			~LightingController();

			struct Lights {											//1360
				unsigned int numberOfPointLights = 0;				//0		4
				unsigned int numberOfSpotLights = 0;				//4		4
				unsigned int numberOfDirLights = 0;					//8		4
				unsigned int padding = 0;								//12	4 - padding
				PointLight pointLights[MAX_POINT_LIGHTS];							//16	48 * 8 = 384
				SpotLight spotLights[MAX_SPOT_LIGHTS];							//400	64 * 8 = 512
				DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];				//912	112. * 4 = 448
			};

			struct LightingData {													//32
				glm::vec3 ambientLight;				//0		12
				float shininness = 2.2;
				glm::vec3 viewerPosition;
				int shadingType = 0;			//12	12
				// 0 - blinnPhong, 1 - toon							//24	4
			};
			//Lights lights;

		public:
			static float DLShadowCastingRange;
			Twin2Engine::Core::EventHandler<> ViewerTransformChanged;
			void UpdateOnTransformChange() {
				ViewerTransformChanged.Invoke();
				RenderShadowMaps();
			}

			static const int SHADOW_WIDTH;
			static const int SHADOW_HEIGHT;
			static const int MAPS_BEGINNING;

			GLuint LightsBuffer;
			GLuint LightingDataBuffer;

			std::unordered_set<PointLight*> pointLights;
			std::unordered_set<SpotLight*> spotLights;
			std::unordered_set<DirectionalLight*> dirLights;

			static LightingController* Instance();
			static bool IsInstantiated();
			static void UnloadAll();

			void UpdateLightsBuffer();
			//Nale�y u�ywa� po dodaniu lub usuni�ciu nowego point light-a
			void UpdatePointLights();
			//Nale�y u�ywa� po dodaniu lub usuni�ciu nowego spot light-a
			void UpdateSpotLights();
			//Nale�y u�ywa� po dodaniu lub usuni�ciu nowego directional light-a
			void UpdateDirLights();
			//U�ycie tej metody w celu modyfikacji warto�ci pozycji, po dodaniu lub usuni�ciu �r�d�a �wiat�a bez wywo�ania UpdatePointLights mo�e przynie�� nieprawid�owywynik
			void UpdatePLPosition(PointLight* pointLight);
			//U�ycie tej metody w celu modyfikacji warto�ci pozycji, po dodaniu lub usuni�ciu �r�d�a �wiat�a bez wywo�ania UpdateSpotLights mo�e przynie�� nieprawid�owywynik
			void UpdateSLTransform(SpotLight* spotLight);
			//U�ycie tej metody w celu modyfikacji warto�ci pozycji, po dodaniu lub usuni�ciu �r�d�a �wiat�a bez wywo�ania UpdateDirLights mo�e przynie�� nieprawid�owywynik
			void UpdateDLTransform(DirectionalLight* dirLight);
			//U�ycie tej metody w celu modyfikacji �r�d�a �wiat�a, po dodaniu lub usuni�ciu �r�d�a �wiat�a bez wywo�ania UpdatePointLights mo�e przynie�� nieprawid�owywynik
			void UpdatePL(PointLight* pointLight);
			//U�ycie tej metody w celu modyfikacji �r�d�a �wiat�a, po dodaniu lub usuni�ciu �r�d�a �wiat�a bez wywo�ania UpdateSpotLights mo�e przynie�� nieprawid�owywynik
			void UpdateSL(SpotLight* spotLight);
			//U�ycie tej metody w celu modyfikacji �r�d�a �wiat�a, po dodaniu lub usuni�ciu �r�d�a �wiat�a bez wywo�ania UpdateDirLights mo�e przynie�� nieprawid�owywynik
			void UpdateDL(DirectionalLight* dirLight);

			void BindLightBuffors(Twin2Engine::GraphicEngine::Shader* shader);
			//void UpdateShadowMapsTab(Twin2Engine::GraphicEngine::Shader* shader);

			static glm::vec3 RecalculateDirLightSpaceMatrix(DirectionalLight* light); //, const glm::mat4& viewProjectionInverse
			void RenderShadowMaps();

			void SetAmbientLight(glm::vec3 ambientLightColor);
			//void SetViewerPosition(glm::vec3& viewerPosition);
			void SetShadingType(int type);
	};
}