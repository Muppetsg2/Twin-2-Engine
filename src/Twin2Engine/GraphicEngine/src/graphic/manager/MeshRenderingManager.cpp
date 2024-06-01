#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/InstantiatingMesh.h>
#include <graphic/Window.h>
#include <graphic/LightingController.h>

using namespace Twin2Engine::Graphic;
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

std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_renderQueueStatic = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();
std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_renderQueueStaticTransparent = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();

std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap> MeshRenderingManager::_depthMapQueueStatic = std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap>();
std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap> MeshRenderingManager::_depthQueueStatic = std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap>();

std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_renderQueueDynamic = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();
std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingData>>> MeshRenderingManager::_renderQueueDynamicTransparent = std::unordered_map<Shader*, std::map<Material, std::unordered_map<InstantiatingMesh*, MeshRenderingData>>>();

std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap> MeshRenderingManager::_depthMapQueueDynamic = std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap>();
std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap> MeshRenderingManager::_depthQueueDynamic = std::unordered_map<InstantiatingMesh*, MeshRenderingManager::MeshRenderingDataDepthMap>();


GLuint MeshRenderingManager::_instanceDataSSBO = 0u;
GLuint MeshRenderingManager::_materialIndexSSBO = 0u;
GLuint MeshRenderingManager::_materialInputUBO = 0u;

MeshRenderingManager::Flags MeshRenderingManager::_flags { .IsStaticChanged = false };

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

	_renderQueueStatic.clear();
	_depthMapQueueStatic.clear();
	_depthQueueStatic.clear();
	_renderQueueDynamic.clear();
	_depthMapQueueDynamic.clear();
	_depthQueueDynamic.clear();
}

bool MeshRenderingManager::RegisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		InstantiatingMesh* mesh;
		if (meshRenderer->IsTransparent())
		{
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& queueElement = _renderQueueStaticTransparent[material.GetShader()][material][mesh];
				queueElement.meshRenderers.push_back(meshRenderer);
				queueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());
			}
		}
		else
		{
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& queueElement = _renderQueueStatic[material.GetShader()][material][mesh];
				queueElement.meshRenderers.push_back(meshRenderer);
				queueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());
			}
		}

		_flags.IsStaticChanged = true;
		return true;
	}

	return false;
}

bool MeshRenderingManager::UnregisterStatic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		// MAMY WSZYSTKIE DANE WIEC PRZECHODZIMY PO KAZDYM MESHU I GO WYREJESTROWUJEMY
		InstantiatingMesh* mesh;
		if (meshRenderer->IsTransparent())
		{
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& meshRenderingData = _renderQueueStaticTransparent[material.GetShader()][material][mesh];

				size_t pos = 0;
				for (size_t z = 0; z < meshRenderingData.meshRenderers.size(); ++z)
				{
					if (meshRenderingData.meshRenderers[z] == meshRenderer)
					{
						pos = z;
						break;
					}
				}

				if (meshRenderingData.meshRenderers.size() != 0) {
					meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + pos);
				}
				if (meshRenderingData.modelTransforms.size() != 0) {
					meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + pos);
				}

				if (meshRenderingData.meshRenderers.size() == 0) {
					_renderQueueStaticTransparent[material.GetShader()][material].erase(mesh);

					if (_renderQueueStaticTransparent[material.GetShader()][material].size() == 0) {
						_renderQueueStaticTransparent[material.GetShader()].erase(material);

						if (_renderQueueStaticTransparent[material.GetShader()].size() == 0) {
							_renderQueueStaticTransparent.erase(material.GetShader());
						}
					}
				}
			}
		}
		else
		{
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& meshRenderingData = _renderQueueStatic[material.GetShader()][material][mesh];

				size_t pos = 0;
				for (size_t z = 0; z < meshRenderingData.meshRenderers.size(); ++z)
				{
					if (meshRenderingData.meshRenderers[z] == meshRenderer)
					{
						pos = z;
						break;
					}
				}

				if (meshRenderingData.meshRenderers.size() != 0) {
					meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + pos);
				}
				if (meshRenderingData.modelTransforms.size() != 0) {
					meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + pos);
				}

				if (meshRenderingData.meshRenderers.size() == 0) {
					_renderQueueStatic[material.GetShader()][material].erase(mesh);

					if (_renderQueueStatic[material.GetShader()][material].size() == 0) {
						_renderQueueStatic[material.GetShader()].erase(material);

						if (_renderQueueStatic[material.GetShader()].size() == 0) {
							_renderQueueStatic.erase(material.GetShader());
						}
					}
				}
			}
		}

		_flags.IsStaticChanged = true;
		return true;
	}
	else if (meshRenderer->GetModel() == nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		// NIE MAMY MESHY, ALE WIEMY, ZE KAZDY MESH W MODELU MUSI MIEC JEDEN Z MATERIALOW. 
		// PRZECHODZIMY WIEC PO MESACH W KAZDYM MATERIALE I SPRAWDZAMY CZY POSIADA KTORYS MESH NASZ MESH_RENDERER
		std::vector<DataToUnregister> pos = std::vector<DataToUnregister>();

		for (size_t i = 0; i < meshRenderer->GetMaterialCount(); ++i) {

			Material material = meshRenderer->GetMaterial(i);
			auto& materialPairs = _renderQueueStatic[material.GetShader()][material];

			for (auto& meshPair : materialPairs) {

				for (size_t z = 0; z < materialPairs[meshPair.first].meshRenderers.size(); ++z)
				{
					if (materialPairs[meshPair.first].meshRenderers[z] == meshRenderer)
					{
						pos.push_back(DataToUnregister{
							._mat = material,
							._mesh = meshPair.first,
							._pos = z
							});
						break;
					}
				}
			}
		}

		for (auto& p : pos) {
			auto& meshRenderingData = _renderQueueStatic[p._mat.GetShader()][p._mat][p._mesh];

			if (meshRenderingData.meshRenderers.size() != 0) {
				meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + p._pos);
			}
			if (meshRenderingData.modelTransforms.size() != 0) {
				meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + p._pos);
			}

			if (meshRenderingData.meshRenderers.size() == 0) {
				_renderQueueStatic[p._mat.GetShader()][p._mat].erase(p._mesh);

				if (_renderQueueStatic[p._mat.GetShader()][p._mat].size() == 0) {
					_renderQueueStatic[p._mat.GetShader()].erase(p._mat);

					if (_renderQueueStatic[p._mat.GetShader()].size() == 0) {
						_renderQueueStatic.erase(p._mat.GetShader());
					}
				}
			}
		}

		pos.clear();

		_flags.IsStaticChanged = true;
		return true;
	}
	else if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() == 0)
	{
		// POSIADAMY MODEL, ALE NIE POSIADAMY MATERIALOW. WIEC PRZECHODZIMY PO WSZYSTKICH MATERIALACH I SPRAWDZAMY CZY JEST W KTORYMS Z MATERIALOW NASZ MESH.
		// JESLI SIE ZNAJDUJE TO GO USUWAMY I SZUKAMY DLA KOLEJNEGO MESHA
		InstantiatingMesh* mesh;
		for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
			mesh = meshRenderer->GetMesh(i);

			DataToUnregister pos;
			bool found = false;

			for (auto& shaderPair : _renderQueueStatic) {

				for (auto& materialPair : shaderPair.second) {

					if (materialPair.second.contains(mesh)) {

						for (size_t z = 0; z < materialPair.second[mesh].meshRenderers.size(); ++z)
						{
							if (materialPair.second[mesh].meshRenderers[z] == meshRenderer)
							{
								pos = DataToUnregister{
									._mat = materialPair.first,
									._mesh = mesh,
									._pos = z
								};
								found = true;
								break;
							}
						}
					}

					if (found) break;
				}

				if (found) break;
			}

			auto& meshRenderingData = _renderQueueStatic[pos._mat.GetShader()][pos._mat][pos._mesh];

			if (meshRenderingData.meshRenderers.size() != 0) {
				meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + pos._pos);
			}
			if (meshRenderingData.modelTransforms.size() != 0) {
				meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + pos._pos);
			}

			if (meshRenderingData.meshRenderers.size() == 0) {
				_renderQueueStatic[pos._mat.GetShader()][pos._mat].erase(pos._mesh);

				if (_renderQueueStatic[pos._mat.GetShader()][pos._mat].size() == 0) {
					_renderQueueStatic[pos._mat.GetShader()].erase(pos._mat);

					if (_renderQueueStatic[pos._mat.GetShader()].size() == 0) {
						_renderQueueStatic.erase(pos._mat.GetShader());
					}
				}
			}
		}

		_flags.IsStaticChanged = true;
		return true;
	}
	else {

		// NICZEGO NIE ZNAMY WIEC SZUKAMY WSZEDZIE NASZEGO MESH_RENDERERA
		std::vector<DataToUnregister> pos = std::vector<DataToUnregister>();

		for (auto& shaderPair : _renderQueueStatic) {

			for (auto& materialPair : shaderPair.second) {

				for (auto& meshPair : materialPair.second) {

					for (size_t z = 0; z < materialPair.second[meshPair.first].meshRenderers.size(); ++z)
					{
						if (materialPair.second[meshPair.first].meshRenderers[z] == meshRenderer)
						{
							pos.push_back(DataToUnregister{
								._mat = materialPair.first,
								._mesh = meshPair.first,
								._pos = z
								});
							break;
						}
					}
				}
			}
		}

		for (auto& p : pos) {
			auto& meshRenderingData = _renderQueueStatic[p._mat.GetShader()][p._mat][p._mesh];

			if (meshRenderingData.meshRenderers.size() != 0) {
				meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + p._pos);
			}
			if (meshRenderingData.modelTransforms.size() != 0) {
				meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + p._pos);
			}

			if (meshRenderingData.meshRenderers.size() == 0) {
				_renderQueueStatic[p._mat.GetShader()][p._mat].erase(p._mesh);

				if (_renderQueueStatic[p._mat.GetShader()][p._mat].size() == 0) {
					_renderQueueStatic[p._mat.GetShader()].erase(p._mat);

					if (_renderQueueStatic[p._mat.GetShader()].size() == 0) {
						_renderQueueStatic.erase(p._mat.GetShader());
					}
				}
			}
		}

		pos.clear();

		_flags.IsStaticChanged = true;
		return true;
	}

	return false;
}

bool MeshRenderingManager::RegisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		InstantiatingMesh* mesh;
		if (meshRenderer->IsTransparent())
		{
			//SPDLOG_DEBUG("Dodanie transparent");
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& queueElement = _renderQueueDynamicTransparent[material.GetShader()][material][mesh];
				queueElement.meshRenderers.push_back(meshRenderer);
				queueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());
			}
		}
		else
		{
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& queueElement = _renderQueueDynamic[material.GetShader()][material][mesh];
				queueElement.meshRenderers.push_back(meshRenderer);
				queueElement.modelTransforms.push_back(meshRenderer->GetTransform()->GetTransformMatrix());
			}
		}
		return true;
	}

	return false;
}

bool MeshRenderingManager::UnregisterDynamic(Twin2Engine::Core::MeshRenderer* meshRenderer)
{
	if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() != 0)
	{
		// MAMY WSZYSTKIE DANE WIEC PRZECHODZIMY PO KAZDYM MESHU I GO WYREJESTROWUJEMY
		InstantiatingMesh* mesh;
		if (meshRenderer->IsTransparent())
		{
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& meshRenderingData = _renderQueueDynamicTransparent[material.GetShader()][material][mesh];

				size_t pos = 0;
				for (size_t z = 0; z < meshRenderingData.meshRenderers.size(); ++z)
				{
					if (meshRenderingData.meshRenderers[z] == meshRenderer)
					{
						pos = z;
						break;
					}
				}

				if (meshRenderingData.meshRenderers.size() != 0) {
					meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + pos);
				}
				if (meshRenderingData.modelTransforms.size() != 0) {
					meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + pos);
				}

				if (meshRenderingData.meshRenderers.size() == 0) {
					_renderQueueDynamicTransparent[material.GetShader()][material].erase(mesh);

					if (_renderQueueDynamicTransparent[material.GetShader()][material].size() == 0) {
						_renderQueueDynamicTransparent[material.GetShader()].erase(material);

						if (_renderQueueDynamicTransparent[material.GetShader()].size() == 0) {
							_renderQueueDynamicTransparent.erase(material.GetShader());
						}
					}
				}
			}
		}
		else
		{
			for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
				mesh = meshRenderer->GetMesh(i);
				Material material = meshRenderer->GetMaterial(i);

				auto& meshRenderingData = _renderQueueDynamic[material.GetShader()][material][mesh];

				size_t pos = 0;
				for (size_t z = 0; z < meshRenderingData.meshRenderers.size(); ++z)
				{
					if (meshRenderingData.meshRenderers[z] == meshRenderer)
					{
						pos = z;
						break;
					}
				}

				if (meshRenderingData.meshRenderers.size() != 0) {
					meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + pos);
				}
				if (meshRenderingData.modelTransforms.size() != 0) {
					meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + pos);
				}

				if (meshRenderingData.meshRenderers.size() == 0) {
					_renderQueueDynamic[material.GetShader()][material].erase(mesh);

					if (_renderQueueDynamic[material.GetShader()][material].size() == 0) {
						_renderQueueDynamic[material.GetShader()].erase(material);

						if (_renderQueueDynamic[material.GetShader()].size() == 0) {
							_renderQueueDynamic.erase(material.GetShader());
						}
					}
				}
			}

		}
		return true;
	}
	else if (meshRenderer->GetModel() == nullptr && meshRenderer->GetMaterialCount() != 0) 
	{
		// NIE MAMY MESHY, ALE WIEMY, ZE KAZDY MESH W MODELU MUSI MIEC JEDEN Z MATERIALOW. 
		// PRZECHODZIMY WIEC PO MESACH W KAZDYM MATERIALE I SPRAWDZAMY CZY POSIADA KTORYS MESH NASZ MESH_RENDERER
		std::vector<DataToUnregister> pos = std::vector<DataToUnregister>();

		for (size_t i = 0; i < meshRenderer->GetMaterialCount(); ++i) {

			Material material = meshRenderer->GetMaterial(i);
			auto& materialPairs = _renderQueueDynamic[material.GetShader()][material];

			for (auto& meshPair : materialPairs) {

				for (size_t z = 0; z < materialPairs[meshPair.first].meshRenderers.size(); ++z)
				{
					if (materialPairs[meshPair.first].meshRenderers[z] == meshRenderer)
					{
						pos.push_back(DataToUnregister{
							._mat = material,
							._mesh = meshPair.first,
							._pos = z
						});
						break;
					}
				}
			}
		}

		for (auto& p : pos) {
			auto& meshRenderingData = _renderQueueDynamic[p._mat.GetShader()][p._mat][p._mesh];

			if (meshRenderingData.meshRenderers.size() != 0) {
				meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + p._pos);
			}
			if (meshRenderingData.modelTransforms.size() != 0) {
				meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + p._pos);
			}

			if (meshRenderingData.meshRenderers.size() == 0) {
				_renderQueueDynamic[p._mat.GetShader()][p._mat].erase(p._mesh);

				if (_renderQueueDynamic[p._mat.GetShader()][p._mat].size() == 0) {
					_renderQueueDynamic[p._mat.GetShader()].erase(p._mat);

					if (_renderQueueDynamic[p._mat.GetShader()].size() == 0) {
						_renderQueueDynamic.erase(p._mat.GetShader());
					}
				}
			}
		}

		pos.clear();
		return true;
	}
	else if (meshRenderer->GetModel() != nullptr && meshRenderer->GetMaterialCount() == 0) 
	{
		// POSIADAMY MODEL, ALE NIE POSIADAMY MATERIALOW. WIEC PRZECHODZIMY PO WSZYSTKICH MATERIALACH I SPRAWDZAMY CZY JEST W KTORYMS Z MATERIALOW NASZ MESH.
		// JESLI SIE ZNAJDUJE TO GO USUWAMY I SZUKAMY DLA KOLEJNEGO MESHA
		InstantiatingMesh* mesh;
		for (size_t i = 0; i < meshRenderer->GetMeshCount(); ++i) {
			mesh = meshRenderer->GetMesh(i);

			DataToUnregister pos;
			bool found = false;

			for (auto& shaderPair : _renderQueueDynamic) {
				
				for (auto& materialPair : shaderPair.second) {

					if (materialPair.second.contains(mesh)) {

						for (size_t z = 0; z < materialPair.second[mesh].meshRenderers.size(); ++z)
						{
							if (materialPair.second[mesh].meshRenderers[z] == meshRenderer)
							{
								pos = DataToUnregister{
									._mat = materialPair.first,
									._mesh = mesh,
									._pos = z
								};
								found = true;
								break;
							}
						}
					}

					if (found) break;
				}

				if (found) break;
			}

			auto& meshRenderingData = _renderQueueDynamic[pos._mat.GetShader()][pos._mat][pos._mesh];

			if (meshRenderingData.meshRenderers.size() != 0) {
				meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + pos._pos);
			}
			if (meshRenderingData.modelTransforms.size() != 0) {
				meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + pos._pos);
			}

			if (meshRenderingData.meshRenderers.size() == 0) {
				_renderQueueDynamic[pos._mat.GetShader()][pos._mat].erase(pos._mesh);

				if (_renderQueueDynamic[pos._mat.GetShader()][pos._mat].size() == 0) {
					_renderQueueDynamic[pos._mat.GetShader()].erase(pos._mat);

					if (_renderQueueDynamic[pos._mat.GetShader()].size() == 0) {
						_renderQueueDynamic.erase(pos._mat.GetShader());
					}
				}
			}
		}
		return true;
	}
	else {

		// NICZEGO NIE ZNAMY WIEC SZUKAMY WSZEDZIE NASZEGO MESH_RENDERERA
		std::vector<DataToUnregister> pos = std::vector<DataToUnregister>();

		for (auto& shaderPair : _renderQueueDynamic) {

			for (auto& materialPair : shaderPair.second) {

				for (auto& meshPair : materialPair.second) {

					for (size_t z = 0; z < materialPair.second[meshPair.first].meshRenderers.size(); ++z)
					{
						if (materialPair.second[meshPair.first].meshRenderers[z] == meshRenderer)
						{
							pos.push_back(DataToUnregister{
								._mat = materialPair.first,
								._mesh = meshPair.first,
								._pos = z
							});
							break;
						}
					}
				}
			}
		}

		for (auto& p : pos) {
			auto& meshRenderingData = _renderQueueDynamic[p._mat.GetShader()][p._mat][p._mesh];

			if (meshRenderingData.meshRenderers.size() != 0) {
				meshRenderingData.meshRenderers.erase(meshRenderingData.meshRenderers.cbegin() + p._pos);
			}
			if (meshRenderingData.modelTransforms.size() != 0) {
				meshRenderingData.modelTransforms.erase(meshRenderingData.modelTransforms.cbegin() + p._pos);
			}

			if (meshRenderingData.meshRenderers.size() == 0) {
				_renderQueueDynamic[p._mat.GetShader()][p._mat].erase(p._mesh);

				if (_renderQueueDynamic[p._mat.GetShader()][p._mat].size() == 0) {
					_renderQueueDynamic[p._mat.GetShader()].erase(p._mat);

					if (_renderQueueDynamic[p._mat.GetShader()].size() == 0) {
						_renderQueueDynamic.erase(p._mat.GetShader());
					}
				}
			}
		}

		pos.clear();
		return true;
	}

	return false;
}

void MeshRenderingManager::UpdateQueues()
{
	Frustum frustum = CameraComponent::GetCurrentCameraFrustum();
	RenderedSegment renderedSegment { .begin = nullptr, .count = 0 };

	for (auto& meshPair : _depthMapQueueStatic)
	{
		meshPair.second.rendered.clear();
	}
	for (auto& meshPair : _depthMapQueueDynamic)
	{
		meshPair.second.rendered.clear();
	}

#pragma region UPDATE_FOR_STATIC

	for (auto& shaderPair : _renderQueueStatic)
	{
		for (auto& materialPair : shaderPair.second)
		{
			for (auto& meshPair : materialPair.second)
			{
				renderedSegment.begin = meshPair.second.modelTransforms.data();
				renderedSegment.count = 0u;

				meshPair.second.rendered.clear();

				meshPair.second.renderedCount = 0u;

				for (size_t index = 0ull; index < meshPair.second.meshRenderers.size(); index++)
				{
					if (meshPair.second.meshRenderers[index]->GetGameObject()->GetActive())
					{
						if (CameraComponent::GetMainCamera()->IsFrustumCullingOn())
						{
							if (meshPair.first->IsOnFrustum(frustum, meshPair.second.modelTransforms[index]))
							{
								renderedSegment.count++;
							}
							else
							{
								if (renderedSegment.count)
								{
									meshPair.second.rendered.push_back(renderedSegment);
									meshPair.second.renderedCount += renderedSegment.count;

									renderedSegment.begin += renderedSegment.count + 1;
									renderedSegment.count = 0u;
								}
								else
								{
									renderedSegment.begin++;
								}
							}
						}
						else
						{
							renderedSegment.count++;
						}
					}
					else
					{
						if (renderedSegment.count)
						{
							meshPair.second.rendered.push_back(renderedSegment);
							meshPair.second.renderedCount += renderedSegment.count;

							renderedSegment.begin += renderedSegment.count + 1;
							renderedSegment.count = 0u;
						}
						else
						{
							renderedSegment.begin++;
						}
					}
				}
				if (renderedSegment.count)
				{
					meshPair.second.rendered.push_back(renderedSegment);
					meshPair.second.renderedCount += renderedSegment.count;
					renderedSegment.count = 0u;
				}

				_depthMapQueueStatic[meshPair.first].renderedCount += meshPair.second.renderedCount;
				//_depthMapQueueStatic[meshPair.first].rendered.insert(_depthMapQueueStatic[meshPair.first].rendered.cend(), meshPair.second.rendered.begin(), meshPair.second.rendered.end());
				for (const auto& element : meshPair.second.rendered)
				{
					_depthMapQueueStatic[meshPair.first].rendered.push_back(element);
				}
			}
		}
	}
	_depthQueueStatic = _depthMapQueueStatic;
#pragma endregion

#pragma region UPDATE_FOR_STATIC_TRANSPARENT

	for (auto& shaderPair : _renderQueueStaticTransparent)
	{
		for (auto& materialPair : shaderPair.second)
		{
			for (auto& meshPair : materialPair.second)
			{
				renderedSegment.begin = meshPair.second.modelTransforms.data();
				renderedSegment.count = 0u;

				meshPair.second.rendered.clear();

				meshPair.second.renderedCount = 0u;

				for (size_t index = 0ull; index < meshPair.second.meshRenderers.size(); index++)
				{
					if (meshPair.second.meshRenderers[index]->GetGameObject()->GetActive())
					{
						if (CameraComponent::GetMainCamera()->IsFrustumCullingOn())
						{
							if (meshPair.first->IsOnFrustum(frustum, meshPair.second.modelTransforms[index]))
							{
								renderedSegment.count++;
							}
							else
							{
								if (renderedSegment.count)
								{
									meshPair.second.rendered.push_back(renderedSegment);
									meshPair.second.renderedCount += renderedSegment.count;

									renderedSegment.begin += renderedSegment.count + 1;
									renderedSegment.count = 0u;
								}
								else
								{
									renderedSegment.begin++;
								}
							}
						}
						else
						{
							renderedSegment.count++;
						}
					}
					else
					{
						if (renderedSegment.count)
						{
							meshPair.second.rendered.push_back(renderedSegment);
							meshPair.second.renderedCount += renderedSegment.count;

							renderedSegment.begin += renderedSegment.count + 1;
							renderedSegment.count = 0u;
						}
						else
						{
							renderedSegment.begin++;
						}
					}
				}
				if (renderedSegment.count)
				{
					meshPair.second.rendered.push_back(renderedSegment);
					meshPair.second.renderedCount += renderedSegment.count;
					renderedSegment.count = 0u;
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
				renderedSegment.begin = meshPair.second.modelTransforms.data();
				renderedSegment.count = 0u;

				meshPair.second.rendered.clear();
				meshPair.second.renderedCount = 0u;

				for (size_t index = 0ull; index < meshPair.second.meshRenderers.size(); index++)
				{
					if (meshPair.second.meshRenderers[index]->IsTransformChanged())
					{
						meshPair.second.modelTransforms[index] = meshPair.second.meshRenderers[index]->GetGameObject()->GetTransform()->GetTransformMatrix();

						meshPair.second.meshRenderers[index]->TransformUpdated();
					}

					if (meshPair.second.meshRenderers[index]->GetGameObject()->GetActive())
					{
						if (CameraComponent::GetMainCamera()->IsFrustumCullingOn())
						{
							if (meshPair.first->IsOnFrustum(frustum, meshPair.second.modelTransforms[index]))
							{
								renderedSegment.count++;
							}
							else
							{
								if (renderedSegment.count)
								{
									meshPair.second.rendered.push_back(renderedSegment);
									meshPair.second.renderedCount += renderedSegment.count;

									renderedSegment.begin += renderedSegment.count + 1;
									renderedSegment.count = 0u;
								}
								else
								{
									renderedSegment.begin++;
								}
							}
						}
						else
						{
							renderedSegment.count++;
						}
					}
					else
					{
						if (renderedSegment.count)
						{
							meshPair.second.rendered.push_back(renderedSegment);
							meshPair.second.renderedCount += renderedSegment.count;

							renderedSegment.begin += renderedSegment.count + 1;
							renderedSegment.count = 0u;
						}
						else
						{
							renderedSegment.begin++;
						}
					}
				}
				if (renderedSegment.count)
				{
					meshPair.second.rendered.push_back(renderedSegment);
					meshPair.second.renderedCount += renderedSegment.count;
					renderedSegment.count = 0u;
				}

				_depthMapQueueDynamic[meshPair.first].renderedCount += meshPair.second.renderedCount;
				//_depthMapQueueDynamic[meshPair.first].rendered.insert(_depthMapQueueDynamic[meshPair.first].rendered.cend(), meshPair.second.rendered.begin(), meshPair.second.rendered.end());
				for (const auto& element : meshPair.second.rendered)
				{
					_depthMapQueueDynamic[meshPair.first].rendered.push_back(element);
				}
			}
		}
	}
	_depthQueueDynamic = _depthMapQueueDynamic;
#pragma endregion

#pragma region UPDATE_FOR_DYNAMIC_TRANSPARENT

	for (auto& shaderPair : _renderQueueDynamicTransparent)
	{
		for (auto& materialPair : shaderPair.second)
		{
			for (auto& meshPair : materialPair.second)
			{
				renderedSegment.begin = meshPair.second.modelTransforms.data();
				renderedSegment.count = 0u;

				meshPair.second.rendered.clear();
				meshPair.second.renderedCount = 0u;

				for (size_t index = 0ull; index < meshPair.second.meshRenderers.size(); index++)
				{
					if (meshPair.second.meshRenderers[index]->IsTransformChanged())
					{
						meshPair.second.modelTransforms[index] = meshPair.second.meshRenderers[index]->GetGameObject()->GetTransform()->GetTransformMatrix();

						meshPair.second.meshRenderers[index]->TransformUpdated();
					}

					if (meshPair.second.meshRenderers[index]->GetGameObject()->GetActive())
					{
						if (CameraComponent::GetMainCamera()->IsFrustumCullingOn())
						{
							if (meshPair.first->IsOnFrustum(frustum, meshPair.second.modelTransforms[index]))
							{
								renderedSegment.count++;
							}
							else
							{
								if (renderedSegment.count)
								{
									meshPair.second.rendered.push_back(renderedSegment);
									meshPair.second.renderedCount += renderedSegment.count;

									renderedSegment.begin += renderedSegment.count + 1;
									renderedSegment.count = 0u;
								}
								else
								{
									renderedSegment.begin++;
								}
							}
						}
						else
						{
							renderedSegment.count++;
						}
					}
					else
					{
						if (renderedSegment.count)
						{
							meshPair.second.rendered.push_back(renderedSegment);
							meshPair.second.renderedCount += renderedSegment.count;

							renderedSegment.begin += renderedSegment.count + 1;
							renderedSegment.count = 0u;
						}
						else
						{
							renderedSegment.begin++;
						}
					}
				}
				if (renderedSegment.count)
				{
					meshPair.second.rendered.push_back(renderedSegment);
					meshPair.second.renderedCount += renderedSegment.count;
					renderedSegment.count = 0u;
				}
			}
		}
	}
#pragma endregion
}

void MeshRenderingManager::PreRender()
{
	if (_flags.IsStaticChanged) {
		Twin2Engine::Graphic::LightingController::Instance()->RenderShadowMaps();
		_flags.IsStaticChanged = false;
	}

	size_t instanceIndex = 0;
	size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

	unsigned int count = 0;
	RenderedSegment currentSegment{ .begin = nullptr, .count = 0u };

	std::list<RenderedSegment>::iterator renderItr;

#pragma region RENDERING_STATIC_DEPTH_MAP

	for (auto& meshPair : _depthMapQueueStatic)
	{
		if (meshPair.second.renderedCount)
		{
			count = meshPair.second.renderedCount;

			meshPair.second.renderedCount = 0u;

			instanceIndex = 0;
			remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

			currentSegment.begin = nullptr;
			currentSegment.count = 0u;
			renderItr = meshPair.second.rendered.begin();

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				instanceIndex = 0ull;
				remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

				if (currentSegment.count)
				{
					if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
						currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
						currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}
					else
					{
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
				}
				while (remaining > 0)
				{
					currentSegment = *renderItr;
					renderItr++;
					//currentSegment = meshPair.second.rendered.front();
					//meshPair.second.rendered.pop_front();

					if (currentSegment.count > remaining)
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
						currentSegment.begin += remaining;
						currentSegment.count -= remaining;

						instanceIndex += remaining;
						remaining = 0ull;
					}
					else
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
				std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}
			while (remaining > 0)
			{
				currentSegment = *renderItr;
				renderItr++;
				//currentSegment = meshPair.second.rendered.front();
				//meshPair.second.rendered.pop_front();

				std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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

	for (auto& meshPair : _depthMapQueueDynamic)
	{
		if (meshPair.second.renderedCount)
		{
			count = meshPair.second.renderedCount;

			meshPair.second.renderedCount = 0u;

			instanceIndex = 0;
			remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

			currentSegment.begin = nullptr;
			currentSegment.count = 0u;
			renderItr = meshPair.second.rendered.begin();

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				instanceIndex = 0ull;
				remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

				if (currentSegment.count)
				{
					if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
						currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
						currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}
					else
					{
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
				}
				while (remaining > 0)
				{
					currentSegment = *renderItr;
					renderItr++;
					//currentSegment = meshPair.second.rendered.front();
					//meshPair.second.rendered.pop_front();

					if (currentSegment.count > remaining)
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
						currentSegment.begin += remaining;
						currentSegment.count -= remaining;

						instanceIndex += remaining;
						remaining = 0ull;
					}
					else
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
				std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}
			while (remaining > 0)
			{
				currentSegment = *renderItr;
				renderItr++;
				//currentSegment = meshPair.second.rendered.front();
				//meshPair.second.rendered.pop_front();

				std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
}

void MeshRenderingManager::RenderStatic()
{
	unsigned int globalDrawCount = 0;

	RenderedSegment currentSegment{ .begin = nullptr, .count = 0u };

	std::list<RenderedSegment>::iterator renderItr;

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

					currentSegment.begin = nullptr;
					currentSegment.count = 0u;

					renderItr = meshPair.second.rendered.begin();

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = *renderItr;
							renderItr++;
							//currentSegment = meshPair.second.rendered.front();
							//meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
								currentSegment.begin += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = *renderItr;
						renderItr++;
						//currentSegment = meshPair.second.rendered.front();
						//meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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

					currentSegment.begin = nullptr;
					currentSegment.count = 0u;

					renderItr = meshPair.second.rendered.begin();

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = *renderItr;
							renderItr++;
							//currentSegment = meshPair.second.rendered.front();
							//meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
								currentSegment.begin += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = *renderItr;
						renderItr++;
						//currentSegment = meshPair.second.rendered.front();
						//meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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

#pragma region RENDERING_STATIC_OBJECTS_TRANSPARENT

	for (auto& shaderPair : _renderQueueStaticTransparent)
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

					currentSegment.begin = nullptr;
					currentSegment.count = 0u;

					renderItr = meshPair.second.rendered.begin();

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = *renderItr;
							renderItr++;
							//currentSegment = meshPair.second.rendered.front();
							//meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
								currentSegment.begin += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = *renderItr;
						renderItr++;
						//currentSegment = meshPair.second.rendered.front();
						//meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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

#pragma region RENDERING_DYNAMIC_OBJECTS_TRANSPARENT

	for (auto& shaderPair : _renderQueueDynamicTransparent)
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

					currentSegment.begin = nullptr;
					currentSegment.count = 0u;

					renderItr = meshPair.second.rendered.begin();

					while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						instanceIndex = 0ull;
						remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

						if (currentSegment.count)
						{
							if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
							{
								std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
								currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
								currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

								instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
								remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
							}
							else
							{
								std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

								instanceIndex += currentSegment.count;
								remaining -= currentSegment.count;
							}
						}
						while (remaining > 0)
						{
							currentSegment = *renderItr;
							renderItr++;
							//currentSegment = meshPair.second.rendered.front();
							//meshPair.second.rendered.pop_front();

							if (currentSegment.count > remaining)
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
								currentSegment.begin += remaining;
								currentSegment.count -= remaining;

								instanceIndex += remaining;
								remaining = 0ull;
							}
							else
							{
								std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
					while (remaining > 0)
					{
						currentSegment = *renderItr;
						renderItr++;
						//currentSegment = meshPair.second.rendered.front();
						//meshPair.second.rendered.pop_front();

						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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

void MeshRenderingManager::RenderDepthMapStatic(const GLuint& depthFBO, const GLuint& depthReplacingTexId, const GLuint& depthReplcedTexId, glm::mat4& projectionViewMatrix)
{
	glEnable(GL_DEPTH_TEST);
	ShaderManager::DepthShader->Use();
	ShaderManager::DepthShader->SetMat4("lightSpaceMatrix", projectionViewMatrix);

	//glViewport(0, 0, bufferWidth, bufferHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthReplacingTexId, 0);

	//glBindTexture(GL_TEXTURE_2D, depthMapTex);
	glClear(GL_DEPTH_BUFFER_BIT);

	unsigned int count = 0;
	RenderedSegment currentSegment{ .begin = nullptr, .count = 0u };

	std::list<RenderedSegment>::iterator renderItr;

#pragma region RENDERING_STATIC_DEPTH_MAP

	for (auto& meshPair : _depthQueueStatic)
	{
		if (meshPair.second.renderedCount)
		{
			count = meshPair.second.renderedCount;

			meshPair.second.renderedCount = 0u;

			size_t instanceIndex = 0;
			size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

			currentSegment.begin = nullptr;
			currentSegment.count = 0u;
			renderItr = meshPair.second.rendered.begin();

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				instanceIndex = 0ull;
				remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

				if (currentSegment.count)
				{
					if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
						currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
						currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}
					else
					{
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
				}
				while (remaining > 0)
				{
					currentSegment = *renderItr;
					renderItr++;
					//currentSegment = meshPair.second.rendered.front();
					//meshPair.second.rendered.pop_front();

					if (currentSegment.count > remaining)
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
						currentSegment.begin += remaining;
						currentSegment.count -= remaining;

						instanceIndex += remaining;
						remaining = 0ull;
					}
					else
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
				std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}
			while (remaining > 0)
			{
				currentSegment = *renderItr;
				renderItr++;
				//currentSegment = meshPair.second.rendered.front();
				//meshPair.second.rendered.pop_front();

				std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("Error: {}", error);
			}
		}
	}

#pragma endregion

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthReplcedTexId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MeshRenderingManager::RenderDepthMapDynamic(const GLuint& depthFBO, glm::mat4& projectionViewMatrix)
{
	glEnable(GL_DEPTH_TEST);
	ShaderManager::DepthShader->Use();
	ShaderManager::DepthShader->SetMat4("lightSpaceMatrix", projectionViewMatrix);

	//glViewport(0, 0, bufferWidth, bufferHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	//glBindTexture(GL_TEXTURE_2D, depthMapTex);
	glClear(GL_DEPTH_BUFFER_BIT);

	unsigned int count = 0;
	RenderedSegment currentSegment{ .begin = nullptr, .count = 0u };

	std::list<RenderedSegment>::iterator renderItr;

#pragma region RENDERING_DYNAMIC_DEPTH_MAP

	for (auto& meshPair : _depthQueueDynamic)
	{
		if (meshPair.second.renderedCount)
		{
			count = meshPair.second.renderedCount;

			meshPair.second.renderedCount = 0u;

			size_t instanceIndex = 0;
			size_t remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

			currentSegment.begin = nullptr;
			currentSegment.count = 0u;
			renderItr = meshPair.second.rendered.begin();

			while (count > MAX_INSTANCE_NUMBER_PER_DRAW)
			{
				instanceIndex = 0ull;
				remaining = MAX_INSTANCE_NUMBER_PER_DRAW;

				if (currentSegment.count)
				{
					if (currentSegment.count > MAX_INSTANCE_NUMBER_PER_DRAW)
					{
						std::memcpy(_modelTransforms, currentSegment.begin, MAX_INSTANCE_NUMBER_PER_DRAW * sizeof(glm::mat4));
						currentSegment.begin += MAX_INSTANCE_NUMBER_PER_DRAW;
						currentSegment.count -= MAX_INSTANCE_NUMBER_PER_DRAW;

						instanceIndex += MAX_INSTANCE_NUMBER_PER_DRAW;
						remaining -= MAX_INSTANCE_NUMBER_PER_DRAW;
					}
					else
					{
						std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

						instanceIndex += currentSegment.count;
						remaining -= currentSegment.count;
					}
				}
				while (remaining > 0)
				{
					currentSegment = *renderItr;
					renderItr++;
					//currentSegment = meshPair.second.rendered.front();
					//meshPair.second.rendered.pop_front();

					if (currentSegment.count > remaining)
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, remaining * sizeof(glm::mat4));
						currentSegment.begin += remaining;
						currentSegment.count -= remaining;

						instanceIndex += remaining;
						remaining = 0ull;
					}
					else
					{
						std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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
				std::memcpy(_modelTransforms, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

				instanceIndex += currentSegment.count;
				remaining -= currentSegment.count;
			}
			while (remaining > 0)
			{
				currentSegment = *renderItr;
				renderItr++;
				//currentSegment = meshPair.second.rendered.front();
				//meshPair.second.rendered.pop_front();

				std::memcpy(_modelTransforms + instanceIndex, currentSegment.begin, currentSegment.count * sizeof(glm::mat4));

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

			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				SPDLOG_ERROR("Error: {}", error);
			}
		}
	}

#pragma endregion

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}