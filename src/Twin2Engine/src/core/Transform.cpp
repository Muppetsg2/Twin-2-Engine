#include <core/Transform.h>
#include <tools/YamlConverters.h>

//Nie moze byc nullptr gdyz niektore warunki w set parent moga sie posypac
//Twin2Engine::Core::Transform* Twin2Engine::Core::Transform::originTransform = new Transform();

#pragma region DIRTY_FLAG

void Twin2Engine::Core::Transform::SetDirtyFlagInChildren()
{
	if (!_dirtyFlags.dirtyFlagInHierarchy)
	{
		for (int index = 0; index < _children.size(); index++)
		{
			_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;
			_children[index]->_dirtyFlags.dirtyFlagGlobalPosition = true;
			_children[index]->_dirtyFlags.dirtyFlagGlobalRotation = true;
			_children[index]->_dirtyFlags.dirtyFlagGlobalScale = true;

			_children[index]->SetDirtyFlagInChildren();
		}
	}
}

void Twin2Engine::Core::Transform::SetDirtyFlagGlobalPositionInChildren()
{
	if (!_dirtyFlags.dirtyFlagGlobalPosition)
	{
		for (int index = 0; index < _children.size(); index++)
		{
			_children[index]->_dirtyFlags.dirtyFlagGlobalPosition = true;
			_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;

			_children[index]->CallPositionChanged();

			_children[index]->SetDirtyFlagGlobalPositionInChildren();
		}
	}
}

void Twin2Engine::Core::Transform::SetDirtyFlagGlobalRotationInChildren()
{
	if (!_dirtyFlags.dirtyFlagGlobalRotation)
	{

		for (int index = 0; index < _children.size(); index++)
		{
			_children[index]->_dirtyFlags.dirtyFlagGlobalRotation = true;
			_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;

			_children[index]->CallRotationChanged();

			_children[index]->SetDirtyFlagGlobalRotationInChildren();
		}
	}
}

void Twin2Engine::Core::Transform::SetDirtyFlagGlobalScaleInChildren()
{
	if (!_dirtyFlags.dirtyFlagGlobalScale)
	{
		for (int index = 0; index < _children.size(); index++)
		{
			_children[index]->_dirtyFlags.dirtyFlagGlobalScale = true;
			_children[index]->_dirtyFlags.dirtyFlagInHierarchy = true;

			_children[index]->CallScaleChanged();

			_children[index]->SetDirtyFlagGlobalScaleInChildren();
		}
	}
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlag() const
{
	return _dirtyFlags.dirtyFlag;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagInHierarchy() const
{
	return _dirtyFlags.dirtyFlagInHierarchy;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagGlobalPosition() const
{
	return _dirtyFlags.dirtyFlagGlobalPosition;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagLocalPosition() const
{
	return _dirtyFlags.dirtyFlagLocalPosition;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagGlobalRotation() const
{
	return _dirtyFlags.dirtyFlagGlobalRotation;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagLocalRotation() const
{
	return _dirtyFlags.dirtyFlagLocalRotation;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagGlobalScale() const
{
	return _dirtyFlags.dirtyFlagGlobalScale;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagLocalScale() const
{
	return _dirtyFlags.dirtyFlagLocalScale;
}

inline bool Twin2Engine::Core::Transform::GetDirtyFlagGlobalRotationQuat2Euler() const
{
	return _dirtyFlags.dirtyFlagGlobalRotationQuat2Euler;
}

#pragma endregion

#pragma region EVENTS

inline void Twin2Engine::Core::Transform::CallPositionChanged()
{
	_callingEvents.positionChanged = true;
	_callingEvents.transformChanged = true;

	//OnEventPositionChanged.Invoke(this);
	//OnEventTransformChanged.Invoke(this);
	//
	//for (int index = 0; index < _children.size(); index++)
	//{
	//	_children[index]->CallPositionChanged();
	//}
}

inline void Twin2Engine::Core::Transform::CallRotationChanged()
{
	_callingEvents.rotationChanged = true;
	_callingEvents.transformChanged = true;

	//OnEventRotationChanged.Invoke(this);
	//OnEventTransformChanged.Invoke(this);
	//
	//for (int index = 0; index < _children.size(); index++)
	//{
	//	_children[index]->CallRotationChanged();
	//}
}

inline void Twin2Engine::Core::Transform::CallScaleChanged()
{
	_callingEvents.scaleChanged = true;
	_callingEvents.transformChanged = true;

	//OnEventScaleChanged.Invoke(this);
	//OnEventTransformChanged.Invoke(this);
	//
	//for (int index = 0; index < _children.size(); index++)
	//{
	//	_children[index]->CallScaleChanged();
	//}
}

inline void Twin2Engine::Core::Transform::CallParentChanged()
{
	_callingEvents.parentChanged = true;

	//OnEventParentChanged.Invoke(this);
}

void Twin2Engine::Core::Transform::CallInHierarchyParentChanged()
{
	if (!_callingEvents.inHierarchyParentChanged)
	{
		_callingEvents.inHierarchyParentChanged = true;

		for (int index = 0; index < _children.size(); index++)
		{
			_children[index]->CallInHierarchyParentChanged();
		}
	}
	//OnEventInHierarchyParentChanged.Invoke(this);
	//
	//for (int index = 0; index < _children.size(); index++)
	//{
	//	_children[index]->CallInHierarchyParentChanged();
	//}
}

inline void Twin2Engine::Core::Transform::CallChildrenChanged()
{
	_callingEvents.childrenChanged = true;

	//OnEventChildrenChanged.Invoke(this);
}
#pragma endregion

Twin2Engine::Core::Transform::Transform()
{
	_localPosition = glm::vec3(0.f, 0.f, 0.f);
	_localRotation = glm::vec3(0.f, 0.f, 0.f);
	_localScale = glm::vec3(1.f, 1.f, 1.f);

	_globalPosition = glm::vec3(0.f, 0.f, 0.f);
	_globalRotation = glm::vec3(0.f, 0.f, 0.f);
	_globalScale = glm::vec3(1.f, 1.f, 1.f);

	_localRotationQuat = glm::quat();
	_globalRotationQuat = glm::quat();

	//_parent = originTransform;
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

void Twin2Engine::Core::Transform::Translate(const glm::vec3& translation)
{
	RecalculateLocalPosition();

	_localPosition += translation;

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalPosition = true;
	_dirtyFlags.dirtyFlagLocalPosition = false;

	// Calling Events
	CallPositionChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalPositionInChildren();
}

void Twin2Engine::Core::Transform::Rotate(const glm::vec3& rotation)
{
	RecalculateLocalRotation();

	_localRotation += glm::radians(rotation);
	_localRotationQuat = glm::quat(_localRotation);

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();
}

void Twin2Engine::Core::Transform::Rotate(const glm::quat& rotation)
{
	RecalculateLocalRotation();

	_localRotationQuat *= rotation;
	_localRotation = glm::eulerAngles(_localRotationQuat);

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();
}

void Twin2Engine::Core::Transform::Scale(const glm::vec3& scaling)
{
	RecalculateLocalScale();

	_localScale *= scaling;

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalScale = true;
	_dirtyFlags.dirtyFlagLocalScale = false;

	//RecalculateGlobalScale();
	// 
	//_globalScale *= scaling;
	//
	//_dirtyFlags.dirtyFlag = true;
	//_dirtyFlags.dirtyFlagLocalScale = true;
	//_dirtyFlags.dirtyFlagGlobalScale = false;

	// Calling Events
	CallScaleChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalScaleInChildren();
}

#pragma endregion

#pragma region MANAGING_HIERARCHY

void Twin2Engine::Core::Transform::AddChild(Transform* child)
{
	child->SetParent(this);
}

void Twin2Engine::Core::Transform::RemoveChild(Transform* child)
{
	//child->SetParent(originTransform);
	child->SetParent(nullptr);
}

void Twin2Engine::Core::Transform::SetParent(Transform* parent)
{
	//Nie usuwac jest to zapasowe w wypadtku zmiany originTransform na nullptr, ale bedzie nal�a� to wtedy jeszcze przerobie
	//if (parent == originTransform)
	//{
	//	_parent->JustRemoveChild(this);
	//	parent->JustAddChild(this);
	//	_parent = originTransform;
	//}
	//else
	//{
	//	_parent->JustRemoveChild(this);
	//	parent->JustAddChild(this);
	//	_parent = parent;
	//}

	/*
	if (parent == nullptr)
	{
		parent = originTransform;
	}
	*/

	if (_parent != parent)
	{
		if (_parent != nullptr) {
			_parent->JustRemoveChild(this);
		}

		if (parent != nullptr) {
			parent->JustAddChild(this);
		}
		_parent = parent;

		//_dirtyFlags.dirtyFlag = true;
		_dirtyFlags.dirtyFlagInHierarchy = true;

		CallParentChanged();
		CallInHierarchyParentChanged();

		SetDirtyFlagInChildren();
	}
}

inline void Twin2Engine::Core::Transform::JustAddChild(Transform* child)
{
	_children.push_back(child);

	// Calling events
	CallChildrenChanged();
}

inline void Twin2Engine::Core::Transform::JustRemoveChild(Transform* child)
{
	if (_children.size() == 0) return;
	_children.erase(std::find(_children.begin(), _children.end(), child));

	// Calling events
	CallChildrenChanged();
}


Twin2Engine::Core::Transform* Twin2Engine::Core::Transform::GetParent() const
{
	return _parent;
}


Twin2Engine::Core::Transform* Twin2Engine::Core::Transform::GetChildAt(size_t index) const
{
	if (index >= _children.size())
	{
		return nullptr;
	}

	return _children[index];
}

size_t Twin2Engine::Core::Transform::GetChildCount() const
{
	return _children.size();
}

#pragma endregion

#pragma region POSITIONS

void Twin2Engine::Core::Transform::SetLocalPosition(const glm::vec3& localPosition)
{
	_localPosition = localPosition;

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalPosition = true;
	_dirtyFlags.dirtyFlagLocalPosition = false;

	// Calling Events
	CallPositionChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalPositionInChildren();
}


glm::vec3 Twin2Engine::Core::Transform::GetLocalPosition()
{
	RecalculateLocalPosition();

	return _localPosition;
}

void Twin2Engine::Core::Transform::RecalculateLocalPosition()
{
	if (_dirtyFlags.dirtyFlagLocalPosition)
	{
		/*
		if (_parent == originTransform || _parent == nullptr)
		*/
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

void Twin2Engine::Core::Transform::SetGlobalPosition(const glm::vec3& globalPosition)
{
	_globalPosition = globalPosition;

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagLocalPosition = true;
	_dirtyFlags.dirtyFlagGlobalPosition = false;

	// Calling Events
	CallPositionChanged();

	RecalculateLocalPosition();

	// Setting dirty flags
	SetDirtyFlagGlobalPositionInChildren();
}

glm::vec3 Twin2Engine::Core::Transform::GetGlobalPosition()
{
	RecalculateGlobalPosition();

	return _globalPosition;
}

void Twin2Engine::Core::Transform::RecalculateGlobalPosition()
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
void Twin2Engine::Core::Transform::SetLocalRotation(const glm::vec3& localRotation)
{
	_localRotation = glm::radians(localRotation);
	_localRotationQuat = glm::quat(_localRotation);

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();
}

// IN EULAR ANGLES
glm::vec3 Twin2Engine::Core::Transform::GetLocalRotation()
{
	RecalculateLocalRotation();

	return glm::degrees(_localRotation);
}

void Twin2Engine::Core::Transform::SetLocalRotation(const glm::quat& localRotation)
{
	_localRotationQuat = localRotation;
	_localRotation = glm::eulerAngles(localRotation);

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalRotation = true;
	_dirtyFlags.dirtyFlagLocalRotation = false;

	// Calling Events
	CallRotationChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();
}
glm::quat Twin2Engine::Core::Transform::GetLocalRotationQuat()
{
	RecalculateLocalRotation();

	return _localRotationQuat;
}

void Twin2Engine::Core::Transform::RecalculateLocalRotation()
{
	if (_dirtyFlags.dirtyFlagLocalRotation)
	{
		/*
		if (_parent == originTransform || _parent == nullptr)
		*/
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
void Twin2Engine::Core::Transform::SetGlobalRotation(const glm::vec3& globalRotation)
{
	_globalRotation = glm::radians(globalRotation);
	_globalRotationQuat = glm::quat(_globalRotation);


	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagLocalRotation = true;
	_dirtyFlags.dirtyFlagGlobalRotation = false;

	// Calling Events
	CallRotationChanged();

	RecalculateLocalRotation();

	// Setting dirty flags
	SetDirtyFlagGlobalRotationInChildren();
}

// IN EULAR ANGLES
glm::vec3 Twin2Engine::Core::Transform::GetGlobalRotation()
{
	RecalculateGlobalRotation();

	//if (_dirtyFlags.dirtyFlagGlobalRotationQuat2Euler)
	//{
	//	_globalRotation = glm::eulerAngles(_globalRotationQuat);
	//
	//	_dirtyFlags.dirtyFlagGlobalRotationQuat2Euler = false;
	//}

	return glm::degrees(_globalRotation);
}

glm::quat Twin2Engine::Core::Transform::GetGlobalRotationQuat()
{
	RecalculateGlobalRotation();

	return _globalRotationQuat;
}

void Twin2Engine::Core::Transform::RecalculateGlobalRotation()
{
	if (_dirtyFlags.dirtyFlagGlobalRotation)
	{
		/*
		if (_parent == originTransform || _parent == nullptr)
		*/
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
		//_dirtyFlags.dirtyFlagGlobalRotationQuat2Euler = true;
	}
}

#pragma endregion

#pragma region SCALES

void Twin2Engine::Core::Transform::SetLocalScale(const glm::vec3& localScale)
{
	_localScale = localScale;

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagGlobalScale = true;
	_dirtyFlags.dirtyFlagLocalScale = false;

	// Calling Events
	CallScaleChanged();

	// Setting dirty flags
	SetDirtyFlagGlobalScaleInChildren();
}

glm::vec3 Twin2Engine::Core::Transform::GetLocalScale()
{
	RecalculateLocalScale();

	return _localScale;
}

void Twin2Engine::Core::Transform::RecalculateLocalScale()
{
	if (_dirtyFlags.dirtyFlagLocalScale)
	{
		/*
		if (_parent == originTransform || _parent == nullptr)
		*/
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

void Twin2Engine::Core::Transform::SetGlobalScale(const glm::vec3& globalScale)
{
	_globalScale = globalScale;

	_dirtyFlags.dirtyFlag = true;
	_dirtyFlags.dirtyFlagLocalScale = true;
	_dirtyFlags.dirtyFlagGlobalScale = false;

	// Calling Events
	CallScaleChanged();

	RecalculateLocalScale();

	// Setting dirty flags
	SetDirtyFlagGlobalScaleInChildren();
}

glm::vec3 Twin2Engine::Core::Transform::GetGlobalScale()
{
	RecalculateGlobalScale();
	//RecalculateTransformMatrix();
	//_globalScale = glm::vec3(_globalTransformMatrix[0][0], _globalTransformMatrix[1][1], _globalTransformMatrix[2][2]);

	return _globalScale;
}

void Twin2Engine::Core::Transform::RecalculateGlobalScale()
{
	if (_dirtyFlags.dirtyFlagGlobalScale)
	{
		/*
		if (_parent == originTransform || _parent == nullptr)
		*/
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

glm::mat4 Twin2Engine::Core::Transform::GetTransformMatrix()
{
	RecalculateTransformMatrix();

	return _globalTransformMatrix;
}

void Twin2Engine::Core::Transform::RecalculateTransformMatrix()
{
	if (_dirtyFlags.dirtyFlag)
	{
		_localTransformMatrix = glm::mat4(1.0f);

		_localTransformMatrix = glm::translate(_localTransformMatrix, GetLocalPosition());

		// Jest to w celu wykonania przebiegu rekalkulacji ale zewzględu na to iżzwraca stopnie to się nie nadaje , ale po tym bezpiecznie można użyć _localRotation
		//glm::vec3 rotation = GetLocalRotation();
		RecalculateLocalRotation();

		_localTransformMatrix = glm::rotate(_localTransformMatrix, _localRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		_localTransformMatrix = glm::rotate(_localTransformMatrix, _localRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		_localTransformMatrix = glm::rotate(_localTransformMatrix, _localRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		_localTransformMatrix = glm::scale(_localTransformMatrix, GetLocalScale());
	}

	if (_dirtyFlags.dirtyFlagInHierarchy)
	{
		/*
		if (_parent != nullptr && _parent != originTransform)
		*/
		if (_parent != nullptr)
		{
			_parent->RecalculateTransformMatrix();
			//_globalTransformMatrix = _parent->GetTransformMatrix() * _localTransformMatrix;

			//_dirtyFlags.dirtyFlagInHierarchy = false;
		}
	}

	if (_dirtyFlags.dirtyFlag || _dirtyFlags.dirtyFlagInHierarchy)
	{
		/*
		if (_parent != nullptr && _parent != originTransform)
		*/
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



void Twin2Engine::Core::Transform::Update()
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

YAML::Node Twin2Engine::Core::Transform::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node.remove("type");
	node["position"] = _localPosition;
	node["scale"] = _localScale;
	node["rotation"] = glm::degrees(_localRotation);
	return node;
}

bool Twin2Engine::Core::Transform::Deserialize(const YAML::Node& node) {
	if (!node["position"] || !node["scale"] || !node["rotation"] || !Component::Deserialize(node)) return false;

	_localPosition = node["position"].as<glm::vec3>();
	_localScale = node["scale"].as<glm::vec3>();
	_localRotation = glm::radians(node["rotation"].as<glm::vec3>());

	return true;
}