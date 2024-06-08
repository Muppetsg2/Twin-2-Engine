#include <ui/InputField.h>
#include <core/Time.h>
#include <core/Transform.h>
#include <graphic/manager/UIRenderingManager.h>
#include <manager/SceneManager.h>
#include <tools/YamlConverters.h>
#include <ui/Canvas.h>
#include <locale>
#include <codecvt>

using namespace Twin2Engine::UI;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace std;
using namespace glm;

const float InputField::_cursorDelay = 1.f;

void InputField::OnTextInput(unsigned int character)
{
	if (!IsEnable() || !_interactable) return;

	_textValue = _textValue.substr(0, _cursorPos) + (wchar_t)character + _textValue.substr(_cursorPos, _textValue.size() - _cursorPos);
	++_cursorPos;
	_textDirty = true;
}

void InputField::SetCanvas(Canvas* canvas)
{
	if (_canvas != canvas) {
		if (_canvas != nullptr) {
			_canvas->GetOnCanvasDestroy() -= _onCanvasDestroyId;
		}

		_canvas = canvas;
		if (_canvas != nullptr) {
			_onCanvasDestroyId = (_canvas->GetOnCanvasDestroy() += [&](Canvas* canv) -> void { SetCanvas(nullptr); });
		}
	}
}

void InputField::Initialize()
{
	_onTextInputID = Input::GetOnTextInput() += [&](unsigned int character) -> void {
		if (!_typing) return;
		OnTextInput(character);
	};
	_isTransparent = true;

	_onParentInHierarchiChangeId = (GetTransform()->OnEventInHierarchyParentChanged += [&](Transform* t) -> void {
		SetCanvas(GetGameObject()->GetComponentInParents<Canvas>());
	});
	SetCanvas(GetGameObject()->GetComponentInParents<Canvas>());
}

void InputField::Update()
{
	// Check where clicked
	if (!_interactable || _placeHolder == nullptr || _text == nullptr) return;

	if (_canvas != nullptr) {
		SPDLOG_WARN("Input Field can't be in canvas");
		return;
	}

	if (Input::IsMouseButtonPressed(MOUSE_BUTTON::LEFT)) {
		Transform* t = GetTransform();
		glm::mat4 inv = glm::inverse(t->GetTransformMatrix());
		glm::vec4 mPos = glm::vec4(Input::GetCursorPos(), 0.f, 1.f);
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

		if (Input::IsKeyPressed(KEY::ENTER) || Input::IsKeyPressed(KEY::ESCAPE)) {
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

		if (Input::IsKeyPressed(KEY::INSERT)) {
			_insertCursorMode = !_insertCursorMode;
		}

		// UPDATE TEXT
		if (_textDirty) {
			std::wstring finalText = _textValue.substr(_textOffset, _textValue.size() - _textOffset);
			_text->SetText(finalText);
			_text->UpdateTextMesh();
			if (finalText != L"") {
				_placeHolder->SetEnable(false);
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
			else {
				_placeHolder->SetEnable(true);
			}
			_textDirty = false;
		}
	}
}

// TODO: add Canvas
void InputField::Render()
{
	// Render Cursor
	if (_cursorVisible) {
		UIImageData cursorImage{};
		// KALKULACJA POZYCJI KURSORA
		cursorImage.canvas = nullptr; // Na ekranie na razie
		cursorImage.layer = _text->_layer + 1; // Nad tekstem
		cursorImage.rectTransform.transform = _text->GetTransform()->GetTransformMatrix();
		cursorImage.rectTransform.size = uvec2(0.f);
		cursorImage.color = glm::vec4(.5f, .5f, .5f, .9f);

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
		cursorImage.rectTransform.transform = glm::translate(cursorImage.rectTransform.transform, glm::vec3(cursor, 0.f));

		if (_text->_overflow != TextOverflow::Overflow) {
			cursorImage.mask = &_inputMask;
			_inputMask.maskSprite = nullptr;
			_inputMask.rectTransform.transform = cursorImage.rectTransform.transform;
			_inputMask.rectTransform.size = { _text->_width + 4, _text->_height };
		}
		else {
			cursorImage.mask = nullptr;
		}

		if (_insertCursorMode && _cursorPos - _textOffset < _text->_displayTextCharCache.size()) {
			cursorImage.rectTransform.size = glm::uvec2(_text->_displayTextCharCache[_cursorPos - _textOffset].character->Size.x, _text->GetSize());
			cursorImage.rectTransform.transform = glm::translate(cursorImage.rectTransform.transform, glm::vec3(cursorImage.rectTransform.size.x * .5f, 0.f, 0.f));
		}
		else {
			cursorImage.rectTransform.size = glm::uvec2(2U, _text->GetSize());
		}

		UIRenderingManager::Render(cursorImage);
	}
}

void InputField::OnDestroy()
{
	Input::GetOnTextInput() -= _onTextInputID;
	SetCanvas(nullptr);
	GetTransform()->OnEventInHierarchyParentChanged -= _onParentInHierarchiChangeId;
}

YAML::Node InputField::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["type"] = "InputField";
	node["interactable"] = _interactable;
	node["width"] = _width;
	node["height"] = _height;
	if (_placeHolder != nullptr) {
		node["placeHolder"] = _placeHolder->GetId();
	}
	if (_text != nullptr) {
		node["text"] = _text->GetId();
	}
	node["textValue"] = _textValue;
	return node;
}

bool InputField::Deserialize(const YAML::Node& node) {
	if (!node["interactable"] || !node["width"] || !node["height"] || !node["textValue"] || 
		!RenderableComponent::Deserialize(node)) return false;

	_interactable = node["interactable"].as<bool>();
	_width = node["width"].as<float>();
	_height = node["height"].as<float>();
	if (node["placeHolder"]) {
		size_t compId = node["placeHolder"].as<size_t>();
		Component* comp = SceneManager::GetComponentWithId(compId);
		if (comp == nullptr) {
			SPDLOG_ERROR("Couldn't find Component of Id {0}", compId);
		}
		else if (dynamic_cast<Text*>(comp) == nullptr) {
			SPDLOG_ERROR("Component {0} is not Text Component", compId);
		}
		else {
			_placeHolder = (Text*)comp;
		}
	}
	if (node["text"]) {
		size_t compId = node["text"].as<size_t>();
		Component* comp = SceneManager::GetComponentWithId(compId);
		if (comp == nullptr) {
			SPDLOG_ERROR("Couldn't find Component of Id {0}", compId);
		}
		else if (dynamic_cast<Text*>(comp) == nullptr) {
			SPDLOG_ERROR("Component {0} is not Text Component", compId);
		}
		else {
			_text = (Text*)comp;
		}
	}
	_textValue = node["textValue"].as<wstring>();

	return true;
}

#if _DEBUG
void InputField::DrawEditor()
{
	// TODO: Przetestowac
	string id = string(std::to_string(this->GetId()));
	string name = string("InputField##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (Component::DrawInheritedFields()) return;

		float v = _width;
		ImGui::DragFloat(string("Width##").append(id).c_str(), &v, 0.1f, 0.f, FLT_MAX);
		if (v != _width) {
			SetWidth(v);
		}

		v = _height;
		ImGui::DragFloat(string("Height##").append(id).c_str(), &v, 0.1f, 0.f, FLT_MAX);
		if (v != _height) {
			SetHeight(v);
		}

		ImGui::Checkbox(string("Interactable##").append(id).c_str(), &_interactable);

		std::unordered_map<size_t, Component*> items = SceneManager::GetComponentsOfType<Text>();
		size_t choosed_holder = _placeHolder == nullptr ? 0 : _placeHolder->GetId();
		size_t choosed_text = _text == nullptr ? 0 : _text->GetId();

		if (ImGui::BeginCombo(string("Placeholder##").append(id).c_str(), choosed_holder == 0 ? "None" : items[choosed_holder]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_text) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_holder)) {

					if (clicked) continue;

					choosed_holder = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_holder != 0) {
					SetPlaceHolderText(static_cast<Text*>(items[choosed_holder]));
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginDragDropTarget()) {

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneHierarchyObject"))
			{
				size_t payload_n = *(const size_t*)payload->Data;
				Text* t = SceneManager::GetGameObjectWithId(payload_n)->GetComponent<Text>();
				if (t != nullptr) { 
					if (t->GetId() != choosed_text) SetPlaceHolderText(t);
				}
			}

			ImGui::EndDragDropTarget();
		}

		string buff = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_placeHolder->GetText());
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

		ImGui::InputTextMultiline(string("Placeholder Value##").append(id).c_str(), &buff, ImVec2(0, 100), flags);

		if (buff != std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_placeHolder->GetText())) {
			SetPlaceHolder(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(buff));
		}

		if (ImGui::BeginCombo(string("Text##").append(id).c_str(), choosed_text == 0 ? "None" : items[choosed_text]->GetGameObject()->GetName().c_str())) {

			bool clicked = false;
			for (auto& item : items) {

				if (item.second->GetId() == choosed_holder) continue;

				if (ImGui::Selectable(std::string(item.second->GetGameObject()->GetName().c_str()).append("##").append(id).c_str(), item.first == choosed_text)) {

					if (clicked) continue;

					choosed_text = item.first;
					clicked = true;
				}
			}

			if (clicked) {
				if (choosed_text != 0) {
					SetInputText(static_cast<Text*>(items[choosed_text]));
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginDragDropTarget()) {

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneHierarchyObject"))
			{
				size_t payload_n = *(const size_t*)payload->Data;
				Text* t = SceneManager::GetGameObjectWithId(payload_n)->GetComponent<Text>();
				if (t != nullptr) {
					if (t->GetId() != choosed_holder) SetInputText(t);
				}
			}

			ImGui::EndDragDropTarget();
		}

		buff = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_textValue);

		ImGui::InputTextMultiline(string("Value##").append(id).c_str(), &buff, ImVec2(0, 100), flags);

		if (buff != std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_textValue)) {
			SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(buff));
		}
	}
}
#endif

void InputField::SetPlaceHolderText(Text* placeHolder)
{
	_placeHolder = placeHolder;
}

void InputField::SetInputText(Text* text)
{
	_text = text;
}

void InputField::SetPlaceHolder(const wstring& placeHolder)
{
	_placeHolder->SetText(placeHolder);
}

void InputField::SetText(const wstring& text)
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

wstring InputField::GetPlaceHolder() const
{
	return _placeHolder->GetText();
}

wstring InputField::GetText() const
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