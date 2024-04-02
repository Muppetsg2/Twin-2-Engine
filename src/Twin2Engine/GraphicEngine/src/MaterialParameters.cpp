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