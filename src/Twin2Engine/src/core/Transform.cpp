#include <core/Transform.h>
#include <tools/YamlConverters.h>

#include <core/GameObject.h>

using namespace Twin2Engine::Core;

#pragma region DIRTY_FLAG

void Transform::SetDirtyFlagInChildren()
{
	//if (!_dirtyFlags.dirtyFlagInHierarchy)
	{
		const size_t size = _children.size();
		for (size_t index = 0ull; index < size; ++index)
		{
			_children[index]->CallPositionChanged();
			_children[index]->CallRotationChanged();
			_children[index]->CallScaleChanged();

			_children[index]->SetDirtyFlagInChildren();

			//_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;
			//_children[index]->_dirtyFlags.dirtyFlagGlobalPosition = true;
			//_children[index]->_dirtyFlags.dirtyFlagGlobalRotation = true;
			//_children[index]->_dirtyFlags.dirtyFlagGlobalScale = true;
		}
	}

	_dirtyFlags.dirtyFlagInHierarchy = true;
	_dirtyFlags.dirtyFlagGlobalPosition = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagGlobalScale = true;
}

void Transform::SetDirtyFlagGlobalPositionInChildren()
{
	//if (!_dirtyFlags.dirtyFlagGlobalPosition)
	{
		const size_t size = _children.size();
		for (size_t index = 0ull; index < size; ++index)
		{
			_children[index]->CallPositionChanged();

			_children[index]->SetDirtyFlagGlobalPositionInChildren();

			//_children[index]->_dirtyFlags.dirtyFlagGlobalPosition = true;
			_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;
		}
	}

	_dirtyFlags.dirtyFlagGlobalPosition = true;
	//_dirtyFlags.dirtyFlagInHierarchy = true;
}

void Transform::SetDirtyFlagGlobalRotationInChildren()
{
	//if (!_dirtyFlags.dirtyFlagGlobalRotation)
	{
		const size_t size = _children.size();
		for (size_t index = 0ull; index < size; ++index)
		{
			_children[index]->CallRotationChanged();

			_children[index]->SetDirtyFlagGlobalRotationInChildren();

			//_children[index]->_dirtyFlags.dirtyFlagGlobalRotation = true;
			_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;
		}
	}

	_dirtyFlags.dirtyFlagGlobalRotation = true;
	//_dirtyFlags.dirtyFlagInHierarchy = true;
}

void Transform::SetDirtyFlagGlobalScaleInChildren()
{
	//if (!_dirtyFlags.dirtyFlagGlobalScale)
	{
		const size_t size = _children.size();
		for (size_t index = 0ull; index < size; ++index)
		{
			_children[index]->CallScaleChanged();

			_children[index]->SetDirtyFlagGlobalScaleInChildren();

			//_children[index]->_dirtyFlags.dirtyFlagGlobalScale = true;
			_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;
		}
	}

	_dirtyFlags.dirtyFlagGlobalScale = true;
	//_dirtyFlags.dirtyFlagInHierarchy = true;
}

inline bool Transform::GetDirtyFlag() const
{
	return _dirtyFlags.dirtyFlag;
}

inline bool Transform::GetDirtyFlagInHierarchy() const
{
	return _dirtyFlags.dirtyFlagInHierarchy;
}

inline bool Transform::GetDirtyFlagGlobalPosition() const
{
	return _dirtyFlags.dirtyFlagGlobalPosition;
}

inline bool Transform::GetDirtyFlagLocalPosition() const
{
	return _dirtyFlags.dirtyFlagLocalPosition;
}

inline bool Transform::GetDirtyFlagGlobalRotation() const
{
	return _dirtyFlags.dirtyFlagGlobalRotation;
}

inline bool Transform::GetDirtyFlagLocalRotation() const
{
	return _dirtyFlags.dirtyFlagLocalRotation;
}

inline bool Transform::GetDirtyFlagGlobalScale() const
{
	return _dirtyFlags.dirtyFlagGlobalScale;
}

inline bool Transform::GetDirtyFlagLocalScale() const
{
	return _dirtyFlags.dirtyFlagLocalScale;
}

inline bool Transform::GetDirtyFlagGlobalRotationQuat2Euler() const
{
	return _dirtyFlags.dirtyFlagGlobalRotationQuat2Euler;
}

#pragma endregion

#pragma region EVENTS

inline void Transform::CallPositionChanged()
{
	_callingEvents.positionChanged = true;
	_callingEvents.transformChanged = true;
}

inline void Transform::CallRotationChanged()
{
	_callingEvents.rotationChanged = true;
	_callingEvents.transformChanged = true;
}

inline void Transform::CallScaleChanged()
{
	_callingEvents.scaleChanged = true;
	_callingEvents.transformChanged = true;
}

inline void Transform::CallParentChanged()
{
	_callingEvents.parentChanged = true;
}

void Transform::CallInHierarchyParentChanged()
{
	if (!_callingEvents.inHierarchyParentChanged)
	{
		_callingEvents.inHierarchyParentChanged = true;

		for (int index = 0; index < _children.size(); index++)
		{
			_children[index]->CallInHierarchyParentChanged();
		}
	}
}

inline void Transform::CallChildrenChanged()
{
	_callingEvents.childrenChanged = true;
}
#pragma endregion

Transform::Transform()
{
	_localPosition = glm::vec3(0.f, 0.f, 0.f);
	_localRotation = glm::vec3(0.f, 0.f, 0.f);
	_localScale = glm::vec3(1.f, 1.f, 1.f);

	_globalPosition = glm::vec3(0.f, 0.f, 0.f);
	_globalRotation = glm::vec3(0.f, 0.f, 0.f);
	_globalScale = glm::vec3(1.f, 1.f, 1.f);

	_localRotationQuat = glm::quat();
	_globalRotationQuat = glm::quat();

	_parent = nullptr;

	_localTransformMatrix = glm::mat4(1.0f);
	_globalTransformMatrix = glm::mat4(1.0f);

	// DirtyFlags
	_dirtyFlags.dirtyFlag = false;
	_dirtyFlags.dirtyFlagInHierarchy = false;

	_dirtyFlags.dirtyFlagGlobalPosition = false;
	_dirtyFlags.dirtyFlagLocalPosition = false;

	_dirtyFlags.dirtyFlagGlobalRotation = false;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	_dirtyFlags.dirtyFlagGlobalScale = false;
	_dirtyFlags.dirtyFlagLocalScale = false;

	// CallingEvents
	_callingEvents.transformChanged = false;
	_callingEvents.positionChanged = false;
	_callingEvents.rotationChanged = false;
	_callingEvents.scaleChanged = false;

	_callingEvents.parentChanged = false;
	_callingEvents.inHierarchyParentChanged = false;
	_callingEvents.childrenChanged = false;
}

#pragma region TRANSFORMATING_METHODS

void Transform::Translate(const glm::vec3& translation)
{
	RecalculateLocalPosition();

	_localPosition += translation;

	// Setting dirty flags
	SetDirtyFlagGlobalPositionInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalPosition = true;
	_dirtyFlags.dirtyFlagLocalPosition = false;

	// Calling Events
	CallPositionChanged();
}

void Transform::Rotate(const glm::vec3& rotation)
{
	RecalculateLocalRotation();

	_localRotation += glm::radians(rotation);
	_localRotationQuat = glm::quat(_localRotation);

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();
}

void Transform::Rotate(const glm::quat& rotation)
{
	RecalculateLocalRotation();

	_localRotationQuat *= rotation;
	_localRotation = glm::eulerAngles(_localRotationQuat);

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();
}

void Transform::Scale(const glm::vec3& scaling)
{
	RecalculateLocalScale();

	_localScale *= scaling;

	// Setting dirty flags
	SetDirtyFlagGlobalScaleInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalScale = true;
	_dirtyFlags.dirtyFlagLocalScale = false;

	// Calling Events
	CallScaleChanged();
}

#pragma endregion

#pragma region MANAGING_HIERARCHY

void Transform::AddChild(Transform* child)
{
	child->SetParent(this);
}

void Transform::RemoveChild(Transform* child)
{
	child->SetParent(nullptr);
}

void Transform::SetParent(Transform* parent)
{
	if (_parent != parent)
	{
		if (_parent != nullptr) {
			_parent->JustRemoveChild(this);
		}

		if (parent != nullptr) {
			parent->JustAddChild(this);
		}
		_parent = parent;

		SetDirtyFlagInChildren();

		//_dirtyFlags.dirtyFlag = true;
		//_dirtyFlags.dirtyFlagGlobalPosition = true;
		//_dirtyFlags.dirtyFlagGlobalRotation = true;
		//_dirtyFlags.dirtyFlagGlobalScale = true;

		//_dirtyFlags.dirtyFlagInHierarchy = true;

		CallParentChanged();
		CallInHierarchyParentChanged();
	}
}

inline void Transform::JustAddChild(Transform* child)
{
	_children.push_back(child);

	// Calling events
	CallChildrenChanged();
}

inline void Transform::JustRemoveChild(Transform* child)
{
	if (_children.size() == 0) return;

	auto t = std::find(_children.begin(), _children.end(), child);

	if (t != _children.end())
		_children.erase(t);

	// Calling events
	CallChildrenChanged();
}


Transform* Transform::GetParent() const
{
	return _parent;
}


Transform* Transform::GetChildAt(size_t index) const
{
	if (index >= _children.size())
	{
		return nullptr;
	}

	return _children[index];
}

size_t Transform::GetChildCount() const
{
	return _children.size();
}

#pragma endregion

#pragma region POSITIONS

void Transform::SetLocalPosition(const glm::vec3& localPosition)
{
	_localPosition = localPosition;

	// Setting dirty flags
	SetDirtyFlagGlobalPositionInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalPosition = true;
	_dirtyFlags.dirtyFlagLocalPosition = false;

	// Calling Events
	CallPositionChanged();
}


glm::vec3 Transform::GetLocalPosition()
{
	RecalculateLocalPosition();

	return _localPosition;
}

void Transform::RecalculateLocalPosition()
{
	if (_dirtyFlags.dirtyFlagLocalPosition)
	{
		if (_parent == nullptr)
		{
			_localPosition = _globalPosition;
		}
		else
		{
			_localPosition = glm::vec3(glm::inverse(_parent->GetTransformMatrix()) * glm::vec4(_globalPosition, 1.0f));
		}

		_dirtyFlags.dirtyFlagLocalPosition = false;
	}
}

void Transform::SetGlobalPosition(const glm::vec3& globalPosition)
{
	_globalPosition = globalPosition;

	// Setting dirty flags
	SetDirtyFlagGlobalPositionInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagLocalPosition = true;
	_dirtyFlags.dirtyFlagGlobalPosition = false;

	RecalculateLocalPosition();
	
	// Calling Events
	CallPositionChanged();
}

glm::vec3 Transform::GetGlobalPosition()
{
	RecalculateGlobalPosition();

	return _globalPosition;
}

void Transform::RecalculateGlobalPosition()
{
	if (_dirtyFlags.dirtyFlagGlobalPosition)
	{
		RecalculateTransformMatrix();

		_globalPosition = glm::vec3(_globalTransformMatrix[3]);

		_dirtyFlags.dirtyFlagGlobalPosition = false;
	}
}

#pragma endregion

#pragma region ROTATIONS

// VEC3 IN EULER ANGLES
void Transform::SetLocalRotation(const glm::vec3& localRotation)
{
	_localRotation = glm::radians(localRotation);
	_localRotationQuat = glm::quat(_localRotation);

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();
}

// IN EULAR ANGLES
glm::vec3 Transform::GetLocalRotation()
{
	RecalculateLocalRotation();

	return glm::degrees(_localRotation);
}

void Transform::SetLocalRotation(const glm::quat& localRotation)
{
	_localRotationQuat = localRotation;
	_localRotation = glm::eulerAngles(localRotation);

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();
}
glm::quat Transform::GetLocalRotationQuat()
{
	RecalculateLocalRotation();

	return _localRotationQuat;
}

void Transform::RecalculateLocalRotation()
{
	if (_dirtyFlags.dirtyFlagLocalRotation)
	{
		if (_parent == nullptr)
		{
			_localRotationQuat = _globalRotationQuat;
			_localRotation = _globalRotation;
		}
		else
		{
			if (_parent->GetGlobalRotationQuat().w != 0)
			{
				_localRotationQuat = glm::inverse(_parent->GetGlobalRotationQuat()) * _globalRotationQuat;
			}
			else
			{
				_localRotationQuat = _globalRotationQuat;
			}
			_localRotation = glm::eulerAngles(_localRotationQuat);
		}

		_dirtyFlags.dirtyFlagLocalRotation = false;
	}
}

// VEC3 IN EULAR ANGLES
void Transform::SetGlobalRotation(const glm::vec3& globalRotation)
{
	_globalRotation = glm::radians(globalRotation);
	_globalRotationQuat = glm::quat(_globalRotation);

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagLocalRotation = true;
	_dirtyFlags.dirtyFlagGlobalRotation = false;

	RecalculateLocalRotation();

	// Calling Events
	CallRotationChanged();
}

// IN EULAR ANGLES
glm::vec3 Transform::GetGlobalRotation()
{
	RecalculateGlobalRotation();

	return glm::degrees(_globalRotation);
}

glm::quat Transform::GetGlobalRotationQuat()
{
	RecalculateGlobalRotation();

	return _globalRotationQuat;
}

void Transform::RecalculateGlobalRotation()
{
	if (_dirtyFlags.dirtyFlagGlobalRotation)
	{
		if (_parent == nullptr)
		{
			_globalRotationQuat = _localRotationQuat;
			_globalRotation = _localRotation;
		}
		else
		{
			if (_parent->GetGlobalRotationQuat().w != 0)
			{
				_globalRotationQuat = _parent->GetGlobalRotationQuat() * _localRotationQuat;
			}
			else
			{
				_globalRotationQuat = _localRotationQuat;
			}

			_globalRotation = glm::eulerAngles(_globalRotationQuat);
		}

		_dirtyFlags.dirtyFlagGlobalRotation = false;
	}
}

#pragma endregion

#pragma region SCALES

void Transform::SetLocalScale(const glm::vec3& localScale)
{
	_localScale = localScale;

	// Setting dirty flags
	SetDirtyFlagGlobalScaleInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalScale = true;
	_dirtyFlags.dirtyFlagLocalScale = false;

	// Calling Events
	CallScaleChanged();
}

glm::vec3 Transform::GetLocalScale()
{
	RecalculateLocalScale();

	return _localScale;
}

void Transform::RecalculateLocalScale()
{
	if (_dirtyFlags.dirtyFlagLocalScale)
	{
		if (_parent == nullptr)
		{
			_localScale = _globalScale;
		}
		else
		{
			_localScale = _globalScale / _parent->GetGlobalScale();
		}

		_dirtyFlags.dirtyFlagLocalScale = false;
	}
}

void Transform::SetGlobalScale(const glm::vec3& globalScale)
{
	_globalScale = globalScale;

	// Setting dirty flags
	SetDirtyFlagGlobalScaleInChildren();

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagLocalScale = true;
	_dirtyFlags.dirtyFlagGlobalScale = false;

	RecalculateLocalScale();

	// Calling Events
	CallScaleChanged();
}

glm::vec3 Transform::GetGlobalScale()
{
	RecalculateGlobalScale();

	return _globalScale;
}

void Transform::RecalculateGlobalScale()
{
	if (_dirtyFlags.dirtyFlagGlobalScale)
	{
		if (_parent == nullptr)
		{
			_globalScale = _localScale;
		}
		else
		{
			_globalScale = _parent->GetGlobalScale() * _localScale;
		}

		RecalculateLocalScale();

		_dirtyFlags.dirtyFlagGlobalScale = false;
	}
}

#pragma endregion

glm::mat4 Transform::GetTransformMatrix()
{
	RecalculateTransformMatrix();

	return _globalTransformMatrix;
}

void Transform::RecalculateTransformMatrix()
{
	if (_dirtyFlags.dirtyFlag)
	{
		_localTransformMatrix = glm::mat4(1.0f);

		_localTransformMatrix = glm::translate(_localTransformMatrix, GetLocalPosition());

		RecalculateLocalRotation();

		_localTransformMatrix = glm::rotate(_localTransformMatrix, _localRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		_localTransformMatrix = glm::rotate(_localTransformMatrix, _localRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		_localTransformMatrix = glm::rotate(_localTransformMatrix, _localRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		_localTransformMatrix = glm::scale(_localTransformMatrix, GetLocalScale());
	}

	if (_dirtyFlags.dirtyFlagInHierarchy)
	{
		if (_parent != nullptr)
		{
			_parent->RecalculateTransformMatrix();
		}
	}

	if (_dirtyFlags.dirtyFlag || _dirtyFlags.dirtyFlagInHierarchy)
	{
		if (_parent != nullptr)
		{
			_globalTransformMatrix = _parent->GetTransformMatrix() * _localTransformMatrix;
		}
		else
		{
			_globalTransformMatrix = _localTransformMatrix;
		}

		_dirtyFlags.dirtyFlag = false;
		_dirtyFlags.dirtyFlagInHierarchy = false;
	}
}



void Transform::Update()
{
	if (_callingEvents.positionChanged)
	{
		_callingEvents.positionChanged = false;
		OnEventPositionChanged.Invoke(this);
	}
	if (_callingEvents.rotationChanged)
	{
		_callingEvents.rotationChanged = false;
		OnEventRotationChanged.Invoke(this);
	}
	if (_callingEvents.scaleChanged)
	{
		_callingEvents.scaleChanged = false;
		OnEventScaleChanged.Invoke(this);
	}
	if (_callingEvents.transformChanged)
	{
		_callingEvents.transformChanged = false;
		OnEventTransformChanged.Invoke(this);
	}
	if (_callingEvents.parentChanged)
	{
		_callingEvents.parentChanged = false;
		OnEventParentChanged.Invoke(this);
	}
	if (_callingEvents.inHierarchyParentChanged)
	{
		_callingEvents.inHierarchyParentChanged = false;
		OnEventInHierarchyParentChanged.Invoke(this);
	}
	if (_callingEvents.childrenChanged)
	{
		_callingEvents.childrenChanged = false;
		OnEventChildrenChanged.Invoke(this);
	}
}

YAML::Node Transform::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node.remove("type");
	node.remove("enabled");
	node["position"] = _localPosition;
	node["scale"] = _localScale;
	node["rotation"] = glm::degrees(_localRotation);
	return node;
}

bool Transform::Deserialize(const YAML::Node& node) {
	if (!node["position"] || !node["scale"] || !node["rotation"]) return false;

	YAML::Node tempNode = node;
	tempNode["enabled"] = true;
	if (!Component::Deserialize(tempNode)) return false;

	SetLocalPosition(node["position"].as<glm::vec3>());
	SetLocalScale(node["scale"].as<glm::vec3>());
	SetLocalRotation(node["rotation"].as<glm::vec3>());

	return true;
}

#if _DEBUG
void Transform::DrawEditor()
{
	std::string id = std::string(std::to_string(this->GetId()));
	std::string name = std::string("Transform##Component").append(id);
	static bool world = false;

	if (ImGui::CollapsingHeader(name.c_str())) {

		if (_parent->GetGameObject()->Id())
		{
			ImGui::Text("Parent: %d", _parent->GetGameObject()->Id());
		}

		if (ImGui::RadioButton(std::string("Local##").append(id).c_str(), world == false))
			world = false;
		ImGui::SameLine();
		if (ImGui::RadioButton(std::string("World##").append(id).c_str(), world == true))
			world = true;

		//glm::vec3 pos = world ? _globalPosition : _localPosition;
		//glm::vec3 rot = world ? glm::degrees(_globalRotation) : glm::degrees(_localRotation);
		//glm::vec3 sc = world ? _globalScale : _localScale;

		glm::vec3 pos = world ? GetGlobalPosition() : GetLocalPosition();
		glm::vec3 rot = world ? GetGlobalRotation() : GetLocalRotation();
		glm::vec3 sc = world ? GetGlobalScale() : GetLocalScale();

		//ImGui::DragFloat3(std::string("Position##").append(id).c_str(), glm::value_ptr(pos));
		//ImGui::DragFloat3(std::string("Rotation##").append(id).c_str(), glm::value_ptr(rot));
		//ImGui::DragFloat3(std::string("Scale##").append(id).c_str(), glm::value_ptr(sc));

		//if ((world ? _globalPosition : _localPosition) != pos) {
		if (ImGui::DragFloat3(std::string("Position##").append(id).c_str(), glm::value_ptr(pos))) {
			if (world) {
				SetGlobalPosition(pos);
			}
			else {
				SetLocalPosition(pos);
			}
		}

		//if ((world ? glm::degrees(_globalRotation) : glm::degrees(_localRotation)) != rot) {
		if (ImGui::DragFloat3(std::string("Rotation##").append(id).c_str(), glm::value_ptr(rot))) {
			if (world) {
				SetGlobalRotation(rot);
			}
			else {
				SetLocalRotation(rot);
			}
		}

		//if ((world ? _globalScale : _localScale) != sc) {
		if (ImGui::DragFloat3(std::string("Scale##").append(id).c_str(), glm::value_ptr(sc))) {
			if (world) {
				SetGlobalScale(sc);
			}
			else {
				SetLocalScale(sc);
			}
		}
	}
}
#endif