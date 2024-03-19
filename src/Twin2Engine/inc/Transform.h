#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <inc/Component.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <vector>



namespace Twin2EngineCore
{
	class Transform : public Component
	{
	private:
		glm::vec3 _localPosition;
		glm::vec3 _localRotation;
		glm::vec3 _localScale;


		glm::vec3 _globalPosition;
		glm::vec3 _globalRotation;
		glm::vec3 _globalScale;

		bool dirtyFlag;

		glm::mat4 _transformMatrix;

		Transform* _parent;

		std::vector<Transform*> _children;


		void JustAddChild(Transform* child);
		void JustRemoveChild(Transform* child);

	public:
		Transform();

		void SetParent(Transform* parent);
		Transform* GetParent() const;

		void AddChild(Transform* child);
		void RemoveChild(Transform* child);

		/**
		* Method used to access children of Transform at specified position.
		* @return Returns Transform of child at specified position. If there is no child at specified index returns nullptr.
		*/
		Transform* GetChildAt(size_t index) const;
		size_t GetChildCount() const;

		void SetLocalPosition(const glm::vec3& localPosition);
		glm::vec3 GetLocalPosition() const;
		void SetLocalRotation(const glm::vec3& localRotation);
		glm::vec3 GetLocalRotation() const;
		void SetLocalScale(const glm::vec3& localScale);
		glm::vec3 GetLocalScale() const;

		void SetGlobalPosition(const glm::vec3& globalPosition);
		glm::vec3 GetGlobalPosition() const;
		void SetGlobalRotation(const glm::vec3& globalRotation);
		glm::vec3 GetGlobalRotation() const;
		void SetGlobalScale(const glm::vec3& globalScale);
		glm::vec3 GetGlobalScale() const;

		glm::mat4 GetTransformMatrix();
	};
}

#endif