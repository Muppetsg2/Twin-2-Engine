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
	protected:
		CloneBaseFunc(MeshRenderer, RenderableComponent, _model, _materials)

		std::vector<Graphic::Material*> _materials = std::vector<Graphic::Material*>();

#if _DEBUG
		size_t _next = 0;
		std::vector<size_t> _addNum = std::vector<size_t>();
#endif

		void Register();
		void Unregister();

		void OnModelDataDestroyed();

#if _DEBUG
		bool DrawInheritedFields() override;
#endif
	private:

		size_t _loadedModel = 0;
		Graphic::InstantiatingModel _model;

		bool _registered = false;
		bool _transformChanged = false;
		unsigned int _meshesToUpdate = 0;

		bool _materialsErasedEventDone = false;
		bool _materialError = false;

		int _transparencyPriority = 0;

		// Events
		int OnStaticChangedId = -1;
		int OnActiveChangedId = -1;
		int OnEventInHierarchyParentChangedId = -1;
		int OnTransformMatrixChangedId = -1;

		void OnGameObjectStaticChanged(GameObject* gameObject);
		void OnGameObjectActiveChanged(GameObject* gameObject);
		void OnTransformMatrixChanged(Transform* transform);
		void OnMaterialsErased();
		bool CheckMaterialsValidation();

		void TransformUpdated();

	public:

		void SetIsTransparent(bool value) override;

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnEnable() override;
		virtual void OnDisable() override;
		virtual void OnDestroy() override;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif

		bool IsTransformChanged() const;

#pragma region MODEL_PART
		Graphic::InstantiatingModel GetModel() const;
		size_t GetMeshCount() const;
		Graphic::InstantiatingMesh* GetMesh(size_t index) const;
		void SetModel(const Graphic::InstantiatingModel& model);
		void SetModel(size_t modelId);
#pragma endregion

#pragma region MATERIALS_PART
		bool IsMaterialError() const;
		size_t GetMaterialCount() const;
		Graphic::Material* GetMaterial(size_t index) const;
		void AddMaterial(Graphic::Material* material);
		void AddMaterial(size_t materialId);
		void SetMaterial(size_t index, Graphic::Material* material);
		void SetMaterial(size_t index, size_t materialId);
		void RemoveMaterial(size_t index);
#pragma endregion
	};
}