#include <graphic/MaterialParameters.h>

#include <spdlog/spdlog.h>

using namespace Twin2Engine::GraphicEngine;
using namespace glm;
using namespace std;

 std::hash<std::string> MaterialParameters::hasher;

 MaterialParameters::MaterialParameters()
 {

 }

MaterialParameters::MaterialParameters(const std::vector<std::string>& variableNames, const std::vector<unsigned int>& parametersSizes, const std::vector<std::string>& textureParametersNames)
{
	unsigned int dataSize = 0;
	const unsigned int vectorSize = variableNames.size() < parametersSizes.size() ? variableNames.size() : parametersSizes.size();
	for (unsigned int i = 0; i < vectorSize; i++)
	{
		unsigned int left = 16u - (dataSize % 16u);
		if (parametersSizes[i] > left)
		{
			dataSize += left;
		}
		_variablesValuesOffsets[hasher(variableNames[i])] = dataSize;
		dataSize += parametersSizes[i];
	}
	unsigned int over = dataSize % 16u;
	if (over)
	{
		_materialData.resize(dataSize + 16u - over);
	}
	_materialData.resize(dataSize);

	_textures.resize(textureParametersNames.size());
	for (int i = 0; i < textureParametersNames.size(); i++)
	{
		_textureMappings[hasher(textureParametersNames[i])] = i;
	}
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
	for (int i = 0; i < _textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + textureBinded);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);

		glUniform1i(beginLocation, textureBinded);

		textureBinded++;
		beginLocation++;
	}
}

void MaterialParameters::AlignData()
{
	unsigned int over = _materialData.size() % 16u;
	if (over)
	{
		_materialData.resize(_materialData.size() + 16u - over);
	}
}

std::vector<char> MaterialParameters::GetData() const
{
	//size_t totalSize = 0;
	//for (const auto& pair : _variablesValuesMappings) {
	//	totalSize += pair.second.size();
	//}
	//// Create the flattened vector with the appropriate size
	//std::vector<char> flattenedVector(totalSize);
	//
	//// Copy the vectors from the map into the flattened vector
	//size_t offset = 0;
	//for (const auto& pair : _variablesValuesMappings) {
	//	const auto& vector = pair.second;
	//	std::copy(vector.begin(), vector.end(), flattenedVector.begin() + offset);
	//	offset += vector.size();
	//}

	return _materialData;
}