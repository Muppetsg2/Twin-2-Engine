#pragma once

#include <core/RenderableComponent.h>
#include <graphic/manager/UIRenderingManager.h>
#include <graphic/manager/GIFManager.h>
#include <ui/Canvas.h>
#include <ui/Mask.h>

namespace Twin2Engine::UI {
	class GIFPlayer : public Core::RenderableComponent {
	private:
		Manager::UIElementData _data{
			.canvas = nullptr,
			.mask = nullptr,
			.rectTransform = {
				.transform = glm::mat4(1.f),
				.size = glm::vec2(100.f, 100.f)
			},
			.color = glm::vec4(1.f),
			.layer = 0,
		};

		Manager::FillData _fillData{
			.isActive = false
		};

		size_t _gifId = 0;
		bool _playing = false;
		bool _paused = false;
		bool _looped = true;
		bool _autoPlay = true;

		float _currTime = 0.f;
		size_t _imgIdx = 0;

		size_t _onTransformChangeId = 0;
		size_t _onRotationChangeId = 0;
		size_t _onParentInHierarchiChangeId = 0;
		size_t _onCanvasDestroyId = 0;
		size_t _onMaskDestroyId = 0;
		Canvas* _canvas = nullptr;
		Mask* _mask = nullptr;

		void SetCanvas(Canvas* canvas);
		void SetMask(Mask* mask);
	
	public:
		virtual void Initialize();
		virtual void OnEnable();
		virtual void Update();
		virtual void Render();
		virtual void OnDestroy();

		void SetSize(const glm::vec2& size);
		void SetColor(const glm::vec4& color);
		void SetLayer(int32_t layer);
		void SetGIF(size_t gifId);
		void SetGIF(const std::string& gifPath);
		void SetGIF(const Graphic::GIF* gif);

		glm::vec2 GetSize() const;
		glm::vec4 GetColor() const;
		int32_t GetLayer() const;
		Graphic::GIF* GetGIF() const;

		void Play();
		void Stop();
		void Pause();
		void Loop(bool loop = true);
		void AutoPlay(bool autoPlay = true);

		bool IsPlaying() const;
		bool IsStopped() const;
		bool IsPaused() const;
		bool IsLooped() const;
		bool HasAutoPlay() const;

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}