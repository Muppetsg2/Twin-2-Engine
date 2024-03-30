#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <core/Component.h>

namespace Twin2Engine
{
	namespace Core {
		class Transform : public Component
		{
		private:
			static Transform* originTransform;

			glm::vec3 _localPosition;
			glm::vec3 _localRotation;
			glm::quat _localRotationQuat;
			glm::vec3 _localScale;


			glm::vec3 _globalPosition;
			glm::vec3 _globalRotation;
			glm::quat _globalRotationQuat;
			glm::vec3 _globalScale;

			bool _dirtyFlag = false;
			bool _dirtyFlagInHierarchy = false;

			bool _dirtyFlagGlobalPosition = false;
			bool _dirtyFlagLocalPosition = false;

			bool _dirtyFlagGlobalRotation = false;
			bool _dirtyFlagLocalRotation = false;

			bool _dirtyFlagGlobalScale = false;
			bool _dirtyFlagLocalScale = false;

			bool _dirtyFlagGlobalRotationQuat2Euler = false;
			//bool _dirtyFlagLocalRotationQuat2Euler = false;

			glm::mat4 _localTransformMatrix;
			glm::mat4 _globalTransformMatrix;

			Transform* _parent;

			std::vector<Transform*> _children;


			inline void JustAddChild(Transform* child);
			inline void JustRemoveChild(Transform* child);

			void SetDirtyFlagInChildren();
			void SetDirtyFlagGlobalPositionInChildren();
			void SetDirtyFlagGlobalRotationInChildren();
			void SetDirtyFlagGlobalScaleInChildren();

			void RecalculateTransformMatrix();

			void RecalculateGlobalPosition();
			void RecalculateLocalPosition();

			void RecalculateGlobalRotation();
			void RecalculateLocalRotation();

			void RecalculateGlobalScale();
			void RecalculateLocalScale();

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

			void Translate(const glm::vec3& translation);
			void Rotate(const glm::vec3& rotation);
			void Rotate(const glm::quat& rotation);
			void Scale(const glm::vec3& scaling);

			void SetLocalPosition(const glm::vec3& localPosition);
			glm::vec3 GetLocalPosition();
			void SetLocalRotation(const glm::vec3& localRotation);
			glm::vec3 GetLocalRotation();
			void SetLocalRotation(const glm::quat& localRotation);
			glm::quat GetLocalRotationQuat();
			void SetLocalScale(const glm::vec3& localScale);
			glm::vec3 GetLocalScale();

			void SetGlobalPosition(const glm::vec3& globalPosition);
			glm::vec3 GetGlobalPosition();
			void SetGlobalRotation(const glm::vec3& globalRotation);
			glm::vec3 GetGlobalRotation();
			glm::quat GetGlobalRotationQuat();
			void SetGlobalScale(const glm::vec3& globalScale);
			glm::vec3 GetGlobalScale();

			glm::mat4 GetTransformMatrix();
		};
	}
}

#endif