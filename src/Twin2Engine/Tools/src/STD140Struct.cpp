#include <tools/STD140Struct.h>

using namespace Twin2Engine::Tools;
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
	if (values.size() == 0) return;

	// SET BASE ALIGEMENT
	if (baseAligement % 16 != 0) {
		baseAligement += 16 - (baseAligement % 16);
	}
	// ADD ARRAY VALUES
	for (size_t i = 0; i < values.size(); ++i) {
		Add(name + "["s + to_string(i) + "]"s, values[i], baseAligement, baseOffset);
	}
	// SET ARRAY BEGIN POINTER
	size_t nameHash = _hasher(name);
	size_t firstElemHash = _hasher(name + "[0]"s);
	_offsets[nameHash] = _offsets[firstElemHash];
	// UPDATE CURRENT OFFSET (ADD PADDING)
	if (_currentOffset % 16 != 0) {
		_currentOffset += 16 - (_currentOffset % 16);
	}
}

bool STD140Struct::Set(const string& name, const vector<char>& value)
{
	size_t nameHash = _hasher(name);
	if (_offsets.contains(nameHash)) {
		memcpy(_data.data() + _offsets[nameHash], value.data(), value.size());
		return true;
	}
	return false;
}

bool STD140Struct::SetArray(const string& name, const vector<vector<char>>& values) 
{
	size_t nameHash = _hasher(name);
	if (_offsets.contains(nameHash)) {
		for (size_t i = 0; i < values.size(); ++i) {
			if (!Set(name + "["s + to_string(i) + "]"s, values[i])) {
				return false;
			}
		}
		return true;
	}
	return false;
}

vector<char> STD140Struct::Get(const string& name, size_t baseOffset) const
{
	size_t nameHash = _hasher(name);
	if (_offsets.contains(nameHash)) {
		vector<char> value;
		value.resize(baseOffset);
		memcpy(value.data(), _data.data() + (*_offsets.find(nameHash)).second, baseOffset);
		return value;
	}
	return vector<char>();
}

vector<vector<char>> STD140Struct::GetArray(const string& name, size_t baseOffset) const
{
	if (_offsets.contains(_hasher(name))) {
		// GET ARRAY VALUES
		vector<vector<char>> values;
		size_t i = 0;
		string arrayElemName = name + "["s + to_string(i) + "]"s;
		while (_offsets.contains(_hasher(arrayElemName))) {
			values.push_back(Get(arrayElemName, baseOffset));

			++i;
			arrayElemName = name + "["s + to_string(i) + "]"s;
		}
		return values;
	}
	return vector<vector<char>>();
}

void STD140Struct::Add(const string& name, const STD140Struct& value)
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

void STD140Struct::Add(const string& name, const STD140Struct*& values, size_t size)
{
	AddStructArray(name, values, size);
}

void STD140Struct::Add(const string& name, const vector<STD140Struct>& values)
{
	AddStructArray(name, values, values.size());
}

bool STD140Struct::Set(const string& name, const STD140Struct& value)
{
	return Set(name, value._data);
}

bool STD140Struct::Set(const string& name, const STD140Struct*& values, size_t size)
{
	return SetStructArray(name, values, size);
}

bool STD140Struct::Set(const string& name, const vector<STD140Struct>& values)
{
	return SetStructArray(name, values, values.size());
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

void STD140Struct::Clear()
{
	_currentOffset = 0;
	_maxAligement = 0;

	_offsets.clear();
	_names.clear();
	_data.clear();
}