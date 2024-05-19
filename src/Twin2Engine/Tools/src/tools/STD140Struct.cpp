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

	// UPDATE SIZE
	_data.resize(_dataOffsets.GetSize());

	// GET VALUE DATA
	vector<char> valueData = value._data;

	// SET VALUE DATA
	memcpy(_data.data() + valueOffset, valueData.data(), valueData.size());
}

void STD140Struct::_AddStructArray(const string& name, const STD140Offsets& structOffsets, const vector<vector<char>>& values)
{
	if (values.size() == 0) return;

	// ADD TO OFFSETS
	vector<size_t> valuesOffsets = _dataOffsets.Add(name, structOffsets, values.size());

	// CHECK ERROR
	if (valuesOffsets.size() == 0) {
		SPDLOG_ERROR("Variable '{0}' already added to structure", name);
		return;
	}

	// UPDATE SIZE
	_data.resize(_dataOffsets.GetSize());

	// SET VALUES DATA
	for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
		// GET VALUE DATA
		vector<char> valueData = values[i];

		// SET VALUE DATA
		memcpy(_data.data() + valuesOffsets[i], valueData.data(), valueData.size());
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

	// GET VALUE DATA
	vector<char> valueData = value._data;

	// GET VALUE DATA MAX SIZE
	size_t valueDataSize = min(valueData.size(), _data.size() - valueOffset);

	// SET VALUE DATA
	memcpy(_data.data() + valueOffset, valueData.data(), valueDataSize);

	return true;
}

bool STD140Struct::_SetStructArray(const string& name, const STD140Offsets& structOffsets, const vector<vector<char>>& values)
{
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
	for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
		// GET VALUE DATA
		vector<char> valueData = values[i];

		// GET VALUE DATA MAX SIZE
		size_t valueDataSize = min(min(valueData.size(), arrayElemDataSize), _data.size() - valuesOffsets[i]);

		// SET VALUE DATA
		memcpy(_data.data() + valuesOffsets[i], valueData.data(), valueDataSize);
	}

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
	_data.resize(_dataOffsets.GetSize());
	memcpy(_data.data(), data.data(), std::min(_data.size(), data.size()));
}

void STD140Struct::Add(const string& name, const STD140Struct& value)
{
	_AddStruct(name, value);
}

void STD140Struct::Add(const string& name, const STD140Offsets& structOffsets, const vector<char>*& values, size_t size)
{
	_AddArray<vector<char>, vector<char>>(name, values, size,
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
	return _SetArray<vector<char>, vector<char>>(name, values, size,
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
	_data.clear();
	_data.resize(_dataOffsets.GetSize());
}

void STD140Struct::Clear()
{
	_dataOffsets.Clear();
	_data.clear();
}