#include <core/RenderableComponent.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::GraphicEngine;
using namespace std;

vector<RenderableComponent*> RenderableComponent::_components = vector<RenderableComponent*>();

RenderableComponent::RenderableComponent()
{
	_components.push_back(this);
}

RenderableComponent::~RenderableComponent()
{
	for (size_t i = 0; i < _components.size(); ++i) {
		if (_components[i] == this) {
			_components.erase(_components.begin() + i);
			break;
		}
	}
}

void RenderableComponent::Render()
{
}

YAML::Node RenderableComponent::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Renderable";
	node["isTransparent"] = _isTransparent;
	return node;
}

void RenderableComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Renderable##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		ImGui::Checkbox(string("Transparent##").append(id).c_str(), &_isTransparent);
	}
}

bool RenderableComponent::IsTransparent() const
{
	return _isTransparent;
}

void RenderableComponent::SetIsTransparent(bool value)
{
	_isTransparent = value;
}