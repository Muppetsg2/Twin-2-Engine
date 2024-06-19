#pragma once

#include <core/RenderableComponent.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/TextureManager.h>

// TODO: Gif Object
// TODO: Gif Manager
// TODO: GIFPlayer Editor
// TODO: Gif Manager Editor
namespace Twin2Engine::UI {
	class GIFPlayer : public Core::RenderableComponent {
	private:
		unsigned char* _buffer = nullptr;
		int* _delays = nullptr;
		int _width = 0;
		int _height = 0;
		int _frames = 0;

		unsigned int* _ids = nullptr;
		Graphic::Texture2D** _textures = nullptr;

		float _currTime = 0.f;
		size_t _imgIdx = 0;

	public:
		virtual void Initialize();
		virtual void Update();
		virtual void Render();
		virtual void OnDestroy();

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}