#include <inc/Transform.h>
//#include "Transform.h"

//Nie mo�e by� nullptr gdy� niekt�re warunki w set parent mog� si� posypa�
Twin2EngineCore::Transform* Twin2EngineCore::Transform::originTransform = new Transform();

#pragma region DIRTY_FLAG

void Twin2EngineCore::Transform::SetDirtyFlagInChildren()
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

void Twin2EngineCore::Transform::SetDirtyFlagGlobalPositionInChildren()
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

void Twin2EngineCore::Transform::SetDirtyFlagGlobalRotationInChildren()
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

void Twin2EngineCore::Transform::SetDirtyFlagGlobalScaleInChildren()
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

Twin2EngineCore::Transform::Transform()
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

#pragma region MANAGING_HIERARCHY

void Twin2EngineCore::Transform::AddChild(Transform* child)
{
	child->SetParent(this);
}

void Twin2EngineCore::Transform::RemoveChild(Transform* child)
{
	child->SetParent(originTransform);
}

void Twin2EngineCore::Transform::SetParent(Transform* parent)
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

	_parent->JustRemoveChild(this);
	parent->JustAddChild(this);
	_parent = parent;


	//_dirtyFlag = true;
	_dirtyFlagInHierarchy = true;
	
	SetDirtyFlagInChildren();
}

void Twin2EngineCore::Transform::JustAddChild(Transform* child)
{
	_children.push_back(child);
}

void Twin2EngineCore::Transform::JustRemoveChild(Transform* child)
{
	_children.erase(std::find(_children.begin(), _children.end(), child));
}


Twin2EngineCore::Transform* Twin2EngineCore::Transform::GetParent() const
{
	return _parent;
}


Twin2EngineCore::Transform* Twin2EngineCore::Transform::GetChildAt(size_t index) const
{
	if (index >= _children.size())
	{
		return nullptr;
	}

	return _children[index];
}

size_t Twin2EngineCore::Transform::GetChildCount() const
{
	return _children.size();
}

#pragma endregion

#pragma region POSITIONS

void Twin2EngineCore::Transform::SetLocalPosition(const glm::vec3& localPosition)
{
	_localPosition = localPosition;

	_dirtyFlag = true;
	_dirtyFlagGlobalPosition = true;
	_dirtyFlagLocalPosition = false;

	SetDirtyFlagGlobalPositionInChildren();
}


glm::vec3 Twin2EngineCore::Transform::GetLocalPosition()
{
	RecalculateLocalPosition();

	return _localPosition;
}

void Twin2EngineCore::Transform::RecalculateLocalPosition()
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

void Twin2EngineCore::Transform::SetGlobalPosition(const glm::vec3& globalPosition)
{
	_globalPosition = globalPosition;

	_dirtyFlag = true;
	_dirtyFlagLocalPosition = true;
	_dirtyFlagGlobalPosition = false;

	SetDirtyFlagGlobalPositionInChildren();
}

glm::vec3 Twin2EngineCore::Transform::GetGlobalPosition()
{
	RecalculateGlobalPosition();

	return _globalPosition;
}

void Twin2EngineCore::Transform::RecalculateGlobalPosition()
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

void Twin2EngineCore::Transform::SetLocalRotation(const glm::vec3& localRotation)
{
	_localRotation = localRotation;
	_localRotationQuat = glm::quat(localRotation);

	_dirtyFlag = true;
	_dirtyFlagGlobalRotation = true;
	_dirtyFlagLocalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}

glm::vec3 Twin2EngineCore::Transform::GetLocalRotation()
{
	RecalculateLocalRotation();

	return _localRotation;
}

void Twin2EngineCore::Transform::SetLocalRotation(const glm::quat& localRotation)
{
	_localRotationQuat = localRotation;
	_localRotation = glm::eulerAngles(localRotation);

	_dirtyFlag = true;
	_dirtyFlagGlobalRotation = true;
	_dirtyFlagLocalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}
glm::quat Twin2EngineCore::Transform::GetLocalRotationQuat()
{
	RecalculateLocalRotation();

	return _localRotationQuat;
}

void Twin2EngineCore::Transform::RecalculateLocalRotation()
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

void Twin2EngineCore::Transform::SetGlobalRotation(const glm::vec3& globalRotation)
{
	_globalRotation = globalRotation;
	_globalRotationQuat = glm::quat(globalRotation);

	_dirtyFlag = true;
	_dirtyFlagLocalRotation = true;
	_dirtyFlagGlobalRotation = false;

	SetDirtyFlagGlobalRotationInChildren();
}

glm::vec3 Twin2EngineCore::Transform::GetGlobalRotation()
{
	RecalculateGlobalRotation();

	if (_dirtyFlagGlobalRotationQuat2Euler)
	{
		_globalRotation = glm::eulerAngles(_globalRotationQuat);

		_dirtyFlagGlobalRotationQuat2Euler = false;
	}

	return _globalRotation;
}

glm::quat Twin2EngineCore::Transform::GetGlobalRotationQuat()
{
	RecalculateGlobalRotation();

	return _globalRotationQuat;
}

void Twin2EngineCore::Transform::RecalculateGlobalRotation()
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

void Twin2EngineCore::Transform::SetLocalScale(const glm::vec3& localScale)
{
	_localScale = localScale;

	_dirtyFlag = true;
	_dirtyFlagGlobalScale = true;
	_dirtyFlagLocalScale = false;

	SetDirtyFlagGlobalScaleInChildren();
}

glm::vec3 Twin2EngineCore::Transform::GetLocalScale()
{
	RecalculateLocalScale();

	return _localScale;
}

void Twin2EngineCore::Transform::RecalculateLocalScale()
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


void Twin2EngineCore::Transform::SetGlobalScale(const glm::vec3& globalScale)
{
	_globalScale = globalScale;

	_dirtyFlag = true;
	_dirtyFlagLocalScale = true;
	_dirtyFlagGlobalScale = false;

	SetDirtyFlagGlobalScaleInChildren();
}

glm::vec3 Twin2EngineCore::Transform::GetGlobalScale()
{
	RecalculateGlobalScale();
	//RecalculateTransformMatrix();
	//_globalScale = glm::vec3(_globalTransformMatrix[0][0], _globalTransformMatrix[1][1], _globalTransformMatrix[2][2]);

	return _globalScale;
}

void Twin2EngineCore::Transform::RecalculateGlobalScale()
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

glm::mat4 Twin2EngineCore::Transform::GetTransformMatrix()
{
	RecalculateTransformMatrix();

	return _globalTransformMatrix;
}

void Twin2EngineCore::Transform::RecalculateTransformMatrix()
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

		//_dirtyFlag = false;
	}


	if (_dirtyFlagInHierarchy)
	{
		if (_parent != originTransform)
		{
			_parent->RecalculateTransformMatrix();
			//_globalTransformMatrix = _parent->GetTransformMatrix() * _localTransformMatrix;
		}
	
		//_dirtyFlagInHierarchy = false;
	}

	if (_dirtyFlag || _dirtyFlagInHierarchy)
	{
		if (_parent != originTransform)
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
