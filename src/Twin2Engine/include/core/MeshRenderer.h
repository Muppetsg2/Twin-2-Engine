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


		void SetModel(const GraphicEngine::InstatiatingModel& model);
		void SetModel(size_t modelId);
	};
}

#endif