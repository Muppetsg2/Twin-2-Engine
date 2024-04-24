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