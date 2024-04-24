#include <graphic/STD140Struct.h>

using namespace Twin2Engine::GraphicEngine;
using namespace std;

hash<string> STD140Struct::_hasher;

void STD140Struct::Add(const string& name, const vector<char>& value, size_t baseAligement, size_t baseOffset)
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
	_data.resize(aligementOffset + baseOffset);

	// SET DATA
	memcpy(_data.data() + aligementOffset, value.data(), value.size());

	// CHECK MAX ALIGEMENT
	if (baseAligement > _maxAligement) {
		_maxAligement = baseAligement;
	}
}

void STD140Struct::AddArray(const string& name, const vector<vector<char>>& values, size_t baseAligement, size_t baseOffset) {
	// SET BASE ALIGEMENT
	if (baseAligement % 16 != 0) {
		baseAligement += 16 - (baseAligement % 16);
	}
	// ADD ARRAY VALUES
	for (size_t i = 0; i < values.size(); ++i) {
		Add(name + "["s + to_string(i) + "]"s, values[i], baseAligement, baseOffset);
	}
	// UPDATE CURRENT OFFSET (ADD PADDING)
	if (_currentOffset % 16 != 0) {
		_currentOffset += 16 - (_currentOffset % 16);
	}
}

void STD140Struct::Add(const std::string& name, const STD140Struct& value)
{
	Add(name, value._data, value.GetBaseAligement(), value._currentOffset);
	size_t aligementOffset = _offsets[_hasher(name)];
	for (const auto& off : value._offsets) {
		size_t nameHash = _hasher(name + "."s + (*value._names.find(off.first)).second);
		_offsets[nameHash] = aligementOffset + off.second;
	}
	if (_currentOffset % 16 != 0) {
		_currentOffset += 16 - (_currentOffset % 16);
	}
}

void STD140Struct::Add(const std::string& name, const const STD140Struct*& values, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		Add(name + "["s + std::to_string(i) + "]"s, values[i]);
	}
}

void STD140Struct::Add(const std::string& name, const STD140Struct*& values, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		Add(name + "["s + std::to_string(i) + "]"s, values[i]);
	}
}

void STD140Struct::Add(const std::string& name, const std::vector<STD140Struct>& values)
{
	for (size_t i = 0; i < values.size(); ++i) {
		Add(name + "["s + std::to_string(i) + "]"s, values[i]);
	}
}

vector<char> STD140Struct::GetData() const
{
	return _data;
}

size_t STD140Struct::GetBaseAligement() const {
	size_t baseAligement = _maxAligement;
	if (_maxAligement % 16 != 0) {
		baseAligement += 16 - (baseAligement % 16);
	}
	return baseAligement;
}

size_t STD140Struct::GetSize() const {
	size_t size = _currentOffset;
	if (size % 16 != 0) {
		size += 16 - (size % 16);
	}
	return size;
}