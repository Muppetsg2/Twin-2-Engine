#include <inc/Transform.h>

void Twin2EngineCore::Transform::JustAddChild(Transform* child)
{
	_children.push_back(child);
}

void Twin2EngineCore::Transform::JustRemoveChild(Transform* child)
{
	_children.erase(std::find(_children.begin(), _children.end(), child));
}

Twin2EngineCore::Transform::Transform()
{
	this->_localPosition = glm::vec3(0.f, 0.f, 0.f);
	this->_localRotation = glm::vec3(0.f, 0.f, 0.f);
	this->_localScale = glm::vec3(1.f, 1.f, 1.f);

	this->_globalPosition = glm::vec3(0.f, 0.f, 0.f);
	this->_globalRotation = glm::vec3(0.f, 0.f, 0.f);
	this->_globalScale = glm::vec3(1.f, 1.f, 1.f);

	_parent = nullptr;

	_transformMatrix = glm::mat4(1.0f);

	dirtyFlag = false;
}

void Twin2EngineCore::Transform::SetParent(Transform* parent)
{

	if (parent == nullptr)
	{
		parent->JustRemoveChild(this);
	}
	else
	{
		parent->JustAddChild(this);
	}

	_parent = parent;
	dirtyFlag = true;
}

Twin2EngineCore::Transform* Twin2EngineCore::Transform::GetParent() const
{
	return _parent;
}

void Twin2EngineCore::Transform::AddChild(Transform* child)
{
	child->SetParent(this);
}

void Twin2EngineCore::Transform::RemoveChild(Transform* child)
{
	child->SetParent(nullptr);
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

void Twin2EngineCore::Transform::SetLocalPosition(const glm::vec3& localPosition)
{
	_localPosition = localPosition;
	dirtyFlag = true;
}

glm::vec3 Twin2EngineCore::Transform::GetLocalPosition() const
{
	return _localPosition;
}

void Twin2EngineCore::Transform::SetLocalRotation(const glm::vec3& localRotation)
{
	_localRotation = localRotation;
	dirtyFlag = true;
}

glm::vec3 Twin2EngineCore::Transform::GetLocalRotation() const
{
	return _localRotation;
}

void Twin2EngineCore::Transform::SetLocalScale(const glm::vec3& localScale)
{
	_localScale = localScale;
	dirtyFlag = true;
}

glm::vec3 Twin2EngineCore::Transform::GetLocalScale() const
{
	return _localScale;
}

void Twin2EngineCore::Transform::SetGlobalPosition(const glm::vec3& globalPosition)
{
}

glm::vec3 Twin2EngineCore::Transform::GetGlobalPosition() const
{
	return _globalPosition;
}

void Twin2EngineCore::Transform::SetGlobalRotation(const glm::vec3& globalRotation)
{
}

glm::vec3 Twin2EngineCore::Transform::GetGlobalRotation() const
{
	return _globalRotation;
}

void Twin2EngineCore::Transform::SetGlobalScale(const glm::vec3& globalScale)
{
}

glm::vec3 Twin2EngineCore::Transform::GetGlobalScale() const
{
	return _globalScale;
}

glm::mat4 Twin2EngineCore::Transform::GetTransformMatrix()
{
	//if (dirtyFlag)
	{
		dirtyFlag = false;

		_transformMatrix = glm::mat4(1.0f);

		_transformMatrix = glm::translate(_transformMatrix, _localPosition);
		_transformMatrix = glm::rotate(_transformMatrix, _localRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		_transformMatrix = glm::rotate(_transformMatrix, _localRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		_transformMatrix = glm::rotate(_transformMatrix, _localRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		_transformMatrix = glm::scale(_transformMatrix, _localScale);

		if (_parent)
		{
			//parent->RecalculateTransform();
			_transformMatrix = _parent->_transformMatrix * _transformMatrix;
		}

	}

	return _transformMatrix;
}
