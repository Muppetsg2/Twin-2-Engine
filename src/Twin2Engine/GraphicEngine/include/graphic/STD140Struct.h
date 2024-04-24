#pragma once

namespace Twin2Engine::GraphicEngine {
	class STD140Struct {
	private:
		size_t _currentOffset = 0;
		size_t _maxAligement = 0;

		std::vector<char> _data;
		std::map<size_t, size_t> _offsets;

		static std::hash<std::string> _hasher;

		template<class T> std::vector<char> GetValueData(const T& value) {
			std::vector<char> valueData;
			valueData.resize(sizeof(T));
			const char* valueDataPtr = reinterpret_cast<const char*>(&value);
			memcpy(valueData.data(), valueDataPtr, sizeof(T));
			return valueData;
		}

		void Add(const std::string& name, const std::vector<char>& value, size_t baseAligement, size_t baseOffset);
		void AddArray(const std::string& name, const std::vector<std::vector<char>>& values, size_t baseAligement, size_t baseOffset);

#pragma region SCALARS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, int> ||
			std::is_same_v<T, unsigned int> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, double>> 
		Add(const std::string& name, const T& value) {
			Add(name, GetValueData(value), 4, 4);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, bool>>
		Add(const std::string& name, const T& value) {
			Add(name, (unsigned int)value);
		}

#pragma region SCALARS_ARRAYS
		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, const int*> ||
			std::is_same_v<T, const unsigned int*> ||
			std::is_same_v<T, const float*> ||
			std::is_same_v<T, const double*>>
		Add(const std::string& name, const T& values, size_t size) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < size; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			AddArray(name, valuesData, 4, 4);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, const bool*>>
		Add(const std::string& name, const T& values, size_t size) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < size; ++i) {
				valuesData.push_back(GetValueData((unsigned int)values[i]));
			}
			AddArray(name, valuesData, 4, 4);
		}

		template<class T, size_t N>
		typename std::enable_if_t<
			std::is_same_v<T, const int[N]> ||
			std::is_same_v<T, const unsigned int[N]> ||
			std::is_same_v<T, const float[N]> ||
			std::is_same_v<T, const double[N]>>
		Add(const std::string& name, const T& values) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < N; ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			AddArray(name, valuesData, 4, 4);
		}

		template<class T, size_t N>
		typename std::enable_if_t<std::is_same_v<T, const bool[N]>>
		Add(const std::string& name, const T& values) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < N; ++i) {
				valuesData.push_back(GetValueData((unsigned int)values[i]));
			}
			AddArray(name, valuesData, 4, 4);
		}

		template<class T>
		typename std::enable_if_t<
			std::is_same_v<T, std::vector<int>> ||
			std::is_same_v<T, std::vector<unsigned int>> ||
			std::is_same_v<T, std::vector<float>> ||
			std::is_same_v<T, std::vector<double>>>
		Add(const std::string& name, const T& values) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < values.size(); ++i) {
				valuesData.push_back(GetValueData(values[i]));
			}
			AddArray(name, valuesData, 4, 4);
		}

		template<class T>
		typename std::enable_if_t<std::is_same_v<T, std::vector<bool>>>
		Add(const std::string& name, const T& values) {
			std::vector<std::vector<char>> valuesData;
			for (size_t i = 0; i < values.size(); ++i) {
				valuesData.push_back(GetValueData((unsigned int)values[i]));
			}
			AddArray(name, valuesData, 4, 4);
		}
#pragma endregion
#pragma endregion

#pragma region VEC_2

#pragma endregion

#pragma region VEC_3

#pragma endregion

#pragma region VEC_4

#pragma endregion

#pragma region MAT

#pragma endregion

#pragma region STRUCT

#pragma endregion

	public:
		STD140Struct() = default;
		virtual ~STD140Struct() = default;
	};
}