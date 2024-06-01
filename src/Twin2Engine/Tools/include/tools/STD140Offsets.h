#pragma once

#include <tools/templates.h>
#include <tools/stringExtension.h>
#include <tools/macros.h>
#if _DEBUG
#include <tools/ValueTypes.h>
#endif

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
		STD140Variable(const std::string& name, const STD140Offsets offsets) : var_name(name), struct_offsets(offsets), array_size(0) {}

		template<typename = std::enable_if_t<std::is_same_v<T, STD140Offsets>>>
		STD140Variable(const std::string& name, const STD140Offsets offsets, const size_t& size) : var_name(name), struct_offsets(offsets), array_size(size) {}
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

		std::unordered_map<size_t, size_t> _offsets;
		std::unordered_map<size_t, std::string> _names;
#if _DEBUG
		std::unordered_map<size_t, const ValueType*> _types;
#endif

		static std::hash<std::string> _hasher;

		static const char* const _arrayElemFormat;
		static const char* const _subElemFormat;

#if TRACY_PROFILER
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
#endif

		bool _CheckVariable(const std::string& name) const;

		template<class T, class... Ts>
		void _AddMultiple(const STD140Variable<T>& var, const STD140Variable<Ts>&... vars) {
#if TRACY_PROFILER
			ZoneScoped;
#endif

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

		size_t _Add(const std::string& name, size_t baseAligement, size_t baseOffset 
#if _DEBUG 
			, const ValueType* type 
#endif
		);
		std::vector<size_t> _AddArray(const std::string& name, size_t arraySize, size_t baseAligement, size_t baseOffset
#if _DEBUG
			, const ValueType* typeTemplate
#endif
		);

	public:
		STD140Offsets() = default;
		STD140Offsets(STD140Offsets& std140off);
		STD140Offsets(const STD140Offsets& std140off);
		STD140Offsets(STD140Offsets&& std140off);
		template<class... Args>
		STD140Offsets(const STD140Variable<Args>&... vars) {
			_AddMultiple(vars...);
		}
		virtual ~STD140Offsets();

		STD140Offsets& operator=(STD140Offsets& std140off);
		STD140Offsets& operator=(const STD140Offsets& std140off);
		STD140Offsets& operator=(STD140Offsets&& std140off);

		DeclareCloneFunc(STD140Offsets)

		bool Contains(const std::string& name) const;

#pragma region ADD_SCALAR
		template<class T>
		typename scalar_enable_if_t<T, size_t>
		Add(const std::string& name) {
#if TRACY_PROFILER
			ZoneScoped;

			FrameMarkStart(tracy_AddScalar);
#endif
			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddScalar);
#endif
				return 0;
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddScalar);
#endif
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				return _Add(name, 4, 4
#if _DEBUG
					, new ScalarType(GetValueType<T>())
#endif
				);
			}
			else {
				return _Add(name, sizeof(T), sizeof(T)
#if _DEBUG
					, new ScalarType(GetValueType<T>())
#endif
				);
			}
		}

#pragma region ADD_SCALAR_ARRAY
		template<class T>
		typename scalar_enable_if_t<T, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
#if TRACY_PROFILER
			ZoneScoped;

			FrameMarkStart(tracy_AddScalarArray);
#endif
			if (size == 0) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddScalarArray);
#endif
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddScalarArray);
#endif
				return std::vector<size_t>();
			}

#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddScalarArray);
#endif
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				return _AddArray(name, size, 4, 4
#if _DEBUG
					, new ScalarType(GetValueType<T>())
#endif
				);
			}
			else {
				return _AddArray(name, size, sizeof(T), sizeof(T)
#if _DEBUG
					, new ScalarType(GetValueType<T>())
#endif
				);
			}
		}

#pragma endregion
#pragma endregion

#pragma region ADD_VEC
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, size_t>
		Add(const std::string& name) {
#if TRACY_PROFILER
			ZoneScoped;

			FrameMarkStart(tracy_AddVec);
#endif
			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddVec);
#endif
				return 0;
			}

#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddVec);
#endif
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _Add(name, 4 * L, 4 * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _Add(name, 4 * (L + 1), 4 * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
			}
			else {
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _Add(name, sizeof(T) * L, sizeof(T) * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _Add(name, sizeof(T) * (L + 1), sizeof(T) * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
			}
		}

#pragma region ADD_VEC_ARRAY
		template<class V, class T = V::value_type, size_t L = V::length()>
		typename vec_enable_if_t<V, T, L, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
#if TRACY_PROFILER
			ZoneScoped;

			FrameMarkStart(tracy_AddVecArray);
#endif
			if (size == 0) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddVecArray);
#endif
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddVecArray);
#endif
				return std::vector<size_t>();
			}

#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddVecArray);
#endif
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _AddArray(name, size, 4 * L, 4 * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _AddArray(name, size, 4 * (L + 1), 4 * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
			}
			else {
				if constexpr (is_num_in_v<L, 1, 2, 4>) {
					return _AddArray(name, size, sizeof(T) * L, sizeof(T) * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
				else if constexpr (is_num_in_v<L, 3>) {
					return _AddArray(name, size, sizeof(T) * (L + 1), sizeof(T) * L
#if _DEBUG
						, new VecType(GetValueType<T>(), L)
#endif
					);
				}
			}
		}

#pragma endregion
#pragma endregion

#pragma region ADD_MAT
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, size_t>
		Add(const std::string& name) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy_AddMat);
#endif

			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddMat);
#endif
				return 0;
			}

#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddMat);
#endif
			if constexpr (std::is_same_v<T, bool>) {
				// sizeof(unsigned int) = 4
				if constexpr (is_num_in_v<R, 1, 2, 4>) {
#if _DEBUG
					size_t offset = _AddArray(name, C, 4 * R, 4 * R, new VecType(GetValueType<T>(), R))[0];
					size_t nameHash = _hasher(name);
					delete _types[nameHash];
					_types[nameHash] = new MatType(GetValueType<T>(), C, R);
					return offset;
#else
					return _AddArray(name, C, 4 * R, 4 * R)[0];
#endif
				}
				else if constexpr (is_num_in_v<R, 3>) {
#if _DEBUG
					size_t offset = _AddArray(name, C, 4 * (R + 1), 4 * R, new VecType(GetValueType<T>(), R))[0];
					size_t nameHash = _hasher(name);
					delete _types[nameHash];
					_types[nameHash] = new MatType(GetValueType<T>(), C, R);
					return offset;
#else
					return _AddArray(name, C, 4 * (R + 1), 4 * R)[0];
#endif
				}
			}
			else {
				if constexpr (is_num_in_v<R, 1, 2, 4>) {
#if _DEBUG
					size_t offset = _AddArray(name, C, sizeof(T) * R, sizeof(T) * R, new VecType(GetValueType<T>(), R))[0];
					size_t nameHash = _hasher(name);
					delete _types[nameHash];
					_types[nameHash] = new MatType(GetValueType<T>(), C, R);
					return offset;
#else
					return _AddArray(name, C, sizeof(T) * R, sizeof(T) * R)[0];
#endif
				}
				else if constexpr (is_num_in_v<R, 3>) {
#if _DEBUG
					size_t offset = _AddArray(name, C, sizeof(T) * (R + 1), sizeof(T) * R, new VecType(GetValueType<T>(), R))[0];
					size_t nameHash = _hasher(name);
					delete _types[nameHash];
					_types[nameHash] = new MatType(GetValueType<T>(), C, R);
					return offset;
#else
					return _AddArray(name, C, sizeof(T) * (R + 1), sizeof(T) * R)[0];
#endif
				}
			}
		}

#pragma region ADD_MAT_ARRAY
		template<class M, class T = M::value_type, size_t C = M::row_type::length(), size_t R = M::col_type::length()>
		typename mat_enable_if_t<M, T, C, R, std::vector<size_t>>
		Add(const std::string& name, size_t size) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy_AddMatArray);
#endif

			if (size == 0) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddMatArray);
#endif
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddMatArray);
#endif
				return std::vector<size_t>();
			}

			std::vector<size_t> values;
#if _DEBUG
			const ValueType* matType = new MatType(GetValueType<T>(), C, R);
			const ValueType* rowType = new VecType(GetValueType<T>(), R);
#endif
			for (size_t i = 0; i < size; ++i) {
#if TRACY_PROFILER
				FrameMarkStart(tracy_AddMatArrayElem);
#endif
				if constexpr (std::is_same_v<T, bool>) {
					// sizeof(unsigned int) = 4
					if constexpr (is_num_in_v<R, 1, 2, 4>) {
#if _DEBUG
						std::string valueName = std::vformat(_arrayElemFormat, std::make_format_args(name, i));
						values.push_back(std::move(_AddArray(valueName, C, 4 * R, 4 * R, rowType)[0]));
						size_t nameHash = _hasher(valueName);
						delete _types[nameHash];
						_types[nameHash] = matType->Clone();
#else
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, 4 * R, 4 * R)[0]));
#endif
					}
					else if constexpr (is_num_in_v<R, 3>) {
#if _DEBUG
						std::string valueName = std::vformat(_arrayElemFormat, std::make_format_args(name, i));
						values.push_back(std::move(_AddArray(valueName, C, 4 * (R + 1), 4 * R, rowType)[0]));
						size_t nameHash = _hasher(valueName);
						delete _types[nameHash];
						_types[nameHash] = matType->Clone();
#else
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, 4 * (R + 1), 4 * R)[0]));
#endif
					}
				}
				else {
					if constexpr (is_num_in_v<R, 1, 2, 4>) {
#if _DEBUG
						std::string valueName = std::vformat(_arrayElemFormat, std::make_format_args(name, i));
						values.push_back(std::move(_AddArray(valueName, C, sizeof(T) * R, sizeof(T) * R, rowType)[0]));
						size_t nameHash = _hasher(valueName);
						delete _types[nameHash];
						_types[nameHash] = matType->Clone();
#else
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, sizeof(T) * R, sizeof(T) * R)[0]));
#endif
					}
					else if constexpr (is_num_in_v<R, 3>) {
#if _DEBUG
						std::string valueName = std::vformat(_arrayElemFormat, std::make_format_args(name, i));
						values.push_back(std::move(_AddArray(valueName, C, sizeof(T) * (R + 1), sizeof(T) * R, rowType)[0]));
						size_t nameHash = _hasher(valueName);
						delete _types[nameHash];
						_types[nameHash] = matType->Clone();
#else
						values.push_back(std::move(_AddArray(std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i))), C, sizeof(T) * (R + 1), sizeof(T) * R)[0]));
#endif
					}
				}
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddMatArrayElem);
#endif
			}

			// SET ARRAY BEGIN POINTER
#if TRACY_PROFILER
			FrameMarkStart(tracy_AddMatArraySetBeginPointer);
#endif
			size_t nameHash = std::move(_hasher(name));
			_offsets[nameHash] = values[0];
			_names[nameHash] = name;
#if _DEBUG
			_types[nameHash] = new ArrayType(matType, size);
#endif
#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddMatArrayElem);

			FrameMarkEnd(tracy_AddMatArray);
#endif
			return values;
		}

#pragma endregion
#pragma endregion

#pragma region ADD_STRUCT
		size_t Add(const std::string& name, const STD140Offsets& structTemplate) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy_AddStruct);
#endif
			
			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddStruct);
#endif
				return 0;
			}

			size_t aligementOffset = std::move(_Add(name, structTemplate.GetBaseAligement(), structTemplate._currentOffset
#if _DEBUG
				, new StructType(structTemplate)
#endif
			));
			std::string valueName;
			size_t nameHash;
			for (const auto& off : structTemplate._offsets) {
#if TRACY_PROFILER
				FrameMarkStart(tracy_AddStructSetElem);
#endif
				valueName = std::move(std::vformat(_subElemFormat, std::make_format_args(name, (*structTemplate._names.find(off.first)).second)));
				
				nameHash = std::move(_hasher(valueName));
				_offsets[nameHash] = aligementOffset + off.second;
				_names[nameHash] = valueName;
#if _DEBUG
				_types[nameHash] = (*structTemplate._types.find(off.first)).second->Clone();
#endif
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddStructSetElem);
#endif
			}

			// ADD PADDING
#if TRACY_PROFILER
			FrameMarkStart(tracy_AddStructAddPadding);
#endif
			if (_currentOffset % 16 != 0) {
				_currentOffset += 16 - (_currentOffset % 16);
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddStructAddPadding);

			FrameMarkEnd(tracy_AddStruct);
#endif
			return aligementOffset;
		}

#pragma region ADD_STRUCT_ARRAY
		std::vector<size_t> Add(const std::string& name, const STD140Offsets& structTemplate, size_t size) {
#if TRACY_PROFILER
			ZoneScoped;
			FrameMarkStart(tracy_AddStructArray);
#endif

			if (size == 0) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddStructArray);
#endif
				return std::vector<size_t>();
			}

			if (_CheckVariable(name)) {
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddStructArray);
#endif
				return std::vector<size_t>();
			}

			std::vector<size_t> values;
			std::string arrayElemName;
			size_t aligementOffset;
			std::string valueName;
			size_t nameHash;

#if TRACY_PROFILER
			FrameMarkStart(tracy_AddStructArrayAddElems);
#endif
#if _DEBUG
			const ValueType* structType = new StructType(structTemplate);
#endif
			for (size_t i = 0; i < size; ++i) {
#if TRACY_PROFILER
				FrameMarkStart(tracy_AddStructArrayAddElem);
#endif

				arrayElemName = std::move(std::vformat(_arrayElemFormat, std::make_format_args(name, i)));
				values.push_back((aligementOffset = std::move(_Add(arrayElemName, structTemplate.GetBaseAligement(), structTemplate._currentOffset
#if _DEBUG
					, structType->Clone()
#endif
				))));

#if TRACY_PROFILER
				FrameMarkStart(tracy_AddStructArrayAddElemSubValues);
#endif
				for (const auto& off : structTemplate._offsets) {
#if TRACY_PROFILER
					FrameMarkStart(tracy_AddStructArrayAddElemSubValue);
#endif
					valueName = std::move(std::vformat(_subElemFormat, std::make_format_args(arrayElemName, (*structTemplate._names.find(off.first)).second)));

					nameHash = std::move(_hasher(valueName));
					_offsets[nameHash] = aligementOffset + off.second;
					_names[nameHash] = valueName;
#if _DEBUG
					_types[nameHash] = (*structTemplate._types.find(off.first)).second->Clone();
#endif
#if TRACY_PROFILER
					FrameMarkEnd(tracy_AddStructArrayAddElemSubValue);
#endif
				}
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddStructArrayAddElemSubValues);
#endif

				// ADD PADDING
#if TRACY_PROFILER
				FrameMarkStart(tracy_AddStructArrayAddElemPadding);
#endif
				if (_currentOffset % 16 != 0) {
					_currentOffset += 16 - (_currentOffset % 16);
				}
#if TRACY_PROFILER
				FrameMarkEnd(tracy_AddStructArrayAddElemPadding);

				FrameMarkEnd(tracy_AddStructArrayAddElem);
#endif
			}
#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddStructArrayAddElems);
#endif

			// SET ARRAY BEGIN POINTER
#if TRACY_PROFILER
			FrameMarkStart(tracy_AddStructArraySetBeginPointer);
#endif
			nameHash = std::move(_hasher(name));
			_offsets[nameHash] = values[0];
			_names[nameHash] = name;
#if _DEBUG
			_types[nameHash] = new ArrayType(structType, size);
#endif
#if TRACY_PROFILER
			FrameMarkEnd(tracy_AddStructArraySetBeginPointer);

			FrameMarkEnd(tracy_AddStructArray);
#endif
			return values;
		}

#pragma endregion
#pragma endregion

		size_t Get(const std::string& name) const;
		std::vector<size_t> GetArray(const std::string& name) const;

#if _DEBUG
		const ValueType* GetType(const std::string& name) const;
#endif

		size_t GetBaseAligement() const;
		size_t GetSize() const;

		void Clear();
	};
}