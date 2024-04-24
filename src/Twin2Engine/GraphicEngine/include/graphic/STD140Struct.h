#pragma once

namespace Twin2Engine::GraphicEngine {
	class STD140Struct {
	private:
		size_t _currentOffset;
		size_t _maxAligement;

		std::vector<char> _data;
		std::map<size_t, size_t> _offsets;

		static std::hash<std::string> _hasher;

		

	public:
		STD140Struct() = default;
		virtual ~STD140Struct() = default;
	};
}