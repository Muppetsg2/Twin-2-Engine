#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/InstantiatingMesh.h>
#include <graphic/Window.h>

using namespace Twin2Engine::GraphicEngine;
using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace std;

template<> struct hash<Material>
{
public:
	std::size_t operator()(const Material& m) const
	{
		return (size_t)m.GetMaterialParameters();
	}
};

#if RENERING_TYPE_MESH_SHADER_MATERIAL
std::map<InstantiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_renderQueue = std::map<InstantiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>();
std::map<InstantiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_depthMapRenderQueue = std::map<InstantiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>();
#elif RENERING_TYPE_SHADER_MATERIAL_MESH
std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_renderQueueStatic = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();
std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_depthMapenderQueueStatic = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();

std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap> MeshRenderingManager::_depthQueueStatic = std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap>();

std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_renderQueueDynamic = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();
std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_depthMapenderQueueDynamic = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();

std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap> MeshRenderingManager::_depthQueueDynamic = std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap>();




std::map<Shader*, std::map<Material, std::map<InstantiatingMesh*, std::queue<MeshRenderData>>>> MeshRenderingManager::_renderQueue = std::map<Shader*, std::map<Material, std::map<InstantiatingMesh*, std::queue<MeshRenderData>>>>();
std::map<Shader*, std::map<Material, std::map<InstantiatingMesh*, std::queue<MeshRenderData>>>> MeshRenderingManager::_depthMapRenderQueue = std::map<Shader*, std::map<Material, std::map<InstantiatingMesh*, std::queue<MeshRenderData>>>>();
#elif RENERING_TYPE_SHADER_MESH_MATERIAL
std::map<Shader*, std::map<InstantiatingMesh*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_renderQueue = std::map<Shader*, std::map<InstantiatingMesh*, std::map<Material, std::queue<MeshRenderData>>>>();
std::map<Shader*, std::map<InstantiatingMesh*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_depthMapRenderQueue = std::map<Shader*, std::map<InstantiatingMesh*, std::map<Material, std::queue<MeshRenderData>>>>();
#endif
//std::map<InstantiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>> MeshRenderingManager::_depthMapRenderQueue = std::map<InstantiatingMesh*, std::map<Shader*, std::map<Material, std::queue<MeshRenderData>>>>();
std::map<InstantiatingMesh*, std::queue<MeshRenderData>> MeshRenderingManager::_depthQueue;

GLuint MeshRenderingManager::_instanceDataSSBO = 0u;
GLuint MeshRenderingManager::_materialIndexSSBO = 0u;
GLuint MeshRenderingManager::_materialInputUBO = 0u;

glm::mat4* MeshRenderingManager::_modelTransforms = nullptr;
unsigned int* MeshRenderingManager::_materialsIndexes = nullptr;

void MeshRenderingManager::Init()
{
	_modelTransforms = new glm::mat4[MAX_INSTANCE_NUMBER_PER_DRAW];
	_materialsIndexes = new unsigned int[MAX_INSTANCE_NUMBER_PER_DRAW];
	;
	for (size_t index = 0; index < MAX_INSTANCE_NUMBER_PER_DRAW; index++)
	{
		_materialsIndexes[index] = 0;
	}

	// Tworzenie SSBO instanceData
	glGenBuffers(1, &_instanceDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
	// Initialization of buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT_INSTANCE_DATA, _instanceDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		SPDLOG_ERROR("Error1: {}", error);
	};

	// Tworzenie SSBO materialIndex
	glGenBuffers(1, &_materialIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
	// Initialization of buffer
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT_MATERIAL_INDEX, _materialIndexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Tworzenie UBO materialInput
	glGenBuffers(1, &_materialInputUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
	// Initialization of buffer
	glBufferData(GL_UNIFORM_BUFFER, MAX_MATERIAL_SIZE * MAX_MATERIAL_NUMBER_PER_DRAW, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, _materialInputUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MeshRenderingManager::UnloadAll()
{
	delete[] _modelTransforms;
	delete[] _materialsIndexes;

	glDeleteBuffers(1, &_instanceDataSSBO);
	glDeleteBuffers(1, &_materialIndexSSBO);
	glDeleteBuffers(1, &_materialInputUBO);

	_renderQueue.clear();
	_depthQueue.clear();
	_depthMapRenderQueue.clear();
}


void MeshRenderingManager::Render(MeshRenderData meshData)
{
	//SPDLOG_INFO("Trying saving to rendering data1!");
	for (int i = 0; i < meshData.meshes.size(); i++)
	{
		//SPDLOG_INFO("Trying saving to rendering data! {}, {}", meshRenderer->GetMesh(i) != nullptr, meshRenderer->GetMaterial(i) != nullptr);
		if (meshData.meshes[i] != nullptr && meshData.materials[i] != nullptr)
		{
			//SPDLOG_INFO("Saving to rendering data!");

#if RENERING_TYPE_MESH_SHADER_MATERIAL

			_renderQueue[meshData.meshes[i]]
				[meshData.materials[i].GetShader()]
				[meshData.materials[i]]
				.push(meshData);

			if (!meshData.isTransparent) {
				_depthMapRenderQueue[meshData.meshes[i]]
					[meshData.materials[i].GetShader()]
					[meshData.materials[i]]
					.push(meshData);

				_depthQueue[meshData.meshes[i]].push(meshData);
			}

#elif RENERING_TYPE_SHADER_MATERIAL_MESH

			_renderQueue[meshData.materials[i].GetShader()]
				[meshData.materials[i]]
				[meshData.meshes[i]]
				.push(meshData);

			if (!meshData.isTransparent) {
				_depthMapRenderQueue[meshData.materials[i].GetShader()]
					[meshData.materials[i]]
					[meshData.meshes[i]]
					.push(meshData);

				_depthQueue[meshData.meshes[i]].push(meshData);
			}
#elif RENERING_TYPE_SHADER_MESH_MATERIAL

			_renderQueue[meshData.materials[i].GetShader()]
				[meshData.meshes[i]]
				[meshData.materials[i]]
				.push(meshData);

			if (!meshData.isTransparent) {
				_depthMapRenderQueue[meshData.materials[i].GetShader()]
					[meshData.meshes[i]]
					[meshData.materials[i]]
					.push(meshData);

				_depthQueue[meshData.meshes[i]].push(meshData);
			}
#endif

		}
		else if (meshData.meshes[i] != nullptr) {
			SPDLOG_WARN("Mesh was null");
		}
		else if (meshData.materials[i] != nullptr) {
			SPDLOG_WARN("Material was null");
		}
	}
}


void MeshRenderingManager::RegisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		InstantiatingMesh* mesh;
		for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
			mesh = meshRenderer->GetMesh(i);
			Material material = meshRenderer->GetMaterial(i);

			auto& queueElement = _renderQueueStatic[material.GetShader()][material][mesh];
			queueElement.meshRenderers.push_back(meshRenderer);
			queueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());

			auto& depthMapEueueElement = _depthMapenderQueueStatic[material.GetShader()][material][mesh];
			depthMapEueueElement.meshRenderers.push_back(meshRenderer);
			depthMapEueueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());

			//_depthQueue[meshData.meshes[i]].push(meshData);
		}
	}
}

void MeshRenderingManager::UnregisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		InstantiatingMesh* mesh;
		for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
			mesh = meshRenderer->GetMesh(i);
			Material material = meshRenderer->GetMaterial(i);

			auto& meshRenderers = _renderQueueStatic[material.GetShader()][material][mesh].meshRenderers;

			for (size_t i = 0; i < meshRenderers.size(); i++)
			{
				if (meshRenderers[i] == meshRenderer)
				{
					meshRenderers.erase(meshRenderers.cbegin() + i);

					_renderQueueStatic[material.GetShader()][material][mesh]
						.modelTransforms.erase(_renderQueueStatic[material.GetShader()][material][mesh].modelTransforms.cbegin() + i);

					_depthMapenderQueueStatic[material.GetShader()][material][mesh]
						.meshRenderers.erase(_depthMapenderQueueStatic[material.GetShader()][material][mesh].meshRenderers.cbegin() + i);

					_depthMapenderQueueStatic[material.GetShader()][material][mesh]
						.modelTransforms.erase(_depthMapenderQueueStatic[material.GetShader()][material][mesh].modelTransforms.cbegin() + i);

					break;
				}
			}
		}
	}
}



void MeshRenderingManager::RegisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		InstantiatingMesh* mesh;
		for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
			mesh = meshRenderer->GetMesh(i);
			Material material = meshRenderer->GetMaterial(i);

			auto& queueElement = _renderQueueDynamic[material.GetShader()][material][mesh];
			queueElement.meshRenderers.push_back(meshRenderer);
			queueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());

			auto& depthMapEueueElement = _depthMapenderQueueDynamic[material.GetShader()][material][mesh];
			depthMapEueueElement.meshRenderers.push_back(meshRenderer);
			depthMapEueueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());

			//_depthQueue[meshData.meshes[i]].push(meshData);
		}
	}
}

void MeshRenderingManager::UnregisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		InstantiatingMesh* mesh;
		for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
			mesh = meshRenderer->GetMesh(i);
			Material material = meshRenderer->GetMaterial(i);

			auto& meshRenderers = _renderQueueDynamic[material.GetShader()][material][mesh].meshRenderers;

			for (size_t i = 0; i < meshRenderers.size(); i++)
			{
				if (meshRenderers[i] == meshRenderer)
				{
					meshRenderers.erase(meshRenderers.cbegin() + i);

					_renderQueueDynamic[material.GetShader()][material][mesh]
						.modelTransforms.erase(_renderQueueDynamic[material.GetShader()][material][mesh].modelTransforms.cbegin() + i);

					_depthMapenderQueueDynamic[material.GetShader()][material][mesh]
						.meshRenderers.erase(_depthMapenderQueueDynamic[material.GetShader()][material][mesh].meshRenderers.cbegin() + i);

					_depthMapenderQueueDynamic[material.GetShader()][material][mesh]
						.modelTransforms.erase(_depthMapenderQueueDynamic[material.GetShader()][material][mesh].modelTransforms.cbegin() + i);

					break;
				}
			}
		}
	}
}

void MeshRenderingManager::UpdateQueues()
{
	Frustum frustum = CameraComponent::GetMainCamera()->GetFrustum();
	RenderedSegment renderedSegment{ .offset = 0, .count = 0 };
	bool lastAdded = true;

#pragma region UPDATE_FOR_STATIC
	for (auto& shaderPair : _renderQueueStatic)
	{
		for (auto& materialPair : shaderPair.second)
		{
			for (auto& meshPair : materialPair.second)
			{
				renderedSegment.offset = 0u;
				renderedSegment.count = 0u;
				lastAdded = true;

				meshPair.second.rendered.clear();
				meshPair.second.renderedCount = 0u;

				for (size_t index = 0ull; index < meshPair.second.meshRenderers.size(); index++)
				{
					//W przypadku gdy ma byæ aktualizacja transforma
					//meshPair.second.modelTransforms[index] = meshPair.second.meshRenderers[index]->GetTransform()->GetTransformMatrix();


					if (!meshPair.second.meshRenderers[index]->IsTransparent() && meshPair.second.meshRenderers[index]->GetGameObject()->GetActive())
					{
						if (CameraComponent::GetMainCamera()->IsFrustumCullingOn)
						{
							if (meshPair.first->IsOnFrustum(frustum, meshPair.second.modelTransforms[index]))
							{
								lastAdded = true;
								renderedSegment.count++;
							}
							else
							{
								if (lastAdded)
								{
									lastAdded = false;
									meshPair.second.rendered.push_back(renderedSegment);
									meshPair.second.renderedCount += renderedSegment.count;
									renderedSegment.count = 0u;
								}
								else
								{
									renderedSegment.offset = index + 1u;
								}
							}
						}
						else
						{
							lastAdded = true;
							renderedSegment.count++;
						}
					}
					else
					{
						if (lastAdded)
						{
							lastAdded = false;
							meshPair.second.rendered.push_back(renderedSegment);
							meshPair.second.renderedCount += renderedSegment.count;
							renderedSegment.count = 0u;
						}
						else
						{
							renderedSegment.offset = index + 1u;
						}
					}
				}
				if (renderedSegment.count)
				{
					meshPair.second.rendered.push_back(renderedSegment);
					meshPair.second.renderedCount += renderedSegment.count;
					renderedSegment.count = 0u;
				}

				_depthMapenderQueueStatic[shaderPair.first][materialPair.first][meshPair.first].rendered = meshPair.second.rendered;
				_depthMapenderQueueStatic[shaderPair.first][materialPair.first][meshPair.first].renderedCount = meshPair.second.renderedCount;

				_depthQueueStatic[meshPair.first].renderedCount += meshPair.second.renderedCount;
				for (const auto& element : meshPair.second.rendered)
				{
					_depthQueueStatic[meshPair.first].rendered.emplace_back(&meshPair.second.modelTransforms, element.offset, element.count);
				}
			}
		}
	}
#pragma endregion

#pragma region UPDATE_FOR_DYNAMIC
	for (auto& shaderPair : _renderQueueDynamic)
	{
		for (auto& materialPair : shaderPair.second)
		{
			for (auto& meshPair : materialPair.second)
			{
				renderedSegment.offset = 0u;
				renderedSegment.count = 0u;
				lastAdded = true;

				meshPair.second.rendered.clear();
				meshPair.second.renderedCount = 0u;

				for (size_t index = 0ull; index < meshPair.second.meshRenderers.size(); index++)
				{
					//W przypadku gdy ma byæ aktualizacja transforma
					if (meshPair.second.meshRenderers[index]->IsTransformChanged())
					{
						//SPDLOG_INFO("Game object ptr: {}", (unsigned int)this);
						//SPDLOG_INFO("Transform ptr: {}", (unsigned int)_transform);
						//SPDLOG_INFO("Game object ptr: {}", (unsigned int)meshPair.second.meshRenderers[index]->GetGameObject());
						//SPDLOG_INFO("Transform ptr: {}", (unsigned int)meshPair.second.meshRenderers[index]->GetGameObject()->GetTransform());
						meshPair.second.modelTransforms[index] = meshPair.second.meshRenderers[index]->GetGameObject()->GetTransform()->GetTransformMatrix();
						meshPair.second.meshRenderers[index]->TransformUpdated();
					}


					if (!meshPair.second.meshRenderers[index]->IsTransparent() && meshPair.second.meshRenderers[index]->GetGameObject()->GetActive())
					{
						if (CameraComponent::GetMainCamera()->IsFrustumCullingOn)
						{
							if (meshPair.first->IsOnFrustum(frustum, meshPair.second.modelTransforms[index]))
							{
								lastAdded = true;
								renderedSegment.count++;
							}
							else
							{
								if (lastAdded)
								{
									lastAdded = false;
									meshPair.second.rendered.push_back(renderedSegment);
									meshPair.second.renderedCount += renderedSegment.count;
									renderedSegment.count = 0u;
								}
								else
								{
									renderedSegment.offset = index + 1u;
								}
							}
						}
						else
						{
							lastAdded = true;
							renderedSegment.count++;
						}
					}
					else
					{
						if (lastAdded)
						{
							lastAdded = false;
							meshPair.second.rendered.push_back(renderedSegment);
							meshPair.second.renderedCount += renderedSegment.count;
							renderedSegment.count = 0u;
						}
						else
						{
							renderedSegment.offset = index + 1u;
						}
					}
				}
				if (renderedSegment.count)
				{
					meshPair.second.rendered.push_back(renderedSegment);
					meshPair.second.renderedCount += renderedSegment.count;
					renderedSegment.count = 0u;
				}

				_depthMapenderQueueDynamic[shaderPair.first][materialPair.first][meshPair.first].rendered = meshPair.second.rendered;
				_depthMapenderQueueDynamic[shaderPair.first][materialPair.first][meshPair.first].renderedCount = meshPair.second.renderedCount;

				_depthQueueDynamic[meshPair.first].renderedCount += meshPair.second.renderedCount;
				for (const auto& element : meshPair.second.rendered)
				{
					_depthQueueDynamic[meshPair.first].rendered.emplace_back(&meshPair.second.modelTransforms, element.offset, element.count);
				}
			}
		}
	}
#pragma endregion

}


#if RENERING_TYPE_MESH_SHADER_MATERIAL

void MeshRenderingManager::Render()
{
	unsigned int globalDrawCount = 0;
	for (auto& meshPair : _renderQueue)
	{
		for (auto& shaderPair : meshPair.second)
		{

			unsigned int count = 0;

			for (auto& material : shaderPair.second)
			{
				count += material.second.size();
			}

			std::vector<glm::mat4> transforms(count);
			std::vector<unsigned int> indexes(count);
			std::vector<InstanceData> instanceData(count);

			unsigned int index = 0;
			unsigned int materialIndex = 0;

			//std::vector<char> materialData(0);

			size_t size = 0;

			for (auto& material : shaderPair.second)
			{
				//const auto& data = material.first.GetMaterialParameters()->GetData();
				const auto& materialParameters = material.first.GetMaterialParameters();

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
				glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
				//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
				glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
				//size += data.size();
				size += materialParameters->GetData();

				while (material.second.size() > 0) {
					auto& renderData = material.second.front();

					transforms[index] = renderData.transform;
					indexes[index] = materialIndex;

					++index;

					material.second.pop();
				}

				materialIndex++;
			}

			shaderPair.first->Use();

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			for (auto& material : shaderPair.second)
			{
				material.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);
			}

			size_t instanceIndex = 0;
			globalDrawCount += count;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#endif

				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}

#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			meshPair.first->Draw(count);

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("Error: {}", error);
			}
		}
	}
	//SPDLOG_WARN("Global draw count: {}", globalDrawCount);
}

#elif RENERING_TYPE_SHADER_MATERIAL_MESH

void MeshRenderingManager::RenderStatic()
{
	unsigned int globalDrawCount = 0;

#if USE_NAMED_BUFFER_SUBDATA
	//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
	glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes);
#else
	//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes);
#endif

#pragma region RENDERING_STATIC_OBJECTS

	for (auto& shaderPair : _renderQueueStatic)
	{
		// Activating Shader Program
		shaderPair.first->Use();

		for (auto& materialPair : shaderPair.second)
		{

#if MATERIAL_INPUT_SINGLE_UBO
			size_t size = 0;
			//const auto& data = materialPair.first.GetMaterialParameters()->GetData();
			const auto& materialParameters = materialPair.first.GetMaterialParameters();
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
			glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
			//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
			glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
#elif MATERIAL_INPUT_MANY_INPUT
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, materialPair.first.GetMaterialParameters()->GetDataUBO());
#endif

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			materialPair.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);

			unsigned int count = 0;

			for (auto& meshPair : materialPair.second)
			{
				if (meshPair.second.renderedCount)
				{
					count = meshPair.second.renderedCount;
					globalDrawCount += count;

					meshPair.second.renderedCount = 0u;

					size_t instanceIndex = 0;
					size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

					RenderedSegment currentSegment{ .offset = 0u, .count = 0u };

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.offset += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = meshPair.second.rendered.front();
							meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, remaining * sizeof(glm::mat4));
								currentSegment.offset += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;

								currentSegment.count = 0u;
							}
						}

#if USE_NAMED_BUFFER_SUBDATA
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms);
						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes);
#else
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
						glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms);

						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
						//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes);
#endif

						meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						count -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}


					instanceIndex = 0ull;
					remaining = count;

					if (currentSegment.count)
					{
						std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = meshPair.second.rendered.front();
						meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}

#if USE_NAMED_BUFFER_SUBDATA
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes);
#else
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
					//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, _materialsIndexes);

					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

					meshPair.first->Draw(count);

					GLenum error = glGetError();
					if (error != GL_NO_ERROR) {
						SPDLOG_ERROR("Error: {}", error);
					}
				}
			}
		}
	}

#pragma endregion

#pragma region RENDERING_DYNAMIC_OBJECTS

	for (auto& shaderPair : _renderQueueDynamic)
	{
		// Activating Shader Program
		shaderPair.first->Use();

		for (auto& materialPair : shaderPair.second)
		{

#if MATERIAL_INPUT_SINGLE_UBO
			size_t size = 0;
			//const auto& data = materialPair.first.GetMaterialParameters()->GetData();
			const auto& materialParameters = materialPair.first.GetMaterialParameters();
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
			glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
			//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
			glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
#elif MATERIAL_INPUT_MANY_INPUT
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, materialPair.first.GetMaterialParameters()->GetDataUBO());
#endif

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			materialPair.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);

			unsigned int count = 0;

			for (auto& meshPair : materialPair.second)
			{
				if (meshPair.second.renderedCount)
				{
					count = meshPair.second.renderedCount;
					globalDrawCount += count;

					meshPair.second.renderedCount = 0u;

					size_t instanceIndex = 0;
					size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

					RenderedSegment currentSegment{ .offset = 0u, .count = 0u };

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.offset += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = meshPair.second.rendered.front();
							meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, remaining * sizeof(glm::mat4));
								currentSegment.offset += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;

								currentSegment.count = 0u;
							}
						}

#if USE_NAMED_BUFFER_SUBDATA
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms);
						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes);
#else
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
						glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms);

						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
						//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes);
#endif

						meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						count -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}


					instanceIndex = 0ull;
					remaining = count;

					if (currentSegment.count)
					{
						std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = meshPair.second.rendered.front();
						meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}

#if USE_NAMED_BUFFER_SUBDATA
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes);
#else
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
					//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, _materialsIndexes);

					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

					meshPair.first->Draw(count);

					GLenum error = glGetError();
					if (error != GL_NO_ERROR) {
						SPDLOG_ERROR("Error: {}", error);
					}
				}
			}
		}
	}

#pragma endregion

	//SPDLOG_WARN("Global static draw count: {}", globalDrawCount);
}
void MeshRenderingManager::Render()
{
	unsigned int globalDrawCount = 0;
	for (auto& shaderPair : _renderQueue)
	{
		// Activating Shader Program
		shaderPair.first->Use();

		for (auto& materialPair : shaderPair.second)
		{

#if MATERIAL_INPUT_SINGLE_UBO
			size_t size = 0;
			//const auto& data = materialPair.first.GetMaterialParameters()->GetData();
			const auto& materialParameters = materialPair.first.GetMaterialParameters();
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
			glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
			//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
			glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
#elif MATERIAL_INPUT_MANY_INPUT
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, materialPair.first.GetMaterialParameters()->GetDataUBO());
#endif

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			materialPair.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);

			unsigned int count = 0;

			for (auto& meshPair : materialPair.second)
			{
				count = meshPair.second.size();
				std::vector<glm::mat4> transforms(count);
				std::vector<unsigned int> indexes(count);
				std::vector<InstanceData> instanceData(count);

				size_t index = 0;
				while (meshPair.second.size() > 0) {
					auto& renderData = meshPair.second.front();

					transforms[index] = renderData.transform;
					indexes[index] = 0;

					++index;

					meshPair.second.pop();
				}

				size_t instanceIndex = 0;
				globalDrawCount += count;

				while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
				{
#if USE_NAMED_BUFFER_SUBDATA
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#else
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#endif

					meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

					instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
					count -= MAX_INSTANCE_NUMBER_PER_DRAW;
				}

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);

				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

				meshPair.first->Draw(count);

				GLenum error = glGetError();
				if (error != GL_NO_ERROR) {
					SPDLOG_ERROR("Error: {}", error);
				}
			}
		}
	}
	//SPDLOG_WARN("Global draw count: {}", globalDrawCount);
}

#elif RENERING_TYPE_SHADER_MESH_MATERIAL

void MeshRenderingManager::Render()
{
	unsigned int globalDrawCount = 0;
	for (auto& shaderPair : _renderQueue)
	{
		shaderPair.first->Use();
		for (auto& meshPair : shaderPair.second)
		{

			unsigned int count = 0;

			for (auto& material : meshPair.second)
			{
				count += material.second.size();
			}

			std::vector<glm::mat4> _modelTransforms(count);
			std::vector<unsigned int> _materialsIndexes(count);
			std::vector<InstanceData> instanceData(count);

			unsigned int index = 0;
			unsigned int materialIndex = 0;

			//std::vector<char> materialData(0);

			size_t size = 0;

			for (auto& material : meshPair.second)
			{
				//const auto& data = material.first.GetMaterialParameters()->GetData();
				const auto& materialParameters = material.first.GetMaterialParameters();

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
				glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
				//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
				glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
				//size += data.size();
				size += materialParameters->GetData();

				while (material.second.size() > 0) {
					auto& renderData = material.second.front();

					_modelTransforms[index] = renderData.transform;
					_materialsIndexes[index] = materialIndex;

					++index;

					material.second.pop();
				}

				materialIndex++;
			}


			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			for (auto& material : meshPair.second)
			{
				material.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);
			}

			size_t instanceIndex = 0;
			globalDrawCount += count;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms.data() + instanceIndex);
				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes.data() + instanceIndex);
#endif

				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}

#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, _modelTransforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, _materialsIndexes.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, _materialsIndexes.data() + instanceIndex);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			meshPair.first->Draw(count);

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("Error: {}", error);
			}
		}
	}
	//SPDLOG_WARN("Global draw count: {}", globalDrawCount);
}
#endif

#if RENERING_TYPE_MESH_SHADER_MATERIAL

void MeshRenderingManager::RenderDepthMap()
{
	for (auto& meshPair : _depthMapRenderQueue)
	{
		for (auto& shaderPair : meshPair.second)
		{

			unsigned int count = 0;

			for (auto& material : shaderPair.second)
			{
				count += material.second.size();
			}

			std::vector<glm::mat4> transforms(count);
			std::vector<unsigned int> indexes(count);
			std::vector<InstanceData> instanceData(count);

			unsigned int index = 0;
			unsigned int materialIndex = 0;

			//std::vector<char> materialData(0);

			size_t size = 0;

			for (auto& material : shaderPair.second)
			{
				//const auto& data = material.first.GetMaterialParameters()->GetData();
				const auto& materialParameters = material.first.GetMaterialParameters();

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
				glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
				//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
				glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
				//size += data.size();
				size += materialParameters->GetData();

				while (material.second.size() > 0) {
					auto& renderData = material.second.front();

					transforms[index] = renderData.transform;
					indexes[index] = materialIndex;

					++index;

					material.second.pop();
				}

				materialIndex++;
			}

			shaderPair.first->Use();

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			for (auto& material : shaderPair.second)
			{
				material.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);
			}

			size_t instanceIndex = 0;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#endif

				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}

#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			meshPair.first->Draw(count);

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("RDMError: {}", error);
			}
		}
	}
}

#elif RENERING_TYPE_SHADER_MATERIAL_MESH

void MeshRenderingManager::RenderDepthMapStatic()
{
	unsigned int globalDrawCount = 0;

#if USE_NAMED_BUFFER_SUBDATA
	//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
	glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes);
#else
	//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes);
#endif

#pragma region RENDERING_STATIC_DEPTH_MAP

	for (auto& shaderPair : _depthMapenderQueueStatic)
	{
		// Activating Shader Program
		shaderPair.first->Use();

		for (auto& materialPair : shaderPair.second)
		{

#if MATERIAL_INPUT_SINGLE_UBO
			size_t size = 0;
			//const auto& data = materialPair.first.GetMaterialParameters()->GetData();
			const auto& materialParameters = materialPair.first.GetMaterialParameters();
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
			glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
			//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
			glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
#elif MATERIAL_INPUT_MANY_INPUT
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, materialPair.first.GetMaterialParameters()->GetDataUBO());
#endif

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			materialPair.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);

			unsigned int count = 0;

			for (auto& meshPair : materialPair.second)
			{
				if (meshPair.second.renderedCount)
				{
					count = meshPair.second.renderedCount;
					globalDrawCount += count;

					meshPair.second.renderedCount = 0u;

					size_t instanceIndex = 0;
					size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

					RenderedSegment currentSegment{ .offset = 0u, .count = 0u };

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.offset += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = meshPair.second.rendered.front();
							meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, remaining * sizeof(glm::mat4));
								currentSegment.offset += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;

								currentSegment.count = 0u;
							}
						}

#if USE_NAMED_BUFFER_SUBDATA
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms);
						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes);
#else
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
						glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms);

						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
						//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes);
#endif

						meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						count -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}


					instanceIndex = 0ull;
					remaining = count;

					if (currentSegment.count)
					{
						std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = meshPair.second.rendered.front();
						meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}

#if USE_NAMED_BUFFER_SUBDATA
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes);
#else
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
					//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, _materialsIndexes);

					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

					meshPair.first->Draw(count);

					GLenum error = glGetError();
					if (error != GL_NO_ERROR) {
						SPDLOG_ERROR("Error: {}", error);
					}
				}
			}
		}
	}

#pragma endregion

#pragma region RENDERING_DYNAMIC_DEPTH_MAP

	for (auto& shaderPair : _depthMapenderQueueDynamic)
	{
		// Activating Shader Program
		shaderPair.first->Use();

		for (auto& materialPair : shaderPair.second)
		{

#if MATERIAL_INPUT_SINGLE_UBO
			size_t size = 0;
			//const auto& data = materialPair.first.GetMaterialParameters()->GetData();
			const auto& materialParameters = materialPair.first.GetMaterialParameters();
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
			glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
			//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
			glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
#elif MATERIAL_INPUT_MANY_INPUT
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, materialPair.first.GetMaterialParameters()->GetDataUBO());
#endif

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			materialPair.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);

			unsigned int count = 0;

			for (auto& meshPair : materialPair.second)
			{
				if (meshPair.second.renderedCount)
				{
					count = meshPair.second.renderedCount;
					globalDrawCount += count;

					meshPair.second.renderedCount = 0u;

					size_t instanceIndex = 0;
					size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

					RenderedSegment currentSegment{ .offset = 0u, .count = 0u };

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.offset += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = meshPair.second.rendered.front();
							meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, remaining * sizeof(glm::mat4));
								currentSegment.offset += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;

								currentSegment.count = 0u;
							}
						}

#if USE_NAMED_BUFFER_SUBDATA
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms);
						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes);
#else
						//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
						glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
						glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms);

						//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
						//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
						//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes);
#endif

						meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						count -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}


					instanceIndex = 0ull;
					remaining = count;

					if (currentSegment.count)
					{
						std::memcpy(_modelTransforms, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = meshPair.second.rendered.front();
						meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, meshPair.second.modelTransforms.data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}

#if USE_NAMED_BUFFER_SUBDATA
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes);
#else
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					//glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
					//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, _materialsIndexes);

					glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

					meshPair.first->Draw(count);

					GLenum error = glGetError();
					if (error != GL_NO_ERROR) {
						SPDLOG_ERROR("Error: {}", error);
					}
				}
			}
		}
	}

#pragma endregion

	//SPDLOG_WARN("Global static draw count: {}", globalDrawCount);
}
void MeshRenderingManager::RenderDepthMap()
{
	for (auto& shaderPair : _depthMapRenderQueue)
	{
		// Activating Shader Program
		shaderPair.first->Use();

		for (auto& materialPair : shaderPair.second)
		{

#if MATERIAL_INPUT_SINGLE_UBO
			size_t size = 0;
			//const auto& data = materialPair.first.GetMaterialParameters()->GetData();
			const auto& materialParameters = materialPair.first.GetMaterialParameters();
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
			glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
			//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
			glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
			//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
			glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
#elif MATERIAL_INPUT_MANY_INPUT
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, materialPair.first.GetMaterialParameters()->GetDataUBO());
#endif

			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			materialPair.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);

			unsigned int count = 0;

			for (auto& meshPair : materialPair.second)
			{
				count = meshPair.second.size();
				std::vector<glm::mat4> transforms(count);
				std::vector<unsigned int> indexes(count);
				std::vector<InstanceData> instanceData(count);

				size_t index = 0;
				while (meshPair.second.size() > 0) {
					auto& renderData = meshPair.second.front();

					transforms[index] = renderData.transform;
					indexes[index] = 0;

					++index;

					meshPair.second.pop();
				}

				size_t instanceIndex = 0;

				while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
				{
#if USE_NAMED_BUFFER_SUBDATA
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#else
					//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);

					//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, indexes.data() + instanceIndex);
#endif

					meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

					instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
					count -= MAX_INSTANCE_NUMBER_PER_DRAW;
				}

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, transforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, indexes.data() + instanceIndex);

				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

				meshPair.first->Draw(count);

				GLenum error = glGetError();
				if (error != GL_NO_ERROR) {
					SPDLOG_ERROR("RDMError: {}", error);
				}
			}
		}
	}
	//SPDLOG_WARN("Global draw count: {}", globalDrawCount);
}

#elif RENERING_TYPE_SHADER_MESH_MATERIAL

void MeshRenderingManager::RenderDepthMap()
{
	for (auto& shaderPair : _depthMapRenderQueue)
	{
		shaderPair.first->Use();
		for (auto& meshPair : shaderPair.second)
		{

			unsigned int count = 0;

			for (auto& material : meshPair.second)
			{
				count += material.second.size();
			}

			std::vector<glm::mat4> _modelTransforms(count);
			std::vector<unsigned int> _materialsIndexes(count);
			std::vector<InstanceData> instanceData(count);

			unsigned int index = 0;
			unsigned int materialIndex = 0;

			//std::vector<char> materialData(0);

			size_t size = 0;

			for (auto& material : meshPair.second)
			{
				//const auto& data = material.first.GetMaterialParameters()->GetData();
				const auto& materialParameters = material.first.GetMaterialParameters();

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				//glNamedBufferSubData(_materialInputUBO, size, data.size(), data.data());
				glNamedBufferSubData(_materialInputUBO, size, materialParameters->GetSize(), materialParameters->GetData());
#else
				//ASSIGNING UBO ASSOCIATED WITH MATERIAL INPUT
				glBindBuffer(GL_UNIFORM_BUFFER, _materialInputUBO);
				//glBufferSubData(GL_UNIFORM_BUFFER, size, data.size(), data.data());
				glBufferSubData(GL_UNIFORM_BUFFER, size, materialParameters->GetSize(), materialParameters->GetData());
#endif
				//size += data.size();
				size += materialParameters->GetData();

				while (material.second.size() > 0) {
					auto& renderData = material.second.front();

					_modelTransforms[index] = renderData.transform;
					_materialsIndexes[index] = materialIndex;

					++index;

					material.second.pop();
				}

				materialIndex++;
			}


			// ASSIGNING TEXTURES
			int beginLocation = 0;
			int textureBind = 0;
			for (auto& material : meshPair.second)
			{
				material.first.GetMaterialParameters()->UploadTextures2D(shaderPair.first->GetProgramId(), beginLocation, textureBind);
			}

			size_t instanceIndex = 0;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms.data() + instanceIndex);
				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes.data() + instanceIndex);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms.data() + instanceIndex);

				//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * MAX_INSTANCE_NUMBER_PER_DRAW, _materialsIndexes.data() + instanceIndex);
#endif

				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}

#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, _modelTransforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glNamedBufferSubData(_materialIndexSSBO, 0, sizeof(unsigned int) * count, _materialsIndexes.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms.data() + instanceIndex);

			//ASSIGNING SSBO ASSOCIATED WITH MATERIAL INDEX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialIndexSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * count, _materialsIndexes.data() + instanceIndex);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			meshPair.first->Draw(count);

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("RDMError: {}", error);
			}
		}
	}
	//SPDLOG_WARN("Global draw count: {}", globalDrawCount);
}
#endif



void MeshRenderingManager::RenderDepthMap(const unsigned int& bufferWidth, const unsigned int& bufferHeight, const GLuint& depthFBO, const GLuint& depthMapTex,
	glm::mat4& projectionViewMatrix)
{
	ShaderManager::DepthShader->Use();
	ShaderManager::DepthShader->SetMat4("lightSpaceMatrix", projectionViewMatrix);

	glViewport(0, 0, bufferWidth, bufferHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	//glBindTexture(GL_TEXTURE_2D, depthMapTex);
	glClear(GL_DEPTH_BUFFER_BIT);

	for (auto& meshPair : _depthQueue)
	{
		unsigned int index = 0;

		std::vector<glm::mat4> transforms(meshPair.second.size());

		while (meshPair.second.size() > 0)
		{
			auto& renderData = meshPair.second.front();

			transforms[index] = renderData.transform;

			++index;

			meshPair.second.pop();
		}

		size_t instanceIndex = 0;

		while (index > MAX_INSTANCE_NUMBER_PER_DRAW)
		{
#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms.data() + instanceIndex);
#endif
			meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

			instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
			index -= MAX_INSTANCE_NUMBER_PER_DRAW;
		}
#if USE_NAMED_BUFFER_SUBDATA
		//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
		glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * index, transforms.data() + instanceIndex);
#else
		//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * index, transforms.data() + instanceIndex);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

		meshPair.first->Draw(index);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glm::ivec2 wSize = Twin2Engine::GraphicEngine::Window::GetInstance()->GetContentSize();
	glViewport(0, 0, wSize.x, wSize.y);
}

void MeshRenderingManager::RenderDepthMapStatic(const unsigned int& bufferWidth, const unsigned int& bufferHeight, const GLuint& depthFBO, const GLuint& depthMapTex,
	glm::mat4& projectionViewMatrix)
{
	ShaderManager::DepthShader->Use();
	ShaderManager::DepthShader->SetMat4("lightSpaceMatrix", projectionViewMatrix);

	glViewport(0, 0, bufferWidth, bufferHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	//glBindTexture(GL_TEXTURE_2D, depthMapTex);
	glClear(GL_DEPTH_BUFFER_BIT);

	unsigned int count = 0;
	RenderedSegmentDepthMap currentSegment{ .offset = 0u, .count = 0u };

#pragma region RENDERING_STATIC_DEPTH_MAP

	for (auto& meshPair : _depthQueueStatic)
	{
		if (meshPair.second.renderedCount)
		{
			count = meshPair.second.renderedCount;

			meshPair.second.renderedCount = 0u;

			size_t instanceIndex = 0;
			size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

			currentSegment.offset = 0u;
			currentSegment.count = 0u;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				instanceIndex = 0ull;
				remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

				if (currentSegment.count)
				{
					if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						std::memcpy(_modelTransforms, currentSegment.modelTransforms->data() + currentSegment.offset, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
						currentSegment.offset += MAX_INSTANCE_NUMBER_PER_DRAW;
						currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}
					else
					{
						std::memcpy(_modelTransforms, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
				}
				while (remaining > 0)
				{
					currentSegment = meshPair.second.rendered.front();
					meshPair.second.rendered.pop_front();

					if (currentSegment.count > remaining)
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.modelTransforms->data() + currentSegment.offset, remaining * sizeof(glm::mat4));
						currentSegment.offset += remaining;
						currentSegment.count -= remaining;

						instanceIndex += remaining;
						remaining = 0ull;
					}
					else
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;

						currentSegment.count = 0u;
					}
				}

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms);
#endif

				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}


			instanceIndex = 0ull;
			remaining = count;

			if (currentSegment.count)
			{
				std::memcpy(_modelTransforms, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}
			while (remaining > 0)
			{
				currentSegment = meshPair.second.rendered.front();
				meshPair.second.rendered.pop_front();

				std::memcpy(_modelTransforms + instanceIndex, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}

#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			meshPair.first->Draw(count);

			meshPair.second.rendered.clear();
		}
	}

#pragma endregion

#pragma region RENDERING_DYNAMIC_DEPTH_MAP

	for (auto& meshPair : _depthQueueDynamic)
	{
		if (meshPair.second.renderedCount)
		{
			count = meshPair.second.renderedCount;

			meshPair.second.renderedCount = 0u;

			size_t instanceIndex = 0;
			size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

			currentSegment.offset = 0u;
			currentSegment.count = 0u;

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				instanceIndex = 0ull;
				remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

				if (currentSegment.count)
				{
					if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						std::memcpy(_modelTransforms, currentSegment.modelTransforms->data() + currentSegment.offset, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
						currentSegment.offset += MAX_INSTANCE_NUMBER_PER_DRAW;
						currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}
					else
					{
						std::memcpy(_modelTransforms, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
				}
				while (remaining > 0)
				{
					currentSegment = meshPair.second.rendered.front();
					meshPair.second.rendered.pop_front();

					if (currentSegment.count > remaining)
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.modelTransforms->data() + currentSegment.offset, remaining * sizeof(glm::mat4));
						currentSegment.offset += remaining;
						currentSegment.count -= remaining;

						instanceIndex += remaining;
						remaining = 0ull;
					}
					else
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;

						currentSegment.count = 0u;
					}
				}

#if USE_NAMED_BUFFER_SUBDATA
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, transforms);
#else
				//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * MAX_INSTANCE_NUMBER_PER_DRAW, _modelTransforms);
#endif

				meshPair.first->Draw(MAX_INSTANCE_NUMBER_PER_DRAW);

				instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
				count -= MAX_INSTANCE_NUMBER_PER_DRAW;
			}


			instanceIndex = 0ull;
			remaining = count;

			if (currentSegment.count)
			{
				std::memcpy(_modelTransforms, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}
			while (remaining > 0)
			{
				currentSegment = meshPair.second.rendered.front();
				meshPair.second.rendered.pop_front();

				std::memcpy(_modelTransforms + instanceIndex, currentSegment.modelTransforms->data() + currentSegment.offset, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}

#if USE_NAMED_BUFFER_SUBDATA
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glNamedBufferSubData(_instanceDataSSBO, 0, sizeof(glm::mat4) * count, transforms);
#else
			//ASSIGNING SSBO ASSOCIATED WITH TRANSFORM MATRIX
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _instanceDataSSBO);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * count, _modelTransforms);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif

			meshPair.first->Draw(count);

			meshPair.second.rendered.clear();
		}
	}

#pragma endregion

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glm::ivec2 wSize = Twin2Engine::GraphicEngine::Window::GetInstance()->GetContentSize();
	glViewport(0, 0, wSize.x, wSize.y);
}