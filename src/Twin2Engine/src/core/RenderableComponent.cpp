#include <core/RenderableComponent.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
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

bool RenderableComponent::IsTransparent() const
{
	return _isTransparent;
}

void RenderableComponent::SetIsTransparent(bool value)
{
	if (_isTransparent != value) {
		_isTransparent = value;
		OnTransparentChangedEvent.Invoke(this);
	}
}

void RenderableComponent::Render() {}

YAML::Node RenderableComponent::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Renderable";
	node["isTransparent"] = _isTransparent;
	return node;
}

bool RenderableComponent::Deserialize(const YAML::Node& node) {
	if (!node["isTransparent"] || !Component::Deserialize(node)) return false;

	_isTransparent = node["isTransparent"].as<bool>();

	return true;
}

#if _DEBUG
void RenderableComponent::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Renderable##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;
		ImGui::Checkbox(string("Transparent##").append(id).c_str(), &_isTransparent);
	}
}
#endif