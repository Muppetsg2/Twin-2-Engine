#include "ui/Button.h"
#include <core/GameObject.h>

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

	Transform* t = getTransform();
}