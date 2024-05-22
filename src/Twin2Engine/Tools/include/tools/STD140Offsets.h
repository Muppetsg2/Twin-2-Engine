#pragma once

#include <tools/templates.h>
#include <tools/stringExtension.h>

namespace Twin2Engine::Tools {
	class STD140Offsets;

	template<class T>
	struct STD140Variable {
	public:
		using var_type = T;
		
		const std::string var_name;
		const size_t array_size;
		typename type_test_t<std::is_same_v<T, STD140Offsets>, const STD140Offsets, void(*)> struct_offsets;

		template<typename = std::enable_if_t<!std::is_same_v<T, STD140Offsets>>>
		STD140Variable(const std::string& name) : var_name(name), array_size(0) {}

		template<typename = std::enable_if_t<!std::is_same_v<T, STD140Offsets>>>
		STD140Variable(const std::string& name, const size_t& size) : var_name(name), array_size(size) {}

		template<typename = std::enable_if_t<std::is_same_v<T, STD140Offsets>>>
		STD140Variable(const std::string& name, const STD140Offsets& offsets) : var_name(name), struct_offsets(offsets), array_size(0) {}

		template<typename = std::enable_if_t<std::is_same_v<T, STD140Offsets>>>
		STD140Variable(const std::string& name, const STD140Offsets& offsets, const size_t& size) : var_name(name), struct_offsets(offsets), array_size(size) {}
	};

	class STD140Offsets {
	private:
#pragma region CHECKS
		template<class T> static constexpr bool scalar_check_v = is_type_in_v<T, bool, int, unsigned int, float, double>;
		template<class T, size_t L> static constexpr bool vec_check_v = scalar_check_v<T> && is_num_in_range_v<L, 1, 4>;
		template<class T, size_t C, size_t R> static constexpr bool mat_check_v = vec_check_v<T, C> && is_num_in_range_v<R, 1, 4>;

		template<class T, class Ret = void> using scalar_enable_if_t = std::enable_if_t<scalar_check_v<T>, Ret>;
		template<class V, class T, size_t L, class Ret = void> using vec_enable_if_t = std::enable_if_t<std::is_same_v<V, glm::vec<L, T>> && vec_check_v<T, L>, Ret>;
		template<class M, class T, size_t C, size_t R, class Ret = void> using mat_enable_if_t = std::enable_if_t<std::is_same_v<M, glm::mat<C, R, T>> && mat_check_v<T, C, R>, Ret>;

#pragma endregion

		size_t _currentOffset = 0;
		size_t _maxAligement = 0;

		std::map<size_t, size_t> _offsets;
		std::map<size_t, std::string> _names;

		static std::hash<std::string> _hasher;

		static const char* const _arrayElemFormat;
		static const char* const _subElemFormat;

		static const char* const tracy_AddScalar;
		static const char* const tracy_AddScalarArray;
		static const char* const tracy_AddVec;
		static const char* const tracy_AddVecArray;
		static const char* const tracy_AddMat;
		static const char* const tracy_AddMatArray;
		static const char* const tracy_AddMatArrayElem;
		static const char* const tracy_AddMatArraySetBeginPointer;
		static const char* const tracy_AddStruct;
		static const char* const tracy_AddStructSetElem;
		static const char* const tracy_AddStructAddPadding;
		static const char* const tracy_AddStructArray;
		static const char* const tracy_AddStructArrayAddElems;
		static const char* const tracy_AddStructArrayAddElem;
		static const char* const tracy_AddStructArrayAddElemSubValues;
		static const char* const tracy_AddStructArrayAddElemSubValue;
		static const char* const tracy_AddStructArrayAddElemPadding;
		static const char* const tracy_AddStructArraySetBeginPointer;

		bool _CheckVariable(const std::string& name) const;

		template<class T, class... Ts>
		void _AddMultiple(const STD140Variable<T>& var, const STD140Variable<Ts>&... vars) {
			if constexpr (std::is_same_v<T, STD140Offsets>) {
				if (var.array_size == 0) {
					Add(var.var_name, var.struct_offsets);
				}
				else {
					Add(var.var_name, var.struct_offsets, var.array_size);
				}
			}
			else {
				if (var.array_size == 0) {
					Add<T>(var.var_name);
				}
				else {
					Add<T>(var.var_name, var.array_size);
				}
			}
			if constexpr (sizeof...(Ts) > 0) {
				_AddMultiple(vars...);
			}
		}

		size_t _Add(const std::string& name, size_t baseAligement, size_t baseOffset);
		std::vector<size_t> _AddArray(const std::string& name, size_t arraySize, size_t baseAligement, size_t baseOffset);

		//static const std::string& _GetArrayElemFormat();
		//static const std::string& _GetSubElemFormat();

	public:
		STD140Offsets() = default;
		STD140Offsets(STD140Offsets& std140off) = default;
		STD140Offsets(const STD140Offsets& std140off) = default;
		STD140Offsets(STD140Offsets&& std140off) = default;
		template<class... Args>
		STD140Offsets(const STD140Variable<Args>&... vars) {
			_AddMultiple(vars...);
		}
		virtual ~STD140Offsets() = default;

		STD140Offsets& operator=(STD140Offsets& std140off) = default;
		STD140Offsets& operator=(const STD140Offsets& std140off) = default;
		STD140Offsets& operator=(STD140Offsets&& std140off) = default;

		bool Contains(const std::string& name) const;

#pragma region ADD_SCALAR
		template<class T>
		typename scalar_enable_if_t<T, size_t>
		Add(const std::string& name) {
			ZoneScoped;

			FrameMarkStart(tracy_AddScalar);
			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddScalar);
				return 0;
			}

			FrameMarkEnd(tracy_AddScalar);
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				return _Add(name, 4, 4);
			}
			else {
				return _Add(name, sizeof(T), sizeof(T));
			}
		}

#pragma region ADD_SCALAR_ARRAY
		template<class T>
		typename scalar_enable_if_t<T, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
			ZoneScoped;

			FrameMarkStart(tracy_AddScalarArray);
			if (size == 0) {
				FrameMarkEnd(tracy_AddScalarArray);
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddScalarArray);
				return std::vector<size_t>();
			}

			FrameMarkEnd(tracy_AddScalarArray);
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				return _AddArray(name, size, 4, 4);
			}
			else {
				return _AddArray(name, size, sizeof(T), sizeof(T));
			}
		}

#pragma endregion
#pragma endregion

#pragma region ADD_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, size_t>
		Add(const std::string& name) {
			ZoneScoped;

			FrameMarkStart(tracy_AddVec);
			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddVec);
				return 0;
			}

			FrameMarkEnd(tracy_AddVec);
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _Add(name, 4 * L, 4 * L);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _Add(name, 4 * (L + 1), 4 * L);
				}
			}
			else {
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _Add(name, sizeof(T) * L, sizeof(T) * L);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _Add(name, sizeof(T) * (L + 1), sizeof(T) * L);
				}
			}
		}

#pragma region ADD_VEC_ARRAY
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
			ZoneScoped;

			FrameMarkStart(tracy_AddVecArray);
			if (size == 0) {
				FrameMarkEnd(tracy_AddVecArray);
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddVecArray);
				return std::vector<size_t>();
			}

			FrameMarkEnd(tracy_AddVecArray);
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _AddArray(name, size, 4 * L, 4 * L);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _AddArray(name, size, 4 * (L + 1), 4 * L);
				}
			}
			else {
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _AddArray(name, size, sizeof(T) * L, sizeof(T) * L);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _AddArray(name, size, sizeof(T) * (L + 1), sizeof(T) * L);
				}
			}
		}

#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, size_t>
		Add(const std::string& name) {
			ZoneScoped;
			FrameMarkStart(tracy_AddMat);

			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddMat);
				return 0;
			}

			FrameMarkEnd(tracy_AddMat);
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				if constexpr (is_num_in_v<R, 1, 2, 4>) {
					return _AddArray(name, C, 4 * R, 4 * R)[0];
				}
				else if constexpr (is_num_in_v<R, 3>) {
					return _AddArray(name, C, 4 * (R + 1), 4 * R)[0];
				}
			}
			else {
				if constexpr (is_num_in_v<R, 1, 2, 4>) {
					return _AddArray(name, C, sizeof(T) * R, sizeof(T) * R)[0];
				}
				else if constexpr (is_num_in_v<R, 3>) {
					return _AddArray(name, C, sizeof(T) * (R + 1), sizeof(T) * R)[0];
				}
			}
		}

#pragma region ADD_MAT_ARRAY
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
			ZoneScoped;
			FrameMarkStart(tracy_AddMatArray);

			if (size == 0) {
				FrameMarkEnd(tracy_AddMatArray);
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddMatArray);
				return std::vector<size_t>();
			}

			std::vector<size_t> values;
			for (size_t i = 0; i < size; ++i) {
				FrameMarkStart(tracy_AddMatArrayElem);
				if constexpr (std::is_same_v<T, bool>) {
					// sizeof(unsigned int) = 4
					if constexpr (is_num_in_v<R, 1, 2, 4>) {
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, 4 * R, 4 * R)[0]));
					}
					else if constexpr (is_num_in_v<R, 3>) {
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, 4 * (R + 1), 4 * R)[0]));
					}
				}
				else {
					if constexpr (is_num_in_v<R, 1, 2, 4>) {
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, sizeof(T) * R, sizeof(T) * R)[0]));
					}
					else if constexpr (is_num_in_v<R, 3>) {
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, sizeof(T) * (R + 1), sizeof(T) * R)[0]));
					}
				}
				FrameMarkEnd(tracy_AddMatArrayElem);
			}

			// SET ARRAY BEGIN POINTER
			FrameMarkStart(tracy_AddMatArraySetBeginPointer);
			size_t nameHash = std::move(_hasher(name));
			_offsets[nameHash] = values[0];
			_names[nameHash] = name;
			FrameMarkEnd(tracy_AddMatArrayElem);

			FrameMarkEnd(tracy_AddMatArray);
			return values;
		}

#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		size_t Add(const std::string& name, const STD140Offsets& structTemplate) {
			ZoneScoped;
			FrameMarkStart(tracy_AddStruct);
			
			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddStruct);
				return 0;
			}

			size_t aligementOffset = std::move(_Add(name, structTemplate.GetBaseAligement(), structTemplate._currentOffset));
			std::string valueName;
			size_t nameHash;
			for (const auto& off : structTemplate._offsets) {
				FrameMarkStart(tracy_AddStructSetElem);
				valueName = std::move(std::vformat(_subElemFormat, std::make_format_args(name, (*structTemplate._names.find(off.first)).second)));
				
				nameHash = std::move(_hasher(valueName));
				_offsets[nameHash] = aligementOffset + off.second;
				_names[nameHash] = valueName;
				FrameMarkEnd(tracy_AddStructSetElem);
			}

			// ADD PADDING
			FrameMarkStart(tracy_AddStructAddPadding);
			if (_currentOffset % 16 != 0) {
				_currentOffset += 16 - (_currentOffset % 16);
			}
			FrameMarkEnd(tracy_AddStructAddPadding);

			FrameMarkEnd(tracy_AddStruct);
			return aligementOffset;
		}

#pragma region ADD_STRUCT_ARRAY
		std::vector<size_t> Add(const std::string& name, const STD140Offsets& structTemplate, size_t size) {
			ZoneScoped;
			FrameMarkStart(tracy_AddStructArray);

			if (size == 0) {
				FrameMarkEnd(tracy_AddStructArray);
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
				FrameMarkEnd(tracy_AddStructArray);
				return std::vector<size_t>();
			}

			std::vector<size_t> values;
			std::string arrayElemName;
			size_t aligementOffset;
			std::string valueName;
			size_t nameHash;

			FrameMarkStart(tracy_AddStructArrayAddElems);
			for (size_t i = 0; i < size; ++i) {
				FrameMarkStart(tracy_AddStructArrayAddElem);

				arrayElemName = std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i)));
				values.push_back((aligementOffset = std::move(_Add(arrayElemName, structTemplate.GetBaseAligement(), structTemplate._currentOffset))));

				FrameMarkStart(tracy_AddStructArrayAddElemSubValues);
				for (const auto& off : structTemplate._offsets) {
					FrameMarkStart(tracy_AddStructArrayAddElemSubValue);
					valueName = std::move(std::vformat(_subElemFormat, std::make_format_args(arrayElemName, (*structTemplate._names.find(off.first)).second)));

					nameHash = std::move(_hasher(valueName));
					_offsets[nameHash] = aligementOffset + off.second;
					_names[nameHash] = valueName;
					FrameMarkEnd(tracy_AddStructArrayAddElemSubValue);
				}
				FrameMarkEnd(tracy_AddStructArrayAddElemSubValues);

				// ADD PADDING
				FrameMarkStart(tracy_AddStructArrayAddElemPadding);
				if (_currentOffset % 16 != 0) {
					_currentOffset += 16 - (_currentOffset % 16);
				}
				FrameMarkEnd(tracy_AddStructArrayAddElemPadding);

				FrameMarkEnd(tracy_AddStructArrayAddElem);
			}
			FrameMarkEnd(tracy_AddStructArrayAddElems);

			// SET ARRAY BEGIN POINTER
			FrameMarkStart(tracy_AddStructArraySetBeginPointer);
			nameHash = std::move(_hasher(name));
			_offsets[nameHash] = values[0];
			_names[nameHash] = name;
			FrameMarkEnd(tracy_AddStructArraySetBeginPointer);

			FrameMarkEnd(tracy_AddStructArray);
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