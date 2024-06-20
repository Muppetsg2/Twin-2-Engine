#pragma once

#include <graphic/Texture2D.h>

namespace Twin2Engine {
	namespace Manager {
		class GIFManager;
	}

	namespace Graphic {
		class GIF {
		private:
			friend class Manager::GIFManager;

			size_t _managerId = 0;

			glm::uvec2 _size = glm::uvec2(0.f); // size of gif
			std::vector<float> _delays; // in seconds like deltaTime
			std::vector<Texture2D*> _textures; // unique frame textures
			std::vector<size_t> _frames; // pointers to frame texture

			GIF(size_t managerId, const glm::uvec2& size, const std::vector<float>& delays, const std::vector<Texture2D*>& textures, const std::vector<size_t>& frames);
			~GIF();
		public:

			size_t GetManagerId() const;
			glm::uvec2 GetSize() const;
			float GetDelay(size_t frameIdx) const;
			const Texture2D* const GetFrameTexture(size_t frameIdx) const;
			size_t GetFramesCount() const;
		};
	}
}