#pragma once

namespace Twin2Engine::GraphicEngine {
	template<class T, class... Ts> constexpr bool is_in_v = (std::is_same_v<T, Ts> || ...);
	template<class T, class... Ts> struct is_in : public std::bool_constant<is_in_v<T, Ts...>> {};

	class STD140Struct {
	private:
		size_t _currentOffset = 0;
		size_t _maxAligement = 0;

		std::vector<char> _data;
		std::map<size_t, size_t> _offsets;
		std::map<size_t, std::string> _names;

		static std::hash<std::string> _hasher;

		template<class T> std::vector<char> GetValueData(const T& value) {
			std::vector<char> valueData;
			valueData.resize(sizeof(T));
			const char* valueDataPtr = reinterpret_cast<const char*>(&value);
			memcpy(valueData.data(), valueDataPtr, sizeof(T));
			return valueData;
		}

		template<class T, size_t C, size_t R> std::vector<glm::vec<R, T>> GetMatRows(const glm::mat<C, R, T>& mat) {
			std::vector<glm::vec<R, T>> rows;
			for (size_t i = 0; i < C; ++i) {
				rows.push_back(mat[i]);
			}
			return rows;
		}

#pragma region ADD
		void Add(const std::string& name, const std::vector<char>& value, size_t baseAligement, size_t baseOffset);

		void AddArray(const std::string& name, const std::vector<std::vector<char>>& values, size_t baseAligement, size_t baseOffset);
		template<class T> void AddArray(const std::string& name, const T& values, size_t size, size_t baseAligement, size_t baseOffset) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < size; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			AddArray(name, valuesData, baseAligement, baseOffset);
		}
		template<class T> void AddMatArray(const std::string& name, const T& values, size_t size) {
			if (size == 0) return;
			for (size_t i = 0; i < size; ++i) {
				Add(name + "[" + std::to_string(i) + "]", values[i]);
			}
			// SET ARRAY BEGIN POINTER
			size_t nameHash = _hasher(name);
			size_t firstElemHash = _hasher(name + "[0]");
			_offsets[nameHash] = _offsets[firstElemHash];
		}
		template<class T> void AddStructArray(const std::string& name, const T& values, size_t size) {
			if (size == 0) return;
			for (size_t i = 0; i < size; ++i) {
				Add(name + "[" + std::to_string(i) + "]", values[i]);
			}
			// SET ARRAY BEGIN POINTER
			size_t nameHash = _hasher(name);
			size_t firstElemHash = _hasher(name + "[0]");
			_offsets[nameHash] = _offsets[firstElemHash];
		}
#pragma endregion

#pragma region SET
		bool Set(const std::string& name, const std::vector<char>& value);

		bool SetArray(const std::string& name, const std::vector<std::vector<char>>& values);
		template<class T> bool SetArray(const std::string& name, const T& values, size_t size) {
			if (size == 0) return true;
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < size; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			return SetArray(name, valuesData);
		}
		template<class T> bool SetMatArray(const std::string& name, const T& values, size_t size) {
			if (size == 0) return true;
			for (size_t i = 0; i < size; ++i) {
				if (!Set(name + "[" + std::to_string(i) + "]", values[i])) {
					return false;
				}
			}
			return true;
		}
		template<class T> bool SetStructArray(const std::string& name, const T& values, size_t size) {
			if (size == 0) return true;
			for (size_t i = 0; i < size; ++i) {
				if (!Set(name + "[" + std::to_string(i) + "]", values[i])) {
					return false;
				}
			}
			return true;
		}
#pragma endregion

#pragma region GET
		std::vector<char> Get(const std::string& name);

		std::vector<std::vector<char>> GetArray(const std::string& name);

#pragma endregion

	public:
		STD140Struct() = default;
		virtual ~STD140Struct() = default;

#pragma region ADD_SCALARS
		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>>
		Add(const std::string& name, const T& value) {
			if constexpr (std::is_same_v<T, bool>) {
				Add(name, GetValueData((unsigned int)value), 4, 4);
			}
			else {
				Add(name, GetValueData(value), sizeof(T), sizeof(T));
			}
		}

#pragma region ADD_SCALARS_ARRAYS
		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>>
		Add(const std::string& name, const T*& values, size_t size) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> bools;
				for (size_t i = 0; i < size; ++i) {
					bools.push_back((unsigned int)values[i]);
				}
				AddArray(name, bools, bools.size(), 4, 4);
			}
			else {
				AddArray(name, values, size, sizeof(T), sizeof(T));
			}
		}

		template<class T, size_t N>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>>
		Add(const std::string& name, const T(&values)[N]) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> bools;
				for (size_t i = 0; i < N; ++i) {
					bools.push_back((unsigned int)values[i]);
				}
				AddArray(name, bools, bools.size(), 4, 4);
			}
			else {
				AddArray(name, values, N, sizeof(T), sizeof(T));
			}
		}

		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>>
		Add(const std::string& name, const std::vector<T>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> bools;
				for (size_t i = 0; i < values.size(); ++i) {
					bools.push_back((unsigned int)values[i]);
				}
				AddArray(name, bools, bools.size(), 4, 4);
			}
			else {
				AddArray(name, values, values.size(), sizeof(T), sizeof(T));
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_VEC
		template<class T, size_t S>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, bool, double>>
		Add(const std::string& name, const glm::vec<S, T>& value) {
			size_t TSize;
			if constexpr (std::is_same_v<T, bool>) {
				TSize = sizeof(unsigned int);
			}
			else {
				TSize = sizeof(T);
			}

			if constexpr (S == 2 || S == 4) {
				Add(name, GetValueData(value), TSize * S, TSize * S);
			}
			else if constexpr (S == 3) {
				Add(name, GetValueData(value), TSize * (S + 1), TSize * S);
			}
		}

#pragma region ADD_VEC_ARRAYS
		template<class T, size_t S>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, bool, double>>
		Add(const std::string& name, const glm::vec<S, T>*& values, size_t size) {
			size_t TSize;
			if constexpr (std::is_same_v<T, bool>) {
				TSize = sizeof(unsigned int);
			}
			else {
				TSize = sizeof(T);
			}

			if constexpr (S == 2 || S == 4) {
				AddArray(name, values, size, TSize * S, TSize * S);
			}
			else if constexpr (S == 3) {
				AddArray(name, values, size, TSize * (S + 1), TSize * S);
			}
		}

		template<class T, size_t S, size_t N>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, bool, double>>
		Add(const std::string& name, const glm::vec<S, T>(&values)[N]) {
			size_t TSize;
			if constexpr (std::is_same_v<T, bool>) {
				TSize = sizeof(unsigned int);
			}
			else {
				TSize = sizeof(T);
			}

			if constexpr (S == 2 || S == 4) {
				AddArray(name, values, N, TSize * S, TSize * S);
			}
			else if constexpr (S == 3) {
				AddArray(name, values, N, TSize * (S + 1), TSize * S);
			}
		}

		template<class T, size_t S>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, bool, double>>
		Add(const std::string& name, const std::vector<glm::vec<S, T>>& values) {
			size_t TSize;
			if constexpr (std::is_same_v<T, bool>) {
				TSize = sizeof(unsigned int);
			}
			else {
				TSize = sizeof(T);
			}

			if constexpr (S == 2 || S == 4) {
				AddArray(name, values, values.size(), TSize * S, TSize * S);
			}
			else if constexpr (S == 3) {
				AddArray(name, values, values.size(), TSize * (S + 1), TSize * S);
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class T, size_t C, size_t R>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, double, bool>>
		Add(const std::string& name, const glm::mat<C, R, T>& value) {
			Add(name, GetMatRows(value));
		}

#pragma region ADD_MAT_ARRAYS
		template<class T, size_t C, size_t R>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, double, bool>>
		Add(const std::string& name, const glm::mat<C, R, T>*& values, size_t size) {
			AddMatArray(name, values, size);
		}

		template<class T, size_t C, size_t R, size_t N>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, double, bool>>
		Add(const std::string& name, const glm::mat<C, R, T>(&values)[N]) {
			AddMatArray(name, values, N);
		}

		template<class T, size_t C, size_t R>
		typename std::enable_if_t<is_in_v<T, float, int, unsigned int, double, bool>>
		Add(const std::string& name, const std::vector<glm::mat<C, R, T>>& values) {
			AddMatArray(name, values, values.size());
		}
#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		void Add(const std::string& name, const STD140Struct& value);

#pragma region ADD_STRUCT_ARRAYS
		void Add(const std::string& name, const STD140Struct*& values, size_t size);
		template<size_t N> void Add(const std::string& name, const STD140Struct(&values)[N]) {
			AddStructArray(name, values, N);
		}
		void Add(const std::string& name, const std::vector<STD140Struct>& values);
#pragma endregion
#pragma endregion


#pragma region SET_SCALARS
		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const T& value) {
			if constexpr (std::is_same_v<T, bool>) {
				return Set(name, GetValueData((unsigned int)value));
			}
			else {
				return Set(name, GetValueData(value));
			}
		}
#pragma region SET_SCALARS_ARRAYS
		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const T*& values, size_t size) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> bools;
				for (size_t i = 0; i < size; ++i) {
					bools.push_back((unsigned int)values[i]);
				}
				return SetArray(name, bools, bools.size());
			}
			else {
				return SetArray(name, values, size);
			}
		}

		template<class T, size_t N>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const T(&values)[N]) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> bools;
				for (size_t i = 0; i < N; ++i) {
					bools.push_back((unsigned int)values[i]);
				}
				return SetArray(name, bools, bools.size());
			}
			else {
				return SetArray(name, values, N);
			}
		}

		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const std::vector<T>& values) {
			if constexpr (std::is_same_v<T, bool>) {
				std::vector<unsigned int> bools;
				for (size_t i = 0; i < values.size(); ++i) {
					bools.push_back((unsigned int)values[i]);
				}
				return SetArray(name, bools, bools.size());
			}
			else {
				return SetArray(name, values, values.size());
			}
		}
#pragma endregion
#pragma endregion

#pragma region SET_VEC
		template<class T, size_t S>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const glm::vec<S, T>& value) {
			return Set(name, GetValueData(value));
		}

#pragma region SET_VEC_ARRAYS
		template<class T, size_t S>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const glm::vec<S, T>*& values, size_t size) {
			return SetArray(name, values, size);
		}

		template<class T, size_t S, size_t N>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const glm::vec<S, T>(&values)[N]) {
			return SetArray(name, values, N);
		}

		template<class T, size_t S>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const std::vector<glm::vec<S, T>>& values) {
			return SetArray(name, values, values.size());
		}
#pragma endregion
#pragma endregion

#pragma region SET_MAT
		template<class T, size_t C, size_t R>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const glm::mat<C, R, T>& value) {
			return Set(name, GetMatRows(value));
		}

#pragma region SET_MAT_ARRAYS
		template<class T, size_t C, size_t R>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const glm::mat<C, R, T>*& values, size_t size) {
			return SetMatArray(name, values, size);
		}

		template<class T, size_t C, size_t R, size_t N>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
		Set(const std::string& name, const glm::mat<C, R, T>(&values)[N]) {
			return SetMatArray(name, values, N);
		}

		template<class T, size_t C, size_t R>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double, bool>, bool>
			Set(const std::string& name, const std::vector<glm::mat<C, R, T>>& values) {
			return SetMatArray(name, values, values.size());
		}
#pragma endregion
#pragma endregion

#pragma region SET_STRUCT
		bool Set(const std::string& name, const STD140Struct& value);

#pragma region SET_STRUCT_ARRAYS
		bool Set(const std::string& name, const STD140Struct*& values, size_t size);
		template<size_t N> bool Set(const std::string& name, const STD140Struct(&values)[N]) {
			return SetStructArray(name, values, N);
		}
		bool Set(const std::string& name, const std::vector<STD140Struct>& values);
#pragma endregion
#pragma endregion

		template<class T> T Get(const std::string& name);

		std::vector<char> GetData() const;
		size_t GetBaseAligement() const;
		size_t GetSize() const;
	};
}