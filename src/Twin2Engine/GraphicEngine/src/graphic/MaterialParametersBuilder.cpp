#include <graphic/MaterialParametersBuilder.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::Graphic;
using namespace glm;
using namespace std;

std::hash<std::string> MaterialParametersBuilder::hasher;

void MaterialParametersBuilder::AddTexture2D(const std::string& textureName, unsigned int textureId)
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

MaterialParameters* MaterialParametersBuilder::Build()
{
	return new MaterialParameters(_parameters, _textureMappings, _textures);
}

void MaterialParametersBuilder::Clear()
{
	_parameters.Clear();
	_textureMappings.clear();
	_textures.clear();
}
