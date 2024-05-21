#include <tools/STD140Struct.h>

using namespace Twin2Engine::Tools;
using namespace std;

const char* const STD140Struct::tracy__GetValueData = "STD140Struct _GetValueData";

const char* const STD140Struct::tracy__Add = "STD140Struct _Add";
const char* const STD140Struct::tracy__AddGetOffset = "STD140Struct _Add Get Offset";
const char* const STD140Struct::tracy__AddCheckError = "STD140Struct _Add Check Error";
const char* const STD140Struct::tracy__AddReserveSize = "STD140Struct _Add Reserve Size";
const char* const STD140Struct::tracy__AddCheckValuePadding = "STD140Struct _Add Check Value Padding";
const char* const STD140Struct::tracy__AddGetValueData = "STD140Struct _Add Get Value Data";
const char* const STD140Struct::tracy__AddSetValueData = "STD140Struct _Add Set Value Data";
const char* const STD140Struct::tracy__AddClearTempValueData = "STD140Struct _Add Clear Temp Value Data";
const char* const STD140Struct::tracy__AddUpdateSize = "STD140Struct _Add Update Size";

const char* const STD140Struct::tracy__AddArray = "STD140Struct _AddArray";
const char* const STD140Struct::tracy__AddArrayCheckSize = "STD140Struct _AddArray Check Size";
const char* const STD140Struct::tracy__AddArrayGetOffsets = "STD140Struct _AddArray Get Offsets";
const char* const STD140Struct::tracy__AddArrayCheckError = "STD140Struct _AddArray Check Error";
const char* const STD140Struct::tracy__AddArrayReserveSize = "STD140Struct _AddArray Reserve Size";
const char* const STD140Struct::tracy__AddArraySetValuesData = "STD140Struct _AddArray Set Values Data";
const char* const STD140Struct::tracy__AddArrayCheckValuePadding = "STD140Struct _AddArray Check Value Padding";
const char* const STD140Struct::tracy__AddArrayGetValueData = "STD140Struct _AddArray Get Value Data";
const char* const STD140Struct::tracy__AddArraySetValueData = "STD140Struct _AddArray Set Value Data";
const char* const STD140Struct::tracy__AddArrayClearTempValueData = "STD140Struct _AddArray Clear Temp Value Data";
const char* const STD140Struct::tracy__AddArrayClearValuesOffsets = "STD140Struct _AddArray Clear Values Offsets";
const char* const STD140Struct::tracy__AddArrayUpdateSize = "STD140Struct _AddArray Update Size";

const char* const STD140Struct::tracy__AddArrayConvert = "STD140Struct _AddArrayConvert";
const char* const STD140Struct::tracy__AddArrayConvertValues = "STD140Struct _AddArrayConvert Values";
const char* const STD140Struct::tracy__AddArrayConvertValue = "STD140Struct _AddArrayConvert Value";
const char* const STD140Struct::tracy__AddArrayConvertAddArray = "STD140Struct _AddArrayConvert AddArray";

const char* const STD140Struct::tracy__Set = "STD140Struct _Set";
const char* const STD140Struct::tracy__SetCheckVariable = "STD140Struct _Set Check Variable";
const char* const STD140Struct::tracy__SetGetOffset = "STD140Struct _Set Get Offset";
const char* const STD140Struct::tracy__SetGetValueData = "STD140Struct _Set Get Value Data";
const char* const STD140Struct::tracy__SetSetValueData = "STD140Struct _Set Set Value Data";
const char* const STD140Struct::tracy__SetClearTempValueData = "STD140Struct _Set Clear Temp Value Data";

const char* const STD140Struct::tracy__SetArray = "STD140Struct _SetArray";
const char* const STD140Struct::tracy__SetArrayCheckSize = "STD140Struct _SetArray Check Size";
const char* const STD140Struct::tracy__SetArrayCheckVariable = "STD140Struct _SetArray Check Variable";
const char* const STD140Struct::tracy__SetArrayGetValuesOffsets = "STD140Struct _SetArray Get Values Offsets";
const char* const STD140Struct::tracy__SetArrayCheckValuesOffsets = "STD140Struct _SetArray Check Values Offsets";
const char* const STD140Struct::tracy__SetArrayGetElemSize = "STD140Struct _SetArray Get Element Size";
const char* const STD140Struct::tracy__SetArraySetValues = "STD140Struct _SetArray Set Values";
const char* const STD140Struct::tracy__SetArrayGetValueData = "STD140Struct _SetArray Get Value Data";
const char* const STD140Struct::tracy__SetArraySetValueData = "STD140Struct _SetArray Set Value Data";
const char* const STD140Struct::tracy__SetArrayClearTempValueData = "STD140Struct _SetArray Clear Temp Value Data";
const char* const STD140Struct::tracy__SetArrayClearValuesOffsets = "STD140Struct _SetArray Clear Values Offsets";

const char* const STD140Struct::tracy__SetArrayConvert = "STD140Struct _SetArrayConvert";
const char* const STD140Struct::tracy__SetArrayConvertValues = "STD140Struct _SetArrayConvert Values";
const char* const STD140Struct::tracy__SetArrayConvertValue = "STD140Struct _SetArrayConvert Value";

size_t STD140Struct::_GetArrayElemSize(const vector<size_t>& offsets) const
{
	ZoneScoped;

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

	valueData.clear();
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

		valueData.clear();
	}

	valuesOffsets.clear();
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

	valueData.clear();

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

		valueData.clear();
	}

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