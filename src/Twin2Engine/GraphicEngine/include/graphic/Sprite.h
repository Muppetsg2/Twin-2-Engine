#pragma once

#include <graphic/Texture2D.h>

namespace Twin2Engine {
	namespace Manager {
		class SpriteManager;
	}

	namespace GraphicEngine {

		class Sprite {
		private:
			size_t _managerId;

			Texture2D* _tex;
			unsigned int _xof; // x offset
			unsigned int _yof; // y offset
			unsigned int _width; // width of sprite
			unsigned int _height; // height of sprite

			Sprite(size_t managerId, Texture2D* tex, unsigned int xof, unsigned int yof, unsigned int width, unsigned int height);

		public:
			virtual ~Sprite() = default;

			size_t GetManagerId() const;
			Texture2D* GetTexture() const;
			unsigned int GetXOffset() const;
			unsigned int GetYOffset() const;
			unsigned int GetWidth() const;
			unsigned int GetHeight() const;

			void Use(unsigned int samplerId = 0) const;

			friend Manager::SpriteManager;
		};
	}
}