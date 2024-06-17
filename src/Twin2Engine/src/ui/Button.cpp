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
#include <manager/SceneManager.h>

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
	if (isHover && _animTime < 1.f) _animTime += Time::GetDeltaTime() * _animTimeFactor;
	else if (isHover && _animTime > 1.f) _animTime = 1.f;
	else if (!isHover && _animTime > 0.f) _animTime -= Time::GetDeltaTime() * _animTimeFactor;
	else if (!isHover && _animTime < 0.f) _animTime = 0.f;

	glm::vec3 locScale = GetTransform()->GetLocalScale();
	if (_onHoverScaleStart != _onHoverScaleEnd) {
		if (!(_animTime == 0.f && locScale == _onHoverScaleStart) && !(_animTime == 1.f && locScale == _onHoverScaleEnd)) {
			GetTransform()->SetLocalScale(glm::mix(_onHoverScaleStart, _onHoverScaleEnd, getEaseFuncValue(_onHoverScaleFunc, _onHoverScaleFuncType, _animTime)));
		}
	}

	if (GetGameObject()->GetComponent<Image>() != nullptr) {
		GetGameObject()->GetComponent<Image>()->SetColor(glm::mix(_onHoverColorStart, _onHoverColorEnd, getEaseFuncValue(_onHoverColorFunc, _onHoverColorFuncType, _animTime)));
	}

	if (_displayObjectOnHover && _objectToDisplay != nullptr) 
	{
		if (_objectSnapToMouse) {
			glm::vec2 pos = Input::GetCursorPos() + _objectOffset;
			_objectToDisplay->GetTransform()->SetGlobalPosition(glm::vec3(pos, 0.f));
		}

		_objectToDisplay->SetActive(isHover);
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
	return _animTimeFactor;
}

void Button::SetTimeFactor(float factor)
{
	if (factor != _animTimeFactor) _animTimeFactor = factor;
}

EaseFunction Button::GetOnHoverScaleFunc()
{
	return _onHoverScaleFunc;
}

void Button::SetOnHoverScaleFunc(EaseFunction func)
{
	_onHoverScaleFunc = func;
}

EaseFunctionType Button::GetOnHoverScaleFuncType()
{
	return _onHoverScaleFuncType;
}

void Button::SetOnHoverScaleFuncType(EaseFunctionType funcType)
{
	_onHoverScaleFuncType = funcType;
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

EaseFunction Button::GetOnHoverColorFunc()
{
	return _onHoverColorFunc;
}

void Button::SetOnHoverColorFunc(EaseFunction func)
{
	_onHoverColorFunc = func;
}

EaseFunctionType Button::GetOnHoverColorFuncType()
{
	return _onHoverColorFuncType;
}

void Button::SetOnHoverColorFuncType(EaseFunctionType funcType)
{
	_onHoverColorFuncType = funcType;
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

void Button::EnableOnHoverDisplayObject()
{
	if (!_displayObjectOnHover) {
		_displayObjectOnHover = true;
	}
}

void Button::DisableOnHoverDisplayObject()
{
	if (_displayObjectOnHover) {
		_displayObjectOnHover = false;
	}
}

void Button::SnapObjectToMouseOnHover(bool value)
{
	if (_objectSnapToMouse != value) {
		_objectSnapToMouse = value;
	}
}

glm::vec2 Button::GetOnHoverObjectOffset()
{
	return _objectOffset;
}

void Button::SetOnHoverObjectOffset(glm::vec2 value)
{
	if (_objectOffset != value) {
		SetOnHoverObjectOffset(_objectOffset.x, _objectOffset.y);
	}
}

void Button::SetOnHoverObjectOffset(float x, float y)
{
	if (_objectOffset.x != x) _objectOffset.x = x;
	if (_objectOffset.y != y) _objectOffset.y = y;
}

GameObject* Button::GetOnHoverDisplayObject()
{
	return _objectToDisplay;
}

void Button::SetOnHoverDisplayObject(Core::GameObject* obj)
{
	_objectToDisplay = obj;
}

void Button::SetOnHoverDisplayObject(size_t objId)
{
	SetOnHoverDisplayObject(Manager::SceneManager::GetGameObjectWithId(objId));
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
	node["onHoverPresetEvents"] = _useOnHoverPresetEvents;

	if (_useOnHoverPresetEvents) {
		node["animTimeFactor"] = _animTimeFactor;
		node["scaleFunc"] = (int)(uint8_t)_onHoverScaleFunc;
		node["scaleFuncType"] = (int)(uint8_t)_onHoverScaleFuncType;
		node["scaleStart"] = _onHoverScaleStart;
		node["scaleEnd"] = _onHoverScaleEnd;
		node["colorFunc"] = (int)(uint8_t)_onHoverColorFunc;
		node["colorFuncType"] = (int)(uint8_t)_onHoverColorFuncType;
		node["colorStart"] = _onHoverColorStart;
		node["colorEnd"] = _onHoverColorEnd;

		node["displayObjectOnHover"] = _displayObjectOnHover;

		if (_displayObjectOnHover) {

			node["snapToMouse"] = _objectSnapToMouse;

			if (_objectSnapToMouse) node["objectOffset"] = _objectOffset;

			node["objectToDisplay"] = _objectToDisplay == nullptr ? 0 : _objectToDisplay->Id();
		}
	}

	node["playAudioOnClick"] = _playAudioOnClick;

	if (_playAudioOnClick) {
		node["onClickAudioId"] = Manager::SceneManager::GetAudioSaveIdx(_onClickAudioId);
	}

	return node;
}

bool Button::Deserialize(const YAML::Node& node)
{
	if (!node["width"] || !node["height"] || !node["interactable"] ||
		!node["onHoverPresetEvents"] || !node["playAudioOnClick"] ||
		!Component::Deserialize(node)) return false;

	_width = node["width"].as<float>();
	_height = node["height"].as<float>();
	_interactable = node["interactable"].as<bool>();
	_useOnHoverPresetEvents = node["onHoverPresetEvents"].as<bool>();

	if (_useOnHoverPresetEvents) {
		_animTimeFactor = node["animTimeFactor"].as<float>();
		_onHoverScaleFunc = (EaseFunction)(uint8_t)node["scaleFunc"].as<int>();
		_onHoverScaleFuncType = (EaseFunctionType)(uint8_t)node["scaleFuncType"].as<int>();
		_onHoverScaleStart = node["scaleStart"].as<glm::vec3>();
		_onHoverScaleEnd = node["scaleEnd"].as<glm::vec3>();
		_onHoverColorFunc = (EaseFunction)(uint8_t)node["colorFunc"].as<int>();
		_onHoverColorFuncType = (EaseFunctionType)(uint8_t)node["colorFuncType"].as<int>();
		_onHoverColorStart = node["colorStart"].as<glm::vec4>();
		_onHoverColorEnd = node["colorEnd"].as<glm::vec4>();

		_displayObjectOnHover = node["displayObjectOnHover"].as<bool>();

		if (_displayObjectOnHover) {

			_objectSnapToMouse = node["snapToMouse"].as<bool>();

			if (_objectSnapToMouse) _objectOffset = node["objectOffset"].as<glm::vec2>();

			_objectToDisplay = node["objectToDisplay"].as<size_t>() == 0 ? nullptr : Manager::SceneManager::GetGameObjectWithId(node["objectToDisplay"].as<size_t>());
		}
	}

	_playAudioOnClick = node["playAudioOnClick"].as<bool>();

	if (_playAudioOnClick) {
		_onClickAudioId = Manager::SceneManager::GetAudio(node["onClickAudioId"].as<size_t>());
	}

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
				float v = _animTimeFactor;
				ImGui::DragFloat(string("Anim Time Factor##").append(id).c_str(), &v, 0.01f, 0.f, FLT_MAX);
				if (v != _animTimeFactor) _animTimeFactor = v;

				if (ImGui::BeginCombo(string("Scale Func##").append(id).c_str(), to_string(_onHoverScaleFunc).c_str())) {

					bool clicked = false;
					uint8_t choosed = (uint8_t)_onHoverScaleFunc;
					for (uint8_t i = (uint8_t)EaseFunction::LINE; i < (uint8_t)EaseFunction::BOUNCE + 1; ++i) {

						if (ImGui::Selectable(to_string((EaseFunction)i).append("##").append(id).c_str(), i == (uint8_t)_onHoverScaleFunc)) {

							if (clicked) continue;

							choosed = i;
							clicked = true;
						}
					}

					if (clicked) {
						_onHoverScaleFunc = (EaseFunction)choosed;
					}

					ImGui::EndCombo();
				}

				if (ImGui::BeginCombo(string("Scale Func Type##").append(id).c_str(), to_string(_onHoverScaleFuncType).c_str())) {

					bool clicked = false;
					uint8_t choosed = (uint8_t)_onHoverScaleFuncType;
					for (uint8_t i = (uint8_t)EaseFunctionType::IN_F; i < (uint8_t)EaseFunctionType::IN_OUT_F + 1; ++i) {

						if (ImGui::Selectable(to_string((EaseFunctionType)i).append("##").append(id).c_str(), i == (uint8_t)_onHoverScaleFuncType)) {

							if (clicked) continue;

							choosed = i;
							clicked = true;
						}
					}

					if (clicked) {
						_onHoverScaleFuncType = (EaseFunctionType)choosed;
					}

					ImGui::EndCombo();
				}

				glm::vec3 s = _onHoverScaleStart;
				ImGui::DragFloat3(string("Start Scale##").append(id).c_str(), glm::value_ptr(s), 0.1f);
				if (s != _onHoverScaleStart) _onHoverScaleStart = s;

				s = _onHoverScaleEnd;
				ImGui::DragFloat3(string("End Scale##").append(id).c_str(), glm::value_ptr(s), 0.1f);
				if (s != _onHoverScaleEnd) _onHoverScaleEnd = s;

				if (ImGui::BeginCombo(string("Color Func##").append(id).c_str(), to_string(_onHoverColorFunc).c_str())) {

					bool clicked = false;
					uint8_t choosed = (uint8_t)_onHoverColorFunc;
					for (uint8_t i = (uint8_t)EaseFunction::LINE; i < (uint8_t)EaseFunction::BOUNCE + 1; ++i) {

						if (ImGui::Selectable(to_string((EaseFunction)i).append("##").append(id).c_str(), i == (uint8_t)_onHoverColorFunc)) {

							if (clicked) continue;

							choosed = i;
							clicked = true;
						}
					}

					if (clicked) {
						_onHoverColorFunc = (EaseFunction)choosed;
					}

					ImGui::EndCombo();
				}

				if (ImGui::BeginCombo(string("Color Func Type##").append(id).c_str(), to_string(_onHoverColorFuncType).c_str())) {

					bool clicked = false;
					uint8_t choosed = (uint8_t)_onHoverColorFuncType;
					for (uint8_t i = (uint8_t)EaseFunctionType::IN_F; i < (uint8_t)EaseFunctionType::IN_OUT_F + 1; ++i) {

						if (ImGui::Selectable(to_string((EaseFunctionType)i).append("##").append(id).c_str(), i == (uint8_t)_onHoverColorFuncType)) {

							if (clicked) continue;

							choosed = i;
							clicked = true;
						}
					}

					if (clicked) {
						_onHoverColorFuncType = (EaseFunctionType)choosed;
					}

					ImGui::EndCombo();
				}

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

				ImGui::Checkbox(string("Display Object##").append(id).c_str(), &_displayObjectOnHover);
				ImGui::BeginDisabled(!_displayObjectOnHover);
				{
					ImGui::Checkbox(string("Snap Object To Mouse##").append(id).c_str(), &_objectSnapToMouse);
					ImGui::BeginDisabled(!_objectSnapToMouse);
					{
						glm::vec2 v2 = _objectOffset;
						ImGui::DragFloat2(string("Object Offset##").append(id).c_str(), glm::value_ptr(v2), 0.1f);
						if (v2 != _objectOffset) _objectOffset = v2;
					}
					ImGui::EndDisabled();


					std::vector<Core::GameObject*> objs = Manager::SceneManager::GetAllGameObjects();
					objs.erase(objs.begin());

					std::sort(objs.begin(), objs.end(), [&](Core::GameObject* const& left, Core::GameObject* const& right) -> bool {
						return left->GetName().compare(right->GetName()) < 0;
					});

					vector<string> names = vector<string>();
					vector<size_t> ids = vector<size_t>();
					names.resize(objs.size());
					ids.resize(objs.size());

					std::transform(objs.begin(), objs.end(), names.begin(), [](Core::GameObject* const& i) -> string {
						return string(i->GetName()).append("##").append(std::to_string(i->Id()));
					});
					std::transform(objs.begin(), objs.end(), ids.begin(), [](Core::GameObject* const& i) -> size_t {
						return i->Id();
					});

					objs.clear();

					names.insert(names.begin(), "None");
					ids.insert(ids.begin(), 0);

					size_t choosed = _objectToDisplay == nullptr ? 0 : std::find(ids.begin(), ids.end(), _objectToDisplay->Id()) - ids.begin();

					if (ImGui::ComboWithFilter(string("Object To Display##").append(id).c_str(), &choosed, names, 20)) {
						if (choosed != 0) {
							_objectToDisplay = Manager::SceneManager::GetGameObjectWithId(ids[choosed]);
						}
						else {
							_objectToDisplay = nullptr;
						}
					}
					ImGui::SameLine();
					ImGui::HelpMarker("If no object provided then no object will be displayed");

					names.clear();
					ids.clear();
				}
				ImGui::EndDisabled();			
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