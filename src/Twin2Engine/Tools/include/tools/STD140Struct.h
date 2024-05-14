#pragma once

#include <tools/EventHandler.h>
#include <tools/STD140Offsets.h>

#undef min

namespace Twin2Engine::Tools {
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

		template<class T> std::vector<char> _GetValueData(const T& value) const {
			std::vector<char> valueData;
			valueData.resize(sizeof(T));
			const char* valueDataPtr = reinterpret_cast<const char*>(&value);
			memcpy(valueData.data(), valueDataPtr, sizeof(T));
			return valueData;
		}

		size_t _GetArrayElemSize(const std::vector<size_t>& offsets) const;

#pragma region ADD
		template<class T> void _Add(const std::string& name, const T& value) {
			// ADD TO OFFSETS
			size_t valueOffset = _dataOffsets.Add<T>(name);

			// CHECK ERROR
			if (valueOffset == 0 && _data.size() != 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				return;
			}

			// UPDATE SIZE
			_data.resize(_dataOffsets.GetSize());

			// GET VALUE DATA
			std::vector<char> valueData = _GetValueData(value);

			// SET VALUE DATA
			memcpy(_data.data() + valueOffset, valueData.data(), valueData.size());
		}
		template<class T> void _AddArray(const std::string& name, const std::vector<T>& values) {
			if (values.size() == 0) return;

			// ADD TO OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.Add<T>(name, values.size());

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
				std::vector<char> valueData = _GetValueData(values[i]);

				// SET VALUE DATA
				memcpy(_data.data() + valuesOffsets[i], valueData.data(), valueData.size());
			}
		}
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		void _AddMat(const std::string& name, const M& value) {
			// ADD TO OFFSETS AND CHECK ERROR
			if (_dataOffsets.Add<M>(name) == 0 && _data.size() != 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				return;
			}

			// UPDATE SIZE
			_data.resize(_dataOffsets.GetSize());

			// GET ROWS OFFSETS
			std::vector<size_t> rowsOffsets = _dataOffsets.GetArray(name);

			// SET ROWS DATA
			for (size_t i = 0; i < C && i < rowsOffsets.size(); ++i) {
				// GET VALUE DATA
				std::vector<char> valueData = _GetValueData(value[i]);

				// SET VALUE DATA
				memcpy(_data.data() + rowsOffsets[i], valueData.data(), valueData.size());
			}
		}
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		void _AddMatArray(const std::string& name, const std::vector<M>& values) {
			if (values.size() == 0) return;

			// ADD TO OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.Add<M>(name, values.size());
			
			// ADD TO OFFSETS AND CHECK ERROR
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				return;
			}

			// UPDATE SIZE
			_data.resize(_dataOffsets.GetSize());

			// SET VALUES DATA
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET ROWS OFFSETS
				std::vector<size_t> rowsOffsets = _dataOffsets.GetArray<glm::vec<R, T>>(name + "[" + std::to_string(i) + "]");

				// SET ROWS DATA
				for (size_t r = 0; r < C && r < rowsOffsets.size(); ++r) {
					// GET VALUE DATA
					std::vector<char> valueData = _GetValueData(values[i][r]);

					// SET VALUE DATA
					memcpy(_data.data() + rowsOffsets[r], valueData.data(), valueData.size());
				}
			}
		}
		void _AddStruct(const std::string& name, const STD140Struct& value);
		void _AddStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

		template<class S, class C, class T>
		void _AddArray(const std::string& name, const T& values, size_t size, const Action<const std::string&, const std::vector<C>&>& addArrayFunc) {
			std::vector<C> convertedValues;
			for (size_t i = 0; i < size; ++i) {
				convertedValues.push_back((C)values[i]);
			}
			addArrayFunc(name, convertedValues);
		}
#pragma endregion

#pragma region SET
		template<class T> bool _Set(const std::string& name, const T& value) {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET VALUE OFFSET
			size_t valueOffset = _dataOffsets.Get(name);

			// GET VALUE DATA
			std::vector<char> valueData = _GetValueData(value);

			// GET VALUE DATA MAX SIZE
			size_t valueDataSize = glm::min(valueData.size(), _data.size() - valueOffset);

			// SET VALUE DATA
			memcpy(_data.data() + valueOffset, valueData.data(), valueDataSize);
			return true;
		}
		template<class T> bool _SetArray(const std::string& name, const std::vector<T>& values) {
			if (values.size() == 0) return false;

			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// CHECK ARRAY ELEMENTS OFFSETS
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				return false;
			}

			// GET ARRAY ELEM DATA MAX SIZE
			size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);

			// SET VALUES DATA
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET VALUE OFFSET
				size_t valueOffset = valuesOffsets[i];

				// GET VALUE DATA
				std::vector<char> valueData = _GetValueData(values[i]);

				// GET LOCAL VALUE DATA MAX SIZE
				size_t valueDataSize = glm::min(glm::min(valueData.size(), arrayElemDataSize), _data.size() - valueOffset);

				// SET VALUE DATA
				memcpy(_data.data() + valueOffset, valueData.size(), valueDataSize);
			}
			return true;
		}
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		bool _SetMat(const std::string& name, const M& value) {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET ROWS OFFSETS
			std::vector<size_t> rowsOffsets = _dataOffsets.GetArray(name);

			// CHECK ROWS ARRAY ELEMENTS OFFSETS
			if (rowsOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any matrix", name);
				return false;
			}

			// GET ROWS ARRAY ELEM MAX SIZE
			size_t rowsArrayElemSize = _GetArrayElemSize(rowsOffsets);

			// SET ROWS DATA
			for (size_t i = 0; i < C && i < rowsOffsets.size(); ++i) {
				// GET VALUE DATA
				std::vector<char> valueData = _GetValueData(value[i]);

				// GET VALUE DATA MAX SIZE
				size_t valueDataSize = glm::min(glm::min(valueData.size(), rowsArrayElemSize), _data.size() - rowsOffsets[i]);

				// SET VALUE DATA
				memcpy(_data.data() + rowsOffsets[i], valueData.data(), valueDataSize);
			}
			return true;
		}
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		bool _SetMatArray(const std::string& name, const std::vector<M>& values) {
			if (values.size() == 0) return false;

			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// CHECK ARRAY ELEMENTS OFFSETS
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				return false;
			}

			// GET ARRAY ELEM DATA MAX SIZE
			size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);

			// SET VALUES DATA
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET ROWS OFFSETS
				std::vector<size_t> rowsOffsets = _dataOffsets.GetArray(name + "[" + std::to_string(i) + "]");

				// CHECK ROWS ARRAY ELEMENTS OFFSETS
				if (rowsOffsets.size() == 0) {
					SPDLOG_ERROR("Value '{0}' was not declared as any matrix", name + "[" + std::to_string(i) + "]");
					return false;
				}

				// GET ROWS ARRAY ELEM DATA MAX SIZE
				size_t rowsArrayElemDataSize = _GetArrayElemSize(rowsOffsets);

				// GET MATRIX DATA MAX SIZE
				size_t matrixDataSize = glm::min(arrayElemDataSize, _data.size() - valuesOffsets[i]);

				// SET ROWS DATA
				for (size_t r = 0; r < C && r < rowsOffsets.size(); ++r) {
					// GET VALUE DATA
					std::vector<char> valueData = _GetValueData(values[i][r]);

					// GET VALUE DATA MAX SIZE
					size_t valueDataSize = glm::min(glm::min(valueData.size(), rowsArrayElemDataSize), valuesOffsets[i] + matrixDataSize - rowsOffsets[r]);

					// SET VALUE DATA
					memcpy(_data.data() + rowsOffsets[r], valueData.data(), valueDataSize);
				}
			}
			return true;
		}
		bool _SetStruct(const std::string& name, const STD140Struct& value);
		bool _SetStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values);

		template<class S, class C, class T>
		bool _SetArray(const std::string& name, const T& values, size_t size, const Func<bool, const std::string&, const std::vector<C>&>& setArrayFunc) {
			std::vector<C> convertedValues;
			for (size_t i = 0; i < size; ++i) {
				convertedValues.push_back((C)values[i]);
			}
			return setArrayFunc(name, convertedValues);
		}
#pragma endregion

#pragma region GET
		template<class T> T _Get(const std::string& name) const {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return T();
			}

			// GET VALUE OFFSET
			size_t valueOffset = _dataOffsets.Get(name);

			// MAKE EMPTY VALUE DATA
			std::vector<char> valueData;
			valueData.resize(sizeof(T));

			// GET MAX VALUE DATA
			size_t valueDataSize = glm::min(valueData.size(), _data.size() - valueOffset);

			// GET VALUE DATA
			memcpy(valueData.data(), _data.data() + valueOffset, valueDataSize);

			// GET VALUE
			T value = *reinterpret_cast<T*>(valueData.data());

			// RETURN VALUE
			return value;
		}
		template<class T> std::vector<T> _GetArray(const std::string& name) const {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return std::vector<T>();
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// CHECK ARRAY ELEMENTS OFFSETS
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				return std::vector<T>();
			}

			// GET ARRAY ELEM DATA MAX SIZE
			size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);

			// GET VALUES DATA
			std::vector<T> values;
			for (size_t i = 0; i < valuesOffsets.size(); ++i) {
				// MAKE EMPTY VALUE DATA
				std::vector<char> valueData;
				valueData.resize(sizeof(T));

				// GET MAX VALUE DATA
				size_t valueDataSize = glm::min(glm::min(valueData.size(), arrayElemDataSize), _data.size() - valuesOffsets[i]);

				// GET VALUE DATA
				memcpy(valueData.data(), _data.data() + valuesOffsets[i], valueDataSize);

				// GET VALUE
				values.push_back(*reinterpret_cast<T*>(valueData.data()));
			}
			
			// RETURN VALUES
			return values;
		}
		template<class M, class T = M::value_type, size_t R = M::column_type::length()>
		M _GetMat(const std::string& name) const {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return M();
			}

			// GET ROWS OFFSETS
			std::vector<size_t> rowsOffsets = _dataOffsets.GetArray(name);

			// CHECK ARRAY ELEMENTS OFFSETS
			if (rowsOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any matrix", name);
				return M();
			}

			// GET ARRAY ELEM DATA MAX SIZE
			size_t rowsArrayElemDataSize = _GetArrayElemSize(rowsOffsets);

			// MAKE EMPTY MAT
			M value{};

			// GET ROWS DATA
			for (size_t i = 0; i < M::row_type::length() && i < rowsOffsets.size(); ++i) {
				// MAKE EMPTY ROW DATA
				std::vector<char> rowData;
				rowData.resize(sizeof(glm::vec<R, T>));

				// GET MAX VALUE DATA
				size_t rowDataSize = glm::min(glm::min(rowData.size(), rowsArrayElemDataSize), _data.size() - rowsOffsets[i]);

				// GET ROW DATA
				memcpy(rowData.data(), _data.data() + rowsOffsets[i], rowDataSize);

				// GET ROW
				glm::vec<R, T> row = *reinterpret_cast<glm::vec<R, T>*>(rowData.data());

				// SET ROW
				value[i] = row;
			}

			// RETURN VALUE
			return value;
		}
		template<class M, class T = M::value_type, size_t R = M::column_type::length()>
		std::vector<M> _GetMatArray(const std::string& name) const {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return std::vector<M>();
			}

			// GET OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// CHECK ARRAY ELEMENTS OFFSETS
			if (valuesOffsets.size() == 0) {
				SPDLOG_ERROR("Value '{0}' was not declared as any array", name);
				return std::vector<M>();
			}

			// GET ARRAY ELEM DATA MAX SIZE
			size_t arrayElemDataSize = _GetArrayElemSize(valuesOffsets);

			// GET VALUES DATA
			std::vector<M> values;
			for (size_t i = 0; i < valuesOffsets.size(); ++i) {
				// GET ROWS OFFSETS
				std::vector<size_t> rowsOffsets = _dataOffsets.GetArray(name + "[" + std::to_string(i) + "]");

				// CHECK ROWS ARRAY ELEMENTS OFFSETS
				if (rowsOffsets.size() == 0) {
					SPDLOG_ERROR("Value '{0}' was not declared as any matrix", name + "[" + std::to_string(i) + "]");
					return std::vector<M>();
				}

				// GET ROWS ARRAY ELEM DATA MAX SIZE
				size_t rowsArrayElemDataSize = _GetArrayElemSize(rowsOffsets);

				// GET MATRIX DATA SIZE
				size_t matrixDataSize = glm::min(arrayElemDataSize, _data.size() - valuesOffsets[i]);

				// MAKE EMPTY MAT
				M value{};

				// GET ROWS DATA
				for (size_t r = 0; r < M::row_type::length() && r < rowsOffsets.size(); ++r) {
					// MAKE EMPTY ROW DATA
					std::vector<char> rowData;
					rowData.resize(sizeof(glm::vec<R, T>));

					// GET MAX VALUE DATA
					size_t rowDataSize = glm::min(glm::min(rowData.size(), rowsArrayElemDataSize), valuesOffsets[i] + matrixDataSize - rowsOffsets[r]);

					// GET ROW DATA
					memcpy(rowData.data(), _data.data() + rowsOffsets[r], rowDataSize);

					// GET ROW
					glm::vec<R, T> row = *reinterpret_cast<glm::vec<R, T>*>(rowData.data());

					// SET ROW
					value[r] = row;
				}

				values.push_back(value);
			}

			// RETURN VALUES
			return values;
		}
		STD140Struct _GetStruct(const std::string& name, const STD140Offsets& structOffsets) const;
		std::vector<STD140Struct> _GetStructArray(const std::string& name, const STD140Offsets& structOffsets) const;

		template<class S, class C>
		std::vector<C> _GetArray(const std::string& name, const Func<std::vector<S>, const std::string&>& getArrayFunc) {
			std::vector<S> values = getArrayFunc(name);
			std::vector<C> convertedValues;
			for (auto& val : values) {
				convertedValues.push_back((C)val);
			}
			return convertedValues;
		}
#pragma endregion

	public:
		STD140Struct() = default;
		STD140Struct(STD140Struct& std140s) = default;
		STD140Struct(const STD140Struct& std140s) = default;
		STD140Struct(STD140Struct&& std140s) = default;
		STD140Struct(const STD140Offsets& structOffsets, const std::vector<char>& data = std::vector<char>());
		virtual ~STD140Struct() = default;

		STD140Struct& operator=(STD140Struct& std140s) = default;
		STD140Struct& operator=(const STD140Struct& std140s) = default;
		STD140Struct& operator=(STD140Struct&& std140s) = default;

#pragma region ADD_SCALARS
		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T& value) {
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_Add(name, (type)value);
		}

#pragma region ADD_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T*& values, size_t size) {
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_AddArray<T, type>(name, values, size, [&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T(&values)[N]) {
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			_AddArray<T, type>(name, values, N, [&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const std::vector<T>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				_AddArray<T, unsigned int>(name, values, values.size(), 
					[&](const std::string& name, const std::vector<unsigned int>& values) -> void { _AddArray(name, values); });
			}
			else {
				_AddArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V& value) {
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_Add(name, (type)value);
		}

#pragma region ADD_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V*& values, size_t size) {
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_AddArray<V, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V(&values)[N]) {
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_AddArray<V, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const std::vector<V>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::vec<L, unsigned int>;
				_AddArray<V, type>(name, values, values.size(), 
					[&](const std::string& name, const std::vector<type>& values) -> void { _AddArray(name, values); });
			}
			else {
				_AddArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M& value) {
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_AddMat(name, (type)value);
		}

#pragma region ADD_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M*& values, size_t size) {
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_AddArray<M, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddMatArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M(&values)[N]) {
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			_AddArray<M, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> void { _AddMatArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, typename M::value_type, M::row_type::length(), M::column_type::length()>
		Add(const std::string& name, const std::vector<M>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				_AddArray<M, type>(name, values, values.size(), 
					[&](const std::string& name, const std::vector<type>& values) -> void { _AddMatArray(name, values); });
			}
			else {
				_AddMatArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		void Add(const std::string& name, const STD140Struct& value);

#pragma region ADD_STRUCT_ARRAYS
		void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>*& values, size_t size);

		template<size_t N> void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>(&values)[N]) {
			_AddArray<std::vector<char>, std::vector<char>>(name, values, N,
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
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _Set(name, (type)value);
		}
#pragma region SET_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T*& values, size_t size) {
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _SetArray<T, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T(&values)[N]) {
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return _SetArray<T, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const std::vector<T>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				return _SetArray<T, unsigned int>(name, values, values.size(), 
					[&](const std::string& name, const std::vector<unsigned int>& values) -> bool { return _SetArray(name, values); });
			}
			else {
				return _SetArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region SET_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V& value) {
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _Set(name, (type)value);
		}

#pragma region SET_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V*& values, size_t size) {
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _SetArray<V, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V(&values)[N]) {
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _SetArray<V, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const std::vector<V>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::vec<L, unsigned int>;
				return _SetArray<V, type>(name, values, values.size(),
					[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
			}
			else {
				return _SetArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region SET_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M& value) {
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _SetMat(name, (type)value);
		}

#pragma region SET_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M*& values, size_t size) {
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return _SetArray<M, type>(name, values, size, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetMatArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M(&values)[N]) {
			using type = glm::mat<C, R, type_test_t<std::is_same<T, bool>, unsigned int, T>>;
			return _SetArray<M, type>(name, values, N, 
				[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetMatArray(name, values); });
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const std::vector<M>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				return _SetArray<M, type>(name, values, values.size(),
					[&](const std::string& name, const std::vector<type>& values) -> bool { return _SetArray(name, values); });
			}
			else {
				return _SetMatArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region SET_STRUCT
		bool Set(const std::string& name, const STD140Struct& value);

#pragma region SET_STRUCT_ARRAYS
		bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>*& values, size_t size);

		template<size_t N> bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>(&values)[N]) {
			return _SetArray<std::vector<char>, std::vector<char>>(name, values, N, 
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
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			return (T)_Get<type>(name);
		}

#pragma region GET_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T>
		Get(const std::string& name, T*& valuesDest, size_t size) const {
			std::vector<T> values;
			if constexpr (std::is_same_v<T, bool>) {
				values = _GetArray<unsigned int, T>(name, [&](const std::string& name) -> std::vector<T> { return _GetArray<unsigned int>(name); });
			}
			else {
				values = _GetArray<T>(name);
			}
			memcpy(valuesDest, values.data(), glm::min(values.size(), size));
		}

		template<class V, class T = V::value_type>
		typename get_vector_enable_if_t<V, T, type_check_v<T>, V>
		Get(const std::string& name) const {
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
			using type = glm::vec<L, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return (V)_Get<type>(name);
		}

#pragma region GET_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Get(const std::string& name, V*& valuesDest, size_t size) const {
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
		}

		template<class Vec, class V = Vec::value_type, class T = V::value_type, size_t L = V::length()>
		typename get_vector_enable_if_t<Vec, V, vec_check_v<V, T, L>, Vec>
		Get(const std::string& name) const {
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
			using type = glm::mat<C, R, type_test_t<std::is_same_v<T, bool>, unsigned int, T>>;
			return (M)_GetMat<type>(name);
		}

#pragma region GET_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Get(const std::string& name, M*& valuesDest, size_t size) const {
			std::vector<M> values;
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				values = _GetArray<type, M>(name, [&](const std::string& name) -> std::vector<type> {
						return _GetMatArray<type>(name); 
					});
			}
			else {
				values = _GetMatArray<M>(name);
			}
			memcpy(valuesDest, values.data(), glm::min(values.size(), size));
		}

		template<class Vec, class M = Vec::value_type, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename get_vector_enable_if_t<Vec, M, mat_check_v<M, T, C, R>, Vec>
		Get(const std::string& name) const {
			if constexpr (std::is_same_v<T, bool>) {
				using type = glm::mat<C, R, unsigned int>;
				return _GetArray<type, M>(name, [&](const std::string& name) -> std::vector<type> {
						return _GetMatArray<type>(name); 
					});
			}
			else {
				return _GetMatArray<M>(name);
			}
		}
#pragma endregion
#pragma endregion

#pragma region GET_STRUCT
		template<class S>
		typename std::enable_if_t<std::is_same_v<S, STD140Struct>, S>
		Get(const std::string& name, const STD140Offsets& structOffsets) const {
			return _GetStruct(name, structOffsets);
		}

#pragma region GET_STRUCT_ARRAYS
		template<class S>
		typename std::enable_if_t<std::is_same_v<S, STD140Struct>>
		Get(const std::string& name, const STD140Offsets& structOffsets, STD140Struct*& valueDest, size_t size) const {
			std::vector<STD140Struct> values = _GetStructArray(name, structOffsets);
			memcpy(valueDest, values.data(), glm::min(values.size(), size));
		}

		template<class V, class S = V::value_type>
		typename get_vector_enable_if_t<V, S, std::is_same_v<S, STD140Struct>, V>
		Get(const std::string& name, const STD140Offsets& structTemplate) const {
			return _GetStructArray(name, structTemplate);
		}
#pragma endregion
#pragma endregion

		STD140Offsets GetOffsets() const;
		std::vector<char> GetData() const;
		size_t GetBaseAligement() const;
		size_t GetSize() const;

		void Clear();
	};
}