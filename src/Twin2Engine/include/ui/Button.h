#pragma once

#include <core/Component.h>
#include <tools/EventHandler.h>
#include <core/EasingFunctions.h>

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
		float _animTime = 0.f;
		bool _useOnHoverPresetEvents = false;
		float _animTimeFactor = 0.1f;
		EaseFunction _onHoverScaleFunc = EaseFunction::LINE;
		EaseFunctionType _onHoverScaleFuncType = EaseFunctionType::IN_F;
		glm::vec3 _onHoverScaleStart = glm::vec3(1.f);
		glm::vec3 _onHoverScaleEnd = glm::vec3(1.f);
		EaseFunction _onHoverColorFunc = EaseFunction::LINE;
		EaseFunctionType _onHoverColorFuncType = EaseFunctionType::IN_F;
		glm::vec4 _onHoverColorStart = glm::vec4(1.f); // Requires Image Component in same GameObject
		glm::vec4 _onHoverColorEnd = glm::vec4(1.f); // Requires Image Component in same GameObject
		bool _displayObjectOnHover = false;
		bool _objectSnapToMouse = false;
		glm::vec2 _objectOffset = glm::vec2(0.f); // Only works when _objectSnapToMouse == true and _displayObjectOnHover == true
		Core::GameObject* _objectToDisplay = nullptr;

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
		EaseFunction GetOnHoverScaleFunc();
		void SetOnHoverScaleFunc(EaseFunction func);
		EaseFunctionType GetOnHoverScaleFuncType();
		void SetOnHoverScaleFuncType(EaseFunctionType funcType);
		std::pair<glm::vec3, glm::vec3> GetOnHoverScale();
		void SetOnHoverScale(std::pair<glm::vec3, glm::vec3> values);
		void SetOnHoverScale(glm::vec3 start, glm::vec3 end);
		EaseFunction GetOnHoverColorFunc();
		void SetOnHoverColorFunc(EaseFunction func);
		EaseFunctionType GetOnHoverColorFuncType();
		void SetOnHoverColorFuncType(EaseFunctionType funcType);
		std::pair<glm::vec4, glm::vec4> GetOnHoverColor();
		void SetOnHoverColor(std::pair<glm::vec4, glm::vec4> values);
		void SetOnHoverColor(glm::vec4 start, glm::vec4 end);

		void EnableOnHoverDisplayObject();
		void DisableOnHoverDisplayObject();
		void SnapObjectToMouseOnHover(bool value);
		glm::vec2 GetOnHoverObjectOffset();
		void SetOnHoverObjectOffset(glm::vec2 value); // Only works when _objectPosUseMouse == true and _displayObjectOnHover == true
		void SetOnHoverObjectOffset(float x, float y); // Only works when _objectPosUseMouse == true and _displayObjectOnHover == true
		Core::GameObject* GetOnHoverDisplayObject();
		void SetOnHoverDisplayObject(Core::GameObject* obj);
		void SetOnHoverDisplayObject(size_t objId);

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