#include <tools/ValueTypes.h>
#include <tools/STD140Offsets.h>

#if _DEBUG

using namespace Twin2Engine::Tools;

ScalarType::ScalarType(const VALUE_TYPE& type) : ValueType(), _type(type) {}

VALUE_TYPE ScalarType::GetType() const {
	return _type;
}

VecType::VecType(const VALUE_TYPE& type, const size_t& length) : ValueType(), _type(type), _length(length) {}

VALUE_TYPE VecType::GetType() const
{
	return _type;
}

size_t VecType::GetLength() const
{
	return _length;
}

MatType::MatType(const VALUE_TYPE& type, const size_t& cols, const size_t& rows) : ValueType(), _type(type), _cols(cols), _rows(rows) {}

VALUE_TYPE MatType::GetType() const
{
	return _type;
}

size_t MatType::GetRows() const
{
	return _rows;
}

size_t MatType::GetCols() const
{
	return _cols;
}

StructType::StructType(const STD140Offsets& offsets) : ValueType() {
	_offsets = new STD140Offsets(offsets);
}

StructType::~StructType() {
	delete _offsets;
}

DefineCloneBaseFunc(StructType, ValueType, PointerDeepClone(_offsets, STD140Offsets))

const STD140Offsets* StructType::GetOffsets() const
{
	return _offsets;
}

ArrayType::ArrayType(const ValueType*& type, const size_t& length) : ValueType(), _length(length) {
	_type = type;
}

ArrayType::~ArrayType() {
	delete _type;
}

const ValueType* ArrayType::GetType() const
{
	return _type;
}

size_t ArrayType::GetLength() const
{
	return _length;
}

#endif