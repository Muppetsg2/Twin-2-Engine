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

		std::wstring _textValue = L"";
		size_t _textOffset = 0;
		bool _textDirty = false;

		static const float _cursorDelay;
		float _currentCursorTime = 0.f;
		bool _cursorVisible = false;
		size_t _cursorPos = 0;

		size_t _onTextInputID = 0;
		void OnTextInput(unsigned int character);

		bool _insertCursorMode = false;

		Manager::MaskData _inputMask;
	public:
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render() override;
		virtual void OnDestroy() override;
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
		virtual void DrawEditor() override;
#endif

		void SetPlaceHolderText(Text* placeHolder);
		void SetInputText(Text* text);

		void SetPlaceHolder(const std::wstring& placeHolder);
		void SetText(const std::wstring& text);
		void SetWidth(float width);
		void SetHeight(float height);
		void SetInteractable(bool interactable);

		bool IsInteractable() const;
		bool IsTyping() const;
		float GetWidth() const;
		float GetHeight() const;
		std::wstring GetPlaceHolder() const;
		std::wstring GetText() const;

		Text* GetPlaceHolderText() const;
		Text* GetInputText() const;
	};
}