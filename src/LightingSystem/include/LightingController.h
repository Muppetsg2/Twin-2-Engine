#ifndef _LIGHTINGCONTROLLER_H_
#define _LIGHTINGCONTROLLER_H_

#include "Light.h"
#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <unordered_set>
#include "Shader.h"


namespace LightingSystem {
	class LightingController {
		private:
			static LightingController* instance;

			LightingController();

			struct Lights {											//1180
				unsigned int numberOfPointLights = 0;				//0		4
				unsigned int numberOfSpotLights = 0;				//4		4
				unsigned int numberOfDirLights = 0;					//8		4
				PointLight pointLights[8];							//12	48 * 8 = 384
				SpotLight spotLights[8];							//396	72 * 8 = 576
				DirectionalLight directionalLights[4];				//972	52 * 4 = 208
			};

			struct LightingData {													//36
				alignas(16) glm::vec3 AmbientLight = glm::vec3(0.0f, 0.0f, 0.0f);	//0		16
				alignas(16) glm::vec3 ViewerPosition = glm::vec3(0.0f, 0.0f, 0.0f);	//16	32
				float gamma = 2.2f;													//32	4
			};
			//Lights lights;

		public:
			GLuint LightsBuffer;
			GLuint LightingDataBuffer;


			std::unordered_set<PointLight*> pointLights;
			std::unordered_set<SpotLight*> spotLights;
			std::unordered_set<DirectionalLight*> dirLights;

			static LightingController* Instance();
			static bool IsInstantiated();

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

			void SetAmbientLight(glm::vec3& ambientLightColor);
			void SetViewerPosition(glm::vec3& viewerPosition);
			void SetGamma(float& gamma);

			/*/
			void RegisterPointLight(PointLight* pointLight);
			void RegisterSpotLight(SpotLight* spotLight);
			void RegisterDirLight(DirectionalLight* dirLight);

			void UnregisterPointLight(PointLight* pointLight);
			void UnregisterSpotLight(SpotLight* spotLight);
			void UnregisterDirLight(DirectionalLight* dirLight);/**/
	};
}

#endif // !_LIGHTINGCONTROLLER_H_
