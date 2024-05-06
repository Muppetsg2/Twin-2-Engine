#include <tools/STD140Offsets.h>

using namespace Twin2Engine::Tools;
using namespace std;

hash<string> STD140Offsets::_hasher;

bool STD140Offsets::CheckVariable(const std::string& name) const
{
	if (Contains(name)) {
		SPDLOG_ERROR("Variable '{0}' already added to structure", name);
		return true;
	}
	return false;
}

bool STD140Offsets::Contains(const std::string& name) const
{
	return _offsets.contains(_hasher(name));
}

size_t STD140Offsets::Add(const string& name, size_t baseAligement, size_t baseOffset)
{
	size_t nameHash = _hasher(name);

	// CALCULATE ALIGEMENT
	size_t aligementOffset = _currentOffset;
	if (aligementOffset % baseAligement != 0) {
		aligementOffset += baseAligement - (aligementOffset % baseAligement);
	}
	_offsets[nameHash] = aligementOffset;
	_names[nameHash] = name;

	// UPDATE SIZE
	_currentOffset = aligementOffset + baseOffset;

	// CHECK MAX ALIGEMENT
	if (baseAligement > _maxAligement) {
		_maxAligement = baseAligement;
	}

	return aligementOffset;
}

vector<size_t> STD140Offsets::AddArray(const string& name, size_t arraySize, size_t baseAligement, size_t baseOffset)
{
	if (arraySize == 0) return vector<size_t>();

	// SET BASE ALIGEMENT
	if (baseAligement % 16 != 0) {
		baseAligement += 16 - (baseAligement % 16);
	}
	// ADD ARRAY VALUES
	vector<size_t> arrayElemOffsets;
	for (size_t i = 0; i < arraySize; ++i) {
		arrayElemOffsets.push_back(Add(name + "["s + to_string(i) + "]"s, baseAligement, baseOffset));
	}
	// SET ARRAY BEGIN POINTER
	size_t nameHash = _hasher(name);
	_offsets[nameHash] = arrayElemOffsets[0];
	_names[nameHash] = name;
	// UPDATE CURRENT OFFSET (ADD PADDING)
	if (_currentOffset % 16 != 0) {
		_currentOffset += 16 - (_currentOffset % 16);
	}

	return arrayElemOffsets;
}

size_t STD140Offsets::Get(const string& name) const
{
	size_t nameHash = _hasher(name);
	if (!_offsets.contains(nameHash)) return 0;
	else return (*_offsets.find(nameHash)).second;
}

vector<size_t> STD140Offsets::GetArray(const string& name) const
{
	size_t nameHash = _hasher(name);
	if (!_offsets.contains(nameHash)) return vector<size_t>();
	
	vector<size_t> values;
	nameHash = _hasher(name + "[0]");
	size_t i = 1;
	while (_offsets.contains(nameHash)) {
		values.push_back((*_offsets.find(nameHash)).second);
		nameHash = _hasher(name + "[" + to_string(i) + "]");
		++i;
	}
	return values;
}

size_t STD140Offsets::GetBaseAligement() const
{
	size_t baseAligement = _maxAligement;
	if (_maxAligement % 16 != 0) {
		baseAligement += 16 - (baseAligement % 16);
	}
	return baseAligement;
}

size_t STD140Offsets::GetSize() const
{
	size_t size = _currentOffset;
	if (size % 16 != 0) {
		size += 16 - (size % 16);
	}
	return size;
}

void STD140Offsets::Clear()
{
	_currentOffset = 0;
	_maxAligement = 0;

	_offsets.clear();
	_names.clear();
}