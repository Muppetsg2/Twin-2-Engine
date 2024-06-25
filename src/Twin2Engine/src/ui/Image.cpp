#include <ui/Image.h>
#include <core/Transform.h>
#include <tools/YamlConverters.h>
#include <manager/SceneManager.h>
#include <graphic/manager/SpriteManager.h>
#include <core/CameraComponent.h>
#include <ui/Canvas.h>
#include <ui/Mask.h>

using namespace Twin2Engine;
using namespace UI;
using namespace Core;
using namespace Graphic;
using namespace Manager;
using namespace Tools;
using namespace glm;
using namespace std;

void Image::SetCanvas(Canvas* canvas)
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

void Image::SetMask(Mask* mask)
{
	if (_mask != mask) {
		if (_mask != nullptr) {
			_mask->GetOnMaskDestroy() -= _onMaskDestroyId;
			_data.mask = nullptr;
		}

		_mask = mask;
		if (_mask != nullptr) {
			_onMaskDestroyId = (_mask->GetOnMaskDestroy() += [&](Mask* mask) -> void { SetMask(nullptr); });
			_data.mask = &_mask->_data;
		}
	}
}

void Image::Initialize()
{
	Transform* tr = GetTransform();
	_data.rectTransform.transform = tr->GetTransformMatrix();
	_onTransformChangeId = (tr->OnEventTransformChanged += [&](Transform* t) -> void { 
		_data.rectTransform.transform = t->GetTransformMatrix(); 
	});
	_onRotationChangeId = (tr->OnEventRotationChanged += [&](Transform* t) -> void {
		_data.fill.rotation = t->GetGlobalRotation().z;
	});
	_onParentInHierarchiChangeId = (tr->OnEventInHierarchyParentChanged += [&](Transform* t) -> void {
		GameObject* obj = GetGameObject();
		SetCanvas(obj->GetComponentInParents<Canvas>());
		SetMask(obj->GetComponentInParents<Mask>());
		_data.rectTransform.transform = t->GetTransformMatrix();
	});
	GameObject* obj = GetGameObject();
	SetCanvas(obj->GetComponentInParents<Canvas>());
	SetMask(obj->GetComponentInParents<Mask>());
}

void Image::Render()
{
	_data.sprite = SpriteManager::GetSprite(_spriteId);
	UIRenderingManager::Render(_data);
}

void Image::OnDestroy()
{
	SetCanvas(nullptr);
	SetMask(nullptr);
	Transform* tr = GetTransform();
	tr->OnEventTransformChanged -= _onTransformChangeId;
	tr->OnEventRotationChanged -= _onRotationChangeId;
	tr->OnEventInHierarchyParentChanged -= _onParentInHierarchiChangeId;
}

YAML::Node Image::Serialize() const
{
	YAML::Node node = RenderableComponent::Serialize();
	node["type"] = "Image";

	if (SpriteManager::GetSprite(_spriteId) != nullptr) {
		node["sprite"] = SceneManager::GetSpriteSaveIdx(_spriteId);
	}
	node["color"] = _data.color;
	node["width"] = _data.rectTransform.size.x;
	node["height"] = _data.rectTransform.size.y;
	node["layer"] = _data.layer;

	// FILL
	node["fillEnabled"] = _data.fill.isActive;
	node["fillType"] = (uint32_t)_data.fill.type;
	node["fillType"].SetTag(to_string((FILL_TYPE)_data.fill.type));
	Tools::Func<string, uint8_t> toString = [](uint8_t value) -> string { return to_string(value); };
	switch (_data.fill.type) {
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
	node["fillSubType"] = (uint32_t)_data.fill.subType;
	node["fillSubType"].SetTag(toString(_data.fill.subType));
	node["fillOffset"] = _data.fill.offset;
	node["fillProgress"] = _data.fill.progress;

	return node;
}

bool Image::Deserialize(const YAML::Node& node)
{
	if (!node["color"] || !node["width"] || !node["height"] ||
		!node["layer"] || !node["fillEnabled"] || !node["fillType"] || !node["fillSubType"] ||
		!node["fillOffset"] || !node["fillProgress"] || !RenderableComponent::Deserialize(node)) return false;

	if (node["sprite"]) {
		_spriteId = SceneManager::GetSprite(node["sprite"].as<size_t>());
	}
	else {
		_spriteId = 0;
	}
	_data.color = node["color"].as<glm::vec4>();
	_data.rectTransform.size.x = node["width"].as<float>();
	_data.rectTransform.size.y = node["height"].as<float>();
	_data.layer = node["layer"].as<int32_t>();

	_data.fill.isActive = node["fillEnabled"].as<bool>();
	_data.fill.type = node["fillType"].as<uint32_t>();
	_data.fill.subType = node["fillSubType"].as<uint32_t>();
	_data.fill.offset = node["fillOffset"].as<float>();
	_data.fill.progress = node["fillProgress"].as<float>();

	return true;
}

#if _DEBUG
void Image::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Image##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll;
		ImGui::InputInt(string("Layer##").append(id).c_str(), &_data.layer, 1, 100, flags);

		std::map<size_t, string> spriteNames = SpriteManager::GetAllSpritesNames();

		vector<string> names = vector<string>();
		names.resize(spriteNames.size());

		std::transform(spriteNames.begin(), spriteNames.end(), names.begin(), [](std::pair<size_t, string> const& i) -> string {
			return i.second;
		});

		spriteNames.clear();

		std::sort(names.begin(), names.end(), [&](string const& left, string const& right) -> bool {
			return left.compare(right) < 0;
		});

		names.insert(names.begin(), "None##Nothing");

		std::string n = SpriteManager::GetSpriteName(_spriteId);

		if (n == "") {
			_spriteId = 0;
		}

		size_t choosed = _spriteId == 0 ? 0 : std::find(names.begin(), names.end(), n) - names.begin();

		if (ImGui::ComboWithFilter(string("Sprite##").append(id).c_str(), &choosed, names, 20)) {
			if (choosed != 0) {
				SetSprite(names[choosed]);
			}
			else {
				SetSprite(0);
			}
		}

		names.clear();

		float v = _data.rectTransform.size.x;
		if (ImGui::DragFloat(string("Width##").append(id).c_str(), &v, 0.1f, 0.f, FLT_MAX)) {
			SetWidth(v);
		}

		v = _data.rectTransform.size.y;
		if (ImGui::DragFloat(string("Height##").append(id).c_str(), &v, 0.1f, 0.f, FLT_MAX)) {
			SetHeight(v);
		}

		glm::vec4 c = _data.color;
		if (ImGui::ColorEdit4(string("Color##").append(id).c_str(), glm::value_ptr(c))) {
			SetColor(c);
		}

		bool b = _data.fill.isActive;
		if (ImGui::Checkbox(string("Enable Fill##").append(id).c_str(), &b)) {
			EnableFill(b);
		}

		if (_data.fill.isActive) {
			FILL_TYPE ft = (FILL_TYPE)_data.fill.type;
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

			uint8_t sft = _data.fill.subType;
			clicked = false;
			Tools::Func<string, uint8_t> toString = [](uint8_t value) -> string { return to_string(value); };
			switch (_data.fill.type) {
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
				switch (_data.fill.type) {
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

			v = _data.fill.offset * 100.f;
			if (ImGui::SliderFloat(string("Fill Offset##").append(id).c_str(), &v, 0.f, 100.f)) {
				SetFillOffset(v);
			}

			v = _data.fill.progress * 100.f;
			if (ImGui::SliderFloat(string("Fill Progress##").append(id).c_str(), &v, _data.fill.offset * 100.f, 100.f)) {
				SetFillProgress(v);
			}
		}
	}
}
#endif

void Image::SetSprite(const std::string& spriteAlias) {
	_spriteId = hash<string>()(spriteAlias);
}

void Image::SetSprite(size_t spriteId)
{
	_spriteId = spriteId;
}

void Image::SetColor(const vec4& color)
{
	if (_data.color != color) {
		if (color.a != 1.f)
			_isTransparent = true;
		_data.color = color;
	}
}

void Image::SetWidth(float width)
{
	if (_data.rectTransform.size.x != width) {
		_data.rectTransform.size.x = width;
	}
}

void Image::SetHeight(float height)
{
	if (_data.rectTransform.size.y != height) {
		_data.rectTransform.size.y = height;
	}
}

void Image::SetLayer(int32_t layer)
{
	if (_data.layer != layer) {
		_data.layer = layer;
	}
}

void Image::EnableFill(bool enable)
{
	if (_data.fill.isActive != enable) {
		_data.fill.isActive = enable;
	}
}

void Image::SetFillType(FILL_TYPE type)
{
	if (_data.fill.type != (uint8_t)type) {
		_data.fill.type = (uint8_t)type;
	}
}

void Image::SetFillSubType(uint8_t subType)
{
	size_t size = 0;
	switch (_data.fill.type) {
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
	if (subType > size - 1) subType = size - 1;

	if (_data.fill.subType != subType) {
		_data.fill.subType = subType;
	}
}

void Image::SetFillOffset(float offset)
{
	if (offset > 100.f) offset = 100.f;
	else if (offset < 0.f) offset = 0.f;

	if (_data.fill.offset != offset * 0.01f) {
		_data.fill.offset = offset * 0.01f;

		if (_data.fill.progress < _data.fill.offset) {
			SetFillProgress(_data.fill.offset);
		}
	}
}

void Image::SetFillProgress(float progress)
{
	if (progress > 100.f) progress = 100.f;
	else if (progress < _data.fill.offset * 100.f) progress = _data.fill.offset * 100.f;

	if (_data.fill.progress != progress * 0.01f) {
		_data.fill.progress = progress * 0.01f;
	}
}

Sprite* Image::GetSprite() const
{
	return SpriteManager::GetSprite(_spriteId);
}

vec4 Image::GetColor() const
{
	return _data.color;
}

float Image::GetWidth() const
{
	return _data.rectTransform.size.x;
}

float Image::GetHeight() const
{
	return _data.rectTransform.size.y;
}

int32_t Image::GetLayer() const
{
	return _data.layer;
}

bool Image::IsFillEnable() const
{
	return _data.fill.isActive;
}

FILL_TYPE Image::GetFillType() const
{
	return (FILL_TYPE)_data.fill.type;
}

uint8_t Image::GetFillSubType() const
{
	return _data.fill.subType;
}

float Image::GetFillOffset() const
{
	return _data.fill.offset * 100.f;
}

float Image::GetFillProgress() const
{
	return _data.fill.progress * 100.f;
}