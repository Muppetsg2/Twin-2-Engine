#include <ui/Button.h>
#include <core/GameObject.h>
#include <core/Input.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::UI;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Tools;

void Button::SetWidth(float width) {
	_width = width;
}

void Button::SetHeight(float height) {
	_height = height;
}

void Button::SetInteractable(bool interactable) {
	_interactable = interactable;
}

float Button::GetWidth() const {
	return _width;
}

float Button::GetHeight() const {
	return _height;
}

bool Button::IsInteractable() const {
	return _interactable;
}

MethodEventHandler& Button::GetOnClickEvent() {
	return _onClickEvent;
}

void Button::Update()
{
	if (!_interactable) return;

	if (Input::IsMouseButtonPressed(MOUSE_BUTTON::LEFT)) {
		Transform* t = GetTransform();
		glm::mat4 inv = glm::inverse(t->GetTransformMatrix());
		glm::vec4 mPos = glm::vec4(Input::GetCursorPos(), 0.f, 1.f);
		glm::vec3 btnLocalMPos = inv * mPos;
		if (btnLocalMPos.x >= -_width / 2.f && btnLocalMPos.x <= _width / 2.f && btnLocalMPos.y >= -_height / 2.f && btnLocalMPos.y <= _height / 2.f) {

			_onClickEvent.Invoke();
		}
	}
}

YAML::Node Button::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Button";
	node.remove("subTypes");
	node["width"] = _width;
	node["height"] = _height;
	node["interactable"] = _interactable;
	return node;
}