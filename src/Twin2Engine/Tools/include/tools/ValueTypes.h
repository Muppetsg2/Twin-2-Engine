#pragma once
#include <tools/macros.h>

namespace Twin2Engine::Tools {
	ENUM_CLASS_BASE(VALUE_TYPE, uint8_t, BOOL, INT, UINT, FLOAT, DOUBLE);

	class ValueType {};

	class ScalarType : public ValueType {
	private:
		const VALUE_TYPE _type;

	public:
		ScalarType(const VALUE_TYPE& type);
		~ScalarType() = default;

		VALUE_TYPE GetType() const;
	};

	class VecType : public ValueType {
	private:
		const VALUE_TYPE _type;
		const size_t _length;

	public:
		VecType(const VALUE_TYPE& type, const size_t& length);
		~VecType() = default;

		VALUE_TYPE GetType() const;
		size_t GetLength() const;
	};

	class MatType : public ValueType {
	private:
		const VALUE_TYPE _type;
		const size_t _cols;
		const size_t _rows;

	public:
		MatType(const VALUE_TYPE& type, const size_t& cols, const size_t& rows);
		~MatType() = default;

		VALUE_TYPE GetType() const;
		size_t GetRows() const;
		size_t GetCols() const;
	};

	class STD140Offsets;

	class StructType : public ValueType {
	private:
		const STD140Offsets* _offsets;

	public:
		StructType(const STD140Offsets*& offsets);
		~StructType() = default;

		const STD140Offsets* GetOffsets() const;
	};

	class ArrayType : public ValueType {
	private:
		const ValueType* _type;
		const size_t _length;

	public:
		ArrayType(const ValueType*& type, const size_t& length);
		~ArrayType();

		const ValueType* GetType() const;
		size_t GetLength() const;
	};
}