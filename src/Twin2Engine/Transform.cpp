#include <core/Transform.h>
//#include "Transform.h"

//Nie mo�e by� nullptr gdy� niekt�re warunki w set parent mog� si� posypa�
Twin2Engine::Core::Transform* Twin2Engine::Core::Transform::originTransform = new Transform();

#pragma region DIRTY_FLAG

void Twin2Engine::Core::Transform::SetDirtyFlagInChildren()
{
	if (!_dirtyFlagInHierarchy)
	{
		for (int index = 0; index < _children.size(); index++)
		{
			_dirtyFlagInHierarchy = true;
			_children[index]->SetDirtyFlagInChildren();
		}
	}
}

void Twin2Engine::Core::Transform::SetDirtyFlagGlobalPositionInChildren()
{
	if (!_dirtyFlagGlobalPosition)
	{
		for (int index = 0; index < _children.size(); index++)
		{
			_dirtyFlagGlobalPosition = true;
			_dirtyFlagInHierarchy = true;
			_children[index]->SetDirtyFlagGlobalPositionInChildren();
		}
	}
}

void Twin2Engine::Core::Transform::SetDirtyFlagGlobalRotationInChildren()
{
	if (!_dirtyFlagGlobalRotation)
	{
		for (int index = 0; index < _children.size(); index++)
		{
			_dirtyFlagGlobalRotation = true;
			_dirtyFlagInHierarchy = true;
			_children[index]->SetDirtyFlagGlobalRotationInChildren();
		}
	}
}

void Twin2Engine::Core::Transform::SetDirtyFlagGlobalScaleInChildren()
{
	if (!_dirtyFlagGlobalScale)
	{
		for (int index = 0; index < _children.size(); index++)
		{
			_dirtyFlagGlobalScale = true;
			_dirtyFlagInHierarchy = true;
			_children[index]->SetDirtyFlagGlobalScaleInChildren();
		}
	}
}

#pragma endregion

Twin2Engine::Core::Transform::Transform()
{
	this->_localPosition = glm::vec3(0.f, 0.f, 0.f);
	this->_localRotation = glm::vec3(0.f, 0.f, 0.f);
	this->_localScale = glm::vec3(1.f, 1.f, 1.f);

	this->_globalPosition = glm::vec3(0.f, 0.f, 0.f);
	this->_globalRotation = glm::vec3(0.f, 0.f, 0.f);
	this->_globalScale = glm::vec3(1.f, 1.f, 1.f);

	_parent = originTransform;

	_globalTransformMatrix = glm::mat4(1.0f);

}

#pragma region TRANSFORMATING_METHODS

void Twin2Engine::Core::Transform::Translate(const glm::vec3& translation)
{
	RecalculateLocalPosition();

	_localPosition += translation;

	_dirtyFlag = true;
	_dirtyFlagGlobalPosition = true;
	_dirtyFlagLocalPosition = false;

	SetDirtyFlagGlobalPositionInChildren();
}

void Twin2Engine::Core::Transform::Rotate(const glm::vec3& rotation)
{
	RecalculateLocalRotation();

	_localRotation += rotation;
	_localRotationQuat = glm::quat(_localRotation);

	_dirtyFlag = true;
	_dirtyFlagGlobalRotation = true;
	_dirtyFlagLocalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}

void Twin2Engine::Core::Transform::Rotate(const glm::quat& rotation)
{
	RecalculateLocalRotation();

	_localRotationQuat *= rotation;
	_localRotation = glm::eulerAngles(_localRotationQuat);

	_dirtyFlag = true;
	_dirtyFlagGlobalRotation = true;
	_dirtyFlagLocalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}

void Twin2Engine::Core::Transform::Scale(const glm::vec3& scaling)
{
	RecalculateLocalScale();

	_localScale *= scaling;

	_dirtyFlag = true;
	_dirtyFlagGlobalScale = true;
	_dirtyFlagLocalScale = false;

	//RecalculateGlobalScale();
	// 
	//_globalScale *= scaling;
	//
	//_dirtyFlag = true;
	//_dirtyFlagLocalScale = true;
	//_dirtyFlagGlobalScale = false;

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
	child->SetParent(originTransform);
}

void Twin2Engine::Core::Transform::SetParent(Transform* parent)
{
	//Nie usuwa� jest to zapasowe w wypadtku zmiany originTransform na nullptr, ale b�dzie nal�a� to wtedy jeszcze przerobi�
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

	if (parent == nullptr)
	{
		parent = originTransform;
	}


	_parent->JustRemoveChild(this);
	parent->JustAddChild(this);
	_parent = parent;


	//_dirtyFlag = true;
	_dirtyFlagInHierarchy = true;
	
	SetDirtyFlagInChildren();
}

void Twin2Engine::Core::Transform::JustAddChild(Transform* child)
{
	_children.push_back(child);
}

void Twin2Engine::Core::Transform::JustRemoveChild(Transform* child)
{
	_children.erase(std::find(_children.begin(), _children.end(), child));
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

	_dirtyFlag = true;
	_dirtyFlagGlobalPosition = true;
	_dirtyFlagLocalPosition = false;

	SetDirtyFlagGlobalPositionInChildren();
}


glm::vec3 Twin2Engine::Core::Transform::GetLocalPosition()
{
	RecalculateLocalPosition();

	return _localPosition;
}

void Twin2Engine::Core::Transform::RecalculateLocalPosition()
{
	if (_dirtyFlagLocalPosition)
	{
		if (_parent == originTransform)
		{
			_localPosition = _globalPosition;
		}
		else
		{
			_localPosition = glm::vec3(glm::inverse(_parent->GetTransformMatrix()) * glm::vec4(_globalPosition, 1.0f));
		}

		_dirtyFlagLocalPosition = false;
	}
}

void Twin2Engine::Core::Transform::SetGlobalPosition(const glm::vec3& globalPosition)
{
	_globalPosition = globalPosition;

	_dirtyFlag = true;
	_dirtyFlagLocalPosition = true;
	_dirtyFlagGlobalPosition = false;

	SetDirtyFlagGlobalPositionInChildren();
}

glm::vec3 Twin2Engine::Core::Transform::GetGlobalPosition()
{
	RecalculateGlobalPosition();

	return _globalPosition;
}

void Twin2Engine::Core::Transform::RecalculateGlobalPosition()
{
	if (_dirtyFlagGlobalPosition)
	{
		RecalculateTransformMatrix();

		_globalPosition = glm::vec3(_globalTransformMatrix[3]);

		_dirtyFlagGlobalPosition = false;
	}
}

#pragma endregion

#pragma region ROTATIONS

void Twin2Engine::Core::Transform::SetLocalRotation(const glm::vec3& localRotation)
{
	_localRotation = localRotation;
	_localRotationQuat = glm::quat(localRotation);

	_dirtyFlag = true;
	_dirtyFlagGlobalRotation = true;
	_dirtyFlagLocalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}

glm::vec3 Twin2Engine::Core::Transform::GetLocalRotation()
{
	RecalculateLocalRotation();

	return _localRotation;
}

void Twin2Engine::Core::Transform::SetLocalRotation(const glm::quat& localRotation)
{
	_localRotationQuat = localRotation;
	_localRotation = glm::eulerAngles(localRotation);

	_dirtyFlag = true;
	_dirtyFlagGlobalRotation = true;
	_dirtyFlagLocalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}
glm::quat Twin2Engine::Core::Transform::GetLocalRotationQuat()
{
	RecalculateLocalRotation();

	return _localRotationQuat;
}

void Twin2Engine::Core::Transform::RecalculateLocalRotation()
{
	if (_dirtyFlagLocalRotation)
	{
		if (_parent == originTransform)
		{
			_localRotationQuat = _globalRotationQuat;
			_localRotation = _globalRotation;
		}
		else
		{
			_localRotationQuat = glm::inverse(_parent->GetGlobalRotationQuat()) * _globalRotationQuat;
			_localRotation = glm::eulerAngles(_localRotationQuat);
		}

		_dirtyFlagLocalRotation = false;
	}
}

void Twin2Engine::Core::Transform::SetGlobalRotation(const glm::vec3& globalRotation)
{
	_globalRotation = globalRotation;
	_globalRotationQuat = glm::quat(globalRotation);

	_dirtyFlag = true;
	_dirtyFlagLocalRotation = true;
	_dirtyFlagGlobalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}

glm::vec3 Twin2Engine::Core::Transform::GetGlobalRotation()
{
	RecalculateGlobalRotation();

	if (_dirtyFlagGlobalRotationQuat2Euler)
	{
		_globalRotation = glm::eulerAngles(_globalRotationQuat);

		_dirtyFlagGlobalRotationQuat2Euler = false;
	}

	return _globalRotation;
}

glm::quat Twin2Engine::Core::Transform::GetGlobalRotationQuat()
{
	RecalculateGlobalRotation();

	return _globalRotationQuat;
}

void Twin2Engine::Core::Transform::RecalculateGlobalRotation()
{
	if (_dirtyFlagGlobalRotation)
	{
		if (_parent == originTransform)
		{
			_globalRotationQuat = _localRotationQuat;
		}
		else
		{
			_globalRotationQuat = _parent->GetGlobalRotation() * _localRotationQuat;
		}

		_dirtyFlagGlobalRotation = false;
		_dirtyFlagGlobalRotationQuat2Euler = true;
	}
}

#pragma endregion

#pragma region SCALES

void Twin2Engine::Core::Transform::SetLocalScale(const glm::vec3& localScale)
{
	_localScale = localScale;

	_dirtyFlag = true;
	_dirtyFlagGlobalScale = true;
	_dirtyFlagLocalScale = false;

	SetDirtyFlagGlobalScaleInChildren();
}

glm::vec3 Twin2Engine::Core::Transform::GetLocalScale()
{
	RecalculateLocalScale();

	return _localScale;
}

void Twin2Engine::Core::Transform::RecalculateLocalScale()
{
	if (_dirtyFlagLocalScale)
	{
		if (_parent == originTransform)
		{
			_localScale = _globalScale;
		}
		else
		{
			_localScale = _globalScale / _parent->GetGlobalScale();
		}

		_dirtyFlagLocalScale = false;
	}
}


void Twin2Engine::Core::Transform::SetGlobalScale(const glm::vec3& globalScale)
{
	_globalScale = globalScale;

	_dirtyFlag = true;
	_dirtyFlagLocalScale = true;
	_dirtyFlagGlobalScale = false;

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
	if (_dirtyFlagGlobalScale)
	{
		if (_parent == originTransform)
		{
			_globalScale = _localScale;
		}
		else
		{
			_globalScale = _parent->GetGlobalScale() * _localScale;
		}

		_dirtyFlagGlobalScale = false;
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
	if (_dirtyFlag)
	{
		_localTransformMatrix = glm::mat4(1.0f);

		_localTransformMatrix = glm::translate(_localTransformMatrix, GetLocalPosition());

		glm::vec3 rotation = GetLocalRotation();

		_localTransformMatrix = glm::rotate(_localTransformMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		_localTransformMatrix = glm::rotate(_localTransformMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		_localTransformMatrix = glm::rotate(_localTransformMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		_localTransformMatrix = glm::scale(_localTransformMatrix, GetLocalScale());
	}

	if (_dirtyFlagInHierarchy)
	{
		if (_parent != nullptr && _parent != originTransform)
		{
			_parent->RecalculateTransformMatrix();
			//_globalTransformMatrix = _parent->GetTransformMatrix() * _localTransformMatrix;
	
			//_dirtyFlagInHierarchy = false;
		}
	}

	if (_dirtyFlag || _dirtyFlagInHierarchy)
	{
		if (_parent != nullptr && _parent != originTransform)
		{
			_globalTransformMatrix = _parent->GetTransformMatrix() * _localTransformMatrix;
		}
		else
		{
			_globalTransformMatrix = _localTransformMatrix;
		}

		_dirtyFlag = false;
		_dirtyFlagInHierarchy = false;
	}
}
