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

			_textValue = _textValue.substr(0, _cursorPos) + value + _textValue.substr(_cursorPos, _textValue.size() - _cursorPos);
			++_cursorPos;
			_textDirty = true;
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
			_currentCursorTime = _cursorDelay;
			_cursorPos = 0;
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

		if (Input::IsKeyPressed(KEY::ENTER)) {
			_typing = false;
			_cursorVisible = false;
			_textOffset = 0;
			_textDirty = true;
		}

		if (Input::IsKeyPressed(KEY::BACKSPACE) || (Input::IsKeyHeldDown(KEY::BACKSPACE) && _cursorVisible)) {
			if (_cursorPos > 0) {
				_textValue = _textValue.substr(0, _cursorPos - 1) + _textValue.substr(_cursorPos, _textValue.size() - _cursorPos);
				--_cursorPos;
				if (_textOffset > 0) {
					--_textOffset;
				}
				_textDirty = true;
				_cursorVisible = true;
				_currentCursorTime = _cursorDelay;
			}
		}

		if (Input::IsKeyPressed(KEY::DELETE_KEY) || (Input::IsKeyHeldDown(KEY::DELETE_KEY) && _cursorVisible)) {
			if (_cursorPos < _textValue.size()) {
				_textValue = _textValue.substr(0, _cursorPos) + _textValue.substr(_cursorPos + 1, _textValue.size() - _cursorPos - 1);
				if (_textOffset > 0) {
					--_textOffset;
				}
				_textDirty = true;
				_cursorVisible = true;
				_currentCursorTime = _cursorDelay;
			}
		}
		
		if (Input::IsKeyPressed(KEY::ARROW_LEFT) || (Input::IsKeyHeldDown(KEY::ARROW_LEFT) && _cursorVisible)) {
			if (_cursorPos > 0) {
				--_cursorPos;
				_textDirty = true;
				_cursorVisible = true;
				_currentCursorTime = _cursorDelay;
			}
		}
		
		if (Input::IsKeyPressed(KEY::ARROW_RIGHT) || (Input::IsKeyHeldDown(KEY::ARROW_RIGHT) && _cursorVisible)) {
			if (_cursorPos < _textValue.size()) {
				++_cursorPos;
				_textDirty = true;
				_cursorVisible = true;
				_currentCursorTime = _cursorDelay;
			}
		}

		// UPDATE TEXT
		if (_textDirty) {
			std::string finalText = _textValue.substr(_textOffset, _textValue.size() - _textOffset);
			_text->SetText(finalText);
			_text->UpdateTextMesh();
			if (finalText != "") {
				_placeHolder->SetEnable(false);
				if (_cursorPos != 0) {
					if (_textOffset > _cursorPos) {
						_textOffset -= _textOffset - _cursorPos;
						finalText = _textValue.substr(_textOffset, _textValue.size() - _textOffset);
						_text->SetText(finalText);
						_text->UpdateTextMesh();
					}
					else if (_cursorPos - _textOffset > _text->_displayTextCharCache.size()) {
						_textOffset += (_cursorPos - _textOffset) - _text->_displayTextCharCache.size();
						finalText = _textValue.substr(_textOffset, _textValue.size() - _textOffset);
						_text->SetText(finalText);
						_text->UpdateTextMesh();
					}
				}
			}
			else {
				_placeHolder->SetEnable(true);
			}
			_textDirty = false;
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
		glm::vec2 cursor = glm::vec2(-_text->_width * .5f + 1, 0.f);
		if (_text->_displayTextCharCache.size() != 0) {
			if (_cursorPos - _textOffset >= _text->_displayTextCharCache.size()) {
				auto& c = _text->_displayTextCharCache[_text->_displayTextCharCache.size() - 1];
				cursor += c.cursorPos + glm::vec2(c.character->Advance >> 6, 0);
			}
			else {
				cursor += _text->_displayTextCharCache[_cursorPos - _textOffset].cursorPos;
			}
		}
		elem.elemTransform = glm::translate(elem.elemTransform, glm::vec3(cursor, 0.f));

		if (_text->_overflow != TextOverflow::Overflow) {
			elem.useMask = true;
			elem.maskSize = { _text->_width, _text->_height };
			elem.maskTransform = _text->GetTransform()->GetTransformMatrix();
		}

		elem.color = glm::vec4(.5f, .5f, .5f, .9f);
		elem.elemSize = glm::ivec2(2, _text->GetSize());
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
	_textValue = text;
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
	return _textValue;
}

Text* InputField::GetPlaceHolderText() const
{
	return _placeHolder;
}

Text* InputField::GetInputText() const
{
	return _text;
}