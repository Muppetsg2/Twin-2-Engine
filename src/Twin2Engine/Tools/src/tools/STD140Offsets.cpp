#include <tools/STD140Offsets.h>

using namespace Twin2Engine::Tools;
using namespace std;

hash<string> STD140Offsets::_hasher;

const char* const STD140Offsets::_arrayElemFormat = "{0}[{1}]";
const char* const STD140Offsets::_subElemFormat = "{0}.{1}";

const char* const STD140Offsets::tracy_AddScalar = "STD140Offsets AddScalar";
const char* const STD140Offsets::tracy_AddScalarArray = "STD140Offsets AddScalarArray";
const char* const STD140Offsets::tracy_AddVec = "STD140Offsets AddVecArray";
const char* const STD140Offsets::tracy_AddVecArray = "STD140Offsets AddVecArray";
const char* const STD140Offsets::tracy_AddMat = "STD140Offsets AddMat";
const char* const STD140Offsets::tracy_AddMatArray = "STD140Offsets AddMatArray";
const char* const STD140Offsets::tracy_AddMatArrayElem = "STD140Offsets AddMatArray Add Elemement";
const char* const STD140Offsets::tracy_AddMatArraySetBeginPointer = "STD140Offsets AddMatArray Set Begin Pointer";
const char* const STD140Offsets::tracy_AddStruct = "STD140Offsets AddStruct";
const char* const STD140Offsets::tracy_AddStructSetElem = "STD140Offsets AddStruct Set Element";
const char* const STD140Offsets::tracy_AddStructAddPadding = "STD140Offsets AddStruct Add Padding";
const char* const STD140Offsets::tracy_AddStructArray = "STD140Offsets AddStructArray";
const char* const STD140Offsets::tracy_AddStructArrayAddElems = "STD140Offsets AddStructArray Add Elements";
const char* const STD140Offsets::tracy_AddStructArrayAddElem = "STD140Offsets AddStructArray Add Element";
const char* const STD140Offsets::tracy_AddStructArrayAddElemSubValues = "STD140Offsets AddStructArray Add Element Sub Values";
const char* const STD140Offsets::tracy_AddStructArrayAddElemSubValue = "STD140Offsets AddStructArray Add Element Sub Value";
const char* const STD140Offsets::tracy_AddStructArrayAddElemPadding = "STD140Offsets AddStructArray Add Element Padding";
const char* const STD140Offsets::tracy_AddStructArraySetBeginPointer = "STD140Offsets AddStructArray Set Begin Pointer";

bool STD140Offsets::_CheckVariable(const std::string& name) const
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

size_t STD140Offsets::_Add(const string& name, size_t baseAligement, size_t baseOffset)
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

vector<size_t> STD140Offsets::_AddArray(const string& name, size_t arraySize, size_t baseAligement, size_t baseOffset)
{
	static const char* const tracy_AddArray = "STD140Offsets AddArray";
	static const char* const tracy_AddArraySetBaseAligement = "STD140Offsets AddArray Set Base Aligement";
	static const char* const tracy_AddArrayCheckMaxAligement = "STD140Offsets AddArray Check Max Aligement";
	static const char* const tracy_AddArrayCalculateAligementOffset = "STD140Offsets AddArray Calculate Aligement Offset";
	static const char* const tracy_AddArrayAddValues = "STD140Offsets AddArray Add Values";
	static const char* const tracy_AddArrayAddValuesAddElem = "STD140Offsets AddArray Add Values Add Element";
	static const char* const tracy_AddArrayAddValuesElemName = "STD140Offsets AddArray Add Values Element Name";
	static const char* const tracy_AddArrayAddValuesElemNameHash = "STD140Offsets AddArray Add Values Element Name Hash";
	static const char* const tracy_AddArrayAddValuesAddElemOffset = "STD140Offsets AddArray Add Values Add Element Offset";
	static const char* const tracy_AddArrayAddValuesAddElemName = "STD140Offsets AddArray Add Values Add Element Name";
	static const char* const tracy_AddArrayUpdateSize = "STD140Offsets AddArray Update Size";
	static const char* const tracy_AddArraySetBeginPointer = "STD140Offsets AddArray Set Begin Pointer";
	ZoneScoped;

	FrameMarkStart(tracy_AddArray);

	vector<size_t> arrayElemOffsets;
	if (arraySize != 0) {
		
		// SET BASE ALIGEMENT
		FrameMarkStart(tracy_AddArraySetBaseAligement);
		if (baseAligement % 16 != 0) {
			baseAligement += 16 - (baseAligement % 16);
		}
		FrameMarkEnd(tracy_AddArraySetBaseAligement);

		// CHECK MAX ALIGEMENT
		FrameMarkStart(tracy_AddArrayCheckMaxAligement);
		if (baseAligement > _maxAligement) {
			_maxAligement = baseAligement;
		}
		FrameMarkEnd(tracy_AddArrayCheckMaxAligement);

		// CALCULATE ALIGEMENT OFFSET]
		FrameMarkStart(tracy_AddArrayCalculateAligementOffset);
		size_t aligementOffset = _currentOffset;
		if (aligementOffset % baseAligement != 0) {
			aligementOffset += baseAligement - (aligementOffset % baseAligement);
		}
		FrameMarkEnd(tracy_AddArrayCalculateAligementOffset);
		
		// ADD ARRAY VALUES
		FrameMarkStart(tracy_AddArrayAddValues);
		string valueName;
		size_t valueNameHash;
		for (size_t i = 0; i < arraySize; ++i) {
			FrameMarkStart(tracy_AddArrayAddValuesAddElem);

			// ELEMENT VALUE NAME
			FrameMarkStart(tracy_AddArrayAddValuesElemName);
			valueName = move(vformat(_arrayElemFormat, make_format_args(name, i)));
			FrameMarkEnd(tracy_AddArrayAddValuesElemName);

			// ELEMENT VALUE NAME HASH
			FrameMarkStart(tracy_AddArrayAddValuesElemNameHash);
			valueNameHash = move(_hasher(valueName));
			FrameMarkEnd(tracy_AddArrayAddValuesElemNameHash);

			// SET ELEMENT VALUE OFFSET
			FrameMarkStart(tracy_AddArrayAddValuesAddElemOffset);
			arrayElemOffsets.push_back((_offsets[valueNameHash] = aligementOffset + i * baseAligement));
			FrameMarkEnd(tracy_AddArrayAddValuesAddElemOffset);

			// SET ELEMENT VALUE NAME
			FrameMarkStart(tracy_AddArrayAddValuesAddElemName);
			_names[valueNameHash] = move(valueName);
			FrameMarkEnd(tracy_AddArrayAddValuesAddElemName);

			FrameMarkEnd(tracy_AddArrayAddValuesAddElem);
		}
		FrameMarkEnd(tracy_AddArrayAddValues);

		// UPDATE SIZE
		FrameMarkStart(tracy_AddArrayUpdateSize);
		_currentOffset = aligementOffset + arraySize * baseAligement;	
		FrameMarkEnd(tracy_AddArrayUpdateSize);

		// SET ARRAY BEGIN POINTER
		FrameMarkStart(tracy_AddArraySetBeginPointer);
		size_t nameHash = move(_hasher(name));
		_offsets[nameHash] = arrayElemOffsets[0];
		_names[nameHash] = name;
		FrameMarkEnd(tracy_AddArraySetBeginPointer);
	}

	FrameMarkEnd(tracy_AddArray);
	return arrayElemOffsets;
}

/*const string& STD140Offsets::_GetArrayElemFormat() {
	static const string arrayElemFormat = "{0}[{1}]";
	return arrayElemFormat;
}

const string& STD140Offsets::_GetSubElemFormat() {
	static const string subElemFormat = "{0}.{1}";
	return subElemFormat;
}*/

size_t STD140Offsets::Get(const string& name) const
{
	static const char* const tracy_Get = "STD140Offsets Get";
	ZoneScoped;

	FrameMarkStart(tracy_Get);
	size_t value = 0;
	map<size_t, size_t>::const_iterator map_iterator = _offsets.find(move(_hasher(name)));
	if (map_iterator != _offsets.end()) {
		value = (*map_iterator).second;
	}
	FrameMarkEnd(tracy_Get);
	return value;
}

vector<size_t> STD140Offsets::GetArray(const string& name) const
{
	static const char* const tracy_GetArray = "STD140Offsets GetArray";
	static const char* const tracy_GetArrayFindElemOffset = "STD140Offsets GetArray Find Element Offset";
	static const char* const tracy_GetArrayGetElemOffset = "STD140Offsets GetArray Get Element Offset";
	ZoneScoped;

	FrameMarkStart(tracy_GetArray);

	vector<size_t> values;

	FrameMarkStart(tracy_GetArrayFindElemOffset);
	map<size_t, size_t>::const_iterator map_iterator = _offsets.find(move(_hasher(move(vformat(_arrayElemFormat, make_format_args(name, 0))))));
	FrameMarkEnd(tracy_GetArrayFindElemOffset);

	size_t i = 1;
	while (map_iterator != _offsets.end()) {
		FrameMarkStart(tracy_GetArrayGetElemOffset);
		values.push_back((*map_iterator).second);

		FrameMarkStart(tracy_GetArrayFindElemOffset);
		map_iterator = _offsets.find(move(_hasher(move(vformat(_arrayElemFormat, make_format_args(name, i++))))));
		FrameMarkEnd(tracy_GetArrayFindElemOffset);

		FrameMarkEnd(tracy_GetArrayGetElemOffset);
	}

	FrameMarkEnd(tracy_GetArray);
	return values;
}

size_t STD140Offsets::GetBaseAligement() const
{
	static const char* const tracy_GetBaseAligement = "STD140Offsets GetBaseAligement";
	ZoneScoped;

	FrameMarkStart(tracy_GetBaseAligement);
	size_t baseAligement = _maxAligement;
	if (_maxAligement % 16 != 0) {
		baseAligement += 16 - (baseAligement % 16);
	}
	FrameMarkEnd(tracy_GetBaseAligement);
	return baseAligement;
}

size_t STD140Offsets::GetSize() const
{
	static const char* const tracy_GetSize = "STD140Offsets GetSize";
	ZoneScoped;

	FrameMarkStart(tracy_GetSize);
	size_t size = _currentOffset;
	if (size % 16 != 0) {
		size += 16 - (size % 16);
	}
	FrameMarkEnd(tracy_GetSize);
	return size;
}

void STD140Offsets::Clear()
{
	static const char* const tracy_Clear = "STD140Offsets Clear";
	ZoneScoped;

	FrameMarkStart(tracy_Clear);
	_currentOffset = 0;
	_maxAligement = 0;

	_offsets.clear();
	_names.clear();
	FrameMarkEnd(tracy_Clear);
}