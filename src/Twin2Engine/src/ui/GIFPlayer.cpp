#include <ui/GIFPlayer.h>
#include <core/Transform.h>
#include <iostream>
#include <core/Time.h>
#include <core/GameObject.h>
#include <manager/SceneManager.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::UI;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Graphic;
using namespace std;
using namespace glm;

void GIFPlayer::SetCanvas(Canvas* canvas)
{
	if (_canvas != canvas) {
		if (_canvas != nullptr) {
			_canvas->GetOnCanvasDestroy() -= _onCanvasDestroyId;
			_data.canvas = nullptr;
		}

		_canvas = canvas;
		if (_canvas != nullptr) {
			_onCanvasDestroyId = (_canvas->GetOnCanvasDestroy() += [&](Canvas* canv) -> void { SetCanvas(nullptr); });
			_data.canvas = &_canvas->_data;
		}
	}
}

void GIFPlayer::Initialize()
{
	Transform* tr = GetTransform();
	_onTransformChangeId = (tr->OnEventTransformChanged += [&](Transform* t) -> void {
		_data.rectTransform.transform = t->GetTransformMatrix();
	});
	_onRotationChangeId = (tr->OnEventRotationChanged += [&](Transform* t) -> void {
		_fillData.rotation = t->GetGlobalRotation().z;
	});
	_onParentInHierarchiChangeId = (tr->OnEventInHierarchyParentChanged += [&](Transform* t) -> void {
		SetCanvas(GetGameObject()->GetComponentInParents<Canvas>());
		_data.rectTransform.transform = t->GetTransformMatrix();
	});
	SetCanvas(GetGameObject()->GetComponentInParents<Canvas>());

	if (_autoPlay && !_paused) {
		Play();
	}
}

void GIFPlayer::OnEnable()
{
	if (_autoPlay && !_paused) {
		Play();
	}
}

void GIFPlayer::Update()
{
	GIF* gif = GIFManager::Get(_gifId);
	if (gif == nullptr) return;

	if (_playing && !_paused && ((_looped && _imgIdx < gif->GetFramesCount()) || !_looped)) {
		_currTime += Time::GetDeltaTime();
		if (_currTime >= gif->GetDelay(_imgIdx)) {
			_currTime = 0.f;
			_imgIdx += 1;
			if (_looped) _imgIdx %= gif->GetFramesCount();
		}
	}
}

void GIFPlayer::Render()
{
	GIF* gif = GIFManager::Get(_gifId);
	Texture2D* tex = nullptr;
	if (gif != nullptr) tex = gif->GetFrameTexture(_imgIdx);
	UIRenderingManager::Render(_data, _fillData, tex);
}

void GIFPlayer::OnDestroy()
{
	SetCanvas(nullptr);
	Transform* tr = GetTransform();
	tr->OnEventTransformChanged -= _onTransformChangeId;
	tr->OnEventRotationChanged -= _onRotationChangeId;
	tr->OnEventInHierarchyParentChanged -= _onParentInHierarchiChangeId;
}

void GIFPlayer::SetSize(const glm::vec2& size)
{
	if (_data.rectTransform.size != size) {
		_data.rectTransform.size = size;
	}
}

void GIFPlayer::SetColor(const glm::vec4& color)
{
	if (_data.color != color) {
		_data.color = color;
	}
}

void GIFPlayer::SetLayer(int32_t layer)
{
	if (_data.layer != layer) {
		_data.layer = layer;
	}
}

void GIFPlayer::SetGIF(size_t gifId)
{
	if (_gifId != gifId) {
		_gifId = gifId;
		Stop();
		if (_autoPlay) {
			Play();
		}
	}
}

void GIFPlayer::SetGIF(const string& gifPath)
{
	SetGIF(hash<string>()(gifPath));
}

void GIFPlayer::SetGIF(const GIF* gif)
{
	SetGIF(gif->GetManagerId());
}

vec2 GIFPlayer::GetSize() const
{
	return _data.rectTransform.size;
}

vec4 GIFPlayer::GetColor() const
{
	return _data.color;
}

int32_t GIFPlayer::GetLayer() const
{
	return _data.layer;
}

GIF* GIFPlayer::GetGIF() const
{
	return GIFManager::Get(_gifId);
}

void GIFPlayer::Play()
{
	_playing = true;
	_paused = false;
}

void GIFPlayer::Stop()
{
	_playing = false;
	_paused = false;
	_imgIdx = 0;
	_currTime = 0.f;
}

void GIFPlayer::Pause()
{
	_paused = true;
}

void GIFPlayer::Loop(bool loop)
{
	if (_looped != loop) {
		_looped = loop;

		if (loop) {
			GIF* gif = GIFManager::Get(_gifId);
			if (gif != nullptr) {
				size_t oldIdx = _imgIdx;
				_imgIdx %= gif->GetFramesCount();
				if (_imgIdx != oldIdx) _currTime = 0.f;
			}
			else {
				_imgIdx = 0;
				_currTime = 0.f;
			}
		}
	}
}

void GIFPlayer::AutoPlay(bool autoPlay)
{
	if (_autoPlay != autoPlay) {
		_autoPlay = autoPlay;
	}
}

bool GIFPlayer::IsPlaying() const
{
	return _playing && !_paused;
}

bool GIFPlayer::IsStopped() const
{
	return !_playing;
}

bool GIFPlayer::IsPaused() const
{
	return _playing && _paused;
}

bool GIFPlayer::IsLooped() const
{
	return _looped;
}

bool GIFPlayer::HasAutoPlay() const
{
	return _autoPlay;
}

YAML::Node GIFPlayer::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["type"] = "GIFPlayer";

	if (GIFManager::Get(_gifId) != nullptr) {
		node["gif"] = SceneManager::GetGIFSaveIdx(_gifId);
	}
	node["color"] = _data.color;
	node["width"] = _data.rectTransform.size.x;
	node["height"] = _data.rectTransform.size.y;
	node["layer"] = _data.layer;

	// FILL
	node["fillEnabled"] = _fillData.isActive;
	node["fillType"] = (uint32_t)_fillData.type;
	node["fillType"].SetTag(to_string((FILL_TYPE)_fillData.type));
	Tools::Func<string, uint8_t> toString = [](uint8_t value) -> string { return to_string(value); };
	switch (_fillData.type) {
	case (uint8_t)FILL_TYPE::HORIZONTAL:
		toString = [](uint8_t value) -> string { return to_string((HORIZONTAL_FILL_SUBTYPE)value); };
		break;
	case (uint8_t)FILL_TYPE::VERTICAL:
		toString = [](uint8_t value) -> string { return to_string((VERTICAL_FILL_SUBTYPE)value); };
		break;
	case (uint8_t)FILL_TYPE::CIRCLE:
		toString = [](uint8_t value) -> string { return to_string((CIRCLE_FILL_SUBTYPE)value); };
		break;
	}
	node["fillSubType"] = (uint32_t)_fillData.subType;
	node["fillSubType"].SetTag(toString(_fillData.subType));
	node["fillOffset"] = _fillData.offset;
	node["fillProgress"] = _fillData.progress;

	node["loop"] = _looped;
	node["autoPlay"] = _autoPlay;

	return node;
}

bool GIFPlayer::Deserialize(const YAML::Node& node)
{
	bool goodData = true;

	goodData = goodData && RenderableComponent::Deserialize(node);

	if (node["gif"]) {
		_gifId = SceneManager::GetGIF(node["gif"].as<size_t>());
	}
	else {
		_gifId = 0;
	}
	goodData = goodData && node["gif"];

	if (node["color"])
		_data.color = node["color"].as<glm::vec4>();
	goodData = goodData && node["color"];

	if (node["width"])
		_data.rectTransform.size.x = node["width"].as<float>();
	goodData = goodData && node["width"];
	
	if (node["height"])
		_data.rectTransform.size.y = node["height"].as<float>();
	goodData = goodData && node["height"];
	
	if (node["layer"])
		_data.layer = node["layer"].as<int32_t>();
	goodData = goodData && node["layer"];

	if (node["fillEnabled"])
		_fillData.isActive = node["fillEnabled"].as<bool>();
	goodData = goodData && node["fillEnabled"];
	
	if (node["fillType"])
		_fillData.type = node["fillType"].as<uint32_t>();
	goodData = goodData && node["fillType"];
	
	if (node["fillSubType"])
		_fillData.subType = node["fillSubType"].as<uint32_t>();
	goodData = goodData && node["fillSubType"];
	
	if (node["fillOffset"])
		_fillData.offset = node["fillOffset"].as<float>();
	goodData = goodData && node["fillOffset"];
	
	if (node["fillProgress"])
		_fillData.progress = node["fillProgress"].as<float>();
	goodData = goodData && node["fillProgress"];

	if (node["loop"])
		_looped = node["loop"].as<bool>();
	goodData = goodData && node["loop"];

	if (node["autoPlaye"])
		_autoPlay = node["autoPlay"].as<bool>();
	goodData = goodData && node["autoPlay"];

	return goodData;
}

#if _DEBUG
void GIFPlayer::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("GIFPlayer##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll;
		ImGui::InputInt(string("Layer##").append(id).c_str(), &_data.layer, 1, 100, flags);

		map<size_t, string> gifNames = GIFManager::GetAllNames();

		vector<pair<size_t, string>> pairs;
		pairs.resize(gifNames.size());

		std::transform(gifNames.begin(), gifNames.end(), pairs.begin(), [](pair<size_t, string> const& i) -> pair<size_t, string> {
			return i;
		});

		gifNames.clear();

		std::sort(pairs.begin(), pairs.end(), [](pair<size_t, string> const& left, pair<size_t, string> const& right) -> bool {
			return left.second.compare(right.second) < 0;
		});

		pairs.insert(pairs.begin(), { 0, "None" });

		vector<string> names;
		names.resize(pairs.size());

		std::transform(pairs.begin(), pairs.end(), names.begin(), [&](pair<size_t, string> const& i) -> string {
			return string(i.second).append("##").append(to_string(i.first)).append(id);
		});

		vector<size_t> ids;
		ids.resize(pairs.size());

		std::transform(pairs.begin(), pairs.end(), ids.begin(), [](pair<size_t, string> const& i) -> size_t {
			return i.first;
		});

		pairs.clear();

		size_t choosed = _gifId == 0 ? 0 : std::find(ids.begin(), ids.end(), _gifId) - ids.begin();

		if (ImGui::ComboWithFilter(string("GIF##").append(id).c_str(), &choosed, names, 20)) {
			SetGIF(ids[choosed]);
		}

		names.clear();
		ids.clear();

		if (ImGui::ArrowButton(string("Play GIF##").append(id).c_str(), ImGuiDir_Right)) {
			Play();
		}
		ImGui::SameLine();
		if (ImGui::PauseButton(string("Pause GIF##").append(id).c_str(), 1.f)) {
			Pause();
		}
		ImGui::SameLine();
		if (ImGui::StopButton(string("Stop GIF##").append(id).c_str(), 1.f)) {
			Stop();
		}

		float w = _data.rectTransform.size.x;
		bool setSize = ImGui::DragFloat(string("Width##").append(id).c_str(), &w, 0.1f, 0.f, FLT_MAX);

		float h = _data.rectTransform.size.y;
		setSize = ImGui::DragFloat(string("Height##").append(id).c_str(), &h, 0.1f, 0.f, FLT_MAX) || setSize;

		if (setSize) {
			SetSize({ w, h });
		}

		glm::vec4 c = _data.color;
		if (ImGui::ColorEdit4(string("Color##").append(id).c_str(), glm::value_ptr(c))) {
			SetColor(c);
		}

		bool b = _looped;
		if (ImGui::Checkbox(string("Loop##").append(id).c_str(), &b)) {
			Loop(b);
		}

		b = _autoPlay;
		if (ImGui::Checkbox(string("Auto Play##").append(id).c_str(), &b)) {
			AutoPlay(b);
		}

		/*
		bool b = _fillData.isActive;
		if (ImGui::Checkbox(string("Enable Fill##").append(id).c_str(), &b)) {
			EnableFill(b);
		}

		if (_fillData.isActive) {
			FILL_TYPE ft = (FILL_TYPE)_fillData.type;
			bool clicked = false;
			if (ImGui::BeginCombo(string("Fill Type##").append(id).c_str(), to_string(ft).c_str())) {
				for (size_t i = 0; i < Manager::size<FILL_TYPE>(); ++i) {
					FILL_TYPE ft_temp = (FILL_TYPE)i;
					if (ImGui::Selectable(to_string(ft_temp).append("##").append(id).c_str(), ft == ft_temp)) {
						if (clicked) {
							continue;
						}
						ft = ft_temp;
						clicked = true;
					}
				}
				ImGui::EndCombo();
			}

			if (clicked) {
				SetFillType(ft);
			}

			uint8_t sft = _fillData.subType;
			clicked = false;
			Tools::Func<string, uint8_t> toString = [](uint8_t value) -> string { return to_string(value); };
			switch (_fillData.type) {
			case (uint8_t)FILL_TYPE::HORIZONTAL:
				toString = [](uint8_t value) -> string { return to_string((HORIZONTAL_FILL_SUBTYPE)value); };
				break;
			case (uint8_t)FILL_TYPE::VERTICAL:
				toString = [](uint8_t value) -> string { return to_string((VERTICAL_FILL_SUBTYPE)value); };
				break;
			case (uint8_t)FILL_TYPE::CIRCLE:
				toString = [](uint8_t value) -> string { return to_string((CIRCLE_FILL_SUBTYPE)value); };
				break;
			}
			if (ImGui::BeginCombo(string("Fill Sub Type##").append(id).c_str(), toString(sft).c_str())) {
				size_t size = 0;
				switch (_fillData.type) {
				case (uint8_t)FILL_TYPE::HORIZONTAL:
					size = Manager::size<HORIZONTAL_FILL_SUBTYPE>();
					break;
				case (uint8_t)FILL_TYPE::VERTICAL:
					size = Manager::size<VERTICAL_FILL_SUBTYPE>();
					break;
				case (uint8_t)FILL_TYPE::CIRCLE:
					size = Manager::size<CIRCLE_FILL_SUBTYPE>();
					break;
				}

				for (size_t i = 0; i < size; ++i) {
					if (ImGui::Selectable(toString(i).append("##").append(id).c_str(), sft == i)) {
						if (clicked) {
							continue;
						}
						sft = i;
						clicked = true;
					}
				}
				ImGui::EndCombo();
			}

			if (clicked) {
				SetFillSubType(sft);
			}

			float v = _fillData.offset * 100.f;
			if (ImGui::SliderFloat(string("Fill Offset##").append(id).c_str(), &v, 0.f, 100.f)) {
				SetFillOffset(v);
			}

			v = _fillData.progress * 100.f;
			if (ImGui::SliderFloat(string("Fill Progress##").append(id).c_str(), &v, _fillData.offset * 100.f, 100.f)) {
				SetFillProgress(v);
			}
		}
		*/
	}
}
#endif