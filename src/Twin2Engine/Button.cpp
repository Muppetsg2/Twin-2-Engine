#include <ui/Button.h>
#include <core/GameObject.h>
#include <core/Input.h>

using namespace Twin2Engine::UI;
using namespace Twin2Engine::Core;

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

const MethodEventHandler& Button::GetOnClickEvent() const {
	return _onClickEvent;
}

void Button::Update()
{
	if (!_interactable) return;
	if (!_notHold && Input::IsMouseButtonHeldUp(MOUSE_BUTTON::LEFT)) {
		_notHold = true;
		return;
	}

	Transform* t = GetTransform();
	glm::mat4 inv = glm::inverse(t->GetTransformMatrix());
	glm::vec4 mPos = glm::vec4(Input::GetMousePos(), 0.f, 1.f);
	glm::vec3 btnLocalMPos = inv * mPos;
	if (btnLocalMPos.x >= -_width / 2.f && btnLocalMPos.x <= _width / 2.f && btnLocalMPos.y >= -_height / 2.f && btnLocalMPos.y <= _height / 2.f) {
		if (Input::IsMouseButtonDown(MOUSE_BUTTON::LEFT)) {
			_notHold = false;
		}
		else if (Input::IsMouseButtonReleased(MOUSE_BUTTON::LEFT) && _notHold) {
			_onClickEvent.Invoke();
		}
	}
}