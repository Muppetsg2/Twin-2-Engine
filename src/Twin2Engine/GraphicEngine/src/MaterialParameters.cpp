#include <MaterialParameters.h>

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