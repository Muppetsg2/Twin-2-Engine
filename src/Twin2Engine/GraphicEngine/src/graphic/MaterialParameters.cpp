#include <graphic/MaterialParameters.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::GraphicEngine;
using namespace glm;
using namespace std;

 std::hash<std::string> MaterialParameters::hasher;

MaterialParameters::MaterialParameters()
{

}

MaterialParameters::MaterialParameters(const STD140Struct& parameters, const map<size_t, char>& textureMappings, const vector<GLuint>& textures)
{
	_parameters = parameters;
	// Tworzenie UBO materialInput
	glGenBuffers(1, &_materialParametersDataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _materialParametersDataUBO);
	// Initialization of buffer
	glBufferData(GL_UNIFORM_BUFFER, _parameters.GetSize(), _parameters.GetData().data(), GL_DYNAMIC_DRAW);
	//glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, _materialParametersDataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	_textureMappings = textureMappings;
	_textures = textures;
}

void MaterialParameters::SetTexture2D(const std::string& textureName, unsigned int textureId)
{
	size_t hashed = hasher(textureName);

	if (_textureMappings.contains(hashed))
	{
		_textures[_textureMappings[hashed]] = textureId;
	}
}

void MaterialParameters::UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded)
{
	for (int i = 0; i < _textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + textureBinded);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);

		glUniform1i(beginLocation, textureBinded);

		textureBinded++;
		beginLocation++;
	}
}


GLuint MaterialParameters::GetDataUBO() const
{
	return _materialParametersDataUBO;
}

const char* MaterialParameters::GetData() const
{
	return _parameters.GetData().data();
}
size_t MaterialParameters::GetSize() const
{
	return _parameters.GetSize();
}