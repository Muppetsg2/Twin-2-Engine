#pragma once

#include <tools/EventHandler.h>
#include <tools/STD140Offsets.h>

namespace Twin2Engine::Tools {
	//class STD140Struct;

	/*template<class T>
	struct STD140Value {
	public:
		using var_type = T;

		const std::string var_name;
		const bool hasValue;
		const T value;
		const size_t array_size;

		template<typename = std::enable_if_t<!std::is_same_v<T, STD140Offsets>>>
		STD140Value(const std::string& name) : var_name(name), value(), hasValue(false), array_size(0) {}

		STD140Value(const std::string& name, T value) : var_name(name), value(value), hasValue(true), array_size(0) {}

		template<typename = std::enable_if_t<!std::is_same_v<T, STD140Offsets>>>
		STD140Value(const std::string& name, const size_t& size) : var_name(name), value(), hasValue(false), array_size(size) {}

		template<typename = std::enable_if_t<std::is_same_v<T, STD140Offsets>>>
		STD140Value(const std::string& name, const T& offsets, const size_t& size) : var_name(name), value(offsets), hasValue(true), array_size(size) {}
	};*/

	class STD140Struct {
	private:
#pragma region CHECKS
		template<class T> 
		static constexpr bool type_check_v = is_type_in_v<T, bool, int, unsigned int, float, double>;

		template<class V, class T, size_t L>
		static constexpr bool vec_check_v = (std::is_same_v<V, glm::vec<L, T>> && type_check_v<T> && is_num_in_range_v<L, 1, 4>);

		template<class M, class T, size_t C, size_t R>
		static constexpr bool mat_check_v = (std::is_same_v<M, glm::mat<C, R, T>> && type_check_v<T> && is_num_in_range_v<C, 1, 4> && is_num_in_range_v<R, 1, 4>);

		template<class V, class T> static constexpr bool is_vector_of_v = std::is_same_v<V, std::vector<T>>;
		template<class V, class T, bool Test> static constexpr bool get_vector_check_v = (is_vector_of_v<V, T> && Test);

		template<class T, class Ret = void> using scalar_enable_if_t = std::enable_if_t<type_check_v<T>, Ret>;
		template<class V, class T, size_t L, class Ret = void> using vec_enable_if_t = std::enable_if_t<vec_check_v<V, T, L>, Ret>;
		template<class M, class T, size_t C, size_t R, class Ret = void> using mat_enable_if_t = std::enable_if_t<mat_check_v<M, T, C, R>, Ret>;

		template<class V, class T, bool Test, class Ret = void> using get_vector_enable_if_t = std::enable_if_t<get_vector_check_v<V, T, Test>, Ret>;
#pragma endregion

		STD140Offsets _dataOffsets;
		std::vector<char> _data;

		static const char* const tracy__GetValueData;

		template<class T> 
		std::vector<char> _GetValueData(const T& value) const {
			ZoneScoped;
			FrameMarkStart(tracy__GetValueData);
			const char* valueDataPtr = reinterpret_cast<const char*>(&value);
			FrameMarkEnd(tracy__GetValueData);
			return std::vector<char>(valueDataPtr, valueDataPtr + sizeof(T));
		}

		size_t _GetArrayElemSize(const std::vector<size_t>& offsets) const;

		static const char* const tracy__ConvertArray;
		static const char* const tracy__ConvertArrayValues;
		static const char* const tracy__ConvertArrayValue;
		static const char* const tracy__ConvertArrayFunc;

		template<class S, class C, class T, class R>
		R _ConvertArray(const std::string& name, const T& values, size_t size, const Func<R, const std::string&, const std::vector<C>&>& arrayFunc) {
			ZoneScoped;
			FrameMarkStart(tracy__ConvertArray);

			if constexpr (std::is_same_v<T, std::vector<S>> && std::is_same_v<S, C>) {
				if constexpr (std::is_void_v<R>) {
					FrameMarkStart(tracy__ConvertArrayFunc);
					arrayFunc(name, values);
					FrameMarkEnd(tracy__ConvertArrayFunc);
				}
				else {
					FrameMarkEnd(tracy__ConvertArray);
					return arrayFunc(name, values);
				}
			}
			else {
				FrameMarkStart(tracy__ConvertArrayValues);
				std::vector<C> convertedValues;
				convertedValues.reserve(size);
				for (size_t i = 0; i < size; ++i) {
					FrameMarkStart(tracy__ConvertArrayValue);
					if constexpr (std::is_same_v<S, C>) {
						convertedValues.insert(convertedValues.end(), values[i]);
					}
					else {
						convertedValues.insert(convertedValues.end(), (C)values[i]);
					}
					FrameMarkEnd(tracy__ConvertArrayValue);
				}
				FrameMarkEnd(tracy__ConvertArrayValues);

				if constexpr (std::is_void_v<R>) {
					FrameMarkStart(tracy__ConvertArrayFunc);
					arrayFunc(name, std::move(convertedValues));
					FrameMarkEnd(tracy__ConvertArrayFunc);
				}
				else {
					FrameMarkEnd(tracy__ConvertArray);
					return arrayFunc(name, std::move(convertedValues));
				}
			}

			if constexpr (std::is_void_v<R>) {
				FrameMarkEnd(tracy__ConvertArray);
			}
		}

		// TODO: SPRAWDZI� KA�DY MAT (TYLKO MAT4 SPRAWDZONY)

#pragma region ADD
		/*template<class T, class... Ts>
		void _AddMultiple(const STD140Value<T>& value, const STD140Value<Ts>&... values) {
			if constexpr (std::is_same_v<T, STD140Offsets>) {
				Add(value.var_name, value.value, std::vector<char>(value.value.GetSize()));
			}
			else {
				Add(value.var_name, value.value);
			}


			if constexpr (sizeof...(Ts) > 0) {
				_AddMultiple(values...);
			}
		}*/

		static const char* const tracy__Add;
		static const char* const tracy__AddGetOffset;
		static const char* const tracy__AddCheckError;
		static const char* const tracy__AddReserveSize;
		static const char* const tracy__AddCheckValuePadding;
		static const char* const tracy__AddGetValueData;
		static const char* const tracy__AddSetValueData;
		static const char* const tracy__AddClearTempValueData;
		static const char* const tracy__AddUpdateSize;

		template<class T> void _Add(const std::string& name, const T& value) {
			ZoneScoped;
			FrameMarkStart(tracy__Add);

			// ADD TO OFFSETS
			FrameMarkStart(tracy__AddGetOffset);
			size_t valueOffset = std::move(_dataOffsets.Add<T>(name));
			FrameMarkEnd(tracy__AddGetOffset);

			// CHECK ERROR
			FrameMarkStart(tracy__AddCheckError);
			if (valueOffset == 0 && _data.size() != 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				FrameMarkEnd(tracy__AddCheckError);
				FrameMarkEnd(tracy__Add);
				return;
			}
			FrameMarkEnd(tracy__AddCheckError);

			// RESERVE SIZE
			FrameMarkStart(tracy__AddReserveSize);
			_data.reserve(_dataOffsets.GetSize());
			FrameMarkEnd(tracy__AddReserveSize);

			// CHECK VALUE PADDING
			FrameMarkStart(tracy__AddCheckValuePadding);
			if (_data.size() < valueOffset) {
				_data.resize(valueOffset);
			}
			FrameMarkEnd(tracy__AddCheckValuePadding);

			// GET VALUE DATA
			FrameMarkStart(tracy__AddGetValueData);
			std::vector<char> valueData = std::move(_GetValueData(value));
			FrameMarkEnd(tracy__AddGetValueData);

			// SET VALUE DATA
			FrameMarkStart(tracy__AddSetValueData);
			_data.insert(_data.end(), valueData.begin(), valueData.end());
			FrameMarkEnd(tracy__AddSetValueData);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__AddClearTempValueData);
			valueData.clear();
			FrameMarkEnd(tracy__AddClearTempValueData);

			// UPDATE SIZE
			FrameMarkStart(tracy__AddUpdateSize);
			if (_data.size() < _data.capacity()) {
				_data.resize(_data.capacity());
			}
			FrameMarkEnd(tracy__AddUpdateSize);

			FrameMarkEnd(tracy__Add);
		}

		static const char* const tracy__AddArray;
		static const char* const tracy__AddArrayCheckSize;
		static const char* const tracy__AddArrayGetOffsets;
		static const char* const tracy__AddArrayCheckError;
		static const char* const tracy__AddArrayReserveSize;
		static const char* const tracy__AddArraySetValuesData;
		static const char* const tracy__AddArrayCheckValuePadding;
		static const char* const tracy__AddArrayGetValueData;
		static const char* const tracy__AddArraySetValueData;
		static const char* const tracy__AddArrayClearTempValueData;
		static const char* const tracy__AddArrayClearValuesOffsets;
		static const char* const tracy__AddArrayUpdateSize;

		template<class T> void _AddArray(const std::string& name, const std::vector<T>& values) {
			ZoneScoped;
			FrameMarkStart(tracy__AddArray);

			// CHECK SIZE
			FrameMarkStart(tracy__AddArrayCheckSize);
			if (values.size() == 0) {
				FrameMarkEnd(tracy__AddArrayCheckSize);
				FrameMarkEnd(tracy__AddArray);
				return;
			}
			FrameMarkEnd(tracy__AddArrayCheckSize);

			// GET OFFSETS
			FrameMarkStart(tracy__AddArrayGetOffsets);
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.Add<T>(name, values.size()));
			FrameMarkEnd(tracy__AddArrayGetOffsets);

			// CHECK ERROR
			FrameMarkStart(tracy__AddArrayCheckError);
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				FrameMarkEnd(tracy__AddArrayCheckError);
				FrameMarkEnd(tracy__AddArray);
				return;
			}
			FrameMarkEnd(tracy__AddArrayCheckError);

			// RESERVE SIZE
			FrameMarkStart(tracy__AddArrayReserveSize);
			_data.reserve(_dataOffsets.GetSize());
			FrameMarkEnd(tracy__AddArrayReserveSize);

			// SET VALUES DATA
			FrameMarkStart(tracy__AddArraySetValuesData);
			std::vector<char> valueData;
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// CHECK VALUE PADDING
				FrameMarkStart(tracy__AddArrayCheckValuePadding);
				if (_data.size() < valuesOffsets[i]) {
					_data.resize(valuesOffsets[i]);
				}
				FrameMarkEnd(tracy__AddArrayCheckValuePadding);

				// GET VALUE DATA
				FrameMarkStart(tracy__AddArrayGetValueData);
				valueData = std::move(_GetValueData(values[i]));
				FrameMarkEnd(tracy__AddArrayGetValueData);

				// SET VALUE DATA
				FrameMarkStart(tracy__AddArraySetValueData);
				_data.insert(_data.end(), valueData.begin(), valueData.end());
				FrameMarkEnd(tracy__AddArraySetValueData);

				// CLEAR VALUE TEMP DATA
				FrameMarkStart(tracy__AddArrayClearTempValueData);
				valueData.clear();
				FrameMarkEnd(tracy__AddArrayClearTempValueData);
			}
			FrameMarkEnd(tracy__AddArraySetValuesData);

			// CLEAR VALUES OFFSETS
			FrameMarkStart(tracy__AddArrayClearValuesOffsets);
			valuesOffsets.clear();
			FrameMarkEnd(tracy__AddArrayClearValuesOffsets);

			// UPDATE SIZE
			FrameMarkStart(tracy__AddArrayUpdateSize);
			if (_data.size() < _data.capacity()) {
				_data.resize(_data.capacity());
			}
			FrameMarkEnd(tracy__AddArrayUpdateSize);

			FrameMarkEnd(tracy__AddArray);
		}

		void _AddStruct(const std::string& name, const STD140Struct& value);

		void _AddStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

#pragma endregion

#pragma region SET
		static const char* const tracy__Set;
		static const char* const tracy__SetCheckVariable;
		static const char* const tracy__SetGetOffset;
		static const char* const tracy__SetGetValueData;
		static const char* const tracy__SetSetValueData;
		static const char* const tracy__SetClearTempValueData;

		template<class T> bool _Set(const std::string& name, const T& value) {
			ZoneScoped;
			FrameMarkStart(tracy__Set);

			// CHECK VARIABLE
			FrameMarkStart(tracy__SetCheckVariable);
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				FrameMarkEnd(tracy__SetCheckVariable);
				FrameMarkEnd(tracy__Set);
				return false;
			}
			FrameMarkEnd(tracy__SetCheckVariable);

			// GET VALUE OFFSET
			FrameMarkStart(tracy__SetGetOffset);
			size_t valueOffset = std::move(_dataOffsets.Get(name));
			FrameMarkEnd(tracy__SetGetOffset);

			// GET VALUE DATA
			FrameMarkStart(tracy__SetGetValueData);
			std::vector<char> valueData = std::move(_GetValueData(value));
			FrameMarkEnd(tracy__SetGetValueData);

			// SET VALUE DATA
			FrameMarkStart(tracy__SetSetValueData);
			memcpy(_data.data() + valueOffset, valueData.data(), glm::min(valueData.size(), _data.size() - valueOffset));
			FrameMarkEnd(tracy__SetSetValueData);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__SetClearTempValueData);
			valueData.clear();
			FrameMarkEnd(tracy__SetClearTempValueData);

			FrameMarkEnd(tracy__Set);
			return true;
		}

		static const char* const tracy__SetArray;
		static const char* const tracy__SetArrayCheckSize;
		static const char* const tracy__SetArrayCheckVariable;
		static const char* const tracy__SetArrayGetValuesOffsets;
		static const char* const tracy__SetArrayCheckValuesOffsets;
		static const char* const tracy__SetArrayGetElemSize;
		static const char* const tracy__SetArraySetValues;
		static const char* const tracy__SetArrayGetValueData;
		static const char* const tracy__SetArraySetValueData;
		static const char* const tracy__SetArrayClearTempValueData;
		static const char* const tracy__SetArrayClearValuesOffsets;

		template<class T> bool _SetArray(const std::string& name, const std::vector<T>& values) {
			ZoneScoped;
			FrameMarkStart(tracy__SetArray);

			// CHECK SIZE
			FrameMarkStart(tracy__SetArrayCheckSize);
			if (values.size() == 0) {
				FrameMarkEnd(tracy__SetArrayCheckSize);
				FrameMarkEnd(tracy__SetArray);
				return false;
			}
			FrameMarkEnd(tracy__SetArrayCheckSize);

			// CHECK VARIABLE
			FrameMarkStart(tracy__SetArrayCheckVariable);
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				FrameMarkEnd(tracy__SetArrayCheckVariable);
				FrameMarkEnd(tracy__SetArray);
				return false;
			}
			FrameMarkEnd(tracy__SetArrayCheckVariable);

			// GET VALUES OFFSETS
			FrameMarkStart(tracy__SetArrayGetValuesOffsets);
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.GetArray(name));
			FrameMarkEnd(tracy__SetArrayGetValuesOffsets);

			// CHECK ARRAY ELEMENTS OFFSETS
			FrameMarkStart(tracy__SetArrayCheckValuesOffsets);
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				FrameMarkEnd(tracy__SetArrayCheckValuesOffsets);
				FrameMarkEnd(tracy__SetArray);
				return false;
			}
			FrameMarkEnd(tracy__SetArrayCheckValuesOffsets);

			// GET ARRAY ELEM DATA MAX SIZE
			FrameMarkStart(tracy__SetArrayGetElemSize);
			size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);
			FrameMarkEnd(tracy__SetArrayGetElemSize);

			// SET VALUES DATA
			FrameMarkStart(tracy__SetArraySetValues);
			std::vector<char> valueData;
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET VALUE DATA
				FrameMarkStart(tracy__SetArrayGetValueData);
				valueData = std::move(_GetValueData(values[i]));
				FrameMarkEnd(tracy__SetArrayGetValueData);

				// SET VALUE DATA
				FrameMarkStart(tracy__SetArraySetValueData);
				memcpy(_data.data(), valueData.data(), glm::min(glm::min(valueData.size(), arrayElemDataSize), _data.size() - valuesOffsets[i]));
				FrameMarkEnd(tracy__SetArraySetValueData);

				// CLEAR TEMP VALUE DATA
				FrameMarkStart(tracy__SetArrayClearTempValueData);
				valueData.clear();
				FrameMarkEnd(tracy__SetArrayClearTempValueData);
			}
			FrameMarkEnd(tracy__SetArraySetValues);

			// CLEAR VALUES OFFSETS
			FrameMarkStart(tracy__SetArrayClearValuesOffsets);
			valuesOffsets.clear();
			FrameMarkEnd(tracy__SetArrayClearValuesOffsets);

			FrameMarkEnd(tracy__SetArray);
			return true;
		}

		bool _SetStruct(const std::string& name, const STD140Struct& value);

		bool _SetStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

#pragma endregion

#pragma region GET
		static const char* const tracy__Get;
		static const char* const tracy__GetCheckVariable;
		static const char* const tracy__GetValueOffset;
		static const char* const tracy__GetEmptyValueData;
		static const char* const tracy__GetReserveSpace;
		static const char* const tracy__GetGetValueData;
		static const char* const tracy__GetCheckValueDataSize;
		static const char* const tracy__GetValue;
		static const char* const tracy__GetClearTempValueData;

		template<class T> T _Get(const std::string& name) const {
			ZoneScoped;
			FrameMarkStart(tracy__Get);

			// CHECK VARIABLE
			FrameMarkStart(tracy__GetCheckVariable);
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				FrameMarkEnd(tracy__GetCheckVariable);
				FrameMarkEnd(tracy__Get);
				return T();
			}
			FrameMarkEnd(tracy__GetCheckVariable);

			// GET VALUE OFFSET
			FrameMarkStart(tracy__GetValueOffset);
			size_t valueOffset = _dataOffsets.Get(name);
			FrameMarkEnd(tracy__GetValueOffset);

			// MAKE EMPTY VALUE DATA
			FrameMarkStart(tracy__GetEmptyValueData);
			std::vector<char> valueData;
			FrameMarkEnd(tracy__GetEmptyValueData);

			// RESERVE SPACE
			FrameMarkStart(tracy__GetReserveSpace);
			valueData.reserve(sizeof(T));
			FrameMarkEnd(tracy__GetReserveSpace);

			// GET VALUE DATA
			FrameMarkStart(tracy__GetGetValueData);
			valueData.insert(valueData.begin(), _data.begin() + valueOffset, _data.begin() + valueOffset + glm::min(valueData.capacity(), _data.size() - valueOffset));
			FrameMarkEnd(tracy__GetGetValueData);

			// CHECK VALUE DATA SIZE
			FrameMarkStart(tracy__GetCheckValueDataSize);
			if (valueData.size() < valueData.capacity()) {
				valueData.resize(valueData.capacity());
			}
			FrameMarkEnd(tracy__GetCheckValueDataSize);

			// GET VALUE
			FrameMarkStart(tracy__GetValue);
			T value = *reinterpret_cast<T*>(valueData.data());
			FrameMarkEnd(tracy__GetValue);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__GetClearTempValueData);
			valueData.clear();
			FrameMarkEnd(tracy__GetClearTempValueData);

			// RETURN VALUE
			FrameMarkEnd(tracy__Get);
			return value;
		}

		static const char* const tracy__GetArray;
		static const char* const tracy__GetArrayCheckVariable;
		static const char* const tracy__GetArrayValuesOffsets;
		static const char* const tracy__GetArrayCheckValuesOffsets;
		static const char* const tracy__GetArrayMaxElemSize;
		static const char* const tracy__GetArrayValuesData;
		static const char* const tracy__GetArrayMaxValueSize;
		static const char* const tracy__GetArrayValueData;
		static const char* const tracy__GetArrayCheckValueDataSize;
		static const char* const tracy__GetArrayValue;
		static const char* const tracy__GetArrayClearTempValueData;
		static const char* const tracy__GetArrayClearValuesOffsets;

		template<class T> std::vector<T> _GetArray(const std::string& name) const {
			ZoneScoped;
			FrameMarkStart(tracy__GetArray);

			// CHECK VARIABLE
			FrameMarkStart(tracy__GetArrayCheckVariable);
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				FrameMarkEnd(tracy__GetArrayCheckVariable);
				FrameMarkEnd(tracy__GetArray);
				return std::vector<T>();
			}
			FrameMarkEnd(tracy__GetArrayCheckVariable);

			// GET VALUES OFFSETS
			FrameMarkStart(tracy__GetArrayValuesOffsets);
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.GetArray(name));
			FrameMarkEnd(tracy__GetArrayValuesOffsets);

			// CHECK ARRAY ELEMENTS OFFSETS
			FrameMarkStart(tracy__GetArrayCheckValuesOffsets);
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				FrameMarkEnd(tracy__GetArrayCheckValuesOffsets);
				FrameMarkEnd(tracy__GetArray);
				return std::vector<T>();
			}
			FrameMarkEnd(tracy__GetArrayCheckValuesOffsets);

			// GET ARRAY ELEM DATA MAX SIZE
			FrameMarkStart(tracy__GetArrayMaxElemSize);
			size_t arrayElemDataSize = glm::min(_GetArrayElemSize(valuesOffsets), sizeof(T));
			FrameMarkEnd(tracy__GetArrayMaxElemSize);

			// GET VALUES DATA
			FrameMarkStart(tracy__GetArrayValuesData);
			std::vector<T> values;
			std::vector<char> valueData;
			valueData.resize(sizeof(T));
			size_t maxSize;
			for (size_t i = 0; i < valuesOffsets.size(); ++i) {
				// GET MAX VALUE SIZE
				FrameMarkStart(tracy__GetArrayMaxValueSize);
				maxSize = glm::min(arrayElemDataSize, _data.size() - valuesOffsets[i]);
				FrameMarkEnd(tracy__GetArrayMaxValueSize);

				// GET VALUE DATA
				FrameMarkStart(tracy__GetArrayValueData);
				memcpy(valueData.data(), _data.data() + valuesOffsets[i], maxSize);
				FrameMarkEnd(tracy__GetArrayValueData);

				// CHECK VALUE DATA SIZE
				FrameMarkStart(tracy__GetArrayCheckValueDataSize);
				if (maxSize < sizeof(T)) {
					memset(valueData.data() + maxSize, 0, sizeof(T) - maxSize);
				}
				FrameMarkEnd(tracy__GetArrayCheckValueDataSize);

				// GET VALUE
				FrameMarkStart(tracy__GetArrayValue);
				values.push_back(*reinterpret_cast<T*>(valueData.data()));
				FrameMarkEnd(tracy__GetArrayValue);
			}
			FrameMarkEnd(tracy__GetArrayValuesData);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__GetArrayClearTempValueData);
			valueData.clear();
			FrameMarkEnd(tracy__GetArrayClearTempValueData);

			// CLEAR VALUES OFFSETS
			FrameMarkStart(tracy__GetArrayClearValuesOffsets);
			valuesOffsets.clear();
			FrameMarkEnd(tracy__GetArrayClearValuesOffsets);
			
			// RETURN VALUES
			FrameMarkEnd(tracy__GetArray);
			return values;
		}

		STD140Struct _GetStruct(const std::string& name, const STD140Offsets& structOffsets) const;

		std::vector<STD140Struct> _GetStructArray(const std::string& name, const STD140Offsets& structOffsets) const;

		static const char* const tracy__GetArrayConvert;
		static const char* const tracy__GetArrayConvertValues;
		static const char* const tracy__GetArrayConvertValue;

		template<class S, class C>
		std::vector<C> _GetArray(const std::string& name, const Func<std::vector<S>, const std::string&>& getArrayFunc) {
			ZoneScoped;
			FrameMarkStart(tracy__GetArrayConvert);

			if (std::is_same_v<S, C>) {
				FrameMarkEnd(tracy__GetArrayConvert);
				return getArrayFunc(name);
			}
			else {
				std::vector<S> values = getArrayFunc(name);

				FrameMarkStart(tracy__GetArrayConvertValues);
				std::vector<C> convertedValues;
				for (auto& val : values) {
					FrameMarkStart(tracy__GetArrayConvertValue);
					convertedValues.push_back((C)val);
					FrameMarkEnd(tracy__GetArrayConvertValue);
				}
				FrameMarkEnd(tracy__GetArrayConvertValues);

				FrameMarkEnd(tracy__GetArrayConvert);
				return convertedValues;
			}
		}
#pragma endregion

	public:
		STD140Struct() = default;
		STD140Struct(STD140Struct& std140s) = default;
		STD140Struct(const STD140Struct& std140s) = default;
		STD140Struct(STD140Struct&& std140s) = default;
		STD140Struct(const STD140Offsets& structOffsets, const std::vector<char>& data = std::vector<char>());
		template<class... Args>
		STD140Struct(const STD140Variable<Args>&... vars) {
			ZoneScoped;
			_dataOffsets = STD140Offsets(vars...);
			_data.resize(_dataOffsets.GetSize());
		}
		/*template<class... Args>
		STD140Struct(const STD140Value<Args>&... values) {
			_AddMultiple(values...);
		}*/
		virtual ~STD140Struct() = default;

		STD140Struct& operator=(STD140Struct& std140s) = default;
		STD140Struct& operator=(const STD140Struct& std140s) = default;
		STD140Struct& operator=(STD140Struct&& std140s) = default;

#pragma region ADD_SCALARS
		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T& value) {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				_Add(name, (unsigned int)value);
			}
			else {
				_Add(name, value);
			}
		}

#pragma region ADD_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T*& values, size_t size) {
			ZoneScoped;
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_ConvertArray<T, type>(name, values, size, [&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T(&values)[N]) {
			ZoneScoped;
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_ConvertArray<T, type>(name, values, N, [&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const std::vector<T>& values) {
			ZoneScoped;
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_ConvertArray<T, type>(name, values, values.size(),
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

#pragma endregion
#pragma endregion

#pragma region ADD_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V& value) {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::vec<L, unsigned int>;
				_Add(name, (type)value);
			}
			else {
				_Add(name, value);
			}
		}

#pragma region ADD_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V*& values, size_t size) {
			ZoneScoped;
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<V, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V(&values)[N]) {
			ZoneScoped;
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<V, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const std::vector<V>& values) {
			ZoneScoped;
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<V, type>(name, values, values.size(),
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M& value) {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				_Add(name, (type)value);
			}
			else {
				_Add(name, value);
			}
		}

#pragma region ADD_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M*& values, size_t size) {
			ZoneScoped;
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<M, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M(&values)[N]) {
			ZoneScoped;
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<M, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, typename M::value_type, M::row_type::length(), M::column_type::length()>
		Add(const std::string& name, const std::vector<M>& values) {
			ZoneScoped;
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<M, type>(name, values, values.size(),
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		void Add(const std::string& name, const STD140Struct& value);

#pragma region ADD_STRUCT_ARRAYS
		void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>*& values, size_t size);

		template<size_t N> void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>(&values)[N]) {
			ZoneScoped;
			_ConvertArray<std::vector<char>, std::vector<char>>(name, values, N,
				[&](const std::string& name, const std::vector<std::vector<char>>& convs) -> void {
					_AddStructArray(name, structOffsets, convs);
				}
			);
		}

		void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

#pragma endregion
#pragma endregion


#pragma region SET_SCALARS
		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T& value) {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				return _Set(name, (unsigned int)value);
			}
			else {
				return _Set(name, value);
			}
		}

#pragma region SET_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T*& values, size_t size) {
			ZoneScoped;
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _ConvertArray<T, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T(&values)[N]) {
			ZoneScoped;
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _ConvertArray<T, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const std::vector<T>& values) {
			ZoneScoped;
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _ConvertArray<T, type>(name, values, values.size(),
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

#pragma endregion
#pragma endregion

#pragma region SET_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V& value) {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::vec<L, unsigned int>;
				return _Set(name, (type)value);
			}
			else {
				return _Set(name, value);
			}
		}

#pragma region SET_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V*& values, size_t size) {
			ZoneScoped;
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _ConvertArray<V, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V(&values)[N]) {
			ZoneScoped;
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _ConvertArray<V, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const std::vector<V>& values) {
			ZoneScoped;
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _ConvertArray<V, type>(name, values, values.size(),
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

#pragma endregion
#pragma endregion

#pragma region SET_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M& value) {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				return _Set(name, (type)value);
			}
			else {
				return _Set(name, value);
			}
		}

#pragma region SET_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M*& values, size_t size) {
			ZoneScoped;
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _ConvertArray<M, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M(&values)[N]) {
			ZoneScoped;
			using type = glm::mat<C, R, type_test_t<std::is_same<T, bool>, unsigned int, T>>;
			return _ConvertArray<M, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const std::vector<M>& values) {
			ZoneScoped;
			using type = glm::mat<C, R, type_test_t<std::is_same<T, bool>, unsigned int, T>>;
			return _ConvertArray<M, type>(name, values, values.size(),
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

#pragma endregion
#pragma endregion

#pragma region SET_STRUCT
		bool Set(const std::string& name, const STD140Struct& value);

#pragma region SET_STRUCT_ARRAYS
		bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>*& values, size_t size);

		template<size_t N> bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>(&values)[N]) {
			ZoneScoped;
			return _ConvertArray<std::vector<char>, std::vector<char>>(name, values, N, 
				[&](const std::string& name, const std::vector<std::vector<char>>& values) -> bool { 
					return _SetStructArray(name, structOffsets, values);
				});
		}

		bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);
#pragma endregion
#pragma endregion


#pragma region GET_SCALARS
		template<class T>
		typename scalar_enable_if_t<T, T>
		Get(const std::string& name) const {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				return (T)_Get<unsigned int>(name);
			}
			else {
				return _Get<T>(name);
			}
		}

#pragma region GET_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T>
		Get(const std::string& name, T*& valuesDest, size_t size) const {
			ZoneScoped;
			std::vector<T> values;
			if constexpr (std::is_same_v<T, bool>) {
				values = _GetArray<unsigned int, T>(name, [&](const std::string& name) -> std::vector<T> { return _GetArray<unsigned int>(name); });
			}
			else {
				values = _GetArray<T>(name);
			}
			memcpy(valuesDest, values.data(), glm::min(values.size(), size));
			values.clear();
		}

		template<class V, class T = V::value_type>
		typename get_vector_enable_if_t<V, T, type_check_v<T>, V>
		Get(const std::string& name) const {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				return _GetArray<unsigned int, T>(name, [&](const std::string& name) -> std::vector<T> { return _GetArray<unsigned int>(name); });
			}
			else {
				return _GetArray<T>(name);
			}
		}

#pragma endregion
#pragma endregion

#pragma region GET_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, V>
		Get(const std::string& name) const {
			ZoneScoped;
			if (std::is_same_v<T, bool>) {
				using type = glm::vec<L, unsigned int>;
				return (V)_Get<type>(name);
			}
			else {
				return _Get<V>(name);
			}
		}

#pragma region GET_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Get(const std::string& name, V*& valuesDest, size_t size) const {
			ZoneScoped;
			std::vector<V> values;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::vec<L, unsigned int>;
				values = _GetArray<type, V>(name, [&](const std::string& name) -> std::vector<type> {
						return _GetArray<type>(name); 
					});
			}
			else {
				values = _GetArray<V>(name);
			}
			memcpy(valuesDest, values.data(), glm::min(values.size(), size));
			values.clear();
		}

		template<class Vec, class V = Vec::value_type, class T = V::value_type, size_t L = V::length()>
		typename get_vector_enable_if_t<Vec, V, vec_check_v<V, T, L>, Vec>
		Get(const std::string& name) const {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::vec<L, unsigned int>;
				return _GetArray<type, V>(name, [&](const std::string& name) -> std::vector<type> {
						return _GetArray<type>(name); 
					});
			}
			else {
				return GetArray<V>(name);
			}
		}

#pragma endregion
#pragma endregion

#pragma region GET_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, M>
		Get(const std::string& name) const {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				return (M)_Get<type>(name);
			}
			else {
				return _Get<M>(name);
			}
		}

#pragma region GET_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Get(const std::string& name, M*& valuesDest, size_t size) const {
			ZoneScoped;
			std::vector<M> values;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				values = _GetArray<type, M>(name, [&](const std::string& name) -> std::vector<type> {
						return _GetArray<type>(name); 
					});
			}
			else {
				values = _GetArray<M>(name);
			}
			memcpy(valuesDest, values.data(), glm::min(values.size(), size));
			values.clear();
		}

		template<class Vec, class M = Vec::value_type, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename get_vector_enable_if_t<Vec, M, mat_check_v<M, T, C, R>, Vec>
		Get(const std::string& name) const {
			ZoneScoped;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				return _GetArray<type, M>(name, [&](const std::string& name) -> std::vector<type> {
						return _GetArray<type>(name); 
					});
			}
			else {
				return _GetArray<M>(name);
			}
		}

#pragma endregion
#pragma endregion

#pragma region GET_STRUCT
		template<class S>
		typename std::enable_if_t<std::is_same_v<S, STD140Struct>, S>
		Get(const std::string& name, const STD140Offsets& structOffsets) const {
			ZoneScoped;
			return _GetStruct(name, structOffsets);
		}

#pragma region GET_STRUCT_ARRAYS
		template<class S>
		typename std::enable_if_t<std::is_same_v<S, STD140Struct>>
		Get(const std::string& name, const STD140Offsets& structOffsets, STD140Struct*& valueDest, size_t size) const {
			ZoneScoped;
			std::vector<STD140Struct> values = _GetStructArray(name, structOffsets);
			memcpy(valueDest, values.data(), glm::min(values.size(), size));
		}

		template<class V, class S = V::value_type>
		typename get_vector_enable_if_t<V, S, std::is_same_v<S, STD140Struct>, V>
		Get(const std::string& name, const STD140Offsets& structTemplate) const {
			ZoneScoped;
			return _GetStructArray(name, structTemplate);
		}

#pragma endregion
#pragma endregion

		STD140Offsets GetOffsets() const;
		std::vector<char> GetData() const;
		size_t GetBaseAligement() const;
		size_t GetSize() const;

		void ClearData();
		void Clear();
	};
}