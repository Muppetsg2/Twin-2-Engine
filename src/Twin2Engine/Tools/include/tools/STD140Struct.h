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

		template<class T> 
		std::vector<char> _GetValueData(const T& value) const {
#if TRACY_PROFILER
			ZoneScoped;
#endif
			const char* valueDataPtr = reinterpret_cast<const char*>(&value);
			return std::vector<char>(valueDataPtr, valueDataPtr + sizeof(T));
		}

		size_t _GetArrayElemSize(const std::vector<size_t>& offsets) const;

		template<class S, class C, class T, class R>
		R _ConvertArray(const std::string& name, const T& values, size_t size, const Func<R, const std::string&, const std::vector<C>&>& arrayFunc) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			if constexpr (std::is_same_v<T, std::vector<S>> && std::is_same_v<S, C>) {
				if constexpr (std::is_void_v<R>) {
					arrayFunc(name, values);
				}
				else {
					return arrayFunc(name, values);
				}
			}
			else {
				std::vector<C> convertedValues;
				convertedValues.reserve(size);
				for (size_t i = 0; i < size; ++i) {
					if constexpr (std::is_same_v<S, C>) {
						convertedValues.insert(convertedValues.end(), values[i]);
					}
					else {
						convertedValues.insert(convertedValues.end(), (C)values[i]);
					}
				}

				if constexpr (std::is_void_v<R>) {
					arrayFunc(name, std::move(convertedValues));
				}
				else {
					return arrayFunc(name, std::move(convertedValues));
				}
			}
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

		template<class T> void _Add(const std::string& name, const T& value) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			// ADD TO OFFSETS
			size_t valueOffset = std::move(_dataOffsets.Add<T>(name));

			// CHECK ERROR
			if (valueOffset == 0 && _data.size() != 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				return;
			}

			// RESERVE SIZE
			_data.reserve(_dataOffsets.GetSize());

			// CHECK VALUE PADDING
			if (_data.size() < valueOffset) {
				_data.resize(valueOffset);
			}

			// GET VALUE DATA
			std::vector<char> valueData = std::move(_GetValueData(value));

			// SET VALUE DATA
			_data.insert(_data.begin() + valueOffset, valueData.begin(), valueData.end());

			// CLEAR TEMP VALUE DATA
			valueData.clear();

			// UPDATE SIZE
			if (_data.size() < _data.capacity()) {
				_data.resize(_data.capacity());
			}
		}

		template<class T> void _AddArray(const std::string& name, const std::vector<T>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			// CHECK SIZE
			if (values.size() == 0) {
				return;
			}

			// GET OFFSETS
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.Add<T>(name, values.size()));

			// CHECK ERROR
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				return;
			}

			// RESERVE SIZE
			_data.reserve(_dataOffsets.GetSize());

			// SET VALUES DATA
			std::vector<char> valueData;
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// CHECK VALUE PADDING
				if (_data.size() < valuesOffsets[i]) {
					_data.resize(valuesOffsets[i]);
				}

				// GET VALUE DATA
				valueData = std::move(_GetValueData(values[i]));

				// SET VALUE DATA
				_data.insert(_data.begin() + valuesOffsets[i], valueData.begin(), valueData.end());

				// CLEAR VALUE TEMP DATA
				valueData.clear();
			}

			// CLEAR VALUES OFFSETS
			valuesOffsets.clear();

			// UPDATE SIZE
			if (_data.size() < _data.capacity()) {
				_data.resize(_data.capacity());
			}
		}

		void _AddStruct(const std::string& name, const STD140Struct& value);

		void _AddStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

#pragma endregion

#pragma region SET

		template<class T> bool _Set(const std::string& name, const T& value) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET VALUE OFFSET
			size_t valueOffset = std::move(_dataOffsets.Get(name));

			// GET VALUE DATA
			std::vector<char> valueData = std::move(_GetValueData(value));

			// SET VALUE DATA
			memcpy(_data.data() + valueOffset, valueData.data(), glm::min(valueData.size(), _data.size() - valueOffset));

			// CLEAR TEMP VALUE DATA
			valueData.clear();

			return true;
		}

		template<class T> bool _SetArray(const std::string& name, const std::vector<T>& values) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			// CHECK SIZE
			if (values.size() == 0) {
				return false;
			}

			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.GetArray(name));

			// CHECK ARRAY ELEMENTS OFFSETS
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				return false;
			}

			// GET ARRAY ELEM DATA MAX SIZE
			size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);

			// SET VALUES DATA
			std::vector<char> valueData;
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET VALUE DATA
				valueData = std::move(_GetValueData(values[i]));

				// SET VALUE DATA

				memcpy(_data.data(), valueData.data(), glm::min(glm::min(valueData.size(), arrayElemDataSize), _data.size() - valuesOffsets[i]));

				// CLEAR TEMP VALUE DATA
				valueData.clear();
			}

			// CLEAR VALUES OFFSETS
			valuesOffsets.clear();

			return true;
		}

		bool _SetStruct(const std::string& name, const STD140Struct& value);

		bool _SetStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

#pragma endregion

#pragma region GET

		template<class T> T _Get(const std::string& name) const {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return T();
			}

			// GET VALUE OFFSET
			size_t valueOffset = _dataOffsets.Get(name);

			// MAKE EMPTY VALUE DATA
			std::vector<char> valueData;

			// RESERVE SPACE
			valueData.reserve(sizeof(T));

			// GET VALUE DATA
			valueData.insert(valueData.begin(), _data.begin() + valueOffset, _data.begin() + valueOffset + glm::min(valueData.capacity(), _data.size() - valueOffset));

			// CHECK VALUE DATA SIZE
			if (valueData.size() < valueData.capacity()) {
				valueData.resize(valueData.capacity());
			}

			// GET VALUE
			T value = *reinterpret_cast<T*>(valueData.data());

			// CLEAR TEMP VALUE DATA
			valueData.clear();

			// RETURN VALUE
			return value;
		}

		template<class T> std::vector<T> _GetArray(const std::string& name) const {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return std::vector<T>();
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = std::move(_dataOffsets.GetArray(name));

			// CHECK ARRAY ELEMENTS OFFSETS
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				return std::vector<T>();
			}

			// GET ARRAY ELEM DATA MAX SIZE
			size_t arrayElemDataSize = glm::min(_GetArrayElemSize(valuesOffsets), sizeof(T));

			// GET VALUES DATA
			std::vector<T> values;
			std::vector<char> valueData;
			valueData.resize(sizeof(T));
			size_t maxSize;
			for (size_t i = 0; i < valuesOffsets.size(); ++i) {
				// GET MAX VALUE SIZE
				maxSize = glm::min(arrayElemDataSize, _data.size() - valuesOffsets[i]);

				// GET VALUE DATA
				memcpy(valueData.data(), _data.data() + valuesOffsets[i], maxSize);

				// CHECK VALUE DATA SIZE
				if (maxSize < sizeof(T)) {
					memset(valueData.data() + maxSize, 0, sizeof(T) - maxSize);
				}

				// GET VALUE
				values.push_back(*reinterpret_cast<T*>(valueData.data()));
			}

			// CLEAR TEMP VALUE DATA
			valueData.clear();

			// CLEAR VALUES OFFSETS
			valuesOffsets.clear();

			// RETURN VALUES
			return values;
		}

		STD140Struct _GetStruct(const std::string& name, const STD140Offsets& structOffsets) const;

		std::vector<STD140Struct> _GetStructArray(const std::string& name, const STD140Offsets& structOffsets) const;

		template<class S, class C>
		std::vector<C> _GetArray(const std::string& name, const Func<std::vector<S>, const std::string&>& getArrayFunc) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

			if (std::is_same_v<S, C>) {
				return getArrayFunc(name);
			}
			else {
				std::vector<S> values = getArrayFunc(name);
				std::vector<C> convertedValues;
				for (auto& val : values) {
					convertedValues.push_back((C)val);
				}
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

		STD140Struct& operator=(STD140Struct& std140s);
		STD140Struct& operator=(const STD140Struct& std140s);
		STD140Struct& operator=(STD140Struct&& std140s);

		CloneFuncDeclaration(STD140Struct)

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
		std::vector<std::string> GetNames() const;
#endif
		std::vector<char> GetData() const;
		size_t GetBaseAligement() const;
		size_t GetSize() const;

		void ClearData();
		void Clear();
	};
}