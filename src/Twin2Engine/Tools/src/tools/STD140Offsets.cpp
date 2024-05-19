#include <tools/STD140Offsets.h>

using namespace Twin2Engine::Tools;
using namespace std;

hash<string> STD140Offsets::_hasher;

bool STD140Offsets::CheckVariable(const std::string& name) const
{
	static const char* const tracy_CheckVariable = "STD140Offsets Check Variable";
	static const char* const tracy_CheckVariableContains = "STD140Offsets Check Variable Contains";
	static const char* const tracy_CheckVariableErrorCheck = "STD140Offsets Check Variable Error Check";
	ZoneScoped;

	FrameMarkStart(tracy_CheckVariable);
	FrameMarkStart(tracy_CheckVariableContains);
	bool c = move(Contains(name));
	FrameMarkEnd(tracy_CheckVariableContains);
	FrameMarkStart(tracy_CheckVariableErrorCheck);
	if (c) {
		SPDLOG_ERROR("Variable '{0}' already added to structure", name);
	}
	FrameMarkEnd(tracy_CheckVariableErrorCheck);
	FrameMarkEnd(tracy_CheckVariable);
	return c;
}

bool STD140Offsets::Contains(const std::string& name) const
{
	ZoneScoped;
	return _offsets.contains(move(_hasher(name)));
}

size_t STD140Offsets::Add(const string& name, size_t baseAligement, size_t baseOffset)
{
	static const char* const tracy_Add = "STD140Offsets Add";
	static const char* const tracy_AddGetNameHash = "STD140Offsets Add Get Name Hash";
	static const char* const tracy_AddCalculateAligement = "STD140Offsets Add Calculate Aligement";
	static const char* const tracy_AddCalculateAligementPushOffset = "STD140Offsets Add Calculate Aligement Push Offset";
	static const char* const tracy_AddCalculateAligementPushName = "STD140Offsets Add Calculate Aligement Push Name";
	static const char* const tracy_AddUpdateSize = "STD140Offsets Add Update Size";
	static const char* const tracy_AddCheckMaxAligement = "STD140Offsets Add Check Max Aligement";
	ZoneScoped;

	FrameMarkStart(tracy_Add);

	FrameMarkStart(tracy_AddGetNameHash);
	size_t nameHash = move(_hasher(name));
	FrameMarkEnd(tracy_AddGetNameHash);

	// CALCULATE ALIGEMENT
	FrameMarkStart(tracy_AddCalculateAligement);
	size_t aligementOffset = move(_currentOffset);
	if (aligementOffset % baseAligement != 0) {
		aligementOffset += baseAligement - (aligementOffset % baseAligement);
	}
	FrameMarkStart(tracy_AddCalculateAligementPushOffset);
	_offsets[nameHash] = aligementOffset;
	FrameMarkEnd(tracy_AddCalculateAligementPushOffset);
	FrameMarkStart(tracy_AddCalculateAligementPushName);
	_names[nameHash] = name;
	FrameMarkEnd(tracy_AddCalculateAligementPushName);
	FrameMarkEnd(tracy_AddCalculateAligement);

	// UPDATE SIZE
	FrameMarkStart(tracy_AddUpdateSize);
	_currentOffset = aligementOffset + baseOffset;
	FrameMarkEnd(tracy_AddUpdateSize);

	// CHECK MAX ALIGEMENT
	FrameMarkStart(tracy_AddCheckMaxAligement);
	if (baseAligement > _maxAligement) {
		_maxAligement = move(baseAligement);
	}
	FrameMarkEnd(tracy_AddCheckMaxAligement);

	FrameMarkEnd(tracy_Add);
	return aligementOffset;
}

vector<size_t> STD140Offsets::AddArray(const string& name, size_t arraySize, size_t baseAligement, size_t baseOffset)
{
	static const char* const tracy_AddArray = "STD140Offsets AddArray";
	static const char* const tracy_AddArraySetBaseAligement = "STD140Offsets AddArray Set Base Aligement";
	static const char* const tracy_AddArrayAddArrayValues = "STD140Offsets AddArray Add Array Values";
	static const char* const tracy_AddArrayAddArrayValuesAddElem = "STD140Offsets AddArray Add Array Values Add Element";
	static const char* const tracy_AddArraySetArrayBeginPointer = "STD140Offsets AddArray Set Array Begin Pointer";
	static const char* const tracy_AddArrayUpdateCurrentOffset = "STD140Offsets AddArray Update Current Offset";
	ZoneScoped;

	FrameMarkStart(tracy_AddArray);

	vector<size_t> arrayElemOffsets;
	if (arraySize != 0) {
		
		// SET BASE ALIGEMENT
		FrameMarkStart(tracy_AddArraySetBaseAligement);
		if (baseAligement % 16 != 0) {
			baseAligement += move(16 - (baseAligement % 16));
		}
		FrameMarkEnd(tracy_AddArraySetBaseAligement);
		
		// ADD ARRAY VALUES
		FrameMarkStart(tracy_AddArrayAddArrayValues);
		string valueName = move(concat(name, "["));
		for (size_t i = 0; i < arraySize; ++i) {
			string elemName = move(concat(valueName, move(to_string(i)), "]"));
			FrameMarkStart(tracy_AddArrayAddArrayValuesAddElem);
			arrayElemOffsets.push_back(move(Add(elemName, baseAligement, baseOffset)));
			FrameMarkEnd(tracy_AddArrayAddArrayValuesAddElem);
		}
		FrameMarkEnd(tracy_AddArrayAddArrayValues);
		
		// SET ARRAY BEGIN POINTER
		FrameMarkStart(tracy_AddArraySetArrayBeginPointer);
		size_t nameHash = move(_hasher(name));
		_offsets[nameHash] = arrayElemOffsets[0];
		_names[nameHash] = name;
		FrameMarkEnd(tracy_AddArraySetArrayBeginPointer);

		// UPDATE CURRENT OFFSET (ADD PADDING)
		FrameMarkStart(tracy_AddArrayUpdateCurrentOffset);
		if (_currentOffset % 16 != 0) {
			_currentOffset += move(16 - (_currentOffset % 16));
		}
		FrameMarkEnd(tracy_AddArrayUpdateCurrentOffset);
	}

	FrameMarkEnd(tracy_AddArray);
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