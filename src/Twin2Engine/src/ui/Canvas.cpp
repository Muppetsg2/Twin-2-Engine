#include <ui/Canvas.h>
#include <core/GameObject.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::UI;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Tools;
using namespace glm;

void Canvas::Initialize()
{
	Transform* t = GetTransform();
	_data.rectTransform.transform = t->GetTransformMatrix();
	_onTransformChangeId = (t->OnEventTransformChanged += [&](Transform* trans) -> void { _data.rectTransform.transform = trans->GetTransformMatrix(); });
}

void Canvas::OnDestroy()
{
	GetTransform()->OnEventTransformChanged -= _onTransformChangeId;
	_OnCanvasDestory(this);
}

YAML::Node Canvas::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Canvas";
	node["size"] = _data.rectTransform.size;
	node["worldSpace"] = _data.worldSpaceCanvas;
	return node;
}

bool Canvas::Deserialize(const YAML::Node& node)
{
	if (!node["size"] || !node["worldSpace"] || !Component::Deserialize(node)) return false;

	_data.rectTransform.size = node["size"].as<vec2>();
	_data.worldSpaceCanvas = node["worldSpace"].as<bool>();

	return true;
}

#if _DEBUG
void Canvas::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Canvas##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		vec2 s = _data.rectTransform.size;
		if (ImGui::InputFloat2(string("Canvas Size##").append(id).c_str(), (float*)&s)) {
			SetSize(s);
		}

		bool ws = _data.worldSpaceCanvas;
		if (ImGui::Checkbox(string("World Space##").append(id).c_str(), &ws)) {
			SetInWorldSpace(ws);
		}
	}
}
#endif

void Canvas::SetInWorldSpace(bool worldSpace)
{
	if (_data.worldSpaceCanvas != worldSpace) {
		_data.worldSpaceCanvas = worldSpace;
	}
}

void Canvas::SetSize(const vec2& size)
{
	if (_data.rectTransform.size != size) {
		_data.rectTransform.size = size;
	}
}

bool Canvas::IsInWorldSpace() const
{
	return _data.worldSpaceCanvas;
}

vec2 Canvas::GetSize() const
{
	return _data.rectTransform.size;
}

EventHandler<Canvas*>& Canvas::GetOnCanvasDestroy()
{
	return _OnCanvasDestory;
}