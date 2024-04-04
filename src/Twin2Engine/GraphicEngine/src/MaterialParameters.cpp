#include "MaterialParameters.h"
#include "MaterialParameters.h"
#include "MaterialParameters.h"
#include <MaterialParameters.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::GraphicEngine;

 std::hash<std::string> MaterialParameters::hasher;

 MaterialParameters::MaterialParameters()
 {

 }

MaterialParameters::MaterialParameters(const std::vector<std::string>& variableNames)
{
	std::hash<std::string> hasher;

	for (int i = 0; i < variableNames.size(); i++)
	{
		_variablesValuesMappings[hasher(variableNames[i])];
	}

}

void MaterialParameters::Add(const std::string& variableName, size_t size, void* value)
{
	size_t hashed = hasher(variableName);

	size_t paramSize = size;
	
	if (size % 2 == 0)
	{
		// Jest to pusty przebieg, aby przy wiêkszoœci przypadków, w których to size jest podzielny przez 2, ¿eby ci¹g warunków zawsze spradzanych by³ krótki
	}
	if (size == 12)
	{
		paramSize = 16;
	}
	else if (size == 36)
	{
		paramSize = 64;
	}

	const char* ptr = reinterpret_cast<const char*>(value);
	std::vector<char> result(paramSize); //(ptr, ptr + size);
	memcpy(result.data(), ptr, size);


	_variablesValuesMappings[hashed] = result;
}

void Twin2Engine::GraphicEngine::MaterialParameters::AddTexture2D(const std::string& textureName, unsigned int textureId)
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
		GLuint samplerID;

		glGenSamplers(1, &samplerID);

		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_samplers.push_back(samplerID);
	}
}

void Twin2Engine::GraphicEngine::MaterialParameters::SetTexture2D(const std::string& textureName, unsigned int textureId)
{
	size_t hashed = hasher(textureName);

	if (_textureMappings.contains(hashed))
	{
		_textures[_textureMappings[hashed]] = textureId;
	}
}

void Twin2Engine::GraphicEngine::MaterialParameters::UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded)
{
	SPDLOG_INFO("Here1 {}", _textures.size());
	for (int i = 0; i < _textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + textureBinded);
		//glActiveTexture(GL_TEXTURE0 + textureBinded);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);
		SPDLOG_INFO("Here4 {} {}", beginLocation, textureBinded);
		glBindSampler(textureBinded, _samplers[i]);
		SPDLOG_INFO("Here6");
		//glUniform1i(beginLocation, GL_TEXTURE0 + textureBinded);
		glUniform1i(beginLocation, textureBinded);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			SPDLOG_ERROR("Error: {}", error);
		}
		//glProgramUniform1i(programId, beginLocation, GL_TEXTURE0 + textureBinded);
		SPDLOG_INFO("Here5");
		textureBinded++;
		beginLocation++;
	}
	//SPDLOG_INFO("Here2");
}

std::vector<char> MaterialParameters::GetData() const
{
	size_t totalSize = 0;
	for (const auto& pair : _variablesValuesMappings) {
		totalSize += pair.second.size();
	}
	// Create the flattened vector with the appropriate size
	std::vector<char> flattenedVector(totalSize);

	// Copy the vectors from the map into the flattened vector
	size_t offset = 0;
	for (const auto& pair : _variablesValuesMappings) {
		const auto& vector = pair.second;
		std::copy(vector.begin(), vector.end(), flattenedVector.begin() + offset);
		offset += vector.size();
	}

	return flattenedVector;
}