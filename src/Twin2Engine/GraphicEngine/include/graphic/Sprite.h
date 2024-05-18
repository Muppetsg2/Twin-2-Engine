#pragma once

#include <graphic/Texture2D.h>

namespace Twin2Engine {
	namespace Manager {
		class SpriteManager;
	}

	namespace Graphic {

		class Sprite {
		private:
			size_t _managerId;

			Texture2D* _tex;
			glm::uvec2 _offset; // offset
			glm::uvec2 _size; // size (width, height)

			Sprite(size_t managerId, Texture2D* tex, unsigned int xof, unsigned int yof, unsigned int width, unsigned int height);
			Sprite(size_t managerId, Texture2D* tex, glm::uvec2 offset, glm::uvec2 size);

		public:
			virtual ~Sprite() = default;

			size_t GetManagerId() const;
			Texture2D* GetTexture() const;
			glm::uvec2 GetOffset() const;
			unsigned int GetXOffset() const;
			unsigned int GetYOffset() const;
			glm::uvec2 GetSize() const;
			unsigned int GetWidth() const;
			unsigned int GetHeight() const;

			void Use(unsigned int samplerId = 0) const;

			friend Manager::SpriteManager;
		};
	}
}