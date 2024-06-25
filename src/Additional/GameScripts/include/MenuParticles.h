#pragma once

#include <UIParticleGenerator.h>

#include <core/Component.h>

#include <string>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;

class MenuParticles : public Component{
private:

	UIParticleGenerator* _generator = nullptr;
	unsigned int _amount = 14;
	float _height = 900.f;
	float _particleWidth = 50.f;
	float _particleHeight = 50.f;
	float _radius = 800.f;

	glm::vec4 _startColor = glm::vec4(1.f);
	glm::vec4 _endColor = glm::vec4(1.f);

	int _onTransformChange = -1;

	void DefineGenerator() {
		if (_generator != nullptr) {
			_generator->active = false;
			delete _generator;
			_generator = nullptr;
		}

		_generator = new UIParticleGenerator( "origin/UIParticleShader", "res/textures/particleDefault.png", _amount, _height, 0.0f, 6.0f, 2.0f, _particleWidth, _particleHeight, _radius, true);
		_generator->active = true; 
		_generator->SetStartPosition(glm::vec3(0.0f, -500.0f, 0.0f));
	}

public:
	virtual void Initialize() override {
		DefineGenerator();
		//_generator->SetStartPosition(GetGameObject()->GetTransform()->GetGlobalPosition());

		//_onTransformChange = GetGameObject()->GetTransform()->OnEventPositionChanged += [&](Transform* t) -> void {
		//	_generator->SetStartPosition(GetGameObject()->GetTransform()->GetGlobalPosition());
		//};
	}

	virtual void Update() override {}

	virtual void OnDestroy() override {
		if (_generator != nullptr) {
			_generator->active = false;
			delete _generator;
			_generator = nullptr;
		}

		if (_onTransformChange != -1) {
			GetGameObject()->GetTransform()->OnEventPositionChanged -= _onTransformChange;
			_onTransformChange = -1;
		}
	}

	virtual YAML::Node Serialize() const override {
		YAML::Node node = Component::Serialize();
		node["type"] = "MenuParticles";
		node["amount"] = _amount;
		node["height"] = _height;
		node["particleWidth"] = _particleWidth;
		node["particleHeight"] = _particleHeight;
		node["radius"] = _radius;
		node["startColor"] = _startColor;
		node["endColor"] = _endColor;

		return node;
	}

	virtual bool Deserialize(const YAML::Node& node) override {
		if (!node["amount"] || !node["height"] || !node["particleWidth"] ||
			!node["particleHeight"] || !node["radius"] || !node["startColor"] || !node["endColor"] ||
			!Component::Deserialize(node))
			return false;

		_amount = node["amount"].as<unsigned int>();
		_height = node["height"].as<float>();
		_particleWidth = node["particleWidth"].as<float>();
		_particleHeight = node["particleHeight"].as<float>();
		_radius = node["radius"].as<float>();
		_startColor = node["startColor"].as<glm::vec4>();
		_endColor = node["endColor"].as<glm::vec4>();

		return true;
	}

#if _DEBUG
	virtual void DrawEditor() override {
		std::string id = std::string(std::to_string(this->GetId()));
		std::string name = std::string("MenuParticles##Component").append(id);
		if (ImGui::CollapsingHeader(name.c_str())) {

			if (Component::DrawInheritedFields()) return;

			bool redefine = false;

			unsigned int a = _amount;
			ImGui::DragUInt(std::string("Amount##").append(id).c_str(), &a, 1, 0, UINT_MAX);

			if (a != _amount) {
				_amount = a;
				redefine = true;
			}

			float v = _height;
			ImGui::DragFloat(std::string("Height##").append(id).c_str(), &v, 0.1f);
			
			if (v != _height) {
				_height = v;
				redefine = true;
			}

			v = _particleWidth;
			ImGui::DragFloat(std::string("ParticleWidth##").append(id).c_str(), &v, 0.01f, 0.f, FLT_MAX);

			if (v != _particleWidth) {
				_particleWidth = v;
				redefine = true;
			}

			v = _particleHeight;
			ImGui::DragFloat(std::string("ParticleHeight##").append(id).c_str(), &v, 0.01f, 0.f, FLT_MAX);

			if (v != _particleHeight) {
				_particleHeight = v;
				redefine = true;
			}

			v = _radius;
			ImGui::DragFloat(std::string("Radius##").append(id).c_str(), &v, 0.01f, 0.f, FLT_MAX);

			if (v != _radius) {
				_radius = v;
				redefine = true;
			}

			if (redefine) {
				DefineGenerator();
			}

			glm::vec4 col = _startColor;
			ImGui::ColorEdit4(std::string("StartColor##").append(id).c_str(), glm::value_ptr(col));

			if (col != _startColor) {
				_startColor = col;
				_generator->SetStartColor(_startColor);
			}

			col = _endColor;
			ImGui::ColorEdit4(std::string("EndColor##").append(id).c_str(), glm::value_ptr(col));

			if (col != _endColor) {
				_endColor = col;
				_generator->SetEndColor(_endColor);
			}
		}
	}
#endif
};