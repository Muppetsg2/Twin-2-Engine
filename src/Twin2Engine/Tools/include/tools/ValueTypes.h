#pragma once
#include <tools/macros.h>

#if _DEBUG

namespace Twin2Engine::Tools {
	ENUM_CLASS_BASE(VALUE_TYPE, uint8_t, OTHER, BOOL, INT, UINT, FLOAT, DOUBLE);

	template<class T>
	static VALUE_TYPE GetValueType() {
		if constexpr (std::is_same_v<T, bool>) {
			return VALUE_TYPE::BOOL;
		}
		else if constexpr (std::is_same_v<T, int>) {
			return VALUE_TYPE::INT;
		}
		else if constexpr (std::is_same_v<T, unsigned int>) {
			return VALUE_TYPE::UINT;
		}
		else if constexpr (std::is_same_v<T, float>) {
			return VALUE_TYPE::FLOAT;
		}
		else if constexpr (std::is_same_v<T, double>) {
			return VALUE_TYPE::BOOL;
		}
		else {
			return VALUE_TYPE::OTHER;
		}
	}

	class ValueType {
	protected:
		ValueType() = default;
	public:
		virtual ~ValueType() = default;

		CloneFunc(ValueType);
	};

	class ScalarType : public ValueType {
	private:
		VALUE_TYPE _type = VALUE_TYPE::OTHER;

	protected:
		ScalarType() = default;
	public:
		ScalarType(const VALUE_TYPE& type);
		virtual ~ScalarType() = default;

		CloneBaseFunc(ScalarType, ValueType, _type)

		VALUE_TYPE GetType() const;
	};

	class VecType : public ValueType {
	private:
		VALUE_TYPE _type = VALUE_TYPE::OTHER;
		size_t _length = 0;

	protected:
		VecType() = default;

	public:
		VecType(const VALUE_TYPE& type, const size_t& length);
		virtual ~VecType() = default;

		CloneBaseFunc(VecType, ValueType, _type, _length)

		VALUE_TYPE GetType() const;
		size_t GetLength() const;
	};

	class MatType : public ValueType {
	private:
		VALUE_TYPE _type = VALUE_TYPE::OTHER;
		size_t _cols = 0;
		size_t _rows = 0;

	protected:
		MatType() = default;
	public:
		MatType(const VALUE_TYPE& type, const size_t& cols, const size_t& rows);
		virtual ~MatType() = default;

		CloneBaseFunc(MatType, ValueType, _type, _cols, _rows)

		VALUE_TYPE GetType() const;
		size_t GetRows() const;
		size_t GetCols() const;
	};

	class STD140Offsets;

	class StructType : public ValueType {
	private:
		const STD140Offsets* _offsets = nullptr;

	protected:
		StructType() = default;
	public:
		StructType(const STD140Offsets& offsets);
		virtual ~StructType();

		DeclareCloneBaseFunc(StructType);

		const STD140Offsets* GetOffsets() const;
	};

	class ArrayType : public ValueType {
	private:
		const ValueType* _type = nullptr;
		size_t _length = 0;

	protected:
		ArrayType() = default;
	public:
		ArrayType(const ValueType*& type, const size_t& length);
		virtual ~ArrayType();

		CloneAdvancedBaseFunc(ArrayType, ValueType, _type, _type->Clone(), StandardClone(_length));

		const ValueType* GetType() const;
		size_t GetLength() const;
	};
}

#endif