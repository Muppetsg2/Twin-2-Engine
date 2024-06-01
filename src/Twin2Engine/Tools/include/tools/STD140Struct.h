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

#if TRACY_PROFILER
		static const char* const tracy__GetValueData;
#endif

		template<class T> 
		std::vector<char> _GetValueData(const T& value) const {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__GetValueData);
#endif
			const char* valueDataPtr = reinterpret_cast<const char*>(&value);
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetValueData);
#endif
			return std::vector<char>(valueDataPtr, valueDataPtr + sizeof(T));
		}

		size_t _GetArrayElemSize(const std::vector<size_t>& offsets) const;

#if TRACY_PROFILER
		static const char* const tracy__ConvertArray;
		static const char* const tracy__ConvertArrayValues;
		static const char* const tracy__ConvertArrayValue;
		static const char* const tracy__ConvertArrayFunc;
#endif

		template<class S, class C, class T, class R>
		R _ConvertArray(const std::string& name, const T& values, size_t size, const Func<R, const std::string&, const std::vector<C>&>& arrayFunc) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__ConvertArray);
#endif

			if constexpr (std::is_same_v<T, std::vector<S>> && std::is_same_v<S, C>) {
				if constexpr (std::is_void_v<R>) {
#if TRACY_PROFILER
					FrameMarkStart(tracy__ConvertArrayFunc);
#endif
					arrayFunc(name, values);
#if TRACY_PROFILER
					FrameMarkEnd(tracy__ConvertArrayFunc);
#endif
				}
				else {
#if TRACY_PROFILER
					FrameMarkEnd(tracy__ConvertArray);
#endif
					return arrayFunc(name, values);
				}
			}
			else {
#if TRACY_PROFILER
				FrameMarkStart(tracy__ConvertArrayValues);
#endif
				std::vector<C> convertedValues;
				convertedValues.reserve(size);
				for (size_t i = 0; i < size; ++i) {
#if TRACY_PROFILER
					FrameMarkStart(tracy__ConvertArrayValue);
#endif
					if constexpr (std::is_same_v<S, C>) {
						convertedValues.insert(convertedValues.end(), values[i]);
					}
					else {
						convertedValues.insert(convertedValues.end(), (C)values[i]);
					}
#if TRACY_PROFILER
					FrameMarkEnd(tracy__ConvertArrayValue);
#endif
				}
#if TRACY_PROFILER
				FrameMarkEnd(tracy__ConvertArrayValues);
#endif

				if constexpr (std::is_void_v<R>) {
#if TRACY_PROFILER
					FrameMarkStart(tracy__ConvertArrayFunc);
#endif
					arrayFunc(name, std::move(convertedValues));
#if TRACY_PROFILER
					FrameMarkEnd(tracy__ConvertArrayFunc);
#endif
				}
				else {
#if TRACY_PROFILER
					FrameMarkEnd(tracy__ConvertArray);
#endif
					return arrayFunc(name, std::move(convertedValues));
				}
			}

#if TRACY_PROFILER
			if constexpr (std::is_void_v<R>) {
				FrameMarkEnd(tracy__ConvertArray);
			}
#endif
		}

		// TODO: SPRAWDZIÆ KA¯DY MAT (TYLKO MAT4 SPRAWDZONY)

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

#if TRACY_PROFILER
		static const char* const tracy__Add;
		static const char* const tracy__AddGetOffset;
		static const char* const tracy__AddCheckError;
		static const char* const tracy__AddReserveSize;
		static const char* const tracy__AddCheckValuePadding;
		static const char* const tracy__AddGetValueData;
		static const char* const tracy__AddSetValueData;
		static const char* const tracy__AddClearTempValueData;
		static const char* const tracy__AddUpdateSize;
#endif

		template<class T> void _Add(const std::string& name, const T& value) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__Add);
#endif

			// ADD TO OFFSETS
#if TRACY_PROFILER
			FrameMarkStart(tracy__AddGetOffset);
#endif
			size_t valueOffset = std::move(_dataOffsets.Add<T>(name));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddGetOffset);

			// CHECK ERROR
			FrameMarkStart(tracy__AddCheckError);
#endif
			if (valueOffset == 0 && _data.size() != 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__AddCheckError);
				FrameMarkEnd(tracy__Add);
#endif
				return;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddCheckError);

			// RESERVE SIZE
			FrameMarkStart(tracy__AddReserveSize);
#endif
			_data.reserve(_dataOffsets.GetSize());
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddReserveSize);

			// CHECK VALUE PADDING
			FrameMarkStart(tracy__AddCheckValuePadding);
#endif
			if (_data.size() < valueOffset) {
				_data.resize(valueOffset);
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddCheckValuePadding);

			// GET VALUE DATA
			FrameMarkStart(tracy__AddGetValueData);
#endif
			std::vector<char> valueData = std::move(_GetValueData(value));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddGetValueData);

			// SET VALUE DATA
			FrameMarkStart(tracy__AddSetValueData);
#endif
			_data.insert(_data.begin() + valueOffset, valueData.begin(), valueData.end());
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddSetValueData);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__AddClearTempValueData);
#endif
			valueData.clear();
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddClearTempValueData);

			// UPDATE SIZE
			FrameMarkStart(tracy__AddUpdateSize);
#endif
			if (_data.size() < _data.capacity()) {
				_data.resize(_data.capacity());
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddUpdateSize);

			FrameMarkEnd(tracy__Add);
#endif
		}

#if TRACY_PROFILER
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
#endif

		template<class T> void _AddArray(const std::string& name, const std::vector<T>& values) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__AddArray);

			// CHECK SIZE
			FrameMarkStart(tracy__AddArrayCheckSize);
#endif
			if (values.size() == 0) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy__AddArrayCheckSize);
				FrameMarkEnd(tracy__AddArray);
#endif
				return;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddArrayCheckSize);

			// GET OFFSETS
			FrameMarkStart(tracy__AddArrayGetOffsets);
#endif
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.Add<T>(name, values.size()));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddArrayGetOffsets);

			// CHECK ERROR
			FrameMarkStart(tracy__AddArrayCheckError);
#endif
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__AddArrayCheckError);
				FrameMarkEnd(tracy__AddArray);
#endif
				return;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddArrayCheckError);

			// RESERVE SIZE
			FrameMarkStart(tracy__AddArrayReserveSize);
#endif
			_data.reserve(_dataOffsets.GetSize());
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddArrayReserveSize);

			// SET VALUES DATA
			FrameMarkStart(tracy__AddArraySetValuesData);
#endif
			std::vector<char> valueData;
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// CHECK VALUE PADDING
#if TRACY_PROFILER
				FrameMarkStart(tracy__AddArrayCheckValuePadding);
#endif
				if (_data.size() < valuesOffsets[i]) {
					_data.resize(valuesOffsets[i]);
				}
#if TRACY_PROFILER
				FrameMarkEnd(tracy__AddArrayCheckValuePadding);

				// GET VALUE DATA
				FrameMarkStart(tracy__AddArrayGetValueData);
#endif
				valueData = std::move(_GetValueData(values[i]));
#if TRACY_PROFILER
				FrameMarkEnd(tracy__AddArrayGetValueData);

				// SET VALUE DATA
				FrameMarkStart(tracy__AddArraySetValueData);
#endif
				_data.insert(_data.begin() + valuesOffsets[i], valueData.begin(), valueData.end());
#if TRACY_PROFILER
				FrameMarkEnd(tracy__AddArraySetValueData);

				// CLEAR VALUE TEMP DATA
				FrameMarkStart(tracy__AddArrayClearTempValueData);
#endif
				valueData.clear();
#if TRACY_PROFILER
				FrameMarkEnd(tracy__AddArrayClearTempValueData);
#endif
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddArraySetValuesData);

			// CLEAR VALUES OFFSETS
			FrameMarkStart(tracy__AddArrayClearValuesOffsets);
#endif
			valuesOffsets.clear();
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddArrayClearValuesOffsets);

			// UPDATE SIZE
			FrameMarkStart(tracy__AddArrayUpdateSize);
#endif
			if (_data.size() < _data.capacity()) {
				_data.resize(_data.capacity());
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__AddArrayUpdateSize);

			FrameMarkEnd(tracy__AddArray);
#endif
		}

		void _AddStruct(const std::string& name, const STD140Struct& value);

		void _AddStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

#pragma endregion

#pragma region SET
#if TRACY_PROFILER
		static const char* const tracy__Set;
		static const char* const tracy__SetCheckVariable;
		static const char* const tracy__SetGetOffset;
		static const char* const tracy__SetGetValueData;
		static const char* const tracy__SetSetValueData;
		static const char* const tracy__SetClearTempValueData;
#endif

		template<class T> bool _Set(const std::string& name, const T& value) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__Set);

			// CHECK VARIABLE
			FrameMarkStart(tracy__SetCheckVariable);
#endif
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__SetCheckVariable);
				FrameMarkEnd(tracy__Set);
#endif
				return false;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetCheckVariable);

			// GET VALUE OFFSET
			FrameMarkStart(tracy__SetGetOffset);
#endif
			size_t valueOffset = std::move(_dataOffsets.Get(name));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetGetOffset);

			// GET VALUE DATA
			FrameMarkStart(tracy__SetGetValueData);
#endif
			std::vector<char> valueData = std::move(_GetValueData(value));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetGetValueData);

			// SET VALUE DATA
			FrameMarkStart(tracy__SetSetValueData);
#endif
			memcpy(_data.data() + valueOffset, valueData.data(), glm::min(valueData.size(), _data.size() - valueOffset));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetSetValueData);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__SetClearTempValueData);
#endif
			valueData.clear();
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetClearTempValueData);

			FrameMarkEnd(tracy__Set);
#endif
			return true;
		}

#if TRACY_PROFILER
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
#endif

		template<class T> bool _SetArray(const std::string& name, const std::vector<T>& values) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__SetArray);

			// CHECK SIZE
			FrameMarkStart(tracy__SetArrayCheckSize);
#endif
			if (values.size() == 0) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy__SetArrayCheckSize);
				FrameMarkEnd(tracy__SetArray);
#endif
				return false;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetArrayCheckSize);

			// CHECK VARIABLE
			FrameMarkStart(tracy__SetArrayCheckVariable);
#endif
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__SetArrayCheckVariable);
				FrameMarkEnd(tracy__SetArray);
#endif
				return false;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetArrayCheckVariable);

			// GET VALUES OFFSETS
			FrameMarkStart(tracy__SetArrayGetValuesOffsets);
#endif
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.GetArray(name));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetArrayGetValuesOffsets);

			// CHECK ARRAY ELEMENTS OFFSETS
			FrameMarkStart(tracy__SetArrayCheckValuesOffsets);
#endif
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__SetArrayCheckValuesOffsets);
				FrameMarkEnd(tracy__SetArray);
#endif
				return false;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetArrayCheckValuesOffsets);

			// GET ARRAY ELEM DATA MAX SIZE
			FrameMarkStart(tracy__SetArrayGetElemSize);
#endif
			size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetArrayGetElemSize);

			// SET VALUES DATA
			FrameMarkStart(tracy__SetArraySetValues);
#endif
			std::vector<char> valueData;
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET VALUE DATA
#if TRACY_PROFILER
				FrameMarkStart(tracy__SetArrayGetValueData);
#endif
				valueData = std::move(_GetValueData(values[i]));
#if TRACY_PROFILER
				FrameMarkEnd(tracy__SetArrayGetValueData);

				// SET VALUE DATA
				FrameMarkStart(tracy__SetArraySetValueData);
#endif
				memcpy(_data.data(), valueData.data(), glm::min(glm::min(valueData.size(), arrayElemDataSize), _data.size() - valuesOffsets[i]));
#if TRACY_PROFILER
				FrameMarkEnd(tracy__SetArraySetValueData);

				// CLEAR TEMP VALUE DATA
				FrameMarkStart(tracy__SetArrayClearTempValueData);
#endif
				valueData.clear();
#if TRACY_PROFILER
				FrameMarkEnd(tracy__SetArrayClearTempValueData);
#endif
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetArraySetValues);

			// CLEAR VALUES OFFSETS
			FrameMarkStart(tracy__SetArrayClearValuesOffsets);
#endif
			valuesOffsets.clear();
#if TRACY_PROFILER
			FrameMarkEnd(tracy__SetArrayClearValuesOffsets);

			FrameMarkEnd(tracy__SetArray);
#endif
			return true;
		}

		bool _SetStruct(const std::string& name, const STD140Struct& value);

		bool _SetStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

#pragma endregion

#pragma region GET
#if TRACY_PROFILER
		static const char* const tracy__Get;
		static const char* const tracy__GetCheckVariable;
		static const char* const tracy__GetValueOffset;
		static const char* const tracy__GetEmptyValueData;
		static const char* const tracy__GetReserveSpace;
		static const char* const tracy__GetGetValueData;
		static const char* const tracy__GetCheckValueDataSize;
		static const char* const tracy__GetValue;
		static const char* const tracy__GetClearTempValueData;
#endif

		template<class T> T _Get(const std::string& name) const {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__Get);

			// CHECK VARIABLE
			FrameMarkStart(tracy__GetCheckVariable);
#endif
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetCheckVariable);
				FrameMarkEnd(tracy__Get);
#endif
				return T();
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetCheckVariable);

			// GET VALUE OFFSET
			FrameMarkStart(tracy__GetValueOffset);
#endif
			size_t valueOffset = _dataOffsets.Get(name);
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetValueOffset);

			// MAKE EMPTY VALUE DATA
			FrameMarkStart(tracy__GetEmptyValueData);
#endif
			std::vector<char> valueData;
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetEmptyValueData);

			// RESERVE SPACE
			FrameMarkStart(tracy__GetReserveSpace);
#endif
			valueData.reserve(sizeof(T));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetReserveSpace);

			// GET VALUE DATA
			FrameMarkStart(tracy__GetGetValueData);
#endif
			valueData.insert(valueData.begin(), _data.begin() + valueOffset, _data.begin() + valueOffset + glm::min(valueData.capacity(), _data.size() - valueOffset));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetGetValueData);

			// CHECK VALUE DATA SIZE
			FrameMarkStart(tracy__GetCheckValueDataSize);
#endif
			if (valueData.size() < valueData.capacity()) {
				valueData.resize(valueData.capacity());
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetCheckValueDataSize);

			// GET VALUE
			FrameMarkStart(tracy__GetValue);
#endif
			T value = *reinterpret_cast<T*>(valueData.data());
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetValue);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__GetClearTempValueData);
#endif
			valueData.clear();
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetClearTempValueData);

			// RETURN VALUE
			FrameMarkEnd(tracy__Get);
#endif
			return value;
		}

#if TRACY_PROFILER
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
#endif

		template<class T> std::vector<T> _GetArray(const std::string& name) const {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__GetArray);

			// CHECK VARIABLE
			FrameMarkStart(tracy__GetArrayCheckVariable);
#endif
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayCheckVariable);
				FrameMarkEnd(tracy__GetArray);
#endif
				return std::vector<T>();
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetArrayCheckVariable);

			// GET VALUES OFFSETS
			FrameMarkStart(tracy__GetArrayValuesOffsets);
#endif
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.GetArray(name));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetArrayValuesOffsets);

			// CHECK ARRAY ELEMENTS OFFSETS
			FrameMarkStart(tracy__GetArrayCheckValuesOffsets);
#endif
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayCheckValuesOffsets);
				FrameMarkEnd(tracy__GetArray);
#endif
				return std::vector<T>();
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetArrayCheckValuesOffsets);

			// GET ARRAY ELEM DATA MAX SIZE
			FrameMarkStart(tracy__GetArrayMaxElemSize);
#endif
			size_t arrayElemDataSize = glm::min(_GetArrayElemSize(valuesOffsets), sizeof(T));
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetArrayMaxElemSize);

			// GET VALUES DATA
			FrameMarkStart(tracy__GetArrayValuesData);
#endif
			std::vector<T> values;
			std::vector<char> valueData;
			valueData.resize(sizeof(T));
			size_t maxSize;
			for (size_t i = 0; i < valuesOffsets.size(); ++i) {
				// GET MAX VALUE SIZE
#if TRACY_PROFILER
				FrameMarkStart(tracy__GetArrayMaxValueSize);
#endif
				maxSize = glm::min(arrayElemDataSize, _data.size() - valuesOffsets[i]);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayMaxValueSize);

				// GET VALUE DATA
				FrameMarkStart(tracy__GetArrayValueData);
#endif
				memcpy(valueData.data(), _data.data() + valuesOffsets[i], maxSize);
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayValueData);

				// CHECK VALUE DATA SIZE
				FrameMarkStart(tracy__GetArrayCheckValueDataSize);
#endif
				if (maxSize < sizeof(T)) {
					memset(valueData.data() + maxSize, 0, sizeof(T) - maxSize);
				}
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayCheckValueDataSize);

				// GET VALUE
				FrameMarkStart(tracy__GetArrayValue);
#endif
				values.push_back(*reinterpret_cast<T*>(valueData.data()));
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayValue);
#endif
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetArrayValuesData);

			// CLEAR TEMP VALUE DATA
			FrameMarkStart(tracy__GetArrayClearTempValueData);
#endif
			valueData.clear();
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetArrayClearTempValueData);

			// CLEAR VALUES OFFSETS
			FrameMarkStart(tracy__GetArrayClearValuesOffsets);
#endif
			valuesOffsets.clear();
#if TRACY_PROFILER
			FrameMarkEnd(tracy__GetArrayClearValuesOffsets);
			
			// RETURN VALUES
			FrameMarkEnd(tracy__GetArray);
#endif
			return values;
		}

		STD140Struct _GetStruct(const std::string& name, const STD140Offsets& structOffsets) const;

		std::vector<STD140Struct> _GetStructArray(const std::string& name, const STD140Offsets& structOffsets) const;

		static const char* const tracy__GetArrayConvert;
		static const char* const tracy__GetArrayConvertValues;
		static const char* const tracy__GetArrayConvertValue;

		template<class S, class C>
		std::vector<C> _GetArray(const std::string& name, const Func<std::vector<S>, const std::string&>& getArrayFunc) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy__GetArrayConvert);
#endif

			if (std::is_same_v<S, C>) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayConvert);
#endif
				return getArrayFunc(name);
			}
			else {
				std::vector<S> values = getArrayFunc(name);

#if TRACY_PROFILER
				FrameMarkStart(tracy__GetArrayConvertValues);
#endif
				std::vector<C> convertedValues;
				for (auto& val : values) {
#if TRACY_PROFILER
					FrameMarkStart(tracy__GetArrayConvertValue);
#endif
					convertedValues.push_back((C)val);
#if TRACY_PROFILER
					FrameMarkEnd(tracy__GetArrayConvertValue);
#endif
				}
#if TRACY_PROFILER
				FrameMarkEnd(tracy__GetArrayConvertValues);

				FrameMarkEnd(tracy__GetArrayConvert);
#endif
				return convertedValues;
			}
		}
#pragma endregion

	public:
		STD140Struct() = default;
		STD140Struct(STD140Struct& std140s);
		STD140Struct(const STD140Struct& std140s);
		STD140Struct(STD140Struct&& std140s);
		STD140Struct(const STD140Offsets& structOffsets, const std::vector<char>& data = std::vector<char>());
		template<class... Args>
		STD140Struct(const STD140Variable<Args>&... vars) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			_dataOffsets = STD140Offsets(vars...);
			_data.resize(_dataOffsets.GetSize());
		}
		/*template<class... Args>
		STD140Struct(const STD140Value<Args>&... values) {
			_AddMultiple(values...);
		}*/
		virtual ~STD140Struct();

		STD140Struct& operator=(STD140Struct& std140s) = default;
		STD140Struct& operator=(const STD140Struct& std140s) = default;
		STD140Struct& operator=(STD140Struct&& std140s) = default;

#pragma region ADD_SCALARS
		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T& value) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_ConvertArray<T, type>(name, values, size, [&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T(&values)[N]) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_ConvertArray<T, type>(name, values, N, [&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const std::vector<T>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<V, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V(&values)[N]) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<V, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const std::vector<V>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<M, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M(&values)[N]) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_ConvertArray<M, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, typename M::value_type, M::row_type::length(), M::column_type::length()>
		Add(const std::string& name, const std::vector<M>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _ConvertArray<T, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T(&values)[N]) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _ConvertArray<T, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const std::vector<T>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _ConvertArray<V, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V(&values)[N]) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _ConvertArray<V, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const std::vector<V>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _ConvertArray<M, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M(&values)[N]) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			using type = glm::mat<C, R, type_test_t<std::is_same<T, bool>, unsigned int, T>>;
			return _ConvertArray<M, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const std::vector<M>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
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
#if TRACY_PROFILER
			ZoneScoped;
#endif
			return _GetStruct(name, structOffsets);
		}

#pragma region GET_STRUCT_ARRAYS
		template<class S>
		typename std::enable_if_t<std::is_same_v<S, STD140Struct>>
		Get(const std::string& name, const STD140Offsets& structOffsets, STD140Struct*& valueDest, size_t size) const {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			std::vector<STD140Struct> values = _GetStructArray(name, structOffsets);
			memcpy(valueDest, values.data(), glm::min(values.size(), size));
		}

		template<class V, class S = V::value_type>
		typename get_vector_enable_if_t<V, S, std::is_same_v<S, STD140Struct>, V>
		Get(const std::string& name, const STD140Offsets& structTemplate) const {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			return _GetStructArray(name, structTemplate);
		}

#pragma endregion
#pragma endregion

		STD140Offsets GetOffsets() const;
		size_t GetOffset(const std::string& name) const;
		std::vector<size_t> GetArrayOffsets(const std::string& name) const;
#if _DEBUG
		const ValueType* GetType(const std::string& name) const;
#endif
		std::vector<char> GetData() const;
		size_t GetBaseAligement() const;
		size_t GetSize() const;

		void ClearData();
		void Clear();
	};
}