#pragma once

namespace Twin2Engine::GraphicEngine {
	template<class T, class... Ts> struct is_in : public std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};
	template<class T, class... Ts> using is_in_v = is_in<T, Ts...>::value;

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
		template<class T> void AddArray(const std::string& name, const T*& values, size_t size, size_t baseAligement, size_t baseOffset) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < size; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			AddArray(name, valuesData, baseAligement, baseOffset);
		}
		template<class T, size_t N> void AddArray(const std::string& name, const T(&values)[N], size_t baseAligement, size_t baseOffset) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < N; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			AddArray(name, valuesData, baseAligement, baseOffset);
		}
		template<class T> void AddArray(const std::string& name, const std::vector<T>& values, size_t baseAligement, size_t baseOffset) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < values.size(); ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			AddArray(name, valuesData, baseAligement, baseOffset);
		}
#pragma endregion

#pragma region SET
		bool Set(const std::string& name, const std::vector<char>& value);

		bool SetArray(const std::string& name, const std::vector<std::vector<char>>& values);
		template<class T> bool SetArray(const std::string& name, const T*& values, size_t size) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < size; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			SetArray(name, valuesData);
		}
		template<class T, size_t N> void SetArray(const std::string& name, const T(&values)[N]) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < N; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			SetArray(name, valuesData);
		}
		template<class T> void SetArray(const std::string& name, const std::vector<T>& values) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < values.size(); ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			SetArray(name, valuesData);
		}
#pragma endregion

	public:
		STD140Struct() = default;
		virtual ~STD140Struct() = default;

#pragma region ADD_SCALARS
		template<class T>
		typename std::enable_if_t<is_in_v<T, int, unsigned int, float, double>>
		Add(const std::string& name, const T& value) {
			Add(name, GetValueData(value), 4, 4);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, bool>>
		Add(const std::string& name, const T& value) {
			Add(name, (unsigned int)value);
		}

#pragma region ADD_SCALARS_ARRAYS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, const int*> ||
			std::is_same_v<T, const unsigned int*> ||
			std::is_same_v<T, const float*> ||
			std::is_same_v<T, const double*> ||
			std::is_same_v<T, int*> ||
			std::is_same_v<T, unsigned int*> ||
			std::is_same_v<T, float*> ||
			std::is_same_v<T, double*>>
			Add(const std::string& name, const T& values, size_t size) {
			Add(name, values, size, 4, 4);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, const bool*>>
			Add(const std::string& name, const T& values, size_t size) {
			std::vector<unsigned int> bools;
			for (size_t i = 0; i < size; ++i) {
				bools.push_back((unsigned int)values[i]);
			}
			Add(name, bools);
		}

		template<class T, size_t N>
		typename std::enable_if_t<
			std::is_same_v<T, int[N]> ||
			std::is_same_v<T, unsigned int[N]> ||
			std::is_same_v<T, float[N]> ||
			std::is_same_v<T, double[N]> ||
			std::is_same_v<T, const int[N]> ||
			std::is_same_v<T, const unsigned int[N]> ||
			std::is_same_v<T, const float[N]> ||
			std::is_same_v<T, const double[N]>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 4, 4);
		}

		template<class T, size_t N>
		typename std::enable_if_t<std::is_same_v<T, const bool[N]>>
			Add(const std::string& name, const T& values) {
			std::vector<unsigned int> bools;
			for (size_t i = 0; i < N; ++i) {
				bools.push_back((unsigned int)values[i]);
			}
			Add(name, values);
		}

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, std::vector<int>> ||
			std::is_same_v<T, std::vector<unsigned int>> ||
			std::is_same_v<T, std::vector<float>> ||
			std::is_same_v<T, std::vector<double>>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 4, 4);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, std::vector<bool>>>
			Add(const std::string& name, const T& values) {
			std::vector<unsigned int> bools;
			for (size_t i = 0; i < values.size(); ++i) {
				bools.push_back((unsigned int)values[i]);
			}
			Add(name, values, 4, 4);
		}
#pragma endregion
#pragma endregion

#pragma region ADD_VEC_2
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec2> ||
			std::is_same_v<T, glm::ivec2> ||
			std::is_same_v<T, glm::uvec2> ||
			std::is_same_v<T, glm::bvec2> ||
			std::is_same_v<T, glm::dvec2>>
			Add(const std::string& name, const T& value) {
			Add(name, GetValueData(value), 8, 8);
		}

#pragma region ADD_VEC_2_ARRAYS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec2*> ||
			std::is_same_v<T, glm::ivec2*> ||
			std::is_same_v<T, glm::uvec2*> ||
			std::is_same_v<T, glm::bvec2*> ||
			std::is_same_v<T, glm::dvec2*> ||
			std::is_same_v<T, const glm::vec2*> ||
			std::is_same_v<T, const glm::ivec2*> ||
			std::is_same_v<T, const glm::uvec2*> ||
			std::is_same_v<T, const glm::bvec2*> ||
			std::is_same_v<T, const glm::dvec2*>>
			Add(const std::string& name, const T& values, size_t size) {
			Add(name, values, size, 8, 8);
		}

		template<class T, size_t N>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec2[N]> ||
			std::is_same_v<T, glm::ivec2[N]> ||
			std::is_same_v<T, glm::uvec2[N]> ||
			std::is_same_v<T, glm::bvec2[N]> ||
			std::is_same_v<T, glm::dvec2[N]> ||
			std::is_same_v<T, const glm::vec2[N]> ||
			std::is_same_v<T, const glm::ivec2[N]> ||
			std::is_same_v<T, const glm::uvec2[N]> ||
			std::is_same_v<T, const glm::bvec2[N]> ||
			std::is_same_v<T, const glm::dvec2[N]>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 8, 8);
		}

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, std::vector<glm::vec2>> ||
			std::is_same_v<T, std::vector<glm::ivec2>> ||
			std::is_same_v<T, std::vector<glm::uvec2>> ||
			std::is_same_v<T, std::vector<glm::bvec2>> ||
			std::is_same_v<T, std::vector<glm::dvec2>>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 8, 8);
		}
#pragma endregion
#pragma endregion

#pragma region ADD_VEC_3
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec3> ||
			std::is_same_v<T, glm::ivec3> ||
			std::is_same_v<T, glm::uvec3> ||
			std::is_same_v<T, glm::bvec3> ||
			std::is_same_v<T, glm::dvec3>>
			Add(const std::string& name, const T& value) {
			Add(name, GetValueData(value), 16, 12);
		}

#pragma region ADD_VEC_3_ARRAYS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec3*> ||
			std::is_same_v<T, glm::ivec3*> ||
			std::is_same_v<T, glm::uvec3*> ||
			std::is_same_v<T, glm::bvec3*> ||
			std::is_same_v<T, glm::dvec3*> ||
			std::is_same_v<T, const glm::vec3*> ||
			std::is_same_v<T, const glm::ivec3*> ||
			std::is_same_v<T, const glm::uvec3*> ||
			std::is_same_v<T, const glm::bvec3*> ||
			std::is_same_v<T, const glm::dvec3*>>
			Add(const std::string& name, const T& values, size_t size) {
			Add(name, values, size, 16, 12);
		}

		template<class T, size_t N>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec3[N]> ||
			std::is_same_v<T, glm::ivec3[N]> ||
			std::is_same_v<T, glm::uvec3[N]> ||
			std::is_same_v<T, glm::bvec3[N]> ||
			std::is_same_v<T, glm::dvec3[N]> ||
			std::is_same_v<T, const glm::vec3[N]> ||
			std::is_same_v<T, const glm::ivec3[N]> ||
			std::is_same_v<T, const glm::uvec3[N]> ||
			std::is_same_v<T, const glm::bvec3[N]> ||
			std::is_same_v<T, const glm::dvec3[N]>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 16, 12);
		}

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, std::vector<glm::vec3>> ||
			std::is_same_v<T, std::vector<glm::ivec3>> ||
			std::is_same_v<T, std::vector<glm::uvec3>> ||
			std::is_same_v<T, std::vector<glm::bvec3>> ||
			std::is_same_v<T, std::vector<glm::dvec3>>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 16, 12);
		}
#pragma endregion
#pragma endregion

#pragma region ADD_VEC_4
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec4> ||
			std::is_same_v<T, glm::ivec4> ||
			std::is_same_v<T, glm::uvec4> ||
			std::is_same_v<T, glm::bvec4> ||
			std::is_same_v<T, glm::dvec4>>
			Add(const std::string& name, const T& value) {
			Add(name, GetValueData(value), 16, 16);
		}

#pragma region ADD_VEC_4_ARRAYS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec4*> ||
			std::is_same_v<T, glm::ivec4*> ||
			std::is_same_v<T, glm::uvec4*> ||
			std::is_same_v<T, glm::bvec4*> ||
			std::is_same_v<T, glm::dvec4*> ||
			std::is_same_v<T, const glm::vec4*> ||
			std::is_same_v<T, const glm::ivec4*> ||
			std::is_same_v<T, const glm::uvec4*> ||
			std::is_same_v<T, const glm::bvec4*> ||
			std::is_same_v<T, const glm::dvec4*>>
			Add(const std::string& name, const T& values, size_t size) {
			Add(name, values, size, 16, 16);
		}

		template<class T, size_t N>
		typename std::enable_if_t<
			std::is_same_v<T, glm::vec4[N]> ||
			std::is_same_v<T, glm::ivec4[N]> ||
			std::is_same_v<T, glm::uvec4[N]> ||
			std::is_same_v<T, glm::bvec4[N]> ||
			std::is_same_v<T, glm::dvec4[N]> ||
			std::is_same_v<T, const glm::vec4[N]> ||
			std::is_same_v<T, const glm::ivec4[N]> ||
			std::is_same_v<T, const glm::uvec4[N]> ||
			std::is_same_v<T, const glm::bvec4[N]> ||
			std::is_same_v<T, const glm::dvec4[N]>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 16, 16);
		}

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, std::vector<glm::vec4>> ||
			std::is_same_v<T, std::vector<glm::ivec4>> ||
			std::is_same_v<T, std::vector<glm::uvec4>> ||
			std::is_same_v<T, std::vector<glm::bvec4>> ||
			std::is_same_v<T, std::vector<glm::dvec4>>>
			Add(const std::string& name, const T& values) {
			Add(name, values, 16, 16);
		}
#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class T, size_t C, size_t R>
		typename std::enable_if_t<
			std::is_same_v<T, glm::mat<C, R, glm::f32>> ||
			std::is_same_v<T, glm::mat<C, R, int>> ||
			std::is_same_v<T, glm::mat<C, R, unsigned int>> ||
			std::is_same_v<T, glm::mat<C, R, glm::f64>>>
			Add(const std::string& name, const T& value) {
			Add(name, GetMatRows(value));
		}

#pragma region ADD_MAT_ARRAYS
		template<class T, size_t C, size_t R>
		typename std::enable_if_t<
			std::is_same_v<T, glm::mat<C, R, glm::f32>*> ||
			std::is_same_v<T, glm::mat<C, R, int>*> ||
			std::is_same_v<T, glm::mat<C, R, unsigned int>*> ||
			std::is_same_v<T, glm::mat<C, R, glm::f64>*> ||
			std::is_same_v<T, const glm::mat<C, R, glm::f32>*> ||
			std::is_same_v<T, const glm::mat<C, R, int>*> ||
			std::is_same_v<T, const glm::mat<C, R, unsigned int>*> ||
			std::is_same_v<T, const glm::mat<C, R, glm::f64>*>>
			Add(const std::string& name, const T& values, size_t size) {
			for (size_t i = 0; i < size; ++i) {
				Add(name + "["s + std::to_string(i) + "]"s, values[i]);
			}
		}

		template<class T, size_t C, size_t R, size_t N>
		typename std::enable_if_t<
			std::is_same_v<T, glm::mat<C, R, glm::f32>[N]> ||
			std::is_same_v<T, glm::mat<C, R, int>[N]> ||
			std::is_same_v<T, glm::mat<C, R, unsigned int>[N]> ||
			std::is_same_v<T, glm::mat<C, R, glm::f64>[N]> ||
			std::is_same_v<T, const glm::mat<C, R, glm::f32>[N]> ||
			std::is_same_v<T, const glm::mat<C, R, int>[N]> ||
			std::is_same_v<T, const glm::mat<C, R, unsigned int>[N]> ||
			std::is_same_v<T, const glm::mat<C, R, glm::f64>[N]>>
			Add(const std::string& name, const T& values) {
			for (size_t i = 0; i < N; ++i) {
				Add(name + "["s + std::to_string(i) + "]"s, values[i]);
			}
		}

		template<class T, size_t C, size_t R>
		typename std::enable_if_t<
			std::is_same_v<T, std::vector<glm::mat<C, R, glm::f32>>> ||
			std::is_same_v<T, std::vector<glm::mat<C, R, int>>> ||
			std::is_same_v<T, std::vector<glm::mat<C, R, unsigned int>>> ||
			std::is_same_v<T, std::vector<glm::mat<C, R, glm::f64>>>>
			Add(const std::string& name, const T& values) {
			for (size_t i = 0; i < values.size(); ++i) {
				Add(name + "["s + std::to_string(i) + "]"s, values[i]);
			}
		}
#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		void Add(const std::string& name, const STD140Struct& value);

#pragma region ADD_STRUCT_ARRAYS
		void Add(const std::string& name, const const STD140Struct*& values, size_t size);
		void Add(const std::string& name, const STD140Struct*& values, size_t size);
		template<size_t N> void Add(const std::string& name, const const STD140Struct(&values)[N]) {
			for (size_t i = 0; i < N; ++i) {
				Add(name + "["s + std::to_string(i) + "]"s, values[i]);
			}
		}
		template<size_t N> void Add(const std::string& name, const STD140Struct(&values)[N]) {
			for (size_t i = 0; i < N; ++i) {
				Add(name + "["s + std::to_string(i) + "]"s, values[i]);
			}
		}
		void Add(const std::string& name, const std::vector<STD140Struct>& values);
#pragma endregion
#pragma endregion


#pragma region SET_SCALARS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, int> ||
			std::is_same_v<T, unsigned int> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, double>>
		Set(const std::string& name, const T& value) {
			Set(name, GetValueData(value));
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, bool>>
		Set(const std::string& name, const T& value) {
			Set(name, (unsigned int)value);
		}
#pragma region SET_SCALARS_ARRAYS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, const int*> ||
			std::is_same_v<T, const unsigned int*> ||
			std::is_same_v<T, const float*> ||
			std::is_same_v<T, const double*> ||
			std::is_same_v<T, int*> ||
			std::is_same_v<T, unsigned int*> ||
			std::is_same_v<T, float*> ||
			std::is_same_v<T, double*>>
		Set(const std::string& name, const T& values, size_t size) {
			SetArray(name, values, size);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, const bool*>>
		Set(const std::string& name, const T& values, size_t size) {
			std::vector<unsigned int> bools;
			for (size_t i = 0; i < size; ++i) {
				bools.push_back((unsigned int)values[i]);
			}
			SetArray(name, bools);
		}

		template<class T, size_t N>
		typename std::enable_if_t<
			std::is_same_v<T, int[N]> ||
			std::is_same_v<T, unsigned int[N]> ||
			std::is_same_v<T, float[N]> ||
			std::is_same_v<T, double[N]> ||
			std::is_same_v<T, const int[N]> ||
			std::is_same_v<T, const unsigned int[N]> ||
			std::is_same_v<T, const float[N]> ||
			std::is_same_v<T, const double[N]>>
		Set(const std::string& name, const T& values) {
			SetArray(name, values);
		}

		template<class T, size_t N>
		typename std::enable_if_t<std::is_same_v<T, const bool[N]>>
		Set(const std::string& name, const T& values) {
			std::vector<unsigned int> bools;
			for (size_t i = 0; i < N; ++i) {
				bools.push_back((unsigned int)values[i]);
			}
			SetArray(name, values);
		}

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, std::vector<int>> ||
			std::is_same_v<T, std::vector<unsigned int>> ||
			std::is_same_v<T, std::vector<float>> ||
			std::is_same_v<T, std::vector<double>>>
		Set(const std::string& name, const T& values) {
			SetArray(name, values);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, std::vector<bool>>>
		Set(const std::string& name, const T& values) {
			std::vector<unsigned int> bools;
			for (size_t i = 0; i < values.size(); ++i) {
				bools.push_back((unsigned int)values[i]);
			}
			SetArray(name, values);
		}
#pragma endregion
#pragma endregion

		std::vector<char> GetData() const;
		size_t GetBaseAligement() const;
		size_t GetSize() const;
	};
}