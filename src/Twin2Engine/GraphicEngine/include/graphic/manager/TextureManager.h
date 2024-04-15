#pragma once

#include <stb_image.h>
#include <graphic/Texture2D.h>

namespace Twin2Engine::Manager {
	enum TextureFileFormat {
		DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
		DEPTH_STENCIL = GL_DEPTH_STENCIL,
		RED = GL_RED,
		RG = GL_RG,
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		R8 = GL_R8,
		R8_SNORM = GL_R8_SNORM,
		R16 = GL_R16,
		R16_SNORM = GL_R16_SNORM,
		RG8 = GL_RG8,
		RG8_SNORM = GL_RG8_SNORM,
		RG16 = GL_RG16,
		RG16_SNORM = GL_RG16_SNORM,
		RG3_B2 = GL_R3_G3_B2,
		RGB4 = GL_RGB4,
		RGB5 = GL_RGB5,
		RGB8 = GL_RGB8,
		RGB8_SNORM = GL_RGB8_SNORM,
		RGB10 = GL_RGB10,
		RGB12 = GL_RGB12,
		RGB16_SNORM = GL_RGB16_SNORM,
		RGBA2 = GL_RGBA2,
		RGBA4 = GL_RGBA4,
		RGB5_A1 = GL_RGB5_A1,
		RGBA8 = GL_RGBA8,
		RGBA8_SNORM = GL_RGBA8_SNORM,
		RGB10_A2 = GL_RGB10_A2,
		RGB10_A2_UINT = GL_RGB10_A2UI,
		RGBA12 = GL_RGBA12,
		RGBA16 = GL_RGBA16,
		SRGB8 = GL_SRGB8,
		SRGBA8 = GL_SRGB8_ALPHA8,
		R16_FLOAT = GL_R16F,
		RG16_FLOAT = GL_RG16F,
		RGB16_FLOAT = GL_RGB16F,
		RGBA16_FLOAT = GL_RGBA16F,
		R32_FLOAT = GL_R32F,
		RG32_FLOAT = GL_RG16F,
		RGB32_FLOAT = GL_RGB16F,
		RGBA32_FLOAT = GL_RGBA16F,
		RG11_B10_FLOAT = GL_R11F_G11F_B10F,
		RGB9_E5 = GL_RGB9_E5,
		R8_INT = GL_R8I,
		R8_UINT = GL_R8UI,
		R16_INT = GL_R16I,
		R16_UINT = GL_R16UI,
		R32_INT = GL_R32I,
		R32_UINT = GL_R32UI,
		RG8_INT = GL_RG8I,
		RG8_UINT = GL_RG8UI,
		RG16_INT = GL_RG16I,
		RG16_UINT = GL_RG16UI,
		RG32_INT = GL_RG32I,
		RG32_UINT = GL_RG32UI,
		RGB8_INT = GL_RGB8I,
		RGB8_UINT = GL_RGB8UI,
		RGB16_INT = GL_RGB16I,
		RGB16_UINT = GL_RGB16UI,
		RGB32_INT = GL_RGB32I,
		RGB32_UINT = GL_RGB32UI,
		RGBA8_INT = GL_RGBA8I,
		RGBA8_UINT = GL_RGBA8UI,
		RGBA16_INT = GL_RGBA16I,
		RGBA16_UINT = GL_RGBA16UI,
		RGBA32_INT = GL_RGBA32I,
		RGBA32_UINT = GL_RGBA32UI,
		COMPRESSED_R = GL_COMPRESSED_RED,
		COMPRESSED_RG = GL_COMPRESSED_RG,
		COMPRESSED_RGB = GL_COMPRESSED_RGB,
		COMPRESSED_RGBA = GL_COMPRESSED_RGBA,
		COMPRESSED_SRGB = GL_COMPRESSED_SRGB,
		COMPRESSED_SRGBA = GL_COMPRESSED_SRGB_ALPHA,
		COMPRESSED_R_RGTC1 = GL_COMPRESSED_RED_RGTC1,
		COMPRESSED_SIGNED_R_RGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
		COMPRESSED_RG_RGTC2 = GL_COMPRESSED_RG_RGTC2,
		COMPRESSED_SIGNED_RG_RGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
		COMPRESSED_RGBA_BPTC_UNORM = GL_COMPRESSED_RGBA_BPTC_UNORM,
		COMPRESSED_SRGBA_BPTC_UNORM = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
		COMPRESSED_RGB_BPTC_SIGNED_FLOAT = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
		COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
	};

	struct TextureData {
		GraphicEngine::TextureWrapMode sWrapMode = GraphicEngine::TextureWrapMode::MIRRORED_REPEAT;
		GraphicEngine::TextureWrapMode tWrapMode = GraphicEngine::TextureWrapMode::MIRRORED_REPEAT;
		GraphicEngine::TextureFilterMode minFilterMode = GraphicEngine::TextureFilterMode::NEAREST_MIPMAP_LINEAR;
		GraphicEngine::TextureFilterMode magFilterMode = GraphicEngine::TextureFilterMode::LINEAR;
	};

	class SceneManager;

	class TextureManager {
	private:
		static std::hash<std::string> _hasher;
		static std::map<size_t, GraphicEngine::Texture2D*> _loadedTextures;

		static std::map<size_t, std::string> _texturesPaths;
		static std::map<size_t, std::pair<GraphicEngine::TextureFormat, TextureFileFormat>> _texturesFormats;

		static void UnloadTexture2D(size_t managerID);
		static void UnloadTexture2D(const std::string& path);
	public:
		static GraphicEngine::Texture2D* GetTexture2D(size_t managerId);
		static GraphicEngine::Texture2D* GetTexture2D(const std::string& path);
		static GraphicEngine::Texture2D* LoadTexture2D(const std::string& path, const TextureData& data = TextureData());
		static GraphicEngine::Texture2D* LoadTexture2D(const std::string& path, const TextureFileFormat& internalFormat, const GraphicEngine::TextureFormat& format, const TextureData& data = TextureData());

		static void UnloadAll();

		friend class SceneManager;
	};
}