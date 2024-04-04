#pragma once
#include <glad/glad.h>

namespace Twin2Engine {
	namespace Manager {
		class TextureManager;
	}

	namespace Core {
		enum TextureFormat {
			RED = GL_RED,
			RG = GL_RG,
			RGB = GL_RGB,
			BGR = GL_BGR,
			RGBA = GL_RGBA,
			BGRA = GL_BGRA,
			R_INT = GL_RED_INTEGER,
			RG_INT = GL_RG_INTEGER,
			RGB_INT = GL_RGB_INTEGER,
			BGR_INT = GL_BGR_INTEGER,
			RGBA_INT = GL_RGBA_INTEGER,
			BGRA_INT = GL_BGRA_INTEGER,
			STENCIL_IDX = GL_STENCIL_INDEX,
			DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
			DEPTH_STENCIL = GL_DEPTH_STENCIL
		};

		enum TextureWrapMode {
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
			REPEAT = GL_REPEAT,
			MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
			MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE
		};

		enum TextureFilterMode {
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR,
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
			LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
		};

		class Texture2D {
		private:
			size_t _managerId;

			unsigned int _id;
			unsigned int _width;
			unsigned int _height;
			unsigned int _channelsNum;

			TextureFormat _format;
			TextureWrapMode _sWrapMode;
			TextureWrapMode _tWrapMode;
			TextureFilterMode _minFilterMode;
			TextureFilterMode _magFilterMode;

			Texture2D(size_t managerId, unsigned int id, unsigned int width, unsigned int height, unsigned int channelsNum, const TextureFormat& format, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode, const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode);

		public:
			virtual ~Texture2D();

			void SetWrapModeS(const TextureWrapMode& mode);
			void SetWrapModeT(const TextureWrapMode& mode);
			void SetMinFilterMode(const TextureFilterMode& mode);
			void SetMagFilterMode(const TextureFilterMode& mode);

			size_t GetManagerId() const;
			unsigned int GetId() const;
			unsigned int GetWidth() const;
			unsigned int GetHeight() const;
			unsigned int GetChannelsNum() const;
			TextureWrapMode GetWrapModeS() const;
			TextureWrapMode GetWrapModeT() const;
			TextureFilterMode GetMinFilterMode() const;
			TextureFilterMode GetMagFilterMode() const;

			void Use(unsigned int samplerId = 0) const;

			friend Manager::TextureManager;
		};
	}
}