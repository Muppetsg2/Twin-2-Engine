#include <ui/Button.h>
#include <core/GameObject.h>
#include <core/Input.h>
#include <tools/YamlConverters.h>
#include <ui/Canvas.h>
#include <graphic/Window.h>
#include <core/Time.h>

using namespace Twin2Engine::UI;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Tools;

void Button::SetCanvas(Canvas* canvas)
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

// TODO: FINISH
void Button::OnHoverPresetEvents(bool isHover)
{
	static float time = 0.f;

	if (isHover && time < 1.f) time += Time::GetDeltaTime() * _timeFactor;
	else if (isHover && time > 1.f) time = 1.f;
	else if (!isHover && time > 0.f) time -= Time::GetDeltaTime() * _timeFactor;
	else if (!isHover && time < 0.f) time = 0.f;

	glm::vec3 locScale = GetTransform()->GetLocalScale();
	if (_onHoverScaleStart != _onHoverScaleEnd) {
		if (!(time == 0.f && locScale == _onHoverScaleStart) && !(time == 1.f && locScale == _onHoverScaleEnd)) {
			GetTransform()->SetLocalScale(glm::mix(_onHoverScaleStart, _onHoverScaleEnd, time));
		}
	}

	if (_onHoverTranslate != glm::vec3(0.f)) {

	}

	glm::vec4 _onHoverColor = glm::vec4(0.f); // Requires Image Component in same GameObject
}

// TODO: FINISH
void Button::OnClickPresetEvents()
{
	bool _playAudioOnClick = false; // Requires Audio Component in same GameObject
	size_t _onClickAudioId = 0;
}

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

MethodEventHandler& Button::GetOnHoverEvent()
{
	return _onHoverEvent;
}

void Button::Initialize()
{
	_onParentInHierarchiChangeId = (GetTransform()->OnEventInHierarchyParentChanged += [&](Transform* t) -> void {
		SetCanvas(GetGameObject()->GetComponentInParents<Canvas>());
	});
	SetCanvas(GetGameObject()->GetComponentInParents<Canvas>());
}

// TODO: Add Button to Canvas (least prority)
void Button::Update()
{
	if (!_interactable) return;

	if (_canvas != nullptr) {
		if (_canvas->_data.worldSpaceCanvas) {
			SPDLOG_WARN("Button not working in world Space");
			return;
		}
	}

	Transform* t = GetTransform();
	glm::mat4 inv = glm::inverse(t->GetTransformMatrix());
	glm::vec4 mPos = glm::vec4(Input::GetCursorPos(), 0.f, 1.f);
	//glm::vec3 btnLocalMPos = canvT * inv * glm::vec4(glm::vec2(invCanvT * mPos) * .5f * canvS, 0.f, 1.f);
	glm::vec3 btnLocalMPos = inv * mPos;

	if (btnLocalMPos.x >= -_width * .5f && btnLocalMPos.x <= _width * .5f && btnLocalMPos.y >= -_height * .5f && btnLocalMPos.y <= _height * .5f) {

		_onHoverEvent.Invoke();
		OnHoverPresetEvents();

		if (Input::IsMouseButtonPressed(MOUSE_BUTTON::LEFT)) {

			/*glm::mat4 canvT = glm::mat4(1.f);
			glm::vec2 canvS = Graphic::Window::GetInstance()->GetContentSize();
			if (_canvas != nullptr) {
				canvT = _canvas->GetTransform()->GetTransformMatrix();
				canvS = _canvas->_data.rectTransform.size;
			}
			glm::mat4 invCanvT = glm::inverse(canvT);*/
			_onClickEvent.Invoke();
		}
	}
}

void Button::OnDestroy()
{
	SetCanvas(nullptr);
	GetTransform()->OnEventInHierarchyParentChanged -= _onParentInHierarchiChangeId;
}

YAML::Node Button::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "Button";
	node["width"] = _width;
	node["height"] = _height;
	node["interactable"] = _interactable;
	return node;
}

bool Button::Deserialize(const YAML::Node& node)
{
	if (!node["width"] || !node["height"] || !node["interactable"] ||
		!Component::Deserialize(node)) return false;

	_width = node["width"].as<float>();
	_height = node["height"].as<float>();
	_interactable = node["interactable"].as<bool>();

	return true;
}

#if _DEBUG
void Button::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Button##Component").append(id);
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
	}
}
#endif