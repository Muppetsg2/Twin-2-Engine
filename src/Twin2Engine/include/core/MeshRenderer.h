#ifndef _MESH_RENDERER_H_
#define _MESH_RENDERER_H_

#include <core/RenderableComponent.h>
#include <core/Transform.h>

#include <graphic/InstatiatingModel.h>
#include <graphic/Material.h>
#include <graphic/manager/MeshRenderingManager.h>

namespace Twin2Engine::Core
{
	class MeshRenderer : public RenderableComponent
	{
		CloneFunctionStart(MeshRenderer, RenderableComponent)
			CloneField(_model)
			CloneField(_materials)
		CloneFunctionEnd()
	private:

		GraphicEngine::InstatiatingModel _model;

		std::vector<GraphicEngine::Material> _materials;

		#ifdef MESH_FRUSTUM_CULLING
		int OnTransformChangedActionId = -1;
		Twin2Engine::Core::Action<Transform*> OnTransformChangedAction = [this](Transform* transform) {
			glm::mat4 tMatrix = transform->GetTransformMatrix();
			CollisionSystem::SphereColliderData* sphereBV;
			for (size_t i = 0; i < _model.GetMeshCount(); ++i) {
				sphereBV = (CollisionSystem::SphereColliderData*)_model.GetMesh(i)->sphericalBV->colliderShape;
				if (sphereBV != nullptr) {
					sphereBV->Position = tMatrix * glm::vec4(sphereBV->LocalPosition, 1.0f);
				}
			}
		};
		#endif // MESH_FRUSTUM_CULLING

	public:
		virtual void Render() override;
		virtual YAML::Node Serialize() const override;

		GraphicEngine::InstatiatingModel GetModel() const;

		size_t GetMeshCount() const;

		GraphicEngine::InstatiatingMesh* GetMesh(size_t index) const;
		size_t GetMaterialCount() const;
		GraphicEngine::Material GetMaterial(size_t index) const;
		void AddMaterial(GraphicEngine::Material material);
		void AddMaterial(size_t materialId);
		void SetMaterial(size_t index, GraphicEngine::Material material);
		void SetMaterial(size_t index, size_t materialId);

		#ifdef MESH_FRUSTUM_CULLING
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;
		#endif // MESH_FRUSTUM_CULLING


		void SetModel(const GraphicEngine::InstatiatingModel& model);
		void SetModel(size_t modelId);
	};
}

#endif