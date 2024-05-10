#pragma once

#include <core/Component.h>
#include <core/EventHandler.h>

namespace Twin2Engine
{
	namespace Core {
		class Transform : public Component
		{
		private:
			//static Transform* originTransform;

			glm::vec3 _localPosition;
			glm::vec3 _localRotation;
			glm::quat _localRotationQuat;
			glm::vec3 _localScale;


			glm::vec3 _globalPosition;
			glm::vec3 _globalRotation;
			glm::quat _globalRotationQuat;
			glm::vec3 _globalScale;

			//bool _dirtyFlag = false;
			//bool _dirtyFlagInHierarchy = false;
			//
			//bool _dirtyFlagGlobalPosition = false;
			//bool _dirtyFlagLocalPosition = false;
			//
			//bool _dirtyFlagGlobalRotation = false;
			//bool _dirtyFlagLocalRotation = false;
			//
			//bool _dirtyFlagGlobalScale = false;
			//bool _dirtyFlagLocalScale = false;
			//
			//bool _dirtyFlagGlobalRotationQuat2Euler = false;
			//bool _dirtyFlagLocalRotationQuat2Euler = false;

			struct DirtyFlags
			{
				bool dirtyFlag : 1;
				bool dirtyFlagInHierarchy : 1;

				bool dirtyFlagGlobalPosition : 1;
				bool dirtyFlagLocalPosition : 1;

				bool dirtyFlagGlobalRotation : 1;
				bool dirtyFlagLocalRotation : 1;

				bool dirtyFlagGlobalScale : 1;
				bool dirtyFlagLocalScale : 1;

				bool dirtyFlagGlobalRotationQuat2Euler : 1;

			} _dirtyFlags;

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


			struct CallingEvents
			{
				bool transformChanged : 1;
				bool positionChanged : 1;
				bool rotationChanged : 1;
				bool scaleChanged : 1;

				bool parentChanged : 1;
				bool inHierarchyParentChanged : 1;
				bool childrenChanged : 1;

			} _callingEvents;

			inline void CallPositionChanged();
			inline void CallRotationChanged();
			inline void CallScaleChanged();
			inline void CallParentChanged();
			void CallInHierarchyParentChanged();
			inline void CallChildrenChanged();

		public:
#pragma region EVENTS
			EventHandler<Transform*> OnEventTransformChanged;
			EventHandler<Transform*> OnEventPositionChanged;
			EventHandler<Transform*> OnEventRotationChanged;
			EventHandler<Transform*> OnEventScaleChanged;
			EventHandler<Transform*> OnEventParentChanged;
			EventHandler<Transform*> OnEventInHierarchyParentChanged;
			EventHandler<Transform*> OnEventChildrenChanged;
#pragma endregion

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

			inline bool GetDirtyFlag() const;
			inline bool GetDirtyFlagInHierarchy() const;
			inline bool GetDirtyFlagGlobalPosition() const;
			inline bool GetDirtyFlagLocalPosition() const;
			inline bool GetDirtyFlagGlobalRotation() const;
			inline bool GetDirtyFlagLocalRotation() const;
			inline bool GetDirtyFlagGlobalScale() const;
			inline bool GetDirtyFlagLocalScale() const;
			inline bool GetDirtyFlagGlobalRotationQuat2Euler() const;

			virtual void Update() override;
			virtual YAML::Node Serialize() const override;
			virtual void DrawEditor() override;
		};
	}
}