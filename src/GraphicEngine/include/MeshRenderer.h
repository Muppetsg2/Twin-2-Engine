#ifndef _MESH_RENDERER_H_
#define _MESH_RENDERER_H_

#include <InstatiatingModel.h>
#include <MeshRenderingManager.h>

#include <inc/RenderableComponent.h>

namespace GraphicEngine
{
	class MeshRenderer : public Twin2EngineCore::RenderableComponent
	{
		InstatiatingModel _model;

		std::vector<Material> _materials;

	public:
		virtual void Initialize() override;
		virtual void OnDestroy() override;

		InstatiatingModel GetModel() const;

		size_t GetMeshCount() const;

		InstatiatingMesh* GetMesh(size_t index) const;
		size_t GetMaterialCount() const;
		Material GetMaterial(size_t index) const;
		void AddMaterial(Material material);
		void SetMaterial(size_t index, Material material);


		void SetModel(const InstatiatingModel& model);
	};
}

#endif