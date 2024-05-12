#pragma once

#include <core/RenderableComponent.h>
#include <core/Input.h>
#include <ui/Text.h>
#include <ui/Image.h>

namespace Twin2Engine::UI {
	class InputField : public Core::RenderableComponent {
	private:
		bool _interactable = true;
		bool _typing = false;

		float _width = 0.f;
		float _height = 0.f;

		Text* _placeHolder = nullptr;
		Text* _text = nullptr;

		std::string _textValue = "";
		size_t _textOffset = 0;
		bool _textDirty = false;

		static const float _cursorDelay;
		float _currentCursorTime = 0.f;
		bool _cursorVisible = false;
		size_t _cursorPos = 0;

		size_t _onKeyStateChangeID = 0;
		void OnKeyStateChange(Core::KEY key, Core::INPUT_STATE state);
	public:
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render() override;
		virtual void OnDestroy() override;
		virtual YAML::Node Serialize() const override;

		void SetPlaceHolderText(Text* placeHolder);
		void SetInputText(Text* text);

		void SetPlaceHolder(const std::string& placeHolder);
		void SetText(const std::string& text);
		void SetWidth(float width);
		void SetHeight(float height);
		void SetInteractable(bool interactable);

		bool IsInteractable() const;
		bool IsTyping() const;
		float GetWidth() const;
		float GetHeight() const;
		std::string GetPlaceHolder() const;
		std::string GetText() const;

		Text* GetPlaceHolderText() const;
		Text* GetInputText() const;
	};
}