#pragma once

#include <core/RenderableComponent.h>
#include <core/Transform.h>

#include <graphic/InstantiatingModel.h>
#include <graphic/Material.h>
#include <graphic/manager/MeshRenderingManager.h>

namespace Twin2Engine::Manager
{
	class MeshRenderingManager;
}

namespace Twin2Engine::Core
{
	class MeshRenderer : public RenderableComponent
	{
		friend class Twin2Engine::Manager::MeshRenderingManager;

		CloneFunctionStart(MeshRenderer, RenderableComponent)
			CloneField(_model)
			CloneField(_materials)
			CloneFunctionEnd()
	private:

		Graphic::InstatiatingModel _model;

		std::vector<Graphic::Material> _materials;

		bool _registered = false;
		bool _transformChanged = false;
		unsigned int _toUpdate = 0;

#ifdef MESH_FRUSTUM_CULLING
		int OnTransformChangedActionId = -1;
		Tools::Action<Transform*> OnTransformChangedAction = [this](Transform* transform) {
			glm::mat4 tMatrix = transform->GetTransformMatrix();
			Physic::SphereColliderData* sphereBV;
			for (size_t i = 0; i < _model.GetMeshCount(); ++i) {
				sphereBV = (Physic::SphereColliderData*)_model.GetMesh(i)->sphericalBV->colliderShape;
				if (sphereBV != nullptr) {
					sphereBV->Position = tMatrix * glm::vec4(sphereBV->LocalPosition, 1.0f);
				}
			}
			};
#endif // MESH_FRUSTUM_CULLING

		void TransformUpdated();

	public:

		virtual void Initialize();


		virtual void Render() override;
		virtual YAML::Node Serialize() const override;


		bool IsTransformChanged() const;

#pragma region MODEL_PART
		Graphic::InstatiatingModel GetModel() const;
		size_t GetMeshCount() const;
		void SetModel(const Graphic::InstatiatingModel& model);
		void SetModel(size_t modelId);
#pragma endregion

#pragma region MATERIALS_PART
		Graphic::InstatiatingMesh* GetMesh(size_t index) const;
		size_t GetMaterialCount() const;
		Graphic::Material GetMaterial(size_t index) const;
		void AddMaterial(Graphic::Material material);
		void AddMaterial(size_t materialId);
		void SetMaterial(size_t index, Graphic::Material material);
		void SetMaterial(size_t index, size_t materialId);
#pragma endregion

#ifdef MESH_FRUSTUM_CULLING
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;
#endif // MESH_FRUSTUM_CULLING


	};
}