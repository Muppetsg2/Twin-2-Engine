#include <ui/Button.h>
#include <core/GameObject.h>
#include <core/Input.h>
#include <tools/YamlConverters.h>
#include <ui/Canvas.h>
#include <graphic/Window.h>
#include <core/Time.h>
#include <ui/Image.h>
#include <core/AudioComponent.h>
#include <manager/AudioManager.h>

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

	if (GetGameObject()->GetComponent<Image>() != nullptr) {
		GetGameObject()->GetComponent<Image>()->SetColor(glm::mix(_onHoverColorStart, _onHoverColorEnd, time));
	}
}

void Button::OnClickPresetEvents()
{
	AudioComponent* audio = GetGameObject()->GetComponent<AudioComponent>();
	if (audio != nullptr && _playAudioOnClick && _onClickAudioId != 0) {

		if (Manager::AudioManager::IsAudioValid(_onClickAudioId)) {
			audio->SetAudio(_onClickAudioId);
			audio->UnLoop();
			audio->Stop();
			audio->Play();
		}
		else {
			SPDLOG_WARN("OnClickAudio in Button is not valid. Please Load Audio in Audio Manager First");
		}
	}
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

void Button::EnableOnHoverPresetEvents()
{
	if (!_useOnHoverPresetEvents) _useOnHoverPresetEvents = true;
}

void Button::DisableOnHoverPresetEvents()
{
	if (_useOnHoverPresetEvents) _useOnHoverPresetEvents = false;
}

float Button::GetTimeFactor()
{
	return _timeFactor;
}

void Button::SetTimeFactor(float factor)
{
	if (factor != _timeFactor) _timeFactor = factor;
}

std::pair<glm::vec3, glm::vec3> Button::GetOnHoverScale()
{
	return std::pair<glm::vec3, glm::vec3>(_onHoverScaleStart, _onHoverScaleEnd);
}

void Button::SetOnHoverScale(std::pair<glm::vec3, glm::vec3> values)
{
	_onHoverScaleStart = values.first;
	_onHoverScaleEnd = values.second;
}

void Button::SetOnHoverScale(glm::vec3 start, glm::vec3 end)
{
	_onHoverScaleStart = start;
	_onHoverScaleEnd = end;
}

std::pair<glm::vec4, glm::vec4> Button::GetOnHoverColor()
{
	return std::pair<glm::vec4, glm::vec4>(_onHoverColorStart, _onHoverColorEnd);
}

void Button::SetOnHoverColor(std::pair<glm::vec4, glm::vec4> values)
{
	_onHoverColorStart = values.first;
	_onHoverColorEnd = values.second;
}

void Button::SetOnHoverColor(glm::vec4 start, glm::vec4 end)
{
	_onHoverColorStart = start;
	_onHoverColorEnd = end;
}

void Button::EnableOnClickAudio()
{
	if (!_playAudioOnClick) _playAudioOnClick = true;
}

void Button::DisableOnClickAudio()
{
	if (_playAudioOnClick) _playAudioOnClick = false;
}

size_t Button::GetAudioOnClick()
{
	return _onClickAudioId;
}

void Button::SetAudioOnClick(size_t id)
{
	if (_onClickAudioId != id) {
		_onClickAudioId = id;
	}
}

void Button::SetAudioOnClick(string path)
{
	size_t id = hash<string>{}(path);
	SetAudioOnClick(id);
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

	bool hover = false;
	Transform* t = GetTransform();
	glm::mat4 inv = glm::inverse(t->GetTransformMatrix());
	glm::vec4 mPos = glm::vec4(Input::GetCursorPos(), 0.f, 1.f);
	//glm::vec3 btnLocalMPos = canvT * inv * glm::vec4(glm::vec2(invCanvT * mPos) * .5f * canvS, 0.f, 1.f);
	glm::vec3 btnLocalMPos = inv * mPos;

	if (btnLocalMPos.x >= -_width * .5f && btnLocalMPos.x <= _width * .5f && btnLocalMPos.y >= -_height * .5f && btnLocalMPos.y <= _height * .5f) {

		_onHoverEvent.Invoke();
		hover = true;
		if (_useOnHoverPresetEvents) {
			OnHoverPresetEvents(hover);
		}

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

	if (!hover && _useOnHoverPresetEvents) {
		OnHoverPresetEvents(hover);
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

		// Preset Events
		if (_interactable) {

			ImGui::Checkbox(string("On Hover Preset Events##").append(id).c_str(), &_useOnHoverPresetEvents);

			ImGui::BeginDisabled(!_useOnHoverPresetEvents);
			{
				float v = _timeFactor;
				ImGui::DragFloat(string("Time Factor##").append(id).c_str(), &v, 0.01f, 0.f, FLT_MAX);
				if (v != _timeFactor) _timeFactor = v;

				glm::vec3 s = _onHoverScaleStart;
				ImGui::DragFloat3(string("Start Scale##").append(id).c_str(), glm::value_ptr(s), 0.1f);
				if (s != _onHoverScaleStart) _onHoverScaleStart = s;

				s = _onHoverScaleEnd;
				ImGui::DragFloat3(string("End Scale##").append(id).c_str(), glm::value_ptr(s), 0.1f);
				if (s != _onHoverScaleEnd) _onHoverScaleEnd = s;

				glm::vec4 c = _onHoverColorStart;
				ImGui::DragFloat3(string("Start Color##").append(id).c_str(), glm::value_ptr(c), 0.1f); 
				ImGui::SameLine();
				ImGui::HelpMarker("You need to add Image Component if you want this preset to work");
				if (c != _onHoverColorStart) _onHoverColorStart = c;

				c = _onHoverColorEnd;
				ImGui::DragFloat3(string("End Color##").append(id).c_str(), glm::value_ptr(c), 0.1f);
				ImGui::SameLine();
				ImGui::HelpMarker("You need to add Image Component if you want this preset to work");
				if (c != _onHoverColorEnd) _onHoverColorEnd = c;
			}
			ImGui::EndDisabled();

			ImGui::Checkbox(string("Play Audio On Click##").append(id).c_str(), &_playAudioOnClick);
			ImGui::SameLine();
			ImGui::HelpMarker("You need to add Audio Component if you want this preset to work");

			ImGui::BeginDisabled(!_playAudioOnClick);
			{
				std::map<size_t, string> audioNames = Manager::AudioManager::GetAllAudiosNames();

				std::vector<std::pair<size_t, string>> reMap = std::vector<std::pair<size_t, string>>(audioNames.begin(), audioNames.end());

				audioNames.clear();

				std::sort(reMap.begin(), reMap.end(), [&](std::pair<size_t, string> const& left, std::pair<size_t, string> const& right) -> bool {
					return left.second.compare(right.second) < 0;
				});

				vector<string> names = vector<string>();
				vector<size_t> ids = vector<size_t>();
				names.resize(reMap.size());
				ids.resize(reMap.size());

				std::transform(reMap.begin(), reMap.end(), names.begin(), [](std::pair<size_t, string> const& i) -> string {
					return i.second;
				});

				std::transform(reMap.begin(), reMap.end(), ids.begin(), [](std::pair<size_t, string> const& i) -> size_t {
					return i.first;
				});

				int choosed = -1;
				if (ImGui::ComboWithFilter(string("OnClickAudio##").append(id).c_str(), &choosed, names, 20)) {
					if (choosed != -1) {
						_onClickAudioId = ids[choosed];
					}
				}
				ImGui::SameLine();
				ImGui::HelpMarker("You need to add Audio Component if you want this preset to work");
			}
			ImGui::EndDisabled();
		}
	}
}
#endif