#pragma once

#include <tools/templates.h>

namespace Twin2Engine::Tools {
	class STD140Offsets {
	private:
#pragma region CHECKS
		template<class T> static constexpr bool scalar_check_v = is_type_in_v<T, bool, int, unsigned int, float, double>;
		template<class T, size_t L> static constexpr bool vec_check_v = scalar_check_v<T> && is_num_in_range_v<L, 1, 4>;
		template<class T, size_t C, size_t R> static constexpr bool mat_check_v = vec_check_v<T, C> && is_num_in_range_v<R, 1, 4>;

		template<class T, class Ret = void> using scalar_enable_if_t = std::enable_if_t<scalar_check_v<T>, Ret>;
		template<class T, size_t L, class Ret = void> using vec_enable_if_t = std::enable_if_t<vec_check_v<T, L>, Ret>;
		template<class T, size_t C, size_t R, class Ret = void> using mat_enable_if_t = std::enable_if_t<mat_check_v<T, C, R>, Ret>;

#pragma endregion

		size_t _currentOffset = 0;
		size_t _maxAligement = 0;

		std::map<size_t, size_t> _offsets;
		std::map<size_t, std::string> _names;

		static std::hash<std::string> _hasher;

		bool CheckVariable(const std::string& name) const;

		size_t Add(const std::string& name, size_t baseAligement, size_t baseOffset);
		std::vector<size_t> AddArray(const std::string& name, size_t arraySize, size_t baseAligement, size_t baseOffset);

	public:
		STD140Offsets() = default;
		STD140Offsets(STD140Offsets& std140off) = default;
		STD140Offsets(const STD140Offsets& std140off) = default;
		STD140Offsets(STD140Offsets&& std140off) = default;
		virtual ~STD140Offsets() = default;

		STD140Offsets& operator=(STD140Offsets& std140off) = default;
		STD140Offsets& operator=(const STD140Offsets& std140off) = default;
		STD140Offsets& operator=(STD140Offsets&& std140off) = default;

		bool Contains(const std::string& name) const;

#pragma region ADD_SCALAR
		template<class T>
		typename scalar_enable_if_t<T, size_t>
		Add(const std::string& name) {
			if (CheckVariable(name)) return 0;

			size_t tSize;

			if constexpr (std::is_same_v<T, bool>) tSize = 4; // sizeof(unsigned int)
			else tSize = sizeof(T);

			return Add(name, tSize, tSize);
		}

#pragma region ADD_SCALAR_ARRAY
		template<class T>
		typename scalar_enable_if_t<T, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
			if (CheckVariable(name) || size == 0) return std::vector<size_t>();

			size_t tSize;

			if constexpr (std::is_same_v<T, bool>) tSize = 4; // sizeof(unsigned int)
			else tSize = sizeof(T);

			return AddArray(name, size, tSize, tSize);
		}
#pragma endregion
#pragma endregion

#pragma region ADD_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<T, L, size_t>
		Add(const std::string& name) {
			if (CheckVariable(name)) return 0;

			size_t tSize;

			if constexpr (std::is_same_v<T, bool>) tSize = 4; // sizeof(unsigned int)
			else tSize = sizeof(T);

			if constexpr (is_num_in_v<L, 1, 2, 4>)
				return Add(name, tSize * L, tSize * L);
			else if constexpr (is_num_in_v<L, 3>)
				return Add(name, tSize * (L + 1), tSize * L);
		}

#pragma region ADD_VEC_ARRAY
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<T, L, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
			if (CheckVariable(name) || size == 0) return std::vector<size_t>();

			size_t tSize;

			if constexpr (std::is_same_v<T, bool>) tSize = 4; // sizeof(unsigned int)
			else tSize = sizeof(T);

			if constexpr (is_num_in_v<L, 1, 2, 4>)
				return AddArray(name, size, tSize * L, tSize * L);
			else if constexpr (is_num_in_v<L, 3>)
				return AddArray(name, size, tSize * (L + 1), tSize * L);
		}
#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<T, C, R, size_t>
		Add(const std::string& name) {
			if (CheckVariable(name)) return 0;

			return Add<glm::vec<R, T>>(name, C)[0];
		}

#pragma region ADD_MAT_ARRAY
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::column_type::length()>
		typename mat_enable_if_t<T, C, R, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
			if (CheckVariable(name) || size == 0) return std::vector<size_t>();

			std::vector<size_t> values;
			for (size_t i = 0; i < size; ++i) {
				values.push_back(Add<M>(name + "[" + std::to_string(i) + "]"));
			}
			// SET ARRAY BEGIN POINTER
			size_t nameHash = _hasher(name);
			_offsets[nameHash] = values[0];

			return values;
		}
#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		size_t Add(const std::string& name, const STD140Offsets& structTemplate) {
			if (CheckVariable(name)) return 0;

			size_t aligementOffset = Add(name, structTemplate.GetBaseAligement(), structTemplate._currentOffset);
			for (const auto& off : structTemplate._offsets) {
				size_t nameHash = _hasher(name + "." + (*structTemplate._names.find(off.first)).second);
				_offsets[nameHash] = aligementOffset + off.second;
			}
			if (_currentOffset % 16 != 0) {
				_currentOffset += 16 - (_currentOffset % 16);
			}

			return aligementOffset;
		}

#pragma region ADD_STRUCT_ARRAY
		std::vector<size_t> Add(const std::string& name, const STD140Offsets& structTemplate, size_t size) {
			if (CheckVariable(name) || size == 0) return std::vector<size_t>();

			std::vector<size_t> values;
			for (size_t i = 0; i < size; ++i) {
				values.push_back(Add(name + "[" + std::to_string(i) + "]", structTemplate));
			}
			// SET ARRAY BEGIN POINTER
			size_t nameHash = _hasher(name);
			_offsets[nameHash] = values[0];

			return values;
		}
#pragma endregion
#pragma endregion

		size_t Get(const std::string& name) const;
		std::vector<size_t> GetArray(const std::string& name) const;

		size_t GetBaseAligement() const;
		size_t GetSize() const;

		void Clear();
	};
}