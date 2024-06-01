#pragma once

#include <graphic/Light.h>
#include <graphic/Shader.h>
#include <tools/EventHandler.h>
#include <tools/STD140Struct.h>

constexpr const unsigned int MAX_POINT_LIGHTS = 8;
constexpr const unsigned int MAX_SPOT_LIGHTS = 8;
constexpr const unsigned int MAX_DIRECTIONAL_LIGHTS = 2;

namespace Twin2Engine::Graphic {

	struct CameraData {
		glm::mat4 projection;
		glm::mat4 view;

		glm::vec3 pos;
		glm::vec3 front;

		float farPlane;

		bool isPerspective;
	};

	class LightingController {
		private:
			static LightingController* instance;
			static glm::vec3 viewerPosition;
			static bool lastViewerPositionSet;
			static glm::vec3 lastViewerPosition;

			LightingController();
			~LightingController();

			struct Lights {
				unsigned int numberOfPointLights = 0;
				unsigned int numberOfSpotLights = 0;
				unsigned int numberOfDirLights = 0;
				PointLight pointLights[MAX_POINT_LIGHTS];
				SpotLight spotLights[MAX_SPOT_LIGHTS];
				DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
			};
			static const Tools::STD140Offsets _lightsOffsets;

			struct LightingData {
				glm::vec3 ambientLight = glm::vec3 (0.1f, 0.1f, 0.1f);
				float shininness = 4.0f;
				glm::vec3 viewerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
				int shadingType = 0; // 0 - blinnPhong, 1 - toon, 2 - gooch
			};
			static const Tools::STD140Offsets _lightingDataOffsets;

		public:
			static float DLShadowCastingRange;
			Twin2Engine::Tools::MethodEventHandler ViewerTransformChanged;
			void UpdateOnTransformChange() {
				//SPDLOG_INFO("VP: {}\t{}\t{}\t\tLVP{}\t{}\t{}", viewerPosition.x, viewerPosition.y, viewerPosition.z, lastViewerPosition.x, lastViewerPosition.y, lastViewerPosition.z);
				//if (lastViewerPositionSet) {
				//	if ((glm::abs(lastViewerPosition.x - viewerPosition.x) <= 5.0f) && (glm::abs(lastViewerPosition.y - viewerPosition.y) <= 5.0f)) {
				//		//SPDLOG_INFO("2");
				//		return;
				//	}
				//}
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

			void BindLightBuffors(Twin2Engine::Graphic::Shader* shader);
			//void UpdateShadowMapsTab(Twin2Engine::Graphic::Shader* shader);

			static glm::vec3 RecalculateDirLightSpaceMatrix(DirectionalLight* light); //, const glm::mat4& viewProjectionInverse
			void RenderShadowMaps();
			void RenderDynamicShadowMaps();

			void SetAmbientLight(glm::vec3 ambientLightColor);
			void SetShininness(float shininness);
			void SetViewerPosition(glm::vec3& viewerPosition);
			void SetShadingType(int type);
	};
}