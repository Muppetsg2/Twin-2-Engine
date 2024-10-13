#include <tools/STD140Struct.h>

using namespace Twin2Engine::Tools;
using namespace std;

size_t STD140Struct::_GetArrayElemSize(const vector<size_t>& offsets) const
{
	if (offsets.size() > 1) {
		return offsets[1] - offsets[0];
	}
	else {
		return _data.size() - offsets[0];
	}
}

void STD140Struct::_AddStruct(const string& name, const STD140Struct& value)
{
	// ADD TO OFFSETS
	size_t valueOffset = _dataOffsets.Add(name, value._dataOffsets);

	// CHECK ERROR
	if (valueOffset == 0 && _data.size() != 0) {
		SPDLOG_ERROR("Variable '{0}' already added to structure", name);
		return;
	}

	// RESERVE SIZE
	_data.reserve(_dataOffsets.GetSize());

	// CHECK PADDING
	if (_data.size() < valueOffset) {
		_data.resize(valueOffset);
	}

	// SET VALUE DATA
	_data.insert(_data.begin() + valueOffset, value._data.begin(), value._data.end());

	// CHECK DATA SIZE
	if (_data.size() < _data.capacity()) {
		_data.resize(_data.capacity());
	}
}

void STD140Struct::_AddStructArray(const string& name, const STD140Offsets& structOffsets, const vector<vector<char>>& values)
{
	// CHECK SIZE
	if (values.size() == 0) return;

	// ADD TO OFFSETS
	vector<size_t> valuesOffsets = _dataOffsets.Add(name, structOffsets, values.size());

	// CHECK ERROR
	if (valuesOffsets.size() == 0) {
		SPDLOG_ERROR("Variable '{0}' already added to structure", name);
		return;
	}

	// UPDATE SIZE
	_data.reserve(_dataOffsets.GetSize());

	// SET VALUES DATA
	for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
		// CHECK PADDING
		if (_data.size() < valuesOffsets[i]) {
			_data.resize(valuesOffsets[i]);
		}

		// SET VALUE DATA
		_data.insert(_data.begin() + valuesOffsets[i], values[i].begin(), values[i].end());
	}

	// CLEAR VALUES OFFSETS
	valuesOffsets.clear();

	// CHECK DATA SIZE
	if (_data.size() < _data.capacity()) {
		_data.resize(_data.capacity());
	}
}

bool STD140Struct::_SetStruct(const string& name, const STD140Struct& value)
{
	// CHECK VARIABLE
	if (!_dataOffsets.Contains(name)) {
		SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
		return false;
	}

	// GET OFFSET
	size_t valueOffset = _dataOffsets.Get(name);

	// SET VALUE DATA
	memcpy(_data.data() + valueOffset, value._data.data(), min(value._data.size(), _data.size() - valueOffset));

	return true;
}

bool STD140Struct::_SetStructArray(const string& name, const STD140Offsets& structOffsets, const vector<vector<char>>& values)
{
	// CHECK SIZE
	if (values.size() == 0) return false;

	// CHECK VARIABLE
	if (!_dataOffsets.Contains(name)) {
		SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
		return false;
	}

	// GET OFFSETS
	vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

	// CHECK ARRAY ELEMENTS OFFSETS
	if (valuesOffsets.size() == 0) {
		SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
		return false;
	}

	// GET ARRAY ELEM DATA MAX SIZE
	size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);

	// SET VALUES DATA
	size_t valueDataSize;
	for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
		// GET VALUE DATA MAX SIZE
		size_t valueDataSize = min(min(values[i].size(), arrayElemDataSize), _data.size() - valuesOffsets[i]);

		// SET VALUE DATA
		memcpy(_data.data() + valuesOffsets[i], values[i].data(), valueDataSize);
	}

	// CLEAR VALUES OFFSETS
	valuesOffsets.clear();

	return true;
}

STD140Struct STD140Struct::_GetStruct(const string& name, const STD140Offsets& structOffsets) const
{
	// CHECK VARIABLE
	if (!_dataOffsets.Contains(name)) {
		SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
		return STD140Struct(structOffsets);
	}

	// GET VALUE OFFSET
	size_t valueOffset = _dataOffsets.Get(name);

	// MAKE EMPTY STRUCT
	STD140Struct value(structOffsets);

	// GET MAX VALUE DATA
	size_t valueDataSize = min(structOffsets.GetSize(), _data.size() - valueOffset);

	// SET VALUE DATA
	memcpy(value._data.data(), _data.data() + valueOffset, valueDataSize);

	// RETURN VALUE
	return value;
}

vector<STD140Struct> STD140Struct::_GetStructArray(const string& name, const STD140Offsets& structOffsets) const
{
	// CHECK VARIABLE
	if (!_dataOffsets.Contains(name)) {
		SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
		return vector<STD140Struct>();
	}

	// GET VALUES OFFSETS
	vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

	// CHECK ARRAY ELEMENTS OFFSETS
	if (valuesOffsets.size() == 0) {
		SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
		return vector<STD140Struct>();
	}

	// GET ARRAY ELEM DATA MAX SIZE
	size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);

	// GET VALUES DATA
	vector<STD140Struct> values;
	for (size_t i = 0; i < valuesOffsets.size(); ++i) {
		// MAKE EMPTY STRUCT
		STD140Struct value(structOffsets);

		// GET MAX VALUE DATA
		size_t valueDataSize = min(min(structOffsets.GetSize(), arrayElemDataSize), _data.size() - valuesOffsets[i]);

		// SET VALUE DATA
		memcpy(value._data.data(), _data.data() + valuesOffsets[i], valueDataSize);

		// ADD VALUE TO VALUES
		values.push_back(value);
	}

	// RETURN VALUE
	return values;
}

STD140Struct::STD140Struct(const STD140Offsets& structOffsets, const vector<char>& data)
{
	_dataOffsets = structOffsets;
	_data.reserve(_dataOffsets.GetSize());
	_data.insert(_data.begin(), data.begin(), data.begin() + std::min(data.size(), _data.capacity()));
	if (_data.size() < _data.capacity()) {
		_data.resize(_data.capacity());
	}
}

STD140Struct::STD140Struct(STD140Struct& std140s) {
	std140s.CloneTo(this);
}

STD140Struct::STD140Struct(const STD140Struct& std140s) {
	std140s.CloneTo(this);
}

STD140Struct::STD140Struct(STD140Struct&& std140s) {
	std140s.CloneTo(this);
}

STD140Struct& STD140Struct::operator=(STD140Struct& std140s) {
	std140s.CloneTo(this);
	return *this;
}

STD140Struct& STD140Struct::operator=(const STD140Struct& std140s) {
	std140s.CloneTo(this);
	return *this;
}

STD140Struct& STD140Struct::operator=(STD140Struct&& std140s) {
	std140s.CloneTo(this);
	return *this;
}

CloneFuncDefinition(Twin2Engine::Tools::STD140Struct, StandardClone(_dataOffsets), StandardClone(_data))

STD140Struct::~STD140Struct()
{
	Clear();
}

void STD140Struct::Add(const string& name, const STD140Struct& value)
{
	_AddStruct(name, value);
}

void STD140Struct::Add(const string& name, const STD140Offsets& structOffsets, const vector<char>*& values, size_t size)
{
	_ConvertArray<vector<char>, vector<char>, const vector<char>*&, void>(name, values, size,
		[&](const string& name, const vector<vector<char>>& convs) -> void {
			_AddStructArray(name, structOffsets, convs);
	});
}

void STD140Struct::Add(const string& name, const STD140Offsets& structOffsets, const vector<vector<char>>& values)
{
	_AddStructArray(name, structOffsets, values);
}

bool STD140Struct::Set(const string& name, const STD140Struct& value)
{
	return _SetStruct(name, value);
}

bool STD140Struct::Set(const string& name, const STD140Offsets& structOffsets, const vector<char>*& values, size_t size)
{
	return _ConvertArray<vector<char>, vector<char>, const vector<char>*&, bool>(name, values, size,
		[&](const string& name, const vector<vector<char>>& values) -> bool {
			return _SetStructArray(name, structOffsets, values);
	});
}

bool STD140Struct::Set(const string& name, const STD140Offsets& structOffsets, const vector<vector<char>>& values)
{
	return _SetStructArray(name, structOffsets, values);
}

STD140Offsets STD140Struct::GetOffsets() const
{
	return _dataOffsets;
}

size_t STD140Struct::GetOffset(const string& name) const
{
	return _dataOffsets.Get(name);
}

vector<size_t> STD140Struct::GetArrayOffsets(const string& name) const
{
	return _dataOffsets.GetArray(name);
}

#if _DEBUG
const ValueType* STD140Struct::GetType(const string& name) const
{
	return _dataOffsets.GetType(name);
}

std::vector<std::string> STD140Struct::GetNames() const
{
	return _dataOffsets.GetNames();
}
#endif

vector<char> STD140Struct::GetData() const
{
	return _data;
}

size_t STD140Struct::GetBaseAligement() const
{
	return _dataOffsets.GetBaseAligement();
}

size_t STD140Struct::GetSize() const
{
	return _data.size();
}

void STD140Struct::ClearData() {
	memset(_data.data(), 0, _data.size());
}

void STD140Struct::Clear()
{
	_dataOffsets.Clear();
	_data.clear();
}