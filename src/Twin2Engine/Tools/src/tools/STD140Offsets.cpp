#include <tools/STD140Offsets.h>

using namespace Twin2Engine::Tools;
using namespace std;

hash<string> STD140Offsets::_hasher;

const char* const STD140Offsets::_arrayElemFormat = "{0}[{1}]";
const char* const STD140Offsets::_subElemFormat = "{0}.{1}";

bool STD140Offsets::_CheckVariable(const std::string& name) const
{
	bool c = move(Contains(name));
	if (c) {
		SPDLOG_ERROR("Variable '{0}' already added to structure", name);
	}
	return c;
}

STD140Offsets::~STD140Offsets()
{
	Clear();
}

STD140Offsets::STD140Offsets(STD140Offsets& std140off)
{
	std140off.CloneTo(this);
}

STD140Offsets::STD140Offsets(const STD140Offsets& std140off)
{
	std140off.CloneTo(this);
}

STD140Offsets::STD140Offsets(STD140Offsets&& std140off)
{
	std140off.CloneTo(this);
}

STD140Offsets& STD140Offsets::operator=(STD140Offsets& std140off)
{
	std140off.CloneTo(this);
	return *this;
}

STD140Offsets& STD140Offsets::operator=(const STD140Offsets& std140off)
{
	std140off.CloneTo(this);
	return *this;
}

STD140Offsets& STD140Offsets::operator=(STD140Offsets&& std140off)
{
	std140off.CloneTo(this);
	return *this;
}

#if _DEBUG
DefineCloneFunc(Twin2Engine::Tools::STD140Offsets,
	StandardClone(_currentOffset),
	StandardClone(_maxAligement),
	StandardClone(_offsets),
	StandardClone(_names),
	_types, _types; for (const auto& type : _types) { cloned->_types[type.first] = type.second->Clone(); }
)
#else
DefineCloneFunc(Twin2Engine::Tools::STD140Offsets,
	StandardClone(_currentOffset),
	StandardClone(_maxAligement),
	StandardClone(_offsets),
	StandardClone(_names)
)
#endif

bool STD140Offsets::Contains(const std::string& name) const
{
	return _offsets.contains(move(_hasher(name)));
}

size_t STD140Offsets::_Add(const string& name, size_t baseAligement, size_t baseOffset
#if _DEBUG
	, const ValueType* type
#endif
	)
{
	size_t nameHash = move(_hasher(name));

	// CALCULATE ALIGEMENT
	size_t aligementOffset = move(_currentOffset);
	if (aligementOffset % baseAligement != 0) {
		aligementOffset += baseAligement - (aligementOffset % baseAligement);
	}

	_offsets[nameHash] = aligementOffset;
	_names[nameHash] = name;
#if _DEBUG
	_types[nameHash] = type;
#endif

	// UPDATE SIZE
	_currentOffset = aligementOffset + baseOffset;

	// CHECK MAX ALIGEMENT
	if (baseAligement > _maxAligement) {
		_maxAligement = move(baseAligement);
	}
	return aligementOffset;
}

vector<size_t> STD140Offsets::_AddArray(const string& name, size_t arraySize, size_t baseAligement, size_t baseOffset
#if _DEBUG
	, const ValueType* typeTemplate
#endif	
	)
{
	vector<size_t> arrayElemOffsets;
	if (arraySize != 0) {
		
		// SET BASE ALIGEMENT
		if (baseAligement % 16 != 0) {
			baseAligement += 16 - (baseAligement % 16);
		}

		// CHECK MAX ALIGEMENT
		if (baseAligement > _maxAligement) {
			_maxAligement = baseAligement;
		}

		// CALCULATE ALIGEMENT OFFSET]
		size_t aligementOffset = _currentOffset;
		if (aligementOffset % baseAligement != 0) {
			aligementOffset += baseAligement - (aligementOffset % baseAligement);
		}

		// ADD ARRAY VALUES
		string valueName;
		size_t valueNameHash;
		for (size_t i = 0; i < arraySize; ++i) {
			// ELEMENT VALUE NAME
			valueName = move(vformat(_arrayElemFormat, make_format_args(name, i)));
			
			// ELEMENT VALUE NAME HASH
			valueNameHash = move(_hasher(valueName));

			// SET ELEMENT VALUE OFFSET
			arrayElemOffsets.push_back((_offsets[valueNameHash] = aligementOffset + i * baseAligement));

			// SET ELEMENT VALUE NAME
			_names[valueNameHash] = move(valueName);
#if _DEBUG
			// SET ELEMENT VALUE TYPE
			_types[valueNameHash] = typeTemplate->Clone();
#endif
		}

		// UPDATE SIZE
		_currentOffset = aligementOffset + arraySize * baseAligement;

		// SET ARRAY BEGIN POINTER
		size_t nameHash = move(_hasher(name));
		_offsets[nameHash] = arrayElemOffsets[0];
		_names[nameHash] = name;
#if _DEBUG
		_types[nameHash] = new ArrayType(typeTemplate, arraySize);
#endif
	}

	return arrayElemOffsets;
}

size_t STD140Offsets::Get(const string& name) const
{
	size_t value = 0;
	unordered_map<size_t, size_t>::const_iterator map_iterator = _offsets.find(move(_hasher(name)));
	if (map_iterator != _offsets.end()) {
		value = (*map_iterator).second;
	}
	return value;
}

vector<size_t> STD140Offsets::GetArray(const string& name) const
{
	vector<size_t> values;

	unordered_map<size_t, size_t>::const_iterator map_iterator = _offsets.find(move(_hasher(move(vformat(_arrayElemFormat, make_format_args(name, unmove(0)))))));

	size_t i = 1;
	while (map_iterator != _offsets.end()) {
		values.push_back((*map_iterator).second);

		map_iterator = _offsets.find(move(_hasher(move(vformat(_arrayElemFormat, make_format_args(name, unmove(i++)))))));
	}

	return values;
}

#if _DEBUG
const ValueType* STD140Offsets::GetType(const string& name) const
{
	const ValueType* value = nullptr;
	unordered_map<size_t, const ValueType*>::const_iterator map_iterator = _types.find(move(_hasher(name)));
	if (map_iterator != _types.end()) {
		value = (*map_iterator).second;
	}
	return value;
}

std::vector<std::string> STD140Offsets::GetNames() const
{
	std::vector<std::string> names = std::vector<std::string>();
	names.reserve(_names.size());

	for (auto name : _names) {
		names.push_back(name.second);
	}

	return names;
}
#endif

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
#if _DEBUG
	for (auto& type : _types) {
		if (type.second == nullptr) continue;
		delete type.second;
		type.second = nullptr;
	}
	_types.clear();
#endif
}