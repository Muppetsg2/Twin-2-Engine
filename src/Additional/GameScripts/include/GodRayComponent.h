#pragma once

#include <core/GameObject.h>
#include <graphic/Shader.h>
#include <manager/SceneManager.h>
#include <Clouds/CloudController.h>
#include <core/Time.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

class GodRayComponent : public Component {
	private:
		float heigth				= 10.6f;
		float heigthLowerTreshhold	= 0.1f;
		float heigthHigherTreshhold	= 10.6f;
		glm::vec3 ray_color			= glm::vec3(1.0f, 0.99f, 0.0f);

		float POS_MULT				= 0.1f;
		glm::vec3 NOISE_D_VEL_3D	= glm::vec3(0.0f, 0.1f, 0.1f);;
		float time					= 0.0f;
		float minAlpha				= 0.3f;
		float na					= 1.5f;
		Shader* godRayShader = nullptr;
	public:

		virtual void Initialize() override {
			godRayShader = ShaderManager::GetShaderProgram("origin/GodRay");
			godRayShader->Use();

			godRayShader->SetFloat("heigth", heigth);
			godRayShader->SetFloat("heigthLowerTreshhold", heigthLowerTreshhold);
			godRayShader->SetFloat("heigthHigherTreshhold", heigthHigherTreshhold);
			godRayShader->SetVec3("ray_color", ray_color);
			godRayShader->SetInt("noiseTexture3d", CloudController::CLOUD_DEPTH_MAP_ID + 2);
			godRayShader->SetFloat("POS_MULT", POS_MULT);
			godRayShader->SetFloat("minAlpha", minAlpha);
			godRayShader->SetFloat("na", na);
			godRayShader->SetVec3("NOISE_D_VEL_3D", NOISE_D_VEL_3D);
		}

		virtual void Update() override {
			godRayShader->Use();
			godRayShader->SetFloat("time", Time::GetTime());
		}

		virtual YAML::Node Serialize() const override {
			YAML::Node node = Component::Serialize();
			node["type"] = "GodRayComponent";
			return node;
		}

		virtual bool Deserialize(const YAML::Node& node) override {
			if (!Component::Deserialize(node))
				return false;

			return true;
		}

#if _DEBUG
		virtual void DrawEditor() override {
			string id = string(std::to_string(this->GetId()));
			string name = string("GodRayComponent##Component").append(id);
			if (ImGui::CollapsingHeader(name.c_str())) {

				if (Component::DrawInheritedFields()) return;

				ImGui::DragFloat(string("heigth##").append(id).c_str(), &heigth);
				ImGui::DragFloat(string("heigthLowerTreshhold##").append(id).c_str(), &heigthLowerTreshhold);
				ImGui::DragFloat(string("heigthHigherTreshhold##").append(id).c_str(), &heigthHigherTreshhold);
				ImGui::ColorEdit3(string("ray_color##").append(id).c_str(), glm::value_ptr(ray_color));
				ImGui::DragFloat(string("POS_MULT##").append(id).c_str(), &POS_MULT);
				ImGui::DragFloat(string("minAlpha##").append(id).c_str(), &minAlpha);
				ImGui::DragFloat(string("na##").append(id).c_str(), &na);
				ImGui::DragFloat3(string("NOISE_D_VEL_3D##").append(id).c_str(), glm::value_ptr(NOISE_D_VEL_3D));

				godRayShader->Use();
				godRayShader->SetFloat("heigth", heigth);
				godRayShader->SetFloat("heigthLowerTreshhold", heigthLowerTreshhold);
				godRayShader->SetFloat("heigthHigherTreshhold", heigthHigherTreshhold);
				godRayShader->SetFloat("POS_MULT", POS_MULT);
				godRayShader->SetFloat("minAlpha", minAlpha);
				godRayShader->SetFloat("na", na);
				godRayShader->SetVec3("ray_color", ray_color);
				godRayShader->SetVec3("NOISE_D_VEL_3D", NOISE_D_VEL_3D);
			}
		}
#endif
};