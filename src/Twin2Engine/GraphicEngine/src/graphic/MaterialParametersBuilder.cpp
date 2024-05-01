#include <graphic/MaterialParametersBuilder.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::GraphicEngine;
using namespace glm;
using namespace std;

std::hash<std::string> MaterialParametersBuilder::hasher;

void Twin2Engine::GraphicEngine::MaterialParametersBuilder::AddTexture2D(const std::string& textureName, unsigned int textureId)
{
	size_t hashed = hasher(textureName);

	if (_textureMappings.contains(hashed))
	{
		_textures[_textureMappings[hashed]] = textureId;
	}
	else
	{
		_textureMappings[hashed] = _textures.size();
		_textures.push_back(textureId);
	}
}

void MaterialParametersBuilder::AlignData()
{
	unsigned int over = _materialData.size() % 16u;
	if (over)
	{
		_materialData.resize(_materialData.size() + 16u - over);
	}
}

MaterialParameters* MaterialParametersBuilder::Build()
{
	AlignData();

	return nullptr;
}

void MaterialParametersBuilder::Clear()
{
	_materialData.clear();
	_variablesValuesOffsets.clear();
	_textureMappings.clear();
	_textures.clear();
}
