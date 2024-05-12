#include <ui/InputField.h>
#include <core/Time.h>
#include <core/Transform.h>
#include <graphic/manager/UIRenderingManager.h>

using namespace Twin2Engine::UI;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace std;

const float InputField::_cursorDelay = .5f;

void InputField::OnKeyStateChange(KEY key, INPUT_STATE state)
{
	if (!IsEnable() || !_interactable) return;

	if (state == INPUT_STATE::PRESSED) {
		if (key >= KEY::SPACE && key <= KEY::GRAVE_ACCENT) {
			char value = (char)key;
			if (key >= KEY::A && key <= KEY::Z) {
				value += 'a' - 'A';
			}

			std::string oldText = _text->GetText();
			_text->SetText(oldText.substr(0, _cursorPos) + value + oldText.substr(_cursorPos, oldText.size() - _cursorPos));
			++_cursorPos;
		}
		else if (key == KEY::ENTER) {
			_typing = false;
			_cursorVisible = false;
		}
		else if (key == KEY::BACKSPACE) {
			std::string oldText = _text->GetText();
			if (_cursorPos > 0) {
				_text->SetText(oldText.substr(0, _cursorPos - 1) + oldText.substr(_cursorPos, oldText.size() - _cursorPos));
				--_cursorPos;
			}
		}
		else if (key == KEY::DELETE_KEY) {
			std::string oldText = _text->GetText();
			if (_cursorPos < oldText.size()) {
				_text->SetText(oldText.substr(0, _cursorPos) + oldText.substr(_cursorPos + 1, oldText.size() - _cursorPos - 1));
			}
		}
		else if (key == KEY::ARROW_LEFT) {
			if (_cursorPos > 0) {
				--_cursorPos;
			}
		}
		else if (key == KEY::ARROW_RIGHT) {
			if (_cursorPos < _text->GetText().size()) {
				++_cursorPos;
			}
		}

		// U¯YWANIE INNYCH PRZYCISKÓW
		// caps lock; num lock; numpad; insert; escape; tab; left/right shift; right alt; left ctrl
	}
}

void InputField::Initialize()
{
	_onKeyStateChangeID = Input::GetOnKeyStateChange() += [&](KEY key, INPUT_STATE state) -> void {
		if (!_typing) return;
		OnKeyStateChange(key, state);
	};
}

void InputField::Update()
{
	// Check where clicked
	if (!_interactable || _placeHolder == nullptr || _text == nullptr) return;

	if (Input::IsMouseButtonPressed(MOUSE_BUTTON::LEFT)) {
		Transform* t = GetTransform();
		glm::mat4 inv = glm::inverse(t->GetTransformMatrix());
		glm::vec4 mPos = glm::vec4(Input::GetMousePos(), 0.f, 1.f);
		glm::vec3 btnLocalMPos = inv * mPos;

		if (!_typing && btnLocalMPos.x >= -_width / 2.f && btnLocalMPos.x <= _width / 2.f && btnLocalMPos.y >= -_height / 2.f && btnLocalMPos.y <= _height / 2.f) {
			_typing = true;
			_cursorVisible = true;
		}
		else if (btnLocalMPos.x < -_width / 2.f || btnLocalMPos.x > _width / 2.f || btnLocalMPos.y < -_height / 2.f || btnLocalMPos.y > _height / 2.f) {
			_typing = false;
			_cursorVisible = false;
		}
	}

	if (_typing) {
		_currentCursorTime -= Time::GetDeltaTime();
		if (_currentCursorTime <= 0.f) {
			_cursorVisible = !_cursorVisible;
			_currentCursorTime = _cursorDelay;
		}
	}
}

void InputField::Render()
{
	// Render Cursor
	if (_cursorVisible) {
		UIElement elem{};
		// KALKULACJA POZYCJI KURSORA
		elem.elemTransform = _text->GetTransform()->GetTransformMatrix();
		glm::vec2 cursor = glm::vec2(-_text->_width * .5f, 0.f);
		if (_text->_displayTextCharCache.size() != 0) {
			if (_cursorPos >= _text->_displayTextCharCache.size()) {
				auto& c = _text->_displayTextCharCache[_text->_displayTextCharCache.size() - 1];
				cursor += c.cursorPos + glm::vec2(c.character->Advance >> 6, 0);
			}
			else {
				cursor += _text->_displayTextCharCache[_cursorPos].cursorPos;
			}
		}
		elem.elemTransform = glm::translate(elem.elemTransform, glm::vec3(cursor, 0.f));

		if (_text->_overflow != TextOverflow::Overflow) {
			elem.useMask = true;
			elem.maskSize = { _text->_width, _text->_height };
			elem.maskTransform = _text->GetTransform()->GetTransformMatrix();
		}

		elem.color = glm::vec4(.0f, .0f, .0f, .9f);
		elem.elemSize = glm::ivec2(4, _text->GetSize());
		elem.hasTexture = false;
		elem.worldSpaceCanvas = false;
		elem.canvasSize = Window::GetInstance()->GetContentSize();
		UIRenderingManager::Render(elem);
	}
}

void InputField::OnDestroy()
{
	Input::GetOnKeyStateChange() -= _onKeyStateChangeID;
}

YAML::Node InputField::Serialize() const
{
	return YAML::Node();
}

void InputField::SetPlaceHolderText(Text* placeHolder)
{
	_placeHolder = placeHolder;
}

void InputField::SetInputText(Text* text)
{
	_text = text;
}

void InputField::SetPlaceHolder(const std::string& placeHolder)
{
	_placeHolder->SetText(placeHolder);
}

void InputField::SetText(const std::string& text)
{
	_text->SetText(text);
}

void InputField::SetWidth(float width)
{
	_width = width;
}

void InputField::SetHeight(float height)
{
	_height = height;
}

void InputField::SetInteractable(bool interactable)
{
	_interactable = interactable;
}

bool InputField::IsInteractable() const
{
	return _interactable;
}

bool InputField::IsTyping() const
{
	return _typing;
}

float InputField::GetWidth() const
{
	return _width;
}

float InputField::GetHeight() const
{
	return _height;
}

string InputField::GetPlaceHolder() const
{
	return _placeHolder->GetText();
}

string InputField::GetText() const
{
	return _text->GetText();
}

Text* InputField::GetPlaceHolderText() const
{
	return _placeHolder;
}

Text* InputField::GetInputText() const
{
	return _text;
}