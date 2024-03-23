#pragma once

#include "Texture2D.h"

namespace Twin2Engine {
	namespace Manager {
		class SpriteManager;
	}

	namespace Core {

		class Sprite {
		private:
			Texture2D* _tex;
			unsigned int _xof; // x offset
			unsigned int _yof; // y offset
			unsigned int _width; // width of sprite
			unsigned int _height; // height of sprite

			Sprite(Texture2D* tex, unsigned int xof, unsigned int yof, unsigned int width, unsigned int height);

		public:
			virtual ~Sprite() = default;

			constexpr Texture2D* GetTexture() const;
			constexpr unsigned int GetXOffset() const;
			constexpr unsigned int GetYOffset() const;
			constexpr unsigned int GetWidth() const;
			constexpr unsigned int GetHeight() const;

			void Use(unsigned int samplerId = 0) const;

			friend Manager::SpriteManager;
		};
	}
}