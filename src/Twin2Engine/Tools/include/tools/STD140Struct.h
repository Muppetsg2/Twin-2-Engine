#pragma once

#include <tools/STD140Offsets.h>

namespace Twin2Engine::Tools {
	class STD140Struct {
	private:
#pragma region CHECKS
		template<class T> static constexpr bool type_check_v = is_type_in_v<T, bool, int, unsigned int, float, double>;
		template<class T, size_t L> static constexpr bool vec_check_v = type_check_v<T> && is_num_in_range_v<L, 1, 4>;
		template<class T, size_t C, size_t R> static constexpr bool mat_check_v = vec_check_v<T, C> && is_num_in_range_v<R, 1, 4>;
		template<class T> static constexpr bool struct_check_v = std::is_same_v<T, STD140Struct>;

		template<class V, class T> static constexpr bool is_vector_of_v = std::is_same_v<V, std::vector<T>>;
		template<class V, class T, bool Test> static constexpr bool get_vector_check_v = is_vector_of_v<V, T> && Test;

		template<class T, class Ret = void> using scalar_enable_if_t = std::enable_if_t<type_check_v<T>, Ret>;
		template<class V, class T, size_t L, class Ret = void> using vec_enable_if_t = std::enable_if_t<std::is_same_v<V, glm::vec<L, T>> && vec_check_v<T, L>, Ret>;
		template<class M, class T, size_t C, size_t R, class Ret = void> using mat_enable_if_t = std::enable_if_t<std::is_same_v<M, glm::mat<C, R, T>> && mat_check_v<T, C, R>, Ret>;

		template<class V, class T, bool Test, class Ret = void> using get_vector_enable_if_t = std::enable_if_t<get_vector_check_v<V, T, Test>, Ret>;
#pragma endregion

		STD140Offsets _dataOffsets;
		std::vector<char> _data;

		template<class T> std::vector<char> _GetValueData(const T& value) {
			std::vector<char> valueData;
			valueData.resize(sizeof(T));
			const char* valueDataPtr = reinterpret_cast<const char*>(&value);
			memcpy(valueData.data(), valueDataPtr, sizeof(T));
			return valueData;
		}

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
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		void _AddMat(const std::string& name, const M& value) {
			// ADD TO OFFSETS AND CHECK ERROR
			if (_dataOffsets.Add<M>(name) == 0 && _data.size() != 0) {
				SPDLOG_ERROR("Variable '{0}' already added to structure", name);
				return;
			}

			// UPDATE SIZE
			_data.resize(_dataOffsets.GetSize());

			// GET ROWS OFFSETS
			std::vector<size_t> rowsOffsets = _dataOffsets.GetArray<glm::vec<R, T>>(name);

			// SET ROWS DATA
			for (size_t i = 0; i < C && i < rowsOffsets.size(); ++i) {
				// GET VALUE DATA
				std::vector<char> valueData = _GetValueData(value[i]);

				// SET VALUE DATA
				memcpy(_data.data() + rowsOffsets[i], valueData.data(), valueData.size());
			}
		}
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		void _AddMatArray(const std::string& name, const std::vector<M>& values) {
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
				std::vector<size_t> rowsOffsets = _dataOffsets.GetArray<glm::vec<R, T>>(name);

				// SET ROWS DATA
				for (size_t r = 0; r < C && r < rowsOffsets.size(); ++r) {
					// GET VALUE DATA
					std::vector<char> valueData = _GetValueData(values[i][r]);

					// SET VALUE DATA
					memcpy(_data.data() + rowsOffsets[r], valueData.data(), valueData.size());
				}
			}
		}
		void _AddStruct(const std::string& name, const STD140Struct& value) {
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
			std::vector<char> valueData = value._data;

			// SET VALUE DATA
			memcpy(_data.data() + valueOffset, valueData.data(), valueData.size());
		}
		void _AddStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values) {
			// ADD TO OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.Add(name, structOffsets, values.size());

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
				std::vector<char> valueData = values[i];

				// SET VALUE DATA
				memcpy(_data.data() + valuesOffsets[i], valueData.data(), valueData.size());
			}
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
			size_t valueDataSize = valueData.size() > (_data.size() - valueOffset) ? (_data.size() - valueOffset) : valueData.size();

			// SET VALUE DATA
			memcpy(_data.data() + valueOffset, valueData.data(), valueDataSize);
			return true;
		}
		template<class T> bool _SetArray(const std::string& name, const std::vector<T>& values) {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// SET VALUES DATA
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET VALUE OFFSET
				size_t valueOffset = valuesOffsets[i];

				// GET VALUE DATA
				std::vector<char> valueData = _GetValueData(values[i]);

				// GET VALUE DATA MAX SIZE
				size_t valueDataSize = valueData.size() > (_data.size() - valueOffset) ? (_data.size() - valueOffset) : valueData.size();

				// SET VALUE DATA
				memcpy(_data.data() + valueOffset, valueData.size(), valueDataSize);
			}
			return true;
		}
		template<class M, class T, size_t C, size_t R>
		bool _SetMat(const std::string& name, const M& value) {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET ROWS OFFSETS
			std::vector<size_t> rowsOffsets = _dataOffsets.GetArray<glm::vec<R, T>>(name);

			// SET ROWS DATA
			for (size_t i = 0; i < C && i < rowsOffsets.size(); ++i) {
				// GET VALUE DATA
				std::vector<char> valueData = _GetValueData(value[i]);

				// GET VALUE DATA MAX SIZE
				size_t valueDataSize = valueData.size() > (_data.size() - rowsOffsets[i]) ? (_data.size() - rowsOffsets[i]) : valueData.size();

				// SET VALUE DATA
				memcpy(_data.data() + rowsOffsets[i], valueData.data(), valueDataSize);
			}
			return true;
		}
		template<class M, class T, size_t C, size_t R>
		bool _SetMatArray(const std::string& name, const std::vector<M>& values) {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// SET VALUES DATA
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET ROWS OFFSETS
				std::vector<size_t> rowsOffsets = _dataOffsets.GetArray<glm::vec<R, T>>(name);

				// SET ROWS DATA
				for (size_t r = 0; r < C && r < rowsOffsets.size(); ++r) {
					// GET VALUE DATA
					std::vector<char> valueData = _GetValueData(values[i][r]);

					// GET VALUE DATA MAX SIZE
					size_t valueDataSize = valueData.size() > (_data.size() - rowsOffsets[r]) ? (_data.size() - rowsOffsets[r]) : valueData.size();

					// SET VALUE DATA
					memcpy(_data.data() + rowsOffsets[r], valueData.data(), valueDataSize);
				}
			}
			return true;
		}
		bool _SetStruct(const std::string& name, const STD140Struct& value) {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET OFFSET
			size_t valueOffset = _dataOffsets.Get(name);

			// GET VALUE DATA
			std::vector<char> valueData = value._data;

			// GET VALUE DATA MAX SIZE
			size_t valueDataSize = valueData.size() > (_data.size() - valueOffset) ? (_data.size() - valueOffset) : valueData.size();

			// SET VALUE DATA
			memcpy(_data.data() + valueOffset, valueData.data(), valueData.size());
		}
		bool _SetStructArray(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values) {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return false;
			}

			// GET OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// SET VALUES DATA
			for (size_t i = 0; i < valuesOffsets.size() && i < values.size(); ++i) {
				// GET VALUE DATA
				std::vector<char> valueData = values[i];

				// GET VALUE DATA MAX SIZE
				size_t valueDataSize = valueData.size() > (_data.size() - valuesOffsets[i]) ? (_data.size() - valuesOffsets[i]) : valueData.size();

				// SET VALUE DATA
				memcpy(_data.data() + valuesOffsets[i], valueData.data(), valueDataSize);
			}
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
			size_t valueDataSize = valueData.size() > (_data.size() - valueOffset) ? (_data.size() - valueOffset) : valueData.size();

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

			// GET VALUES DATA
			std::vector<T> values;
			for (size_t i = 0; i < valuesOffsets.size(); ++i) {
				// MAKE EMPTY VALUE DATA
				std::vector<char> valueData;
				valueData.resize(sizeof(T));

				// GET MAX VALUE DATA
				size_t valueDataSize = std::min(valueData.size(), _data.size() - valuesOffsets[i]);

				// GET VALUE DATA
				memcpy(valueData.data(), _data.data() + valuesOffsets[i], valueDataSize);

				// GET VALUE
				values.push_back(*reinterpret_cast<T*>(valueData.data()));
			}
			
			// RETURN VALUES
			return values;
		}
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		M _GetMat(const std::string& name) const {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return M();
			}

			// GET ROWS OFFSETS
			std::vector<size_t> rowsOffsets = _dataOffsets.GetArray(name);

			// MAKE EMPTY MAT
			M value{};

			// GET ROWS DATA
			for (size_t i = 0; i < C && i < rowsOffsets.size(); ++i) {
				// MAKE EMPTY ROW DATA
				std::vector<char> rowData;
				rowData.resize(sizeof(glm::vec<R, T>));

				// GET MAX VALUE DATA
				size_t rowDataSize = rowData.size() > (_data.size() - rowsOffsets[i]) ? (_data.size() - rowsOffsets[i]) : rowData.size();

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
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		std::vector<M> _GetMatArray(const std::string& name) const {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return std::vector<M>();
			}

			// GET VALUES DATA
			std::vector<M> values;
			for (size_t i = 0; i < _dataOffsets.GetArray(name).size(); ++i) {
				// GET ROWS OFFSETS
				std::vector<size_t> rowsOffsets = _dataOffsets.GetArray(name + "[" + std::to_string(i) + "]");

				// MAKE EMPTY MAT
				M value{};

				// GET ROWS DATA
				for (size_t i = 0; i < C && i < rowsOffsets.size(); ++i) {
					// MAKE EMPTY ROW DATA
					std::vector<char> rowData;
					rowData.resize(sizeof(glm::vec<R, T>));

					// GET MAX VALUE DATA
					size_t rowDataSize = rowData.size() > (_data.size() - rowsOffsets[i]) ? (_data.size() - rowsOffsets[i]) : rowData.size();

					// GET ROW DATA
					memcpy(rowData.data(), _data.data() + rowsOffsets[i], rowDataSize);

					// GET ROW
					glm::vec<R, T> row = *reinterpret_cast<glm::vec<R, T>*>(rowData.data());

					// SET ROW
					value[i] = row;
				}

				values.push_back(value);
			}

			// RETURN VALUES
			return values;
		}
		STD140Struct _GetStruct(const std::string& name, const STD140Offsets& structOffsets) const {
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
			size_t valueDataSize = std::min(_data.size() - valueOffset, structOffsets.GetSize());

			// SET VALUE DATA
			memcpy(value._data.data(), _data.data() + valueOffset, valueDataSize);

			// RETURN VALUE
			return value;
		}
		std::vector<STD140Struct> _GetStructArray(const std::string& name, const STD140Offsets& structOffsets) const {
			// CHECK VARIABLE
			if (!_dataOffsets.Contains(name)) {
				SPDLOG_ERROR("No value called '{0}' was added to this structure", name);
				return std::vector<STD140Struct>();
			}

			// GET VALUES OFFSETS
			std::vector<size_t> valuesOffsets = _dataOffsets.GetArray(name);

			// GET VALUES DATA
			std::vector<STD140Struct> values;
			for (size_t i = 0; i < valuesOffsets.size(); ++i) {
				// MAKE EMPTY STRUCT
				STD140Struct value(structOffsets);

				// GET MAX VALUE DATA
				size_t valueDataSize = std::min(_data.size() - valuesOffsets[i], structOffsets.GetSize());

				// SET VALUE DATA
				memcpy(value._data.data(), _data.data() + valuesOffsets[i], valueDataSize);

				// ADD VALUE TO VALUES
				values.push_back(value);
			}

			// RETURN VALUE
			return values;
		}
#pragma endregion

	public:
		STD140Struct() = default;
		STD140Struct(STD140Struct& std140s) = default;
		STD140Struct(const STD140Struct& std140s) = default;
		STD140Struct(STD140Struct&& std140s) = default;
		STD140Struct(const STD140Offsets& structOffsets, const std::vector<char>& data = std::vector<char>()) {
			_dataOffsets = structOffsets;
			_data.resize(_dataOffsets.GetSize());
			memcpy(_data.data(), data.data(), std::min(_data.size(), data.size()));
		}
		virtual ~STD140Struct() = default;

		STD140Struct& operator=(STD140Struct& std140s) = default;
		STD140Struct& operator=(const STD140Struct& std140s) = default;
		STD140Struct& operator=(STD140Struct&& std140s) = default;

#pragma region ADD_SCALARS
		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T& value) {
			if constexpr (std::is_same_v<T, bool>)
				_Add(name, (unsigned int)value);
			else 
				_Add(name, value);
		}

#pragma region ADD_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T*& values, size_t size) {
			using type = type_test_t<std::is_same_v<T, bool>, unsigned int, T>;
			std::vector<type> scalars;
			for (size_t i = 0; i < size; ++i) {
				scalars.push_back((type)values[i]);
			}
			_AddArray(name, scalars);
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const T(&values)[N]) {
			using type = type_test<std::is_same_v<T, bool>, unsigned int, T>;
			std::vector<type> scalars;
			for (size_t i = 0; i < N; ++i) {
				scalars.push_back((type)values[i]);
			}
			_AddArray(name, scalars);
		}

		template<class T>
		typename scalar_enable_if_t<T>
		Add(const std::string& name, const std::vector<T>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> scalars;
				for (size_t i = 0; i < values.size(); ++i) {
					scalars.push_back((unsigned int)values[i]);
				}
				_AddArray(name, scalars);
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
			if constexpr (std::is_same_v<T, bool>)
				_Add(name, (glm::vec<L, unsigned int>)value);
			else
				_Add(name, value);
		}

#pragma region ADD_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::lenght()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V*& values, size_t size) {
			using type = type_test_t<std::is_same_v<T, bool>, glm::vec<L, unsigned int>, V>;
			std::vector<type> vecs;
			for (size_t i = 0; i < size; ++i) {
				vecs.push_back((type)values[i]);
			}
			_AddArray(name, vecs);
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const V(&values)[N]) {
			using type = type_test_t<std::is_same_v<T, bool>, glm::vec<L, unsigned int>, V>;
			std::vector<type> vecs;
			for (size_t i = 0; i < N; ++i) {
				vecs.push_back((type)values[i]);
			}
			_AddArray(name, vecs);
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Add(const std::string& name, const std::vector<V>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::vec<L, unsigned int>> vecs;
				for (size_t i = 0; i < values.size(); ++i) {
					vecs.push_back((glm::vec<L, unsigned int>)values[i]);
				}
				_AddArray(name, vecs);
			}
			else {
				_AddArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M& value) {
			if constexpr (std::is_same_v<T, bool>) {
				_AddMat(name, (glm::mat<C, R, unsigned int>)value);
			}
			else {
				_AddMat(name, value);
			}
		}

#pragma region ADD_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M*& values, size_t size) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::mat<C, R, unsigned int>> mats;
				for (size_t i = 0; i < size; ++i) {
					mats.push_back((glm::mat<C, R, unsigned int>)values[i]);
				}
				_AddMatArray(name, mats);
			}
			else {
				std::vector<M> mats;
				for (size_t i = 0; i < size; ++i) {
					mats.push_back(values[i]);
				}
				_AddMatArray(name, mats);
			}
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const M(&values)[N]) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::mat<C, R, unsigned int>> mats;
				for (size_t i = 0; i < N; ++i) {
					mats.push_back((glm::mat<C, R, unsigned int>)values[i]);
				}
				_AddMatArray(name, mats);
			}
			else {
				std::vector<M> mats;
				for (size_t i = 0; i < N; ++i) {
					mats.push_back(values[i]);
				}
				_AddMatArray(name, mats);
			}
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Add(const std::string& name, const std::vector<glm::mat<C, R, T>>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::mat<C, R, unsigned int>> mats;
				for (size_t i = 0; i < values.size(); ++i) {
					mats.push_back((glm::mat<C, R, unsigned int>)values[i]);
				}
				_AddMatArray(name, mats);
			}
			else {
				_AddMatArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		void Add(const std::string& name, const STD140Struct& value) {
			_AddStruct(name, value);
		}

#pragma region ADD_STRUCT_ARRAYS
		void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>*& values, size_t size) {
			std::vector<std::vector<char>> structs;
			for (size_t i = 0; i < size; ++i) {
				structs.push_back(values[i]);
			}
			_AddStructArray(name, structOffsets, structs);
		}
		template<size_t N> void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>(&values)[N]) {
			std::vector<std::vector<char>> structs;
			for (size_t i = 0; i < N; ++i) {
				structs.push_back(values[i]);
			}
			_AddStructArray(name, structOffsets, structs);
		}
		void Add(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values) {
			_AddStructArray(name, structOffsets, values);
		}
#pragma endregion
#pragma endregion


#pragma region SET_SCALARS
		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T& value) {
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
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> scalars;
				for (size_t i = 0; i < size; ++i) {
					scalars.push_back((unsigned int)values[i]);
				}
				return _SetArray(name, scalars);
			}
			else {
				std::vector<T> scalars;
				for (size_t i = 0; i < size; ++i) {
					scalars.push_back(values[i]);
				}
				return _SetArray(name, scalars);
			}
		}

		template<class T, size_t N>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const T(&values)[N]) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> scalars;
				for (size_t i = 0; i < N; ++i) {
					scalars.push_back((unsigned int)values[i]);
				}
				return _SetArray(name, scalars);
			}
			else {
				std::vector<T> scalars;
				for (size_t i = 0; i < N; ++i) {
					scalars.push_back(values[i]);
				}
				return _SetArray(name, values, N);
			}
		}

		template<class T>
		typename scalar_enable_if_t<T, bool>
		Set(const std::string& name, const std::vector<T>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> scalars;
				for (size_t i = 0; i < values.size(); ++i) {
					scalars.push_back((unsigned int)values[i]);
				}
				return _SetArray(name, scalars);
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
			if constexpr (std::is_same_v<T, bool>) {
				return _Set(name, (glm::vec<L, unsigned int>)value);
			}
			else {
				return _Set(name, value);
			}
		}

#pragma region SET_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V*& values, size_t size) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::vec<L, unsigned int>> vecs;
				for (size_t i = 0; i < size; ++i) {
					vecs.push_back((glm::vec<L, unsigned int>)values[i]);
				}
				return _SetArray(name, vecs);
			}
			else {
				std::vector<V> vecs;
				for (size_t i = 0; i < size; ++i) {
					vecs.pop_back(values[i]);
				}
				return _SetArray(name, vecs);
			}
		}

		template<class V, class T = V::value_type, size_t L = V::length(), size_t N>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const V(&values)[N]) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::vec<L, unsigned int>> vecs;
				for (size_t i = 0; i < N; ++i) {
					vecs.push_back((glm::vec<L, unsigned int>)values[i]);
				}
				return _SetArray(name, vecs);
			}
			else {
				std::vector<V> vecs;
				for (size_t i = 0; i < N; ++i) {
					vecs.pop_back(values[i]);
				}
				return _SetArray(name, vecs);
			}
		}

		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, bool>
		Set(const std::string& name, const std::vector<V>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::vec<L, unsigned int>> vecs;
				for (size_t i = 0; i < values.size(); ++i) {
					vecs.push_back((glm::vec<L, unsigned int>)values[i]);
				}
				return _SetArray(name, vecs);
			}
			else {
				return _SetArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region SET_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::lenght(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M& value) {
			if constexpr (std::is_same_v<T, bool>) {
				return _SetMat(name, (glm::mat<C, R, unsigned int>)value);
			}
			else {
				return _SetMat(name, value);
			}
		}

#pragma region SET_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M*& values, size_t size) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::mat<C, R, unsigned int>> mats;
				for (size_t i = 0; i < size; ++i) {
					mats.push_back(values[i]);
				}
				return _SetMatArray(name, mats);
			}
			else {
				std::vector<M> mats;
				for (size_t i = 0; i < size; ++i) {
					mats.push_back(values[i]);
				}
				return _SetMatArray(name, mats);
			}
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length(), size_t N>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const M(&values)[N]) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::mat<C, R, unsigned int>> mats;
				for (size_t i = 0; i < N; ++i) {
					mats.push_back(values[i]);
				}
				return _SetMatArray(name, mats);
			}
			else {
				std::vector<M> mats;
				for (size_t i = 0; i < N; ++i) {
					mats.push_back(values[i]);
				}
				return _SetMatArray(name, mats);
			}
		}

		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<M, T, C, R, bool>
		Set(const std::string& name, const std::vector<M>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::mat<C, R, unsigned int>> mats;
				for (size_t i = 0; i < values.size(); ++i) {
					mats.push_back(values[i]);
				}
				return _SetMatArray(name, mats);
			}
			else {
				return _SetMatArray(name, values);
			}
		}
#pragma endregion
#pragma endregion

#pragma region SET_STRUCT
		bool Set(const std::string& name, const STD140Struct& value) {
			return _SetStruct(name, value);
		}

#pragma region SET_STRUCT_ARRAYS
		bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>*& values, size_t size) {
			std::vector<std::vector<char>> structs;
			for (size_t i = 0; i < size; ++i) {
				structs.push_back(values[i]);
			}
			return _SetStructArray(name, structOffsets, structs);
		}

		template<size_t N> bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<char>(&values)[N]) {
			std::vector<std::vector<char>> structs;
			for (size_t i = 0; i < N; ++i) {
				structs.push_back(values[i]);
			}
			return _SetStructArray(name, structOffsets, structs);
		}

		bool Set(const std::string& name, const STD140Offsets& structOffsets, const std::vector<std::vector<char>>& values) {
			return _SetStructArray(name, structOffsets, values);
		}
#pragma endregion
#pragma endregion


#pragma region GET_SCALARS
		template<class T>
		typename scalar_enable_if_t<T, T>
		Get(const std::string& name) const {
			if constexpr (std::is_same_v<T, bool>) {
				return (bool)_Get<unsigned int>(name);
			}
			else {
				return _Get<T>(name);
			}
		}

#pragma region GET_SCALARS_ARRAYS
		template<class T>
		typename scalar_enable_if_t<T>
		Get(const std::string& name, T*& valuesDest, size_t size) const {
			std::vector<T> values;
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> valuesData = _GetArray<unsigned int>(name);
				for (auto& valueData : valuesData) {
					values.push_back((bool)valueData);
				}
				
			}
			else {
				values = _GetArray<T>(name);
			}
			memcpy(valuesDest, values.data(), values.size() < size ? values.size() : size);
		}

		template<class V, class T = V::value_type>
		typename get_vector_enable_if_t<V, T, type_check_v<T>, V>
		Get(const std::string& name) const {
			V values{};
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> valuesData = _GetArray<unsigned int>(name);
				for (auto& valueData : valuesData) {
					values.push_back((bool)valueData);
				}
			}
			else {
				values = _GetArray<T>(name);
			}
			return values;
		}
#pragma endregion
#pragma endregion

#pragma region GET_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, V>
		Get(const std::string& name) const {
			if constexpr (std::is_same_v<T, bool>) {
				return (V)Get<glm::vec<L, unsigned int>>(name);
			}
			else {
				return Get<V>(name);
			}
		}

#pragma region GET_VEC_ARRAYS
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L>
		Get(const std::string& name, V*& valuesDest, size_t size) const {
			std::vector<V> values;
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::vec<L, unsigned int>> valuesData = _GetArray<glm::vec<L, unsigned int>>(name);
				for (auto& valueData : valuesData) {
					values.push_back((V)valueData);
				}
			}
			else {
				values = _GetArray<V>(name);
			}
			memcpy(valuesDest, values.data(), values.size() < size ? values.size() : size);
		}

		template<class Vec, class V = Vec::value_type, class T = V::value_type, size_t L = V::length()>
		typename get_vector_enable_if_t<Vec, V, std::is_same_v<V, glm::vec<L, T>> && vec_check_v<T, L>, Vec>
		Get(const std::string& name) const {
			Vec values{};
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::vec<L, unsigned int>> valuesData = _GetArray<glm::vec<L, unsigned int>>(name);
				for (auto& valueData : valuesData) {
					values.push_back((V)valueData);
				}
			}
			else {
				values = GetArray<V>(name);
			}
			return values;
		}
#pragma endregion
#pragma endregion

#pragma region GET_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, M>
		Get(const std::string& name) const {
			M value{};
			if constexpr (std::is_same_v<T, bool>) {
				value = (M)_GetMat<glm::mat<C, R, unsigned int>>(name);
			}
			else {
				value = _GetMat<M>(name);
			}
			return value;
		}

#pragma region GET_MAT_ARRAYS
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R>
		Get(const std::string& name, M*& valuesDest, size_t size) const {
			std::vector<M> values;
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<glm::mat<C, R, unsigned int>> mats = _GetMatArray<glm::mat<C, R, unsigned int>>(name);
				for (auto& mat : mats) {
					values.push_back((M)mat);
				}
			}
			else {
				values = _GetMatArray<M>(name);
			}
			memcpy(valuesDest, values.data(), values.size() < size ? values.size() : size);
		}

		template<class V, class M = V::value_type, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename get_vector_enable_if_t<V, M, mat_check_v<T, C, R>, V>
		Get(const std::string& name) const {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<M> values;
				std::vector<glm::mat<C, R, unsigned int>> mats = _GetMatArray<glm::mat<C, R, unsigned int>>(name);
				for (auto& mat : mats) {
					values.push_back((M)mat);
				}
				return values;
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
			memcpy(valueDest, values.data(), std::min(values.size(), size));
		}

		template<class V, class S = V::value_type>
		typename get_vector_enable_if_t<V, S, std::is_same_v<S, STD140Struct>, V>
		Get(const std::string& name, const STD140Offsets& structTemplate) const {
			return _GetStructArray(name, structTemplate);
		}
#pragma endregion
#pragma endregion

		STD140Offsets GetOffsets() const {
			return _dataOffsets;
		}
		std::vector<char> GetData() const {
			return _data;
		}
		size_t GetBaseAligement() const {
			return _dataOffsets.GetBaseAligement();
		}
		size_t GetSize() const {
			return _data.size();
		}

		void Clear() {
			_dataOffsets.Clear();
			_data.clear();
		}
	};
}