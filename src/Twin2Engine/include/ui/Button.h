#pragma once

#include <core/Component.h>
#include <tools/EventHandler.h>

namespace Twin2Engine::UI {
	class Canvas;

	class Button : public Core::Component {
	private:
		float _width = 0.f;
		float _height = 0.f;
		bool _interactable = true;
		Tools::MethodEventHandler _onClickEvent = Tools::MethodEventHandler();
		Tools::MethodEventHandler _onHoverEvent = Tools::MethodEventHandler();

		// Preset Events
		bool _useOnHoverPresetEvents = false;
		float _timeFactor = 0.1f;
		glm::vec3 _onHoverScaleStart = glm::vec3(1.f);
		glm::vec3 _onHoverScaleEnd = glm::vec3(1.f);
		glm::vec4 _onHoverColorStart = glm::vec4(1.f); // Requires Image Component in same GameObject
		glm::vec4 _onHoverColorEnd = glm::vec4(1.f); // Requires Image Component in same GameObject

		bool _playAudioOnClick = false; // Requires Audio Component in same GameObject
		size_t _onClickAudioId = 0; // Requires Audio Component in same GameObject

		size_t _onParentInHierarchiChangeId = 0;
		size_t _onCanvasDestroyId = 0;
		Canvas* _canvas = nullptr;

		void SetCanvas(Canvas* canvas);

		void OnHoverPresetEvents(bool isHover);
		void OnClickPresetEvents();

	public:
		void SetWidth(float width);
		void SetHeight(float height);
		void SetInteractable(bool interactable);

		float GetWidth() const;
		float GetHeight() const;
		bool IsInteractable() const;
		Tools::MethodEventHandler& GetOnClickEvent();
		Tools::MethodEventHandler& GetOnHoverEvent();

		// Preset Events
		void EnableOnHoverPresetEvents();
		void DisableOnHoverPresetEvents();
		float GetTimeFactor();
		void SetTimeFactor(float factor);
		std::pair<glm::vec3, glm::vec3> GetOnHoverScale();
		void SetOnHoverScale(std::pair<glm::vec3, glm::vec3> values);
		void SetOnHoverScale(glm::vec3 start, glm::vec3 end);
		std::pair<glm::vec4, glm::vec4> GetOnHoverColor();
		void SetOnHoverColor(std::pair<glm::vec4, glm::vec4> values);
		void SetOnHoverColor(glm::vec4 start, glm::vec4 end);

		void EnableOnClickAudio();
		void DisableOnClickAudio();
		size_t GetAudioOnClick();
		void SetAudioOnClick(size_t id);
		void SetAudioOnClick(std::string path);

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnDestroy() override;
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif
	};
}